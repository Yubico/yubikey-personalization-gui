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

#include "ui/helpbox.h"
#include "ui_helpbox.h"

#include "common.h"
#include "help.h"
#include "yubikeylogger.h"

HelpBox::HelpBox(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::HelpBox)
{
    ui->setupUi(this);
}

HelpBox::~HelpBox() {
    delete ui;
}

void HelpBox::setHelpIndex(Help helpIndex) {
    QString title;
    QString helpMsg;

    switch(helpIndex) {
    case Help_ConfigurationSlot:
        title = tr("Configuration Slots");
        helpMsg = tr(HELP_CONFIG_SLOT);
        break;
    case Help_PublicID:
        title = tr("Public Identity");
        helpMsg = tr(HELP_PUBLIC_ID);
        break;
    case Help_PrivateID:
        title = tr("Private Identity");
        helpMsg = tr(HELP_PRIVATE_ID);
        break;
    case Help_SecretKey:
        title = tr("Secret Key");
        helpMsg = tr(HELP_SECRET_KEY);
        break;
    case Help_ParameterGeneration:
        title = tr("Parameter Generation Scheme");
        helpMsg = tr(HELP_PARAM_GENERATION);
        break;
    case Help_ConfigurationProtection:
        title = tr("Configuration Protection");
        helpMsg = tr(HELP_CONFIG_PROTECTION);
        break;
    case Help_OutputFormat:
        title = tr("Output Format");
        helpMsg = tr(HELP_OUTPUT_FORMAT);
        break;
    case Help_OutputSpeed:
        title = tr("Output Speed Throttling");
        helpMsg = tr(HELP_OUTPUT_SPEED);
        break;
    case Help_SrNoVisibility:
        title = tr("Serial # Visibility");
        helpMsg = tr(HELP_SR_NO_VISIBILITY);
        break;
    case Help_OathPublicID:
        title = tr("OATH Token Identifier");
        helpMsg = tr(HELP_OATH_PUBLIC_ID);
        break;
    case Help_HotpLen:
        title = tr("HOTP Length");
        helpMsg = tr(HELP_HOTP_LEN);
        break;
    case Help_HotpParam:
        title = tr("HOTP Moving Factor Seed");
        helpMsg = tr(HELP_HOTP_PARAM);
        break;
    case Help_StaticScanCode:
        title = tr("Static Password Scan Code Mode");
        helpMsg = tr(HELP_STATIC_SCAN_CODE);
        break;
    case Help_ChalRespYubico:
        title = tr("Challenge-Response Yubico OTP Mode");
        helpMsg = tr(HELP_CHALRESP_YUBICO);
        break;
    case Help_ChalRespHmac:
        title = tr("Challenge-Response HMAC-SHA1 Mode");
        helpMsg = tr(HELP_CHALRESP_HMAC);
        break;
    case Help_ChalRespOption:
        title = tr("Challenge-Response Options");
        helpMsg = tr(HELP_CHALRESP_OPTION);
        break;
    case Help_AllowUpdate:
        title = tr("Allow Update");
        helpMsg = tr(HELP_ALLOW_UPDATE_OPTION);
        break;
    case Help_Swap:
        title = tr("Swap");
        helpMsg = tr(HELP_SWAP_OPTION);
        break;
    case Help_ManUpdate:
        title = tr("Manual Update");
        helpMsg = tr(HELP_MAN_UPDATE_OPTION);
        break;
    case Help_LogFormat:
        {
            title = tr("Flexible Log Format");
            helpMsg = tr(HELP_LOG_FORMAT);
            QStringList names = YubiKeyLogger::getLogNames();
            helpMsg.append(names.join(", "));
            helpMsg.append("</p>");
        }
        break;

    default:
        helpMsg = tr("");
    }

    this->setWindowTitle(tr("Help: %1").arg(title));
    ui->helpTxtBrowser->setText(helpMsg);
}
