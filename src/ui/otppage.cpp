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

#include "otppage.h"
#include "yubikeywriter.h"
#include "yubikeyutil.h"
#include "yubikeyfinder.h"
#include "ui_otppage.h"
#include "ui/helpbox.h"
#include "ui/confirmbox.h"

#include <QDesktopServices>
#include <QDateTime>
#include <QDebug>
#include <QSignalMapper>
#include <QSettings>
#include <QUrl>
#include <QDir>
#include <QTableWidgetItem>

#include "common.h"

OtpPage::OtpPage(QWidget *parent) :
        QStackedWidget(parent),
        ui(new Ui::OtpPage)
{
    ui->setupUi(this);

    m_customerPrefix = -1;
    m_ykConfig = 0;
    m_keyPresent = false;
    clearState();

    //Connect pages
    connectPages();

    //Connect help buttons
    connectHelpButtons();

    //Connect other signals and slots
    connect(YubiKeyFinder::getInstance(), SIGNAL(keyFound(bool, bool*, int)),
            this, SLOT(keyFound(bool, bool*)));

    connect(ui->quickWriteConfigBtn, SIGNAL(clicked()),
            this, SLOT(writeQuickConfig()));
    connect(ui->quickUploadBtn, SIGNAL(clicked()),
            this, SLOT(uploadQuickConfig()));
    connect(ui->advResetBtn, SIGNAL(clicked()),
            this, SLOT(resetAdvPage()));

    //Load settings
    loadSettings();

    ui->advResultsWidget->resizeColumnsToContents();
}

OtpPage::~OtpPage() {
    if(m_ykConfig != 0) {
        delete m_ykConfig;
        m_ykConfig = 0;
    }
    delete ui;
}

/*
 Common
*/

void OtpPage::connectPages() {
    //Map the values of the navigation buttons with the indexes of
    //the stacked widget

    //Create a QMapper
    QSignalMapper *mapper = new QSignalMapper(this);

    //Connect the clicked signal with the QSignalMapper
    connect(ui->quickBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->quickBackBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    //connect(ui->quickUploadBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    connect(ui->advBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advBackBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    //Set a value for each button
    mapper->setMapping(ui->quickBtn, Page_Quick);
    mapper->setMapping(ui->quickBackBtn, Page_Base);
    //mapper->setMapping(ui->quickUploadBtn, Page_Upload);

    mapper->setMapping(ui->advBtn, Page_Advanced);
    mapper->setMapping(ui->advBackBtn, Page_Base);

    //Connect the mapper to the widget
    //The mapper will set a value to each button and
    //set that value to the widget
    //connect(pageMapper, SIGNAL(mapped(int)), this, SLOT(setCurrentIndex(int)));
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(setCurrentPage(int)));

    //Set the current page
    m_currentPage = 0;
    setCurrentIndex(Page_Base);
}

void OtpPage::setCurrentPage(int pageIndex) {
    //Page changed...

    m_currentPage = pageIndex;

    switch(pageIndex){
    case Page_Quick:
        resetQuickPage();
        break;
    case Page_Advanced:
        resetAdvPage();
        break;
    }

    setCurrentIndex(pageIndex);

    //Clear state
    m_keysProgrammedCtr = 0;
    clearState();
}

