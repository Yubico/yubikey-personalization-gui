/*
Copyright (C) 2011-2014 Yubico AB.  All rights reserved.

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
#include "yubikeyfinder.h"
#include "yubikeyutil.h"
#include "yubikeywriter.h"
#include "yubikeylogger.h"
#include "ui_settingpage.h"
#include "ui/helpbox.h"
#include "toolpage.h"

#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QSignalMapper>
#include <QRegExpValidator>

#include "common.h"

#define DECIMAL 0
#define MODHEX 1
#define HEX 2

#define OUT_CHAR_RATE_STD 0
#define OUT_CHAR_RATE_SLOWDOWN_20 1
#define OUT_CHAR_RATE_SLOWDOWN_40 2
#define OUT_CHAR_RATE_SLOWDOWN_60 3

SettingPage::SettingPage(QWidget *parent) :
        QStackedWidget(parent),
        ui(new Ui::SettingPage)
{
    QSignalMapper *mapper = new QSignalMapper(this);
    ui->setupUi(this);

    m_ykConfig = NULL;

    //Connect help buttons
    connectHelpButtons();

    connect(ui->updateBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->updateBackBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->updateBtn, Page_Update);
    mapper->setMapping(ui->updateBackBtn, Page_Base);
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(setCurrentPage(int)));
    m_currentPage = 0;
    setCurrentIndex(Page_Base);

    //Connect other signals and slots
    connect(ui->restoreBtn, SIGNAL(clicked()),
            this, SLOT(restore()));

    // autosave when changing settings
    connect(ui->custPrefixCheck, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->logOutputCheck, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->tabFirstBtn, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->appendTab1Btn, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->appendTab2Btn, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->appendCRBtn, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->appendDelay1Check, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->appendDelay2Check, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->srBtnVisibleCheck, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->srUsbVisibleCheck, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->srApiVisibleCheck, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->manUpdateCheck, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->updateCheck, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->fastTrigCheck, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->ledInvertCheck, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->useNumericKeypadCheck, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->logFormatCombo, SIGNAL(activated(int)), this, SLOT(save()));
    connect(ui->logFormatEdit, SIGNAL(textEdited(QString)), this, SLOT(save()));
    connect(ui->outCharRateCombo, SIGNAL(activated(int)), this, SLOT(save()));
    connect(ui->exportCheck, SIGNAL(clicked()), this, SLOT(save()));

    connect(YubiKeyFinder::getInstance(), SIGNAL(keyFound(bool, bool*, int)),
            this, SLOT(keyFound(bool, bool*)));

    QRegExp modHexRx("^[cbdefghijklnrtuv]{0,4}$");
    ui->custPrefixModhexTxt->setValidator(new QRegExpValidator(modHexRx, this));
    QRegExp decRx("^[0-9]{0,5}$");
    ui->custPrefixDecTxt->setValidator(new QRegExpValidator(decRx, this));
    QRegExp hexRx("^[0-9a-f]{0,4}$");
    ui->custPrefixHexTxt->setValidator(new QRegExpValidator(hexRx, this));
}

SettingPage::~SettingPage() {
    delete ui->custPrefixModhexTxt->validator();
    if(m_ykConfig != NULL) {
        delete m_ykConfig;
    }
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
    connect(ui->updateHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->swapHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->manUpdateHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->logFormatEditHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    //Set a value for each button
    mapper->setMapping(ui->outFormatHelpBtn, HelpBox::Help_OutputFormat);
    mapper->setMapping(ui->outSpeedHelpBtn, HelpBox::Help_OutputSpeed);
    mapper->setMapping(ui->srVisibilityHelpBtn, HelpBox::Help_SrNoVisibility);
    mapper->setMapping(ui->updateHelpBtn, HelpBox::Help_AllowUpdate);
    mapper->setMapping(ui->swapHelpBtn, HelpBox::Help_Swap);
    mapper->setMapping(ui->manUpdateHelpBtn, HelpBox::Help_ManUpdate);
    mapper->setMapping(ui->logFormatEditHelpBtn, HelpBox::Help_LogFormat);

    //Connect the mapper
    connect(mapper, SIGNAL(mapped(int)), this, SIGNAL(showHelp(int)));
    connect(ui->configProtectionBox, SIGNAL(showHelp(int)), this, SIGNAL(showHelp(int)));
}

void SettingPage::setCurrentPage(int pageIndex) {
    m_currentPage = pageIndex;
    setCurrentIndex(pageIndex);
}

void SettingPage::init() {
    load();
}

void SettingPage::restoreDefaults() {
    QSettings settings;

    settings.setValue(SG_CUSTOMER_PREFIX_USED,  false);
    settings.setValue(SG_CUSTOMER_PREFIX,       0);

    settings.setValue(SG_LOG_DISABLED,          false);
    // we continue loading LOG_FILENAME since we use that to check if settings are loaded
    settings.setValue(SG_LOG_FILENAME,          YubiKeyLogger::defaultLogFilename());
    settings.setValue(SG_LOG_FORMAT,            YubiKeyLogger::Format_Traditional);

    settings.setValue(SG_EXPORT_FILENAME,       YubiKeyWriter::defaultExportFilename());
    settings.setValue(SG_IMPORT_FILENAME,       ToolPage::defaultImportFilename());

    settings.setValue(SG_TAB_FIRST,             false);
    settings.setValue(SG_APPEND_TAB1,           false);
    settings.setValue(SG_APPEND_TAB2,           false);
    settings.setValue(SG_APPEND_CR,             true);
    settings.setValue(SG_APPEND_DELAY1,         false);
    settings.setValue(SG_APPEND_DELAY2,         false);
    settings.setValue(SG_PACING_10MS,           false);
    settings.setValue(SG_PACING_20MS,           false);

    settings.setValue(SG_SR_BTN_VISIBLE,        true);
    settings.setValue(SG_SR_USB_VISIBLE,        false);
    settings.setValue(SG_SR_API_VISIBLE,        true);

    settings.setValue(SG_MAN_UPDATE,            false);

    settings.setValue(SG_ALLOW_UPDATE,          true);
    settings.setValue(SG_FAST_TRIG,             false);
    settings.setValue(SG_USE_NUMERIC_KEYPAD,    false);

    settings.setValue(SG_OATH_HOTP8,            false);
    settings.setValue(SG_REQUIRE_INPUT,         false);
    settings.setValue(SG_HMAC_LT64,             true);

    settings.setValue(SG_STRONG_PW1,            false);
    settings.setValue(SG_STRONG_PW2,            false);
    settings.setValue(SG_STRONG_PW3,            false);

    settings.setValue(SG_EXPORT_PREFERENCE,     false);
}

void SettingPage::load() {
    QSettings settings;

    QString logFilename = settings.value(SG_LOG_FILENAME).toString();
    bool logDisabled = settings.value(SG_LOG_DISABLED).toBool();
    int logFormat = settings.value(SG_LOG_FORMAT).toInt();
    if(logFilename.isEmpty()) {
        //This is the first time... set defaults
        restoreDefaults();
    }

    YubiKeyLogger::setLogFilename(logFilename);

    QString exportFilename = settings.value(SG_EXPORT_FILENAME).toString();
    if(!exportFilename.isEmpty()) {
        YubiKeyWriter::setExportFilename(exportFilename);
    }

    QString importFilename = settings.value(SG_IMPORT_FILENAME).toString();
    if(!importFilename.isEmpty()) {
        ToolPage::setImportFilename(importFilename);
    }

    //General settings...
    //Customer prefix
    bool custPrefixUsed = settings.value(SG_CUSTOMER_PREFIX_USED).toBool();
    if(custPrefixUsed) {
        ui->custPrefixCheck->setChecked(true);

        int custPrefix = settings.value(SG_CUSTOMER_PREFIX).toInt();
        if(custPrefix > 0) {
            custPrefixChanged(DECIMAL, QString::number(custPrefix));
            ui->custPrefixDecTxt->setEnabled(true);
            ui->custPrefixModhexTxt->setEnabled(true);
            ui->custPrefixHexTxt->setEnabled(true);
        }
    } else {
        ui->custPrefixCheck->setChecked(false);

        ui->custPrefixDecTxt->clear();
        ui->custPrefixDecTxt->setEnabled(false);
        ui->custPrefixModhexTxt->clear();
        ui->custPrefixModhexTxt->setEnabled(false);
        ui->custPrefixHexTxt->clear();
        ui->custPrefixHexTxt->setEnabled(false);
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

    //Updatable settings...
    if(settings.contains(SG_ALLOW_UPDATE)) {
        ui->updateCheck->setChecked(settings.value(SG_ALLOW_UPDATE).toBool());
    } else {
        ui->updateCheck->setChecked(true);
    }

    // extended settings..
    ui->fastTrigCheck->setChecked(settings.value(SG_FAST_TRIG).toBool());
    ui->useNumericKeypadCheck->setChecked(settings.value(SG_USE_NUMERIC_KEYPAD).toBool());
    ui->ledInvertCheck->setChecked(settings.value(SG_LED_INVERT).toBool());

    //Logging settings...
    if(logDisabled) {
        YubiKeyLogger::disableLogging();

        ui->logOutputCheck->setChecked(false);
        ui->logFormatCombo->setEnabled(false);
        ui->logFormatEdit->setEnabled(false);
    } else {
        YubiKeyLogger::enableLogging();

        ui->logOutputCheck->setChecked(true);
        ui->logFormatCombo->setEnabled(true);
        ui->logFormatCombo->setCurrentIndex(logFormat);
        YubiKeyLogger::setLogFormat((YubiKeyLogger::Format)logFormat);
        if(logFormat == YubiKeyLogger::Format_Flexible) {
            QString format = settings.value(SG_LOG_FLEXIBLE).toString();
            ui->logFormatEdit->setEnabled(true);
            ui->logFormatEdit->setText(format);
            YubiKeyLogger::setFlexibleFormat(format);
        } else {
            ui->logFormatEdit->setEnabled(false);
        }
    }

    ui->exportCheck->setChecked(settings.value(SG_EXPORT_PREFERENCE).toBool());

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
        custPrefix = ui->custPrefixDecTxt->text().toInt();
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

    // Updatable settings...
    settings.setValue(SG_ALLOW_UPDATE, ui->updateCheck->isChecked());

    // Extended settings
    settings.setValue(SG_FAST_TRIG, ui->fastTrigCheck->isChecked());
    settings.setValue(SG_USE_NUMERIC_KEYPAD, ui->useNumericKeypadCheck->isChecked());
    settings.setValue(SG_LED_INVERT, ui->ledInvertCheck->isChecked());

    //Logging settings...
    if(ui->logOutputCheck->isChecked()) {
        settings.setValue(SG_LOG_DISABLED,  false);
        settings.setValue(SG_LOG_FORMAT, ui->logFormatCombo->currentIndex());
        if(ui->logFormatCombo->currentIndex() == YubiKeyLogger::Format_Flexible) {
            settings.setValue(SG_LOG_FLEXIBLE, ui->logFormatEdit->text());
        }
    } else {
        settings.setValue(SG_LOG_DISABLED,  true);
    }

    settings.setValue(SG_EXPORT_PREFERENCE, ui->exportCheck->isChecked());

    //Reload settings
    load();

    showStatusMessage(tr(SETTINGS_SAVED), 0);
}

void SettingPage::restore() {
    //Confirm restore
    //Confirm from client
    switch (
            QMessageBox::question(
                    this, tr(RESTORE_SETTING),
                    tr(WARN_RESTORE_SETTING),
                    tr("&Yes"), tr("&No"), QString::null, 1, 1 ))
    {
    case 0:
        restoreDefaults();
        //Reload settings
        load();
        showStatusMessage(tr(SETTINGS_RESTORED), 0);

        break;
    default:
        break;
    }
}

void SettingPage::on_custPrefixCheck_stateChanged(int state) {
    if(state == 0) {
        ui->custPrefixDecTxt->setEnabled(false);
        ui->custPrefixModhexTxt->setEnabled(false);
        ui->custPrefixHexTxt->setEnabled(false);
    } else {
        ui->custPrefixDecTxt->setEnabled(true);
        ui->custPrefixModhexTxt->setEnabled(true);
        ui->custPrefixHexTxt->setEnabled(true);
    }
}

void SettingPage::on_custPrefixDecTxt_editingFinished() {
    custPrefixChanged(DECIMAL, ui->custPrefixDecTxt->text());
    save();
}

void SettingPage::on_custPrefixModhexTxt_editingFinished() {
    custPrefixChanged(MODHEX, ui->custPrefixModhexTxt->text());
    save();
}

void SettingPage::on_custPrefixHexTxt_editingFinished() {
    custPrefixChanged(HEX, ui->custPrefixHexTxt->text());
    save();
}

void SettingPage::on_doUpdateBtn_clicked() {
    int slot;

    if(ui->updateSlot1Radio->isChecked()) {
        slot = 1;
    } else if(ui->updateSlot2Radio->isChecked()) {
        slot = 2;
    } else {
      emit showStatusMessage(tr(ERR_CONF_SLOT_NOT_SELECTED), 1);
      return;
    }

    if(m_ykConfig != NULL) {
        delete m_ykConfig;
    }
    m_ykConfig = new YubiKeyConfig();

    m_ykConfig->setProgrammingMode(YubiKeyConfig::Mode_Update);
    m_ykConfig->setConfigSlot(slot);

    // set serial
    m_ykConfig->setSerial(QString::number(YubiKeyFinder::getInstance()->serial()));

    // access code
    m_ykConfig->setCurrentAccessCodeTxt(
        ui->configProtectionBox->currentAccessCode());
    m_ykConfig->setNewAccessCodeTxt(
        ui->configProtectionBox->newAccessCode(),
        ui->configProtectionBox->newAccMode());

    if(ui->updateDormantCheck->isChecked()) {
        m_ykConfig->setDormant(true);
    }

    //Write
    connect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
            this, SLOT(updateConfigWritten(bool, const QString &)));

    YubiKeyWriter::getInstance()->writeConfig(m_ykConfig);
}

void SettingPage::updateConfigWritten(bool written, const QString &msg) {
    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
        this, SLOT(updateConfigWritten(bool, const QString &)));

    if(written) {
        qDebug() << "Configuration updated." << msg;
        emit showStatusMessage(tr("Configuration successfully updated.", 0));
    } else {
        qDebug() << "Failed update.";
        emit showStatusMessage(msg, 1);;
    }

}

void SettingPage::on_swapBtn_clicked() {
    if(m_ykConfig != NULL) {
        delete m_ykConfig;
    }
    m_ykConfig = new YubiKeyConfig();

    m_ykConfig->setProgrammingMode(YubiKeyConfig::Mode_Swap);

    // set serial
    m_ykConfig->setSerial(QString::number(YubiKeyFinder::getInstance()->serial()));

    // access code
    m_ykConfig->setCurrentAccessCodeTxt(
        ui->configProtectionBox->currentAccessCode());
    m_ykConfig->setNewAccessCodeTxt(
        ui->configProtectionBox->newAccessCode(),
        ui->configProtectionBox->newAccMode());

    //Write
    connect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
            this, SLOT(swapWritten(bool, const QString &)));

    YubiKeyWriter::getInstance()->writeConfig(m_ykConfig);
}

void SettingPage::swapWritten(bool written, const QString &msg) {
    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
        this, SLOT(swapWritten(bool, const QString &)));

    if(written) {
        qDebug() << "Configurations swapped." << msg;
        emit showStatusMessage(tr("Configuration successfully swapped.", 0));
    } else {
        qDebug() << "Failed swapping." << msg;
        emit showStatusMessage(msg, 1);;
    }
}

void SettingPage::keyFound(bool found, bool* featuresMatrix) {
    if(found) {
        if(featuresMatrix[YubiKeyFinder::Feature_Updatable]) {
            ui->updateBtn->setEnabled(true);
        }
    } else {
        ui->updateBtn->setEnabled(false);
    }
}

void SettingPage::custPrefixChanged(int type, QString src) {
    unsigned char buf[16];
    memset(buf, 0, sizeof(buf));
    size_t bufLen = 0;

    switch(type) {
        // decimal
        case DECIMAL:
            {
                QString tmp = QString::number(src.toULongLong(), 16);
                size_t len = tmp.length();
                if(len % 2 != 0) {
                    len++;
                }
                YubiKeyUtil::qstrClean(&tmp, (size_t)len, true);
                YubiKeyUtil::qstrHexDecode(buf, &bufLen, tmp);
                break;
            }
        // modhex
        case MODHEX:
            YubiKeyUtil::qstrModhexDecode(buf, &bufLen, src);
            break;
        // hex
        case HEX:
            YubiKeyUtil::qstrHexDecode(buf, &bufLen, src);
            break;
    }
    QString hex = YubiKeyUtil::qstrHexEncode(buf, bufLen);
    QString modhex = YubiKeyUtil::qstrModhexEncode(buf, bufLen);
    bool ok = false;
    qulonglong dec = hex.toULongLong(&ok, 16);
    if(dec > 0) {
        ui->custPrefixDecTxt->setText(QString::number(dec));
        ui->custPrefixModhexTxt->setText(modhex);
        ui->custPrefixHexTxt->setText(hex);
    }
}

void SettingPage::reloadSettings() {
    load();
}
