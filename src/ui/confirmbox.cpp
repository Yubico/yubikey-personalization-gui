/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#include "confirmbox.h"
#include "ui_confirmbox.h"

#include "common.h"

ConfirmBox::ConfirmBox(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ConfirmBox)
{
    ui->setupUi(this);
}

ConfirmBox::~ConfirmBox() {
    delete ui;
}

void ConfirmBox::setConfirmIndex(Confirm confirmIndex) {
    QString title;
    QString confirmMsg;

    switch(confirmIndex) {
    case Confirm_ConfigurationSlot:
        title = OVERWRITE_CONF_SLOT1;
        confirmMsg = WARN_OVERWRITE_CONF_SLOT1;
        break;

    case Confirm_PublicID:
        title = DIFF_PUBLIC_ID_LEN;
        confirmMsg = WARN_DIFF_PUBLIC_ID_LEN;
        break;

    case Confirm_ConfigurationProtection:
        title = ENABLE_CONF_PROTECTION;
        confirmMsg = WARN_ENABLE_CONF_PROTECTION;
        break;

    default:
        confirmMsg = tr("");
    }

    m_confirmIndex = confirmIndex;

    this->setWindowTitle(tr("Confirm: %1").arg(title));

    QPixmap question(QMessageBox::standardIcon(QMessageBox::Question));
    ui->iconLbl->setPixmap(question);

    ui->msgLbl->setText(confirmMsg);
}

void ConfirmBox::on_dontaskCheck_stateChanged(int state) {
    bool checked = (state != 0);

    QSettings settings;
    switch(m_confirmIndex) {
    case Confirm_ConfigurationSlot:
        settings.setValue(SG_OVERWRITE_CONF_SLOT1,      checked);
        break;

    case Confirm_PublicID:
        settings.setValue(SG_DIFF_PUBLIC_ID_LEN,        checked);
        break;

    case Confirm_ConfigurationProtection:
        settings.setValue(SG_ENABLE_CONF_PROTECTION,    checked);
        break;
    }
}