void OtpPage::connectHelpButtons() {
    //Map the values of the help buttons

    //Create a QMapper
    QSignalMapper *mapper = new QSignalMapper(this);

    //Connect the clicked signal with the QSignalMapper
    connect(ui->quickConfigHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->quickPubIdHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->quickPvtIdHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->quickSecretKeyHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    connect(ui->advConfigHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advParamGenSchemeHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advPubIdHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advPvtIdHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advSecretKeyHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    //Set a value for each button
    mapper->setMapping(ui->quickConfigHelpBtn, HelpBox::Help_ConfigurationSlot);
    mapper->setMapping(ui->quickPubIdHelpBtn, HelpBox::Help_PublicID);
    mapper->setMapping(ui->quickPvtIdHelpBtn, HelpBox::Help_PrivateID);
    mapper->setMapping(ui->quickSecretKeyHelpBtn, HelpBox::Help_SecretKey);

    mapper->setMapping(ui->advConfigHelpBtn, HelpBox::Help_ConfigurationSlot);
    mapper->setMapping(ui->advParamGenSchemeHelpBtn, HelpBox::Help_ParameterGeneration);
    mapper->setMapping(ui->advPubIdHelpBtn, HelpBox::Help_PublicID);
    mapper->setMapping(ui->advPvtIdHelpBtn, HelpBox::Help_PrivateID);
    mapper->setMapping(ui->advSecretKeyHelpBtn, HelpBox::Help_SecretKey);

    //Connect the mapper
    connect(mapper, SIGNAL(mapped(int)), this, SIGNAL(showHelp(int)));
    connect(ui->advConfigProtectionBox, SIGNAL(showHelp(int)), this, SIGNAL(showHelp(int)));
}

void OtpPage::keyFound(bool found, bool* featuresMatrix) {
    if(found) {
        if(m_state == State_Initial) {
            ui->quickWriteConfigBtn->setEnabled(true);
            ui->advWriteConfigBtn->setEnabled(true);
            ui->advExportConfigBtn->setEnabled(true);

            if(!featuresMatrix[YubiKeyFinder::Feature_MultipleConfigurations]) {
                ui->quickConfigSlot2Radio->setEnabled(false);
                ui->advConfigSlot2Radio->setEnabled(false);
            } else {
                ui->quickConfigSlot2Radio->setEnabled(true);
                ui->advConfigSlot2Radio->setEnabled(true);
            }
        } else if(this->currentIndex() == Page_Advanced &&
                  m_state >= State_Programming_Multiple &&
                  m_keyPresent == false) {
            if(m_keysProgrammedCtr > 0 && !m_ready) {
                changeAdvConfigParams();
            }
            if(m_state == State_Programming_Multiple) {
                ui->advWriteConfigBtn->setEnabled(true);
                ui->advExportConfigBtn->setEnabled(true);
            } else {
                writeAdvConfig(WRITE_CONFIG);
            }
        }
        m_keyPresent = true;
    } else {
        ui->quickWriteConfigBtn->setEnabled(false);
        ui->advWriteConfigBtn->setEnabled(false);
        ui->advExportConfigBtn->setEnabled(false);

        m_keyPresent = false;

        if(m_state == State_Initial) {
            ui->quickConfigSlot2Radio->setEnabled(true);
            ui->advConfigSlot2Radio->setEnabled(true);
        }
    }
}

void OtpPage::clearState() {
    m_state = State_Initial;
    m_ready = true;

    if(m_ykConfig != 0) {
        delete m_ykConfig;
        m_ykConfig = 0;
    }
}

void OtpPage::loadSettings() {
    QSettings settings;
    m_customerPrefix = settings.value(SG_CUSTOMER_PREFIX).toInt();

    if(m_customerPrefix > 0) {
        ui->advPubIdLenBox->setValue(PUBLIC_ID_DEFAULT_SIZE);
        ui->advPubIdLenBox->setEnabled(false);

        //Build Public Identity prefix based on customer prefix
        QByteArray prefix;

        // As per old tool
        prefix.resize(2);
        prefix[0] = (unsigned char) (m_customerPrefix >> 8U);
        prefix[1] = (unsigned char) m_customerPrefix;

        char pubIdPrefix[prefix.size() * 2 + 1];
        size_t pubIdPrefixLen = 0;
        memset(&pubIdPrefix, 0, sizeof(pubIdPrefix));

        YubiKeyUtil::hexModhexEncode(pubIdPrefix, &pubIdPrefixLen,
                                     (unsigned char *)prefix.data(), prefix.size(),
                                     true);

        m_pubIdPrefix = QString(pubIdPrefix);

        if(m_currentPage == Page_Advanced) {
            on_advPubIdTxt_editingFinished();
        }
    } else {
        ui->advPubIdLenBox->setEnabled(true);

        m_pubIdPrefix = QString("");
    }

    ui->advExportConfigBtn->setVisible(settings.value(SG_EXPORT_PREFERENCE).toBool());
}

/*
 Quick Page handling
*/
void OtpPage::resetQuickPage() {
    if(ui->quickConfigSlot1Radio->isChecked()) {
        ui->quickConfigSlot2Radio->setChecked(true);
    }

    on_quickResetBtn_clicked();

    ui->quickUploadBtn->setEnabled(false);
}

void OtpPage::on_quickResetBtn_clicked() {
    QString pubIdTxt = YubiKeyUtil::generateRandomModhex(PUBLIC_ID_DEFAULT_SIZE * 2);
    pubIdTxt.replace(0, 2, YUBICO_OTP_SERVER_PUBLIC_ID_PREFIX);

    ui->quickPubIdTxt->setText(pubIdTxt);
    on_quickPubIdTxt_editingFinished();

    ui->quickPvtIdTxt->setText(
            YubiKeyUtil::generateRandomHex((size_t)UID_SIZE * 2));
    on_quickPvtIdTxt_editingFinished();

    ui->quickSecretKeyTxt->setText(
            YubiKeyUtil::generateRandomHex((size_t)KEY_SIZE * 2));
    on_quickSecretKeyTxt_editingFinished();
}

void OtpPage::on_quickHideParams_clicked(bool checked) {
    if(checked) {
        ui->quickPvtIdTxt->setEchoMode(QLineEdit::Password);
        ui->quickSecretKeyTxt->setEchoMode(QLineEdit::Password);
    } else {
        ui->quickPvtIdTxt->setEchoMode(QLineEdit::Normal);
        ui->quickSecretKeyTxt->setEchoMode(QLineEdit::Normal);
    }
}

void OtpPage::on_quickPubIdTxt_editingFinished() {
    QString txt = ui->quickPubIdTxt->text();
    YubiKeyUtil::qstrModhexClean(&txt, (size_t)PUBLIC_ID_DEFAULT_SIZE * 2);
    ui->quickPubIdTxt->setText(txt);
}

void OtpPage::on_quickPvtIdTxt_editingFinished() {
    QString txt = ui->quickPvtIdTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)UID_SIZE * 2);
    ui->quickPvtIdTxt->setText(txt);
}

