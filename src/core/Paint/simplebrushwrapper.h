// enve - 2D animations software
// Copyright (C) 2016-2019 Maurycy Liebner

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

#ifndef SIMPLEBRUSHWRAPPER_H
#define SIMPLEBRUSHWRAPPER_H
#include <mypaint-brush.h>
#include <QColor>
#include "pointhelpers.h"
#include "smartPointers/ememory.h"

class SimpleBrushWrapper : public StdSelfRef {
    e_OBJECT
public:
    ~SimpleBrushWrapper();

    stdsptr<SimpleBrushWrapper> createDuplicate();

    MyPaintBrush * getBrush() const { return mBrush; }

    QColor getColor() const {
        const float h = getBaseValue(MYPAINT_BRUSH_SETTING_COLOR_H);
        const float s = getBaseValue(MYPAINT_BRUSH_SETTING_COLOR_S);
        const float v = getBaseValue(MYPAINT_BRUSH_SETTING_COLOR_V);
        const float o = getBaseValue(MYPAINT_BRUSH_SETTING_OPAQUE);
        QColor col;
        col.setHsvF(qreal(h), qreal(s), qreal(v), qreal(o));
        return col;
    }

    void setColor(const float hue,
                  const float saturation,
                  const float value) const {
        setBaseValue(MYPAINT_BRUSH_SETTING_COLOR_H, clamp(hue, 0, 1));
        setBaseValue(MYPAINT_BRUSH_SETTING_COLOR_S, clamp(saturation, 0, 1));
        setBaseValue(MYPAINT_BRUSH_SETTING_COLOR_V, clamp(value, 0, 1));
    }

    void setColor(const float hue,
                  const float saturation,
                  const float value,
                  const float opacity) const {
        setColor(hue, saturation, value);
        setOpactiy(opacity);
    }

    void setColor(const QColor& color) const {
        setColor(float(color.hueF()), float(color.saturationF()),
                 float(color.valueF()), float(color.alphaF()));
    }

    void setOpactiy(const float opacity) const {
        setBaseValue(MYPAINT_BRUSH_SETTING_OPAQUE, clamp(opacity, 0, 1));
    }

    void incOpacity(const qreal inc) {
        const float opacity = getBaseValue(MYPAINT_BRUSH_SETTING_OPAQUE);
        setOpactiy(opacity + float(inc));
    }

    void decOpacity(const qreal dec) {
        incOpacity(-dec);
    }

    float getBaseValue(const MyPaintBrushSetting& id) const {
        return mypaint_brush_get_base_value(mBrush, id);
    }

    void setBaseValue(const MyPaintBrushSetting& id, const float val) const {
        mypaint_brush_set_base_value(mBrush, id, val);
    }

    void setPaintBrushSize(const qreal size) const {
        const float brushSize = static_cast<float>(size);
        setBaseValue(MYPAINT_BRUSH_SETTING_RADIUS_LOGARITHMIC, brushSize);
    }

    void incPaintBrushSize(const qreal inc) const {
        const float sizeF = getBaseValue(MYPAINT_BRUSH_SETTING_RADIUS_LOGARITHMIC);
        setPaintBrushSize(static_cast<qreal>(sizeF) + inc);
    }

    void decPaintBrushSize(const qreal dec) const {
        incPaintBrushSize(-dec);
    }

    void startEraseMode() const {
        setBaseValue(MYPAINT_BRUSH_SETTING_ERASER, 1);
    }

    void finishEraseMode() const {
        setBaseValue(MYPAINT_BRUSH_SETTING_ERASER, 0);
    }

    void startAlphaLockMode() const {
        setBaseValue(MYPAINT_BRUSH_SETTING_LOCK_ALPHA, 1);
    }

    void finishAlphaLockMode() const {
        setBaseValue(MYPAINT_BRUSH_SETTING_LOCK_ALPHA, 0);
    }

    void startColorizeMode() const {
        setBaseValue(MYPAINT_BRUSH_SETTING_COLORIZE, 1);
    }

    void finishColorizeMode() const {
        setBaseValue(MYPAINT_BRUSH_SETTING_COLORIZE, 0);
    }

    const QString& getBrushName() const {
        return mBrushName;
    }

    const QString& getCollectionName() const {
        return mCollectionName;
    }
protected:
    SimpleBrushWrapper(const QString& collName,
                       const QString& brushName,
                       MyPaintBrush * const brush,
                       const QByteArray& wholeFile);
private:
    MyPaintBrush * const mBrush;
    const QByteArray mWholeFile;
    const QString mCollectionName;
    const QString mBrushName;
};

#endif // SIMPLEBRUSHWRAPPER_H
