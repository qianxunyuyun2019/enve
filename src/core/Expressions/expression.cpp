// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "expression.h"

#include "exceptions.h"

Expression::ResultTester Expression::sQrealAnimatorTester =
        [](const QJSValue& val) {
            if(!val.isNumber()) PrettyRuntimeThrow("Invalid return type");
        };

Expression::Expression(const QString& definitionsStr,
                       const QString& scriptStr,
                       PropertyBindingMap&& bindings,
                       std::unique_ptr<QJSEngine>&& engine,
                       QJSValue&& eEvaluate) :
    mDefinitionsStr(definitionsStr),
    mScriptStr(scriptStr),
    mEEvaluate(std::move(eEvaluate)),
    mBindings(std::move(bindings)),
    mEngine(std::move(engine)) {
    for(const auto& binding : mBindings) {
        connect(binding.second.get(), &PropertyBinding::currentValueChanged,
                this, &Expression::currentValueChanged);
        connect(binding.second.get(), &PropertyBinding::relRangeChanged,
                this, &Expression::relRangeChanged);
    }
}


void throwIfError(const QJSValue& value, const QString& name) {
    if(value.isError()) {
        PrettyRuntimeThrow("Uncaught exception in " + name + " at line "
                           + value.property("lineNumber").toString() +
                           ":\n" + value.toString());
    }
}

void Expression::sAddDefinitionsTo(const QString& definitionsStr,
                                   QJSEngine& e) {
    const auto defRet = e.evaluate(definitionsStr);
    throwIfError(defRet, "Definitions");
}

void Expression::sAddScriptTo(const QString& scriptStr,
                              const PropertyBindingMap& bindings,
                              QJSEngine& e, QJSValue& eEvaluate,
                              const ResultTester& resultTester) {
    QStringList bindingVars;
    QJSValueList testArgs;
    for(const auto& binding : bindings) {
        bindingVars << binding.first;
        testArgs << binding.second->getJSValue(e);
    }
    const QString evalVars = bindingVars.join(", ");
    eEvaluate = e.evaluate(
            "var eEvaluate;"
            "eEvaluate = function(" + evalVars + ") {" +
                scriptStr +
            "}");
    throwIfError(eEvaluate, "Script");
    if(!eEvaluate.isCallable())
        PrettyRuntimeThrow("Uncallable script.");
    const auto testResult = eEvaluate.call(testArgs);
    if(testResult.isError()) {
        PrettyRuntimeThrow("Script test error:\n" +
                           testResult.toString());
    } else if(resultTester) resultTester(testResult);
}

qsptr<Expression> Expression::sCreate(const QString& bindingsStr,
                                      const QString& definitionsStr,
                                      const QString& scriptStr,
                                      const Property* const context,
                                      const ResultTester& resultTester) {
    auto bindings = PropertyBindingParser::parseBindings(
                              bindingsStr, nullptr, context);
    auto engine = std::make_unique<QJSEngine>();
    sAddDefinitionsTo(definitionsStr, *engine);
    QJSValue eEvaluate;
    sAddScriptTo(scriptStr, bindings, *engine, eEvaluate, resultTester);
    return sCreate(definitionsStr, scriptStr,
                   std::move(bindings),
                   std::move(engine),
                   std::move(eEvaluate));
}

qsptr<Expression> Expression::sCreate(const QString& definitionsStr,
                                      const QString& scriptStr,
                                      PropertyBindingMap&& bindings,
                                      std::unique_ptr<QJSEngine>&& engine,
                                      QJSValue&& eEvaluate) {
    if(!eEvaluate.isCallable())
        RuntimeThrow("Uncallable script:\n" + scriptStr);
    return qsptr<Expression>(new Expression(definitionsStr, scriptStr,
                                            std::move(bindings),
                                            std::move(engine),
                                            std::move(eEvaluate)));
}

bool Expression::setAbsFrame(const int absFrame) {
    bool changed = false;
    for(const auto& binding : mBindings) {
        changed = changed || binding.second->setAbsFrame(absFrame);
    }
    return changed;
}

bool Expression::isValid() {
    for(const auto& binding : mBindings) {
        const bool valid = binding.second->isValid();
        if(!valid) return false;
    }
    return true;
}

bool Expression::dependsOn(const Property* const prop) {
    for(const auto& binding : mBindings) {
        const bool depends = binding.second->dependsOn(prop);
        if(depends) return true;
    }
    return false;
}

QJSValue Expression::evaluate() {
    QJSValueList values;
    for(const auto& binding : mBindings) {
        values << binding.second->getJSValue(*mEngine);
    }
    return mEEvaluate.call(values);
}

QJSValue Expression::evaluate(const qreal relFrame) {
    QJSValueList values;
    for(const auto& binding : mBindings) {
        values << binding.second->getJSValue(*mEngine, relFrame);
    }
    return mEEvaluate.call(values);
}

FrameRange Expression::identicalRange(const int absFrame) {
    FrameRange result = FrameRange::EMINMAX;
    for(const auto& binding : mBindings) {
        result *= binding.second->identicalRange(absFrame);
    }
    return result;
}

QString Expression::bindingsString() const {
    QString result;
    for(const auto& binding : mBindings) {
        result += binding.first + " = " + binding.second->path() + ";\n";
    }
    return result;
}