void OtpPage::on_quickSecretKeyTxt_editingFinished() {
    QString txt = ui->quickSecretKeyTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)KEY_SIZE * 2);
    ui->quickSecretKeyTxt->setText(txt);
}

bool OtpPage::validateQuickSettings() {
    if(!(ui->quickConfigSlot1Radio->isChecked() ||
         ui->quickConfigSlot2Radio->isChecked())) {
        emit showStatusMessage(tr(ERR_CONF_SLOT_NOT_SELECTED), 1);
        return false;
    }

    QSettings settings;

    //Check if configuration slot 1 is being programmed
    if (!settings.value(SG_OVERWRITE_CONF_SLOT1).toBool() &&
        ui->quickConfigSlot1Radio->isChecked()) {
        //Confirm from client
        ConfirmBox confirm(this);
        confirm.setConfirmIndex(ConfirmBox::Confirm_ConfigurationSlot);
        int ret = confirm.exec();

        switch (ret) {
        case 1:     //Yes
            break;
        default:    //No
            return false;
        }
    }
    return true;
}

void OtpPage::writeQuickConfig() {
    //Clear status
    emit showStatusMessage(NULL, -1);

    //Validate settings
    if(!validateQuickSettings()) {
        return;
    }

    //
    m_state = State_Programming;

    //Write configuration
    ui->quickWriteConfigBtn->setEnabled(false);
    ui->quickUploadBtn->setEnabled(false);
    ui->quickBackBtn->setEnabled(false);

    if(m_ykConfig != 0) {
        delete m_ykConfig;
        m_ykConfig = 0;
    }
    m_ykConfig = new YubiKeyConfig();

    //Programming mode...
    m_ykConfig->setProgrammingMode(YubiKeyConfig::Mode_YubicoOtp);

    // set serial
    m_ykConfig->setSerial(QString::number(YubiKeyFinder::getInstance()->serial()));

    //Configuration slot...
    int configSlot = 1;
    if(ui->quickConfigSlot2Radio->isChecked()) {
        configSlot = 2;
    }
    m_ykConfig->setConfigSlot(configSlot);

    //Public ID...
    m_ykConfig->setPubIdTxt(ui->quickPubIdTxt->text());

    //Private ID...
    m_ykConfig->setPvtIdTxt(ui->quickPvtIdTxt->text());

    //Secret Key...
    m_ykConfig->setSecretKeyTxt(ui->quickSecretKeyTxt->text());

    //Prepare upload url
    m_uploadUrl.clear();
    unsigned int serial = YubiKeyFinder::getInstance()->serial();
    m_uploadUrl = tr(UPLOAD_URL).
                  arg(serial == 0? "": QString::number(serial)).
                  arg(m_ykConfig->pubIdTxt()).
                  arg(m_ykConfig->pvtIdTxt()).
                  arg(m_ykConfig->secretKeyTxt());

    //Write
    connect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
            this, SLOT(quickConfigWritten(bool, const QString &)));

    YubiKeyWriter::getInstance()->writeConfig(m_ykConfig);
}

