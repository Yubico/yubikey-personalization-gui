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
    connect(ui->saveBtn, SIGNAL(clicked()),
            this, SLOT(save()));
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
    connect(ui->useNumericKeypadCheck, SIGNAL(clicked()), this, SLOT(save()));

    connect(YubiKeyFinder::getInstance(), SIGNAL(keyFound(bool, bool*)),
            this, SLOT(keyFound(bool, bool*)));
}

SettingPage::~SettingPage() {
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
    connect(ui->configProtectionHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->swapHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    //Set a value for each button
    mapper->setMapping(ui->outFormatHelpBtn, HelpBox::Help_OutputFormat);
    mapper->setMapping(ui->outSpeedHelpBtn, HelpBox::Help_OutputSpeed);
    mapper->setMapping(ui->srVisibilityHelpBtn, HelpBox::Help_SrNoVisibility);
    mapper->setMapping(ui->updateHelpBtn, HelpBox::Help_Updatable);
    mapper->setMapping(ui->configProtectionHelpBtn, HelpBox::Help_ConfigurationProtection);
    mapper->setMapping(ui->swapHelpBtn, HelpBox::Help_Swap);

    //Connect the mapper
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(helpBtn_pressed(int)));
}

void SettingPage::setCurrentPage(int pageIndex) {
    m_currentPage = pageIndex;
    setCurrentIndex(pageIndex);
}

void SettingPage::helpBtn_pressed(int helpIndex) {
    HelpBox help(this);
    help.setHelpIndex((HelpBox::Help)helpIndex);
    help.exec();
}

void SettingPage::init() {
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
    settings.setValue(SG_SR_USB_VISIBLE,        false);
    settings.setValue(SG_SR_API_VISIBLE,        true);

    settings.setValue(SG_MAN_UPDATE,            false);

    settings.setValue(SG_UPDATABLE,             true);
    settings.setValue(SG_FAST_TRIG,             false);
    settings.setValue(SG_USE_NUMERIC_KEYPAD,    false);
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

    //Updatable settings...
    if(settings.contains(SG_UPDATABLE)) {
        ui->updateCheck->setChecked(settings.value(SG_UPDATABLE).toBool());
    } else {
        ui->updateCheck->setChecked(true);
    }

    // extended settings..
    ui->fastTrigCheck->setChecked(settings.value(SG_FAST_TRIG).toBool());
    ui->useNumericKeypadCheck->setChecked(settings.value(SG_USE_NUMERIC_KEYPAD).toBool());

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

    // Updatable settings...
    settings.setValue(SG_UPDATABLE,         ui->updateCheck->isChecked());

    // Extended settings
    settings.setValue(SG_FAST_TRIG, ui->fastTrigCheck->isChecked());
    settings.setValue(SG_USE_NUMERIC_KEYPAD, ui->useNumericKeypadCheck->isChecked());

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
    save();
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
        save();
    }
}

void SettingPage::on_doUpdateBtn_clicked() {
    int slot;

    if(ui->updateSlot1Radio->isChecked()) {
        slot = 1;
    } else if(ui->updateSlot2Radio->isChecked()) {
        slot = 2;
    } else {
      emit showStatusMessage(ERR_CONF_SLOT_NOT_SELECTED, 1);
      return;
    }

    if(m_ykConfig != NULL) {
        delete m_ykConfig;
    }
    m_ykConfig = new YubiKeyConfig();

    m_ykConfig->setProgrammingMode(YubiKeyConfig::Mode_Update);
    m_ykConfig->setConfigSlot(slot);

    // access code
    m_ykConfig->setCurrentAccessCodeTxt(ui->currentAccessCodeTxt->text());
    if(ui->configProtectionCombo->currentIndex() ==
        CONFIG_PROTECTION_DISABLE) {
        m_ykConfig->setNewAccessCodeTxt(ACCESS_CODE_DEFAULT);
    } else {
        m_ykConfig->setNewAccessCodeTxt(ui->newAccessCodeTxt->text());
    }

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

    // access code
    m_ykConfig->setCurrentAccessCodeTxt(ui->currentAccessCodeTxt->text());
    if(ui->configProtectionCombo->currentIndex() ==
        CONFIG_PROTECTION_DISABLE) {
        m_ykConfig->setNewAccessCodeTxt(ACCESS_CODE_DEFAULT);
    } else {
        m_ykConfig->setNewAccessCodeTxt(ui->newAccessCodeTxt->text());
    }

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


void SettingPage::on_configProtectionCombo_currentIndexChanged(int index) {
    switch(index) {
    case CONFIG_PROTECTION_DISABLED:
        ui->currentAccessCodeTxt->clear();
        ui->currentAccessCodeTxt->setEnabled(false);

        ui->newAccessCodeTxt->clear();
        ui->newAccessCodeTxt->setEnabled(false);
        break;
    case CONFIG_PROTECTION_ENABLE:
        ui->currentAccessCodeTxt->clear();
        ui->currentAccessCodeTxt->setEnabled(false);

        on_newAccessCodeTxt_editingFinished();
        ui->newAccessCodeTxt->setEnabled(true);
        break;
    case CONFIG_PROTECTION_DISABLE:
    case CONFIG_PROTECTION_ENABLED:
        on_currentAccessCodeTxt_editingFinished();
        ui->currentAccessCodeTxt->setEnabled(true);

        ui->newAccessCodeTxt->clear();
        ui->newAccessCodeTxt->setEnabled(false);
        break;
    case CONFIG_PROTECTION_CHANGE:
        on_currentAccessCodeTxt_editingFinished();
        ui->currentAccessCodeTxt->setEnabled(true);

        on_newAccessCodeTxt_editingFinished();
        ui->newAccessCodeTxt->setEnabled(true);
        break;
    }
}

void SettingPage::on_currentAccessCodeTxt_editingFinished() {
    QString txt = ui->currentAccessCodeTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)ACC_CODE_SIZE * 2);
    ui->currentAccessCodeTxt->setText(txt);
}

void SettingPage::on_newAccessCodeTxt_editingFinished() {
    QString txt = ui->newAccessCodeTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)ACC_CODE_SIZE * 2);
    ui->newAccessCodeTxt->setText(txt);
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
