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
        helpMsg = HELP_CONFIG_SLOT;
        break;
    case Help_PublicID:
        title = tr("Public Identity");
        helpMsg = HELP_PUBLIC_ID;
        break;
    case Help_PrivateID:
        title = tr("Private Identity");
        helpMsg = HELP_PRIVATE_ID;
        break;
    case Help_SecretKey:
        title = tr("Secret Key");
        helpMsg = HELP_SECRET_KEY;
        break;
    case Help_ParameterGeneration:
        title = tr("Parameter Generation Scheme");
        helpMsg = HELP_PARAM_GENERATION;
        break;
    case Help_ConfigurationProtection:
        title = tr("Configuration Protection");
        helpMsg = HELP_CONFIG_PROTECTION;
        break;
    case Help_OutputFormat:
        title = tr("Output Format");
        helpMsg = HELP_OUTPUT_FORMAT;
        break;
    case Help_OutputSpeed:
        title = tr("Output Speed Throttling");
        helpMsg = HELP_OUTPUT_SPEED;
        break;
    case Help_SrNoVisibility:
        title = tr("Serial # Visibility");
        helpMsg = HELP_SR_NO_VISIBILITY;
        break;
    case Help_OathPublicID:
        title = tr("OATH Token Identifier");
        helpMsg = HELP_OATH_PUBLIC_ID;
        break;
    case Help_HotpLen:
        title = tr("HOTP Length");
        helpMsg = HELP_HOTP_LEN;
        break;
    case Help_HotpParam:
        title = tr("HOTP Moving Factor Seed");
        helpMsg = HELP_HOTP_PARAM;
        break;
    case Help_StaticScanCode:
        title = tr("Static Password Scan Code Mode");
        helpMsg = HELP_STATIC_SCAN_CODE;
        break;
    case Help_ChalRespYubico:
        title = tr("Challenge-Response Yubico OTP Mode");
        helpMsg = HELP_CHALRESP_YUBICO;
        break;
    case Help_ChalRespHmac:
        title = tr("Challenge-Response HMAC-SHA1 Mode");
        helpMsg = HELP_CHALRESP_HMAC;
        break;
    case Help_ChalRespOption:
        title = tr("Challenge-Response Options");
        helpMsg = HELP_CHALRESP_OPTION;
        break;
    case Help_AllowUpdate:
        title = tr("Allow Update");
        helpMsg = HELP_ALLOW_UPDATE_OPTION;
        break;
    case Help_Swap:
        title = tr("Swap");
        helpMsg = HELP_SWAP_OPTION;
        break;
    case Help_ManUpdate:
        title = tr("Manual Update");
        helpMsg = HELP_MAN_UPDATE_OPTION;
        break;

    default:
        helpMsg = tr("");
    }

    this->setWindowTitle(tr("Help: %1").arg(title));
    ui->helpTxtBrowser->setText(helpMsg);
}