void OtpPage::quickConfigWritten(bool written, __attribute__((unused)) const QString &msg) {
    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
               this, SLOT(quickConfigWritten(bool, const QString &)));

    ui->quickWriteConfigBtn->setEnabled(true);
    ui->quickBackBtn->setEnabled(true);

    if(written && m_ykConfig != 0) {
        ui->quickUploadBtn->setEnabled(true);

        QString keyDetail = tr(" (Public ID: %1)").arg(m_ykConfig->pubIdTxt());
        showStatusMessage(tr(KEY_CONFIGURED).arg(keyDetail), 0);
    } else {
        m_uploadUrl.clear();
        ui->quickUploadBtn->setEnabled(false);
    }

    clearState();
}

void OtpPage::uploadQuickConfig() {
    m_tempFile.setFileTemplate(QDir::tempPath() + "/ykpers.XXXXXX.html");
    m_tempFile.open();
    m_tempFile.write(m_uploadUrl.toLatin1(), m_uploadUrl.length());
    m_tempFile.close();
    qDebug() << "temporary file for upload is: " << m_tempFile.fileName();
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_tempFile.fileName()));
}

/*
 Advanced Page handling
*/

void OtpPage::resetAdvPage() {
    if(ui->advConfigSlot1Radio->isChecked()) {
        ui->advConfigSlot2Radio->setChecked(true);
    }

    ui->advConfigParamsCombo->setCurrentIndex(2);
    ui->advAutoProgramKeysCheck->setChecked(false);
    ui->advProgramMulKeysBox->setChecked(false);

    ui->advConfigProtectionBox->reset();

    ui->advPubIdCheck->setChecked(true);
    ui->advPubIdTxt->clear();
    set_advPubId_default();
    on_advPubIdTxt_editingFinished();
    ui->advPubIdLenBox->setValue(PUBLIC_ID_DEFAULT_SIZE);
    if(m_customerPrefix > 0) {
        ui->advPubIdLenBox->setEnabled(false);
    } else {
        ui->advPubIdLenBox->setEnabled(true);
    }

    ui->advPvtIdCheck->setChecked(true);
    ui->advPvtIdTxt->clear();
    on_advPvtIdTxt_editingFinished();

    ui->advSecretKeyTxt->clear();
    on_advSecretKeyTxt_editingFinished();

    ui->advStopBtn->setEnabled(false);
}

void OtpPage::freezeAdvPage(bool freeze) {
    bool disable = !freeze;
    ui->advConfigBox->setEnabled(disable);
    ui->advProgramMulKeysBox->setEnabled(disable);
    ui->advConfigProtectionBox->setEnabled(disable);
    ui->advKeyParamsBox->setEnabled(disable);

    ui->advWriteConfigBtn->setEnabled(disable);
    ui->advExportConfigBtn->setEnabled(disable);
    ui->advStopBtn->setEnabled(!disable);
    ui->advBackBtn->setEnabled(disable);
}

void OtpPage::on_advProgramMulKeysBox_clicked(bool checked) {
    if(checked) {
        changeAdvConfigParams();
    }
}


void OtpPage::on_advPubIdCheck_stateChanged(int state) {
    bool disable = (state != 0);
    ui->advPubIdTxt->setEnabled(disable);
    ui->advPubIdGenerateBtn->setEnabled(disable);
    if(m_customerPrefix <= 0) {
        ui->advPubIdLenBox->setEnabled(disable);
    }
}

void OtpPage::set_advPubId_default() {
    QString txt = "cccccc";
    unsigned int serial = YubiKeyFinder::getInstance()->serial();

    //Convert serial number to modhex
    unsigned char buf[16];
    memset(buf, 0, sizeof(buf));
    size_t bufLen = 0;

    QString tmp = QString::number(serial, 16);
    size_t len = tmp.length();
    if(len % 2 != 0) {
      len++;
    }
    YubiKeyUtil::qstrClean(&tmp, (size_t)len, true);
    YubiKeyUtil::qstrHexDecode(buf, &bufLen, tmp);

    txt.append(YubiKeyUtil::qstrModhexEncode(buf, bufLen));
    ui->advPubIdTxt->setText(txt);
    on_advPubIdTxt_editingFinished();
}

