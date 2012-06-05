/*
Copyright (C) 2011-2012 Yubico AB.  All rights reserved.

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

#include "settingpage.h"
#include "ui_settingpage.h"
#include "ui/helpbox.h"
#include <QFile>

#include "common.h"
#include "version.h"

SettingPage::SettingPage(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::SettingPage)
{
    ui->setupUi(this);

    //Connect help buttons
    connectHelpButtons();

    //Connect other signals and slots
    connect(ui->saveBtn, SIGNAL(clicked()),
            this, SLOT(save()));
    connect(ui->cancelBtn, SIGNAL(clicked()),
            this, SLOT(load()));
    connect(ui->restoreBtn, SIGNAL(clicked()),
            this, SLOT(restore()));
}

SettingPage::~SettingPage() {
    delete ui;
}

void SettingPage::connectHelpButtons() {
    //Map the values of the help buttons

    //Create a QMapper
    QSignalMapper *mapper = new QSignalMapper(this);

    //Connect the clicked signal with the QSignalMapper
    connect(ui->outFormatHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->outSpeedHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->srVisibilityHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    //Set a value for each button
    mapper->setMapping(ui->outFormatHelpBtn, HelpBox::Help_OutputFormat);
    mapper->setMapping(ui->outSpeedHelpBtn, HelpBox::Help_OutputSpeed);
    mapper->setMapping(ui->srVisibilityHelpBtn, HelpBox::Help_SrNoVisibility);

    //Connect the mapper
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(helpBtn_pressed(int)));
}

void SettingPage::helpBtn_pressed(int helpIndex) {
    HelpBox help(this);
    help.setHelpIndex((HelpBox::Help)helpIndex);
    help.exec();
}

void SettingPage::init() {
    QCoreApplication::setOrganizationName(VER_COMPANYNAME_STR);
    QCoreApplication::setApplicationName(VER_PRODUCTNAME_STR);

    load();
}

void SettingPage::restoreDefaults() {
    QSettings settings;

    settings.setValue(SG_CUSTOMER_PREFIX_USED,  false);
    settings.setValue(SG_CUSTOMER_PREFIX,       0);

    settings.setValue(SG_LOG_DISABLED,          false);
    settings.setValue(SG_LOG_FILENAME,          YubiKeyLogger::defaultLogFilename());

    settings.setValue(SG_TAB_FIRST,             false);
    settings.setValue(SG_APPEND_TAB1,           false);
    settings.setValue(SG_APPEND_TAB2,           false);
    settings.setValue(SG_APPEND_CR,             true);
    settings.setValue(SG_APPEND_DELAY1,         false);
    settings.setValue(SG_APPEND_DELAY2,         false);
    settings.setValue(SG_PACING_10MS,           false);
    settings.setValue(SG_PACING_20MS,           false);

    settings.setValue(SG_SR_BTN_VISIBLE,        true);
    settings.setValue(SG_SR_USB_VISIBLE,        true);
    settings.setValue(SG_SR_API_VISIBLE,        true);

    settings.setValue(SG_MAN_UPDATE,            false);
}

void SettingPage::load() {
    QSettings settings;

    QString logFilename = settings.value(SG_LOG_FILENAME).toString();
    bool logDisabled = settings.value(SG_LOG_DISABLED).toBool();
    if(logFilename.isEmpty()) {
        //This is the first time... set defaults
        restoreDefaults();
    }

    //General settings...
    //Customer prefix
    bool custPrefixUsed = settings.value(SG_CUSTOMER_PREFIX_USED).toBool();
    if(custPrefixUsed) {
        ui->custPrefixCheck->setChecked(true);

        int custPrefix = settings.value(SG_CUSTOMER_PREFIX).toInt();
        ui->custPrefixTxt->setText(QString::number(custPrefix));
        ui->custPrefixTxt->setEnabled(true);
    } else {
        ui->custPrefixCheck->setChecked(false);

        ui->custPrefixTxt->setText("0");
        ui->custPrefixTxt->setEnabled(false);
    }


    // OTP output settings...
    ui->tabFirstBtn->setChecked(settings.value(SG_TAB_FIRST).toBool());
    ui->appendTab1Btn->setChecked(settings.value(SG_APPEND_TAB1).toBool());
    ui->appendTab2Btn->setChecked(settings.value(SG_APPEND_TAB2).toBool());
    ui->appendCRBtn->setChecked(settings.value(SG_APPEND_CR).toBool());

    ui->appendDelay1Check->setChecked(settings.value(SG_APPEND_DELAY1).toBool());
    ui->appendDelay2Check->setChecked(settings.value(SG_APPEND_DELAY2).toBool());

    bool pacing10ms = settings.value(SG_PACING_10MS).toBool();
    bool pacing20ms = settings.value(SG_PACING_20MS).toBool();
    int outCharRateIndex = 0;
    if(pacing10ms) {
        if(pacing20ms) {
            outCharRateIndex = OUT_CHAR_RATE_SLOWDOWN_60;
        } else {
            outCharRateIndex = OUT_CHAR_RATE_SLOWDOWN_20;
        }
    } else if(pacing20ms) {
        outCharRateIndex =  OUT_CHAR_RATE_SLOWDOWN_40;
    }
    ui->outCharRateCombo->setCurrentIndex(outCharRateIndex);


    //Serial # visibility settings...
    ui->srBtnVisibleCheck->setChecked(settings.value(SG_SR_BTN_VISIBLE).toBool());
    ui->srUsbVisibleCheck->setChecked(settings.value(SG_SR_USB_VISIBLE).toBool());
    ui->srApiVisibleCheck->setChecked(settings.value(SG_SR_API_VISIBLE).toBool());

    //Static Password settings...
    ui->manUpdateCheck->setChecked(settings.value(SG_MAN_UPDATE).toBool());

    //Logging settings...
    if(logDisabled) {
        YubiKeyLogger::disableLogging();

        ui->logOutputCheck->setChecked(false);
        ui->logFileTxt->setEnabled(false);
        ui->browseBtn->setEnabled(false);
    } else {
        YubiKeyLogger::enableLogging();

        ui->logOutputCheck->setChecked(true);
        ui->logFileTxt->setEnabled(true);
        ui->browseBtn->setEnabled(true);
    }

    if(!logFilename.isEmpty()) {
        YubiKeyLogger::setLogFilename(logFilename);
    } else {
        logFilename = YubiKeyLogger::logFilename();
    }

    ui->logFileTxt->setText(logFilename);
    ui->logFileTxt->setCursorPosition(0);

    //Signal everyone
    emit settingsChanged();
}

void SettingPage::save() {
    QSettings settings;

    //General settings...

    //Customer prefix
    int custPrefix = 0;
    if(ui->custPrefixCheck->isChecked()) {
        settings.setValue(SG_CUSTOMER_PREFIX_USED, true);
        custPrefix = ui->custPrefixTxt->text().toInt();
    } else {
        settings.setValue(SG_CUSTOMER_PREFIX_USED, false);
    }
    settings.setValue(SG_CUSTOMER_PREFIX, custPrefix);


    // OTP output settings...
    settings.setValue(SG_TAB_FIRST,         ui->tabFirstBtn->isChecked());
    settings.setValue(SG_APPEND_TAB1,       ui->appendTab1Btn->isChecked());
    settings.setValue(SG_APPEND_TAB2,       ui->appendTab2Btn->isChecked());
    settings.setValue(SG_APPEND_CR,         ui->appendCRBtn->isChecked());
    settings.setValue(SG_APPEND_DELAY1,     ui->appendDelay1Check->isChecked());
    settings.setValue(SG_APPEND_DELAY2,     ui->appendDelay2Check->isChecked());

    switch(ui->outCharRateCombo->currentIndex()) {
    case OUT_CHAR_RATE_STD:
        settings.setValue(SG_PACING_10MS,   false);
        settings.setValue(SG_PACING_20MS,   false);
        break;
    case OUT_CHAR_RATE_SLOWDOWN_20:
        settings.setValue(SG_PACING_10MS,   true);
        settings.setValue(SG_PACING_20MS,   false);
        break;
    case OUT_CHAR_RATE_SLOWDOWN_40:
        settings.setValue(SG_PACING_10MS,   false);
        settings.setValue(SG_PACING_20MS,   true);
        break;
    case OUT_CHAR_RATE_SLOWDOWN_60:
        settings.setValue(SG_PACING_10MS,   true);
        settings.setValue(SG_PACING_20MS,   true);
        break;
    }

    //Serial # visibility settings...
    settings.setValue(SG_SR_BTN_VISIBLE,    ui->srBtnVisibleCheck->isChecked());
    settings.setValue(SG_SR_USB_VISIBLE,    ui->srUsbVisibleCheck->isChecked());
    settings.setValue(SG_SR_API_VISIBLE,    ui->srApiVisibleCheck->isChecked());

    //Static Password settings...
    settings.setValue(SG_MAN_UPDATE,        ui->manUpdateCheck->isChecked());

    //Logging settings...
    if(ui->logOutputCheck->isChecked()) {
        settings.setValue(SG_LOG_DISABLED,  false);

        QString logFilename = ui->logFileTxt->text().trimmed();
        settings.setValue(SG_LOG_FILENAME,  logFilename);
    } else {
        settings.setValue(SG_LOG_DISABLED,  true);
    }

    //Reload settings
    load();

    showStatusMessage(SETTINGS_SAVED, 0);
}

void SettingPage::restore() {
    //Confirm restore
    //Confirm from client
    switch (
            QMessageBox::question(
                    this, RESTORE_SETTING,
                    WARN_RESTORE_SETTING,
                    "&Yes", "&No", QString::null, 1, 1 ))
    {
    case 0:
        restoreDefaults();
        //Reload settings
        load();
        showStatusMessage(SETTINGS_RESTORED, 0);

        break;
    default:
        break;
    }
}

void SettingPage::on_custPrefixCheck_stateChanged(int state) {
    if(state == 0) {
        ui->custPrefixTxt->setText("0");
        ui->custPrefixTxt->setEnabled(false);
    } else {
        ui->custPrefixTxt->setEnabled(true);
    }
}

void SettingPage::on_custPrefixTxt_editingFinished() {
    int custPrefix = ui->custPrefixTxt->text().toInt();
    ui->custPrefixTxt->setText(QString::number(custPrefix));
}

void SettingPage::on_logOutputCheck_stateChanged(int state) {
    if(state == 0) {
        ui->logFileTxt->setEnabled(false);
        ui->browseBtn->setEnabled(false);
    } else {
        ui->logFileTxt->setEnabled(true);
        ui->browseBtn->setEnabled(true);
    }
}

void SettingPage::on_browseBtn_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Open"),
                                                    QDir(ui->logFileTxt->text()).absolutePath(),
                                                    tr("Comma-separated text file (*.txt *.csv)"));

    if(!fileName.isEmpty()) {
        ui->logFileTxt->setText(fileName);
    }
}
