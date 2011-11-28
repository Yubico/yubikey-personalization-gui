/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#include "ui/helpbox.h"
#include "ui_helpbox.h"

#include "common.h"

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

    default:
        helpMsg = tr("");
    }

    this->setWindowTitle(tr("Help: %1").arg(title));
    ui->helpTxtBrowser->setText(helpMsg);
}