void OtpPage::on_advPubIdTxt_editingFinished() {
    QString txt = ui->advPubIdTxt->text();
    int len = ui->advPubIdLenBox->value() * 2;
    YubiKeyUtil::qstrModhexClean(&txt, (size_t)len);

    if(m_customerPrefix > 0) {
        txt.replace(0, 4, m_pubIdPrefix);
    }
    ui->advPubIdTxt->setText(txt);
    len = txt.length();
    ui->advPubIdTxt->setCursorPosition(len + len/2);
}

void OtpPage::on_advPubIdLenBox_valueChanged(int value) {
    if(value > FIXED_SIZE) {
        ui->advPubIdLenBox->setValue(PUBLIC_ID_DEFAULT_SIZE);
    }
    on_advPubIdTxt_editingFinished();
}

void OtpPage::on_advPubIdGenerateBtn_clicked() {
    int len = ui->advPubIdLenBox->value();
    QString txt = YubiKeyUtil::generateRandomModhex((size_t)len * 2);
    ui->advPubIdTxt->setText(txt);
    on_advPubIdTxt_editingFinished();
}

void OtpPage::on_advPvtIdCheck_stateChanged(int state) {
    bool disable = (state != 0);
    ui->advPvtIdTxt->setEnabled(disable);
    ui->advPvtIdGenerateBtn->setEnabled(disable);
}

void OtpPage::on_advPvtIdTxt_editingFinished() {
    QString txt = ui->advPvtIdTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)UID_SIZE * 2);
    ui->advPvtIdTxt->setText(txt);
}

void OtpPage::on_advPvtIdGenerateBtn_clicked() {
    ui->advPvtIdTxt->setText(
            YubiKeyUtil::generateRandomHex((size_t)UID_SIZE * 2));
}

void OtpPage::on_advSecretKeyTxt_editingFinished() {
    QString txt = ui->advSecretKeyTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)KEY_SIZE * 2);
    ui->advSecretKeyTxt->setText(txt);
}

void OtpPage::on_advSecretKeyGenerateBtn_clicked() {
    ui->advSecretKeyTxt->setText(
            YubiKeyUtil::generateRandomHex((size_t)KEY_SIZE * 2));
}

void OtpPage::on_advExportConfigBtn_clicked() {
    qDebug() << "foo";
    //Validate settings
    if(!validateAdvSettings()) {
        return;
    }

    clearState();

    freezeAdvPage(true);

    writeAdvConfig(EXPORT_CONFIG);
}

void OtpPage::on_advWriteConfigBtn_clicked() {
    emit showStatusMessage(NULL, -1);

    if(!ui->advProgramMulKeysBox->isChecked()) {
        m_keysProgrammedCtr = 0;
    }

    //Validate settings
    if(!validateAdvSettings()) {
        return;
    }

    clearState();

    freezeAdvPage(true);

    // Change state
    if(ui->advProgramMulKeysBox->isChecked()) {
        if(ui->advAutoProgramKeysCheck->isChecked()) {
            m_keysProgrammedCtr = 0;
            m_state = State_Programming_Multiple_Auto;
        } else {
            m_state = State_Programming_Multiple;
        }
    } else {
        m_keysProgrammedCtr = 0;
        m_state = State_Programming;
    }

    writeAdvConfig(WRITE_CONFIG);
}

void OtpPage::on_advStopBtn_clicked() {
    ui->advStopBtn->setEnabled(false);
    m_state = State_Initial;
    stopAdvConfigWritting();
}

void OtpPage::stopAdvConfigWritting() {
    qDebug() << "Stopping adv configuration writing...";

    if(m_state >= State_Programming_Multiple) {
        ui->advStopBtn->setEnabled(true);
        return;
    }

    m_keysProgrammedCtr = 0;
    clearState();

    freezeAdvPage(false);
}

