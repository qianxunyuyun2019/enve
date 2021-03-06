#include "propertynamedialog.h"

#include <QVBoxLayout>
#include <QPushButton>

#include "Properties/property.h"
#include "Boxes/boundingbox.h"
#include "canvas.h"

PropertyNameDialog::PropertyNameDialog(
        const QString &initialValue,
        QWidget* const parent) : QDialog(parent) {
    setWindowTitle("Rename " + initialValue);

    const auto mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    mName = new QLineEdit(initialValue, this);
    mainLayout->addWidget(mName);
    connect(mName, &QLineEdit::textChanged,
            this, &PropertyNameDialog::validate);

    mErrorLabel = new QLabel(this);
    mErrorLabel->setObjectName("errorLabel");
    mainLayout->addWidget(mErrorLabel);

    const auto buttonsLayout = new QHBoxLayout;
    mOkButton = new QPushButton("Ok", this);
    const auto cancelButton = new QPushButton("Cancel", this);

    buttonsLayout->addWidget(mOkButton);
    buttonsLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonsLayout);

    connect(mOkButton, &QPushButton::released,
            this, &PropertyNameDialog::accept);
    connect(cancelButton, &QPushButton::released,
            this, &PropertyNameDialog::reject);

    validate();
}

bool PropertyNameDialog::sRenameBox(BoundingBox * const box,
                                    QWidget * const parent) {
    const auto dialog = new PropertyNameDialog(box->prp_getName(),
                                               parent);
    const auto result = dialog->exec() == QDialog::Accepted;
    if(result) {
        const QString inputName = dialog->name();
        box->rename(inputName);
    }
    delete dialog;
    return result;
}

bool PropertyNameDialog::sRenameProperty(Property * const prop,
                                         QWidget * const parent) {
    const auto dialog = new PropertyNameDialog(prop->prp_getName(),
                                               parent);
    const auto result = dialog->exec() == QDialog::Accepted;
    if(result) prop->prp_setNameAction(dialog->name());
    delete dialog;
    return result;
}

bool PropertyNameDialog::validate() {
    QString error;
    const bool valid = Property::prp_sValidateName(mName->text(), &error);
    mErrorLabel->setText(error);
    mOkButton->setEnabled(valid);
    return valid;
}
