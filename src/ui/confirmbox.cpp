/*
Copyright (C) 2011-2013 Yubico AB.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "confirmbox.h"
#include "ui_confirmbox.h"

#include <QMessageBox>
#include <QSettings>

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
        title = tr(OVERWRITE_CONF_SLOT1);
        confirmMsg = tr(WARN_OVERWRITE_CONF_SLOT1);
        break;

    case Confirm_PublicID:
        title = tr(DIFF_PUBLIC_ID_LEN);
        confirmMsg = tr(WARN_DIFF_PUBLIC_ID_LEN);
        break;

    case Confirm_ConfigurationProtection:
        title = tr(ENABLE_CONF_PROTECTION);
        confirmMsg = tr(WARN_ENABLE_CONF_PROTECTION);
        break;

    case Confirm_NdefWithoutHttp:
        title = tr(NDEF_WITHOUT_HTTP);
        confirmMsg = tr(WARN_NDEF_WITHOUT_HTTP);
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

    case Confirm_NdefWithoutHttp:
        settings.setValue(SG_NDEF_WITHOUT_HTTP,    checked);
        break;
    }
}