void OtpPage::changeAdvConfigParams() {
    qDebug() << "Changing adv configuration params...";

    int index = ui->advConfigParamsCombo->currentIndex();
    int idScheme = GEN_SCHEME_FIXED;
    int secretScheme = GEN_SCHEME_FIXED;

    switch(index) {
    case SCHEME_INCR_ID_RAND_SECRET:
        //Increment IDs only if in programming mode
        if(m_state != State_Initial) {
            idScheme = GEN_SCHEME_INCR;
        }
        secretScheme = GEN_SCHEME_RAND;
        break;

    case SCHEME_RAND_ALL:
        idScheme = GEN_SCHEME_RAND;
        secretScheme = GEN_SCHEME_RAND;
        break;

    case SCHEME_ID_FROM_SERIAL_RAND_SECRET:
        idScheme = GEN_SCHEME_SERIAL;
        secretScheme = GEN_SCHEME_RAND;
        break;
    }

    //Public ID...
    if(ui->advPubIdCheck->isChecked()) {
        if(idScheme == GEN_SCHEME_SERIAL) {
            set_advPubId_default();
        } else {
            QString pubIdTxt = YubiKeyUtil::getNextModhex(
                ui->advPubIdLenBox->value() * 2,
                ui->advPubIdTxt->text(), idScheme);

            ui->advPubIdTxt->setText(pubIdTxt);
            on_advPubIdTxt_editingFinished();
        }
    }

    //Private ID...
    if(ui->advPvtIdCheck->isChecked()) {
        QString pvtIdTxt = YubiKeyUtil::getNextHex(
                UID_SIZE * 2,
                ui->advPvtIdTxt->text(), secretScheme);
        ui->advPvtIdTxt->setText(pvtIdTxt);
    }

    //Secret Key...
    QString secretKeyTxt = YubiKeyUtil::getNextHex(
            KEY_SIZE * 2,
            ui->advSecretKeyTxt->text(), secretScheme);
    ui->advSecretKeyTxt->setText(secretKeyTxt);

    m_ready = true;
}

bool OtpPage::validateAdvSettings() {
    if(!(ui->advConfigSlot1Radio->isChecked() ||
         ui->advConfigSlot2Radio->isChecked())) {
        emit showStatusMessage(tr(ERR_CONF_SLOT_NOT_SELECTED), 1);
        return false;
    }

    QSettings settings;

    //Check if configuration slot 1 is being programmed
    if (!settings.value(SG_OVERWRITE_CONF_SLOT1).toBool() &&
        ui->advConfigSlot1Radio->isChecked() &&
        m_keysProgrammedCtr == 0) {
        //Confirm from client
        ConfirmBox confirm(this);
        confirm.setConfirmIndex(ConfirmBox::Confirm_ConfigurationSlot);
        int ret = confirm.exec();

        switch (ret) {
        case 1:     //Yes
            break;
        default:    //No
            return false;
        }
    }


    //Check if public id length is 6
    if (!settings.value(SG_DIFF_PUBLIC_ID_LEN).toBool() &&
        ui->advPubIdLenBox->value() != PUBLIC_ID_DEFAULT_SIZE &&
        m_keysProgrammedCtr == 0) {
        //Confirm from client
        ConfirmBox confirm(this);
        confirm.setConfirmIndex(ConfirmBox::Confirm_PublicID);
        int ret = confirm.exec();

        switch (ret) {
        case 1:     //Yes
            break;
        default:    //No
            return false;
        }
    }

    if(!ui->advConfigProtectionBox->checkConfirm()) {
        return false;
    }

    return true;
}

void OtpPage::writeAdvConfig(int mode) {
    qDebug() << "Writing configuration...";

    //Disable stop button while configuration is being written
    ui->advStopBtn->setEnabled(false);

    //Write configuration
    if(m_ykConfig != 0) {
        qDebug() << "ykConfig destroyed";
        delete m_ykConfig;
        m_ykConfig = 0;
    }
    m_ykConfig = new YubiKeyConfig();

    //Programming mode...
    m_ykConfig->setProgrammingMode(YubiKeyConfig::Mode_YubicoOtp);

    // set serial
    m_ykConfig->setSerial(QString::number(YubiKeyFinder::getInstance()->serial()));

    //Configuration slot...
    int configSlot = 1;
    if(ui->advConfigSlot2Radio->isChecked()) {
        configSlot = 2;
    }
    m_ykConfig->setConfigSlot(configSlot);

    //Public ID...
    if(ui->advPubIdCheck->isChecked()) {
        m_ykConfig->setPubIdTxt(ui->advPubIdTxt->text());
    }

    //Private ID...
    if(ui->advPvtIdCheck->isChecked()) {
        m_ykConfig->setPvtIdTxt(ui->advPvtIdTxt->text());
    }

    //Secret Key...
    m_ykConfig->setSecretKeyTxt(ui->advSecretKeyTxt->text());

    //Configuration protection...
    m_ykConfig->setCurrentAccessCodeTxt(
        ui->advConfigProtectionBox->currentAccessCode());
    m_ykConfig->setNewAccessCodeTxt(
        ui->advConfigProtectionBox->newAccessCode(),
        ui->advConfigProtectionBox->newAccMode());

    if(mode == WRITE_CONFIG) {
        //Write
        connect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
                this, SLOT(advConfigWritten(bool, const QString &)));

        YubiKeyWriter::getInstance()->writeConfig(m_ykConfig);
    } else if(mode == EXPORT_CONFIG) {
        connect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
                this, SLOT(advConfigExported(bool, const QString &)));

        YubiKeyWriter::getInstance()->exportConfig(m_ykConfig);
    }
}

void OtpPage::advConfigWritten(bool written, const QString &msg) {
    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
               this, SLOT(advConfigWritten(bool, const QString &)));

    QString message;

    if(written && m_ykConfig != 0) {
        qDebug() << "Configuration written....";

        m_keysProgrammedCtr++;

        QString keyDetail("");
        if(ui->advPubIdCheck->isChecked()) {
            keyDetail = tr(" (Public ID: %1)").arg(m_ykConfig->pubIdTxt());
        }

        if(m_state == State_Programming){
            message = tr(KEY_CONFIGURED).arg(keyDetail);
        } else {
            message = tr("%1. %2").arg(tr(KEY_CONFIGURED).arg(keyDetail)).arg(tr(REMOVE_KEY));
        }

        showStatusMessage(message, 0);

        message = tr(KEY_CONFIGURED).arg("");
    } else {
        qDebug() << "Configuration could not be written....";

        message = msg;
    }

    advUpdateResults(written, message);

    m_ready = false;
    stopAdvConfigWritting();
}

void OtpPage::advConfigExported(bool written, const QString &msg) {
    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
               this, SLOT(advConfigExported(bool, const QString &)));

    QString message;

    if(written && m_ykConfig != 0) {
        qDebug() << "Configuration exported....";

        QString keyDetail("");
        if(ui->advPubIdCheck->isChecked()) {
            keyDetail = tr(" (Public ID: %1)").arg(m_ykConfig->pubIdTxt());
        }

        message = tr(KEY_EXPORTED);

        showStatusMessage(message, 0);
    } else {
        qDebug() << "Configuration could not be exported....";

        message = msg;
    }

    m_ready = false;
    stopAdvConfigWritting();
}

void OtpPage::advUpdateResults(bool written, const QString &msg) {
    int row = 0;

    ui->advResultsWidget->insertRow(row);

    //Sr. No....
    QTableWidgetItem *srnoItem = new QTableWidgetItem(
            tr("%1").arg(ui->advResultsWidget->rowCount()));
    if(written) {
        srnoItem->setIcon(QIcon(QPixmap(tr(":/res/images/tick.png"))));
    } else {
        srnoItem->setIcon(QIcon(QPixmap(tr(":/res/images/cross.png"))));
    }
    srnoItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->advResultsWidget->setItem(row, 0, srnoItem);


    //Public ID...
    QString pubId;
    if(ui->advPubIdCheck->isChecked() && m_ykConfig != 0) {
        pubId = m_ykConfig->pubIdTxt();
    } else {
        pubId = NA;
    }
    QTableWidgetItem *idItem = new QTableWidgetItem(
            tr("%1").arg(pubId));

    idItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->advResultsWidget->setItem(row, 1, idItem);


    //Status...
    QTableWidgetItem *statusItem = new QTableWidgetItem(
            tr("%1").arg(msg));
    statusItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->advResultsWidget->setItem(row, 2, statusItem);


    //Timestamp...
    QDateTime timstamp = QDateTime::currentDateTime();
    QTableWidgetItem *timeItem = new QTableWidgetItem(
            tr("%1").arg(timstamp.toString(Qt::SystemLocaleShortDate)));
    timeItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->advResultsWidget->setItem(row, 3, timeItem);


    ui->advResultsWidget->resizeColumnsToContents();
    ui->advResultsWidget->resizeRowsToContents();
}

void OtpPage::setCurrentSlot(int slot) {
    if(m_currentPage == Page_Advanced) {
        ui->advConfigSlot1Radio->setChecked(slot == 1);
        ui->advConfigSlot2Radio->setChecked(slot == 2);
    } else if(m_currentPage == Page_Quick) {
        ui->quickConfigSlot1Radio->setChecked(slot == 1);
        ui->quickConfigSlot2Radio->setChecked(slot == 2);
    }
}
