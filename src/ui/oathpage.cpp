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

#include "oathpage.h"
#include "ui_oathpage.h"
#include "ui/helpbox.h"
#include "ui/confirmbox.h"

#define OATH_FIXED_NUMERIC      0
#define OATH_FIXED_MODHEX1      1
#define OATH_FIXED_MODHEX2      2
#define OATH_FIXED_MODHEX       3

#define MOVING_FACTOR_ZERO      0
#define MOVING_FACTOR_FIXED     1
#define MOVING_FACTOR_RAND      2

OathPage::OathPage(QWidget *parent) :
        QStackedWidget(parent),
        ui(new Ui::OathPage)
{
    ui->setupUi(this);

    m_customerPrefix = -1;
    memset(&m_pubId, 0, sizeof(m_pubId));
    m_pubIdFormat = 0;
    m_ykConfig = 0;
    clearState();

    //Connect pages
    connectPages();

    //Connect help buttons
    connectHelpButtons();

    //Connect other signals and slots
    connect(YubiKeyFinder::getInstance(), SIGNAL(keyFound(bool, bool*)),
            this, SLOT(keyFound(bool, bool*)));

    connect(ui->quickWriteConfigBtn, SIGNAL(clicked()),
            this, SLOT(writeQuickConfig()));
    connect(ui->advResetBtn, SIGNAL(clicked()),
            this, SLOT(resetAdvPage()));

    connect(ui->advMovingFactorSeedTxt, SIGNAL(editingFinished()),
            this, SLOT(on_advMovingFactorSeedTxt_editingFinished()));

    //Load settings
    loadSettings();

    ui->advResultsWidget->resizeColumnsToContents();
}

OathPage::~OathPage() {
    if(m_ykConfig != 0) {
        delete m_ykConfig;
        m_ykConfig = 0;
    }
    delete ui;
}

/*
 Common
*/

void OathPage::connectPages() {
    //Map the values of the navigation buttons with the indexes of
    //the stacked widget

    //Create a QMapper
    QSignalMapper *mapper = new QSignalMapper(this);

    //Connect the clicked signal with the QSignalMapper
    connect(ui->quickBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->quickBackBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    connect(ui->advBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advBackBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    //Set a value for each button
    mapper->setMapping(ui->quickBtn, Page_Quick);
    mapper->setMapping(ui->quickBackBtn, Page_Base);

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

void OathPage::setCurrentPage(int pageIndex) {
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

void OathPage::connectHelpButtons() {
    //Map the values of the help buttons

    //Create a QMapper
    QSignalMapper *mapper = new QSignalMapper(this);

    //Connect the clicked signal with the QSignalMapper
    connect(ui->quickConfigHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->quickPubIdHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->quickHotpLenHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->quickSecretKeyHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    connect(ui->advConfigHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advParamGenSchemeHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advConfigProtectionHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advPubIdHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advHotpLenHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advHotpParamsHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advSecretKeyHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    //Set a value for each button
    mapper->setMapping(ui->quickConfigHelpBtn, HelpBox::Help_ConfigurationSlot);
    mapper->setMapping(ui->quickPubIdHelpBtn, HelpBox::Help_OathPublicID);
    mapper->setMapping(ui->quickHotpLenHelpBtn, HelpBox::Help_HotpLen);
    mapper->setMapping(ui->quickSecretKeyHelpBtn, HelpBox::Help_SecretKey);

    mapper->setMapping(ui->advConfigHelpBtn, HelpBox::Help_ConfigurationSlot);
    mapper->setMapping(ui->advParamGenSchemeHelpBtn, HelpBox::Help_ParameterGeneration);
    mapper->setMapping(ui->advConfigProtectionHelpBtn, HelpBox::Help_ConfigurationProtection);
    mapper->setMapping(ui->advPubIdHelpBtn, HelpBox::Help_OathPublicID);
    mapper->setMapping(ui->advHotpLenHelpBtn, HelpBox::Help_HotpLen);
    mapper->setMapping(ui->advHotpParamsHelpBtn, HelpBox::Help_HotpParam);
    mapper->setMapping(ui->advSecretKeyHelpBtn, HelpBox::Help_SecretKey);

    //Connect the mapper
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(helpBtn_pressed(int)));
}

void OathPage::helpBtn_pressed(int helpIndex) {
    HelpBox help(this);
    help.setHelpIndex((HelpBox::Help)helpIndex);
    help.exec();
}

void OathPage::keyFound(bool found, bool* featuresMatrix) {
    if(found) {
        if(m_state == State_Initial) {
            ui->quickWriteConfigBtn->setEnabled(true);
            ui->advWriteConfigBtn->setEnabled(true);

            if(!featuresMatrix[YubiKeyFinder::Feature_MultipleConfigurations]) {
                ui->quickConfigSlot2Radio->setEnabled(false);
                ui->advConfigSlot2Radio->setEnabled(false);
            } else {
                ui->quickConfigSlot2Radio->setEnabled(true);
                ui->advConfigSlot2Radio->setEnabled(true);
            }

            if(!featuresMatrix[YubiKeyFinder::Feature_MovingFactor]) {
                ui->advMovingFactorSeedCombo->setEnabled(false);
                ui->advMovingFactorSeedTxt->setEnabled(false);
            }

            if(!featuresMatrix[YubiKeyFinder::Feature_OathHotp]) {
                this->setEnabled(false);
            }

            if(this->currentIndex() == Page_Quick) {
                resetQuickPrefix();
            }
        } else if(this->currentIndex() == Page_Advanced &&
                  m_state >= State_Programming_Multiple) {
            if(m_state == State_Programming_Multiple) {
                ui->advWriteConfigBtn->setEnabled(true);
            } else {
                writeAdvConfig();
            }
        }
    } else {

        ui->quickWriteConfigBtn->setEnabled(false);
        ui->advWriteConfigBtn->setEnabled(false);

        if(m_state == State_Initial) {
            ui->quickConfigSlot2Radio->setEnabled(true);
            ui->advConfigSlot2Radio->setEnabled(true);

            ui->advMovingFactorSeedCombo->setEnabled(true);
            if(ui->advMovingFactorSeedCombo->currentIndex() == MOVING_FACTOR_ZERO) {
                ui->advMovingFactorSeedTxt->setEnabled(false);
            } else {
                ui->advMovingFactorSeedTxt->setEnabled(true);
            }

            this->setEnabled(true);
        } else if(this->currentIndex() == Page_Advanced &&
                  m_state >= State_Programming_Multiple) {
            if(m_keysProgrammedCtr > 0 && !m_ready) {
                changeAdvConfigParams();
            }
        }
    }
}

void OathPage::clearState() {
    m_state = State_Initial;
    m_ready = true;

    if(m_ykConfig != 0) {
        delete m_ykConfig;
        m_ykConfig = 0;
    }
}

void OathPage::updatePrefix() {
    if(m_currentPage == Page_Quick || m_customerPrefix > 0) {
        //OMP
        m_pubId[0] = YUBICO_OMP_CODE;

        //TT
        m_pubId[1] = OATH_HOTP_CUSTOMER_PREFIX_START +
                     (unsigned char) (m_customerPrefix / 1000);

        if(m_customerPrefix > 0) {
            unsigned short tmp;
            tmp = m_customerPrefix % 1000;

            m_pubId[2] = ((tmp / 100) << 4) | (( tmp % 100) / 10);
            m_pubId[3] = (m_pubId[3] & 0xf) | ((tmp % 10) << 4);
        }
    }

    QString pubIdTxt;
    QString pubIdModhexTxt = YubiKeyUtil::qstrModhexEncode(
            m_pubId, OATH_HOTP_PUBLIC_ID_SIZE);

    switch(m_currentPage) {
    case Page_Quick:
        fixBCD(m_pubId + 2, OATH_HOTP_TT_SIZE +  OATH_HOTP_MUI_SIZE);

        pubIdTxt = YubiKeyUtil::qstrHexEncode(
                m_pubId, OATH_HOTP_PUBLIC_ID_SIZE);

        ui->quickPrefixTxt->setText(pubIdModhexTxt.left(4));
        ui->quickMUITxt->setText(pubIdTxt.right(8));
        break;

    case Page_Advanced:
        switch(ui->advPubIdFormatCombo->currentIndex()){
        case OATH_FIXED_NUMERIC:
            fixBCD(m_pubId, OATH_HOTP_PUBLIC_ID_SIZE);

            pubIdTxt = YubiKeyUtil::qstrHexEncode(
                    m_pubId, OATH_HOTP_PUBLIC_ID_SIZE);

            ui->advOMPTxt->setText(pubIdTxt.left(2));
            ui->advTTTxt->setText(pubIdTxt.mid(2, 2));
            ui->advMUITxt->setText(pubIdTxt.right(8));
            break;

        case OATH_FIXED_MODHEX1:
            fixBCD(m_pubId + 1, OATH_HOTP_TT_SIZE +  OATH_HOTP_MUI_SIZE);

            pubIdTxt = YubiKeyUtil::qstrHexEncode(
                    m_pubId, OATH_HOTP_PUBLIC_ID_SIZE);


            ui->advOMPTxt->setText(pubIdModhexTxt.left(2));
            ui->advTTTxt->setText(pubIdTxt.mid(2, 2));
            ui->advMUITxt->setText(pubIdTxt.right(8));
            break;

        case OATH_FIXED_MODHEX2:
            fixBCD(m_pubId + 2, OATH_HOTP_MUI_SIZE);

            pubIdTxt = YubiKeyUtil::qstrHexEncode(
                    m_pubId, OATH_HOTP_PUBLIC_ID_SIZE);

            ui->advOMPTxt->setText(pubIdModhexTxt.left(2));
            ui->advTTTxt->setText(pubIdModhexTxt.mid(2, 2));
            ui->advMUITxt->setText(pubIdTxt.right(8));
            break;

        case OATH_FIXED_MODHEX:
            ui->advOMPTxt->setText(pubIdModhexTxt.left(2));
            ui->advTTTxt->setText(pubIdModhexTxt.mid(2, 2));
            ui->advMUITxt->setText(pubIdModhexTxt.right(8));
            break;
        }

        break;
    }
}

void OathPage::fixBCD(unsigned char *bp, int bcnt) {
    while (bcnt--) {
        if ((*bp & 0x0f) > 0x09) *bp -= 0x09;
        if ((*bp & 0xf0) > 0x90) *bp -= 0x90;
        bp++;
    }
}

void OathPage::loadSettings() {
    QSettings settings;
    m_customerPrefix = settings.value(SG_CUSTOMER_PREFIX).toInt();

    if(m_customerPrefix > 0) {
        ui->advPubIdFormatCombo->setCurrentIndex(OATH_FIXED_MODHEX2);
    }
    bool customerPrefixFlag = !(m_customerPrefix > 0);

    ui->advPubIdFormatCombo->setEnabled(customerPrefixFlag);
    ui->advOMPTxt->setEnabled(customerPrefixFlag);
    ui->advTTTxt->setEnabled(customerPrefixFlag);
}

/*
 Quick Page handling
*/
void OathPage::resetQuickPage() {
    memset(&m_pubId, 0, sizeof(m_pubId));

    if(ui->quickConfigSlot1Radio->isChecked()) {
        ui->quickConfigSlot2Radio->setChecked(true);
    }

    ui->quickPubIdCheck->setChecked(true);
    ui->quickHotpLen6Radio->setChecked(true);

    on_quickResetBtn_clicked();
}

void OathPage::on_quickResetBtn_clicked() {
    resetQuickPrefix();

    ui->quickSecretKeyTxt->setText(
            YubiKeyUtil::generateRandomHex((size_t)KEY_SIZE_OATH * 2));
    on_quickSecretKeyTxt_editingFinished();
}

void OathPage::on_quickHideParams_clicked(bool checked) {
    if(checked) {
        ui->quickSecretKeyTxt->setEchoMode(QLineEdit::Password);
    } else {
        ui->quickSecretKeyTxt->setEchoMode(QLineEdit::Normal);
    }
}

void OathPage::on_quickPubIdCheck_stateChanged(int state) {
    bool disable = (state != 0);

    ui->quickPrefixTxt->setEnabled(disable);
    ui->quickMUITxt->setEnabled(disable);
    ui->quickMUIGenerateBtn->setEnabled(disable);
}

void OathPage::resetQuickPrefix() {
    ui->quickMUITxt->setText(
            QString::number(YubiKeyFinder::getInstance()->serial()));

    updateQuickMUI();
    updatePrefix();
}

void OathPage::updateQuickMUI() {
    QString txt = ui->quickMUITxt->text();

    unsigned char buf[MAX_SIZE];
    memset(buf, 0, sizeof(buf));
    size_t bufLen = 0;

    YubiKeyUtil::qstrClean(&txt, OATH_HOTP_MUI_SIZE * 2, true);
    YubiKeyUtil::qstrDecDecode(buf, &bufLen, txt);

    ui->quickMUITxt->setText(txt);
    memcpy(m_pubId + 2, buf, OATH_HOTP_MUI_SIZE);
}

void OathPage::on_quickMUITxt_editingFinished() {
    updateQuickMUI();
    updatePrefix();
}

void OathPage::on_quickMUIGenerateBtn_clicked() {
    YubiKeyUtil::generateRandom(m_pubId + 2, OATH_HOTP_MUI_SIZE);
    updatePrefix();
}

void OathPage::on_quickSecretKeyTxt_editingFinished() {
    QString txt = ui->quickSecretKeyTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)KEY_SIZE_OATH * 2);
    ui->quickSecretKeyTxt->setText(txt);
}

bool OathPage::validateQuickSettings() {
    if(!(ui->quickConfigSlot1Radio->isChecked() ||
         ui->quickConfigSlot2Radio->isChecked())) {
        emit showStatusMessage(ERR_CONF_SLOT_NOT_SELECTED, 1);
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

void OathPage::writeQuickConfig() {
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
    ui->quickResetBtn->setEnabled(false);
    ui->quickBackBtn->setEnabled(false);

    if(m_ykConfig != 0) {
        delete m_ykConfig;
        m_ykConfig = 0;
    }
    m_ykConfig = new YubiKeyConfig();

    //Programming mode...
    m_ykConfig->setProgrammingMode(YubiKeyConfig::Mode_OathHotp);

    // set serial
    m_ykConfig->setSerial(QString::number(YubiKeyFinder::getInstance()->serial()));

    //Configuration slot...
    int configSlot = 1;
    if(ui->quickConfigSlot2Radio->isChecked()) {
        configSlot = 2;
    }
    m_ykConfig->setConfigSlot(configSlot);

    //Public ID...
    if(ui->quickPubIdCheck->isChecked()) {
        QString pubIdTxt = YubiKeyUtil::qstrModhexEncode(m_pubId, OATH_HOTP_PUBLIC_ID_SIZE);
        m_ykConfig->setPubIdTxt(pubIdTxt);

        //OATH Public ID Type...
        m_ykConfig->setOathFixedModhex2(true);
    }

    //HOTP Len...
    m_ykConfig->setOathHotp8(ui->quickHotpLen8Radio->isChecked());

    //HOTP Moving Factor Seed...
    m_ykConfig->setOathMovingFactorSeed(0);

    //Secret Key...
    m_ykConfig->setSecretKeyTxt(ui->quickSecretKeyTxt->text());

    //Write
    connect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
            this, SLOT(quickConfigWritten(bool, const QString &)));

    YubiKeyWriter::getInstance()->writeConfig(m_ykConfig);
}

void OathPage::quickConfigWritten(bool written, const QString &msg) {

    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
               this, SLOT(quickConfigWritten(bool, const QString &)));

    ui->quickWriteConfigBtn->setEnabled(true);
    ui->quickResetBtn->setEnabled(true);
    ui->quickBackBtn->setEnabled(true);

    if(written && m_ykConfig != 0) {
        QString keyDetail("");
        if(ui->quickPubIdCheck->isChecked()) {
            QString pubIdTxt = ui->quickPrefixTxt->text() +
                               ui->quickMUITxt->text().replace(QRegExp("\\s"), QString(""));

            keyDetail = tr(" (OATH ID: %1)").arg(pubIdTxt);
        }

        showStatusMessage(KEY_CONFIGURED.arg(keyDetail), 0);
    }

    clearState();
}

/*
 Advanced Page handling
*/
void OathPage::resetAdvPage() {
    memset(&m_pubId, 0, sizeof(m_pubId));

    bool customerPrefixFlag = !(m_customerPrefix > 0);

    if(ui->advConfigSlot1Radio->isChecked()) {
        ui->advConfigSlot2Radio->setChecked(true);
    }

    ui->advConfigParamsCombo->setCurrentIndex(0);
    ui->advAutoProgramKeysCheck->setChecked(false);
    ui->advProgramMulKeysBox->setChecked(false);

    ui->advConfigProtectionCombo->setCurrentIndex(0);

    ui->advPubIdCheck->setChecked(true);
    if(customerPrefixFlag) {
        ui->advPubIdFormatCombo->setCurrentIndex(OATH_FIXED_NUMERIC);
    } else {
        ui->advPubIdFormatCombo->setCurrentIndex(OATH_FIXED_MODHEX2);
    }
    ui->advPubIdFormatCombo->setEnabled(customerPrefixFlag);
    on_advOMPTxt_editingFinished();
    ui->advOMPTxt->setEnabled(customerPrefixFlag);
    on_advTTTxt_editingFinished();
    ui->advTTTxt->setEnabled(customerPrefixFlag);
    on_advMUITxt_editingFinished();

    ui->advHotpLen6Radio->setChecked(true);
    ui->advMovingFactorSeedCombo->setCurrentIndex(0);
    ui->advMovingFactorSeedTxt->setText(tr("0"));
    ui->advMovingFactorSeedTxt->setEnabled(false);

    ui->advSecretKeyTxt->clear();
    on_advSecretKeyTxt_editingFinished();

    ui->advStopBtn->setEnabled(false);
}

void OathPage::freezeAdvPage(bool freeze) {
    bool disable = !freeze;
    ui->advConfigBox->setEnabled(disable);
    ui->advProgramMulKeysBox->setEnabled(disable);
    ui->advConfigProtectionBox->setEnabled(disable);
    ui->advKeyParamsBox->setEnabled(disable);

    ui->advWriteConfigBtn->setEnabled(disable);
    ui->advStopBtn->setEnabled(!disable);
    ui->advResetBtn->setEnabled(disable);
    ui->advBackBtn->setEnabled(disable);
}

void OathPage::on_advProgramMulKeysBox_clicked(bool checked) {
    if(checked) {
        changeAdvConfigParams();
    }
}

void OathPage::on_advConfigParamsCombo_currentIndexChanged(int index) {
    changeAdvConfigParams();
}

void OathPage::on_advConfigProtectionCombo_currentIndexChanged(int index) {
    switch(index) {
    case CONFIG_PROTECTION_DISABLED:
        ui->advCurrentAccessCodeTxt->clear();
        ui->advCurrentAccessCodeTxt->setEnabled(false);

        ui->advNewAccessCodeTxt->clear();
        ui->advNewAccessCodeTxt->setEnabled(false);
        break;
    case CONFIG_PROTECTION_ENABLE:
        ui->advCurrentAccessCodeTxt->clear();
        ui->advCurrentAccessCodeTxt->setEnabled(false);

        on_advNewAccessCodeTxt_editingFinished();
        ui->advNewAccessCodeTxt->setEnabled(true);
        break;
    case CONFIG_PROTECTION_DISABLE:
    case CONFIG_PROTECTION_ENABLED:
        on_advCurrentAccessCodeTxt_editingFinished();
        ui->advCurrentAccessCodeTxt->setEnabled(true);

        ui->advNewAccessCodeTxt->clear();
        ui->advNewAccessCodeTxt->setEnabled(false);
        break;
    case CONFIG_PROTECTION_CHANGE:
        on_advCurrentAccessCodeTxt_editingFinished();
        ui->advCurrentAccessCodeTxt->setEnabled(true);

        on_advNewAccessCodeTxt_editingFinished();
        ui->advNewAccessCodeTxt->setEnabled(true);
        break;
    }
}

void OathPage::on_advCurrentAccessCodeTxt_editingFinished() {
    QString txt = ui->advCurrentAccessCodeTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)ACC_CODE_SIZE * 2);
    ui->advCurrentAccessCodeTxt->setText(txt);
}

void OathPage::on_advNewAccessCodeTxt_editingFinished() {
    QString txt = ui->advNewAccessCodeTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)ACC_CODE_SIZE * 2);
    ui->advNewAccessCodeTxt->setText(txt);
}

void OathPage::on_advPubIdCheck_stateChanged(int state) {
    bool disable = (state != 0);

    if(m_customerPrefix <= 0) {
        ui->advPubIdFormatCombo->setEnabled(disable);
        ui->advOMPTxt->setEnabled(disable);
        ui->advTTTxt->setEnabled(disable);
    }

    ui->advMUITxt->setEnabled(disable);
    ui->advMUIGenerateBtn->setEnabled(disable);
}

void OathPage::on_advPubIdFormatCombo_currentIndexChanged(int index) {
    updateAdvOMP(m_pubIdFormat);
    updateAdvTT(m_pubIdFormat);
    updateAdvMUI(m_pubIdFormat);

    //        QRegExp modhex2("([b-lnrt-v ]){2}");
    //        QRegExp modhex8("([b-lnrt-v ]){8}");

    //        switch(index) {
    //        case OATH_FIXED_MODHEX1:
    //            ui->advOMPTxt->setInputMask("");
    //            ui->advOMPTxt->setValidator(new QRegExpValidator(modhex2, this));
    //            ui->advTTTxt->setInputMask(tr("00;"));
    //            ui->advMUITxt->setInputMask(tr("00 00 00 00;"));
    //            break;

    //        case OATH_FIXED_MODHEX2:
    //            ui->advOMPTxt->setInputMask("");
    //            ui->advOMPTxt->setValidator(new QRegExpValidator(modhex2, this));
    //            ui->advTTTxt->setInputMask("");
    //            ui->advTTTxt->setValidator(new QRegExpValidator(modhex2, this));
    //            ui->advMUITxt->setInputMask(tr("00 00 00 00;"));
    //            break;

    //        case OATH_FIXED_MODHEX:
    //            ui->advOMPTxt->setInputMask("");
    //            ui->advOMPTxt->setValidator(new QRegExpValidator(modhex2, this));
    //            ui->advTTTxt->setInputMask("");
    //            ui->advTTTxt->setValidator(new QRegExpValidator(modhex2, this));
    //            ui->advMUITxt->setInputMask("");
    //            ui->advMUITxt->setValidator(new QRegExpValidator(modhex8, this));
    //            break;

    //        default:
    //            ui->advOMPTxt->setInputMask(tr("00;"));
    //            ui->advTTTxt->setInputMask(tr("00;"));
    //            ui->advMUITxt->setInputMask(tr("00 00 00 00;"));
    //            break;
    //        }

    updatePrefix();

    m_pubIdFormat = index;
}

void OathPage::updateAdvOMP(int index) {
    QString txt = ui->advOMPTxt->text();

    unsigned char buf[MAX_SIZE];
    memset(buf, 0, sizeof(buf));
    size_t bufLen = 0;

    if (index == OATH_FIXED_NUMERIC) {
        YubiKeyUtil::qstrClean(&txt, OATH_HOTP_OMP_SIZE * 2, true);
        YubiKeyUtil::qstrDecDecode(buf, &bufLen, txt);
    } else {
        YubiKeyUtil::qstrModhexClean(&txt, OATH_HOTP_OMP_SIZE * 2);
        YubiKeyUtil::qstrModhexDecode(buf, &bufLen, txt);
    }
    ui->advOMPTxt->setText(txt);
    memcpy(m_pubId + 0, buf, OATH_HOTP_OMP_SIZE);
}

void OathPage::on_advOMPTxt_editingFinished() {
    updateAdvOMP(ui->advPubIdFormatCombo->currentIndex());
    updatePrefix();
}

void OathPage::updateAdvTT(int index) {
    QString txt = ui->advTTTxt->text();

    unsigned char buf[MAX_SIZE];
    memset(buf, 0, sizeof(buf));
    size_t bufLen = 0;

    if (index == OATH_FIXED_NUMERIC ||
        index == OATH_FIXED_MODHEX1) {
        YubiKeyUtil::qstrClean(&txt, OATH_HOTP_TT_SIZE * 2, true);
        YubiKeyUtil::qstrDecDecode(buf, &bufLen, txt);
    } else {
        YubiKeyUtil::qstrModhexClean(&txt, OATH_HOTP_TT_SIZE * 2);
        YubiKeyUtil::qstrModhexDecode(buf, &bufLen, txt);
    }
    ui->advTTTxt->setText(txt);
    memcpy(m_pubId + 1, buf, OATH_HOTP_TT_SIZE);
}

void OathPage::on_advTTTxt_editingFinished() {
    updateAdvTT(ui->advPubIdFormatCombo->currentIndex());
    updatePrefix();
}

void OathPage::updateAdvMUI(int index) {
    QString txt = ui->advMUITxt->text();

    unsigned char buf[MAX_SIZE];
    memset(buf, 0, sizeof(buf));
    size_t bufLen = 0;

    if (index != OATH_FIXED_MODHEX) {
        YubiKeyUtil::qstrClean(&txt, OATH_HOTP_MUI_SIZE * 2, true);
        YubiKeyUtil::qstrDecDecode(buf, &bufLen, txt);
    } else {
        YubiKeyUtil::qstrModhexClean(&txt, OATH_HOTP_MUI_SIZE * 2);
        YubiKeyUtil::qstrModhexDecode(buf, &bufLen, txt);
    }

    ui->advMUITxt->setText(txt);
    memcpy(m_pubId + 2, buf, OATH_HOTP_MUI_SIZE);
}

void OathPage::on_advMUITxt_editingFinished() {
    updateAdvMUI(ui->advPubIdFormatCombo->currentIndex());
    updatePrefix();
}

void OathPage::on_advMUIGenerateBtn_clicked() {
    YubiKeyUtil::generateRandom(m_pubId + 2, OATH_HOTP_MUI_SIZE);
    updatePrefix();
}

void OathPage::on_advMovingFactorSeedCombo_currentIndexChanged(int index) {
    switch(index) {
    case MOVING_FACTOR_ZERO:
        ui->advMovingFactorSeedTxt->setText("0");
        ui->advMovingFactorSeedTxt->setEnabled(false);
        break;
    case MOVING_FACTOR_FIXED:
        ui->advMovingFactorSeedTxt->setEnabled(true);
        break;
    case MOVING_FACTOR_RAND:
        unsigned int tmp;
        YubiKeyUtil::generateRandom((unsigned char *) &tmp, sizeof(tmp));
        ui->advMovingFactorSeedTxt->setText(QString::number(tmp));
        on_advMovingFactorSeedTxt_editingFinished();
        ui->advMovingFactorSeedTxt->setEnabled(true);
        break;
    }
}

void OathPage::on_advMovingFactorSeedTxt_editingFinished() {
    unsigned int val = ui->advMovingFactorSeedTxt->text().toUInt();
    val = (((val + 8) >> 4UL) & 0xffff) << 4UL;
    ui->advMovingFactorSeedTxt->setText(QString::number(val));
}

void OathPage::on_advSecretKeyTxt_editingFinished() {
    QString txt = ui->advSecretKeyTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)KEY_SIZE_OATH * 2);
    ui->advSecretKeyTxt->setText(txt);
}

void OathPage::on_advSecretKeyGenerateBtn_clicked() {
    ui->advSecretKeyTxt->setText(
            YubiKeyUtil::generateRandomHex((size_t)KEY_SIZE_OATH * 2));
    ui->advSecretKeyTxt->setCursorPosition(0);
}

void OathPage::on_advWriteConfigBtn_clicked() {
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

    writeAdvConfig();
}

void OathPage::on_advStopBtn_clicked() {
    ui->advStopBtn->setEnabled(false);
    m_state = State_Initial;
    stopAdvConfigWritting();
}

void OathPage::stopAdvConfigWritting() {
    qDebug() << "Stopping adv configuration writing...";

    if(m_state >= State_Programming_Multiple) {
        ui->advStopBtn->setEnabled(true);
        return;
    }

    m_keysProgrammedCtr = 0;
    clearState();

    freezeAdvPage(false);
    ui->advResetBtn->setEnabled(true);
}

void OathPage::changeAdvConfigParams() {
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
    }

    //Public ID...
    if(ui->advPubIdCheck->isChecked()) {
        switch(idScheme) {
        case GEN_SCHEME_INCR:
            {
                size_t pubIdLen = sizeof(m_pubId)/sizeof(m_pubId[0]);
                for (int i = pubIdLen; i--; ) {
                    if (++m_pubId[i]) {
                        break;
                    }
                }
                updatePrefix();
            }
            break;

        case GEN_SCHEME_RAND:

            YubiKeyUtil::generateRandom(m_pubId, OATH_HOTP_PUBLIC_ID_SIZE);
            updatePrefix();
            break;
        }
    }

    //HOTP Moving Factor Seed...
    if(ui->advMovingFactorSeedCombo->currentIndex() == MOVING_FACTOR_RAND) {
        unsigned int tmp;
        YubiKeyUtil::generateRandom((unsigned char *) &tmp, sizeof(tmp));
        ui->advMovingFactorSeedTxt->setText(QString::number(tmp));
        on_advMovingFactorSeedTxt_editingFinished();
    }

    //Secret Key...
    QString secretKeyTxt = YubiKeyUtil::getNextHex(
            KEY_SIZE_OATH * 2,
            ui->advSecretKeyTxt->text(), secretScheme);
    ui->advSecretKeyTxt->setText(secretKeyTxt);
    on_advSecretKeyTxt_editingFinished();
    m_ready = true;
}

bool OathPage::validateAdvSettings() {
    if(!(ui->advConfigSlot1Radio->isChecked() ||
         ui->advConfigSlot2Radio->isChecked())) {
        emit showStatusMessage(ERR_CONF_SLOT_NOT_SELECTED, 1);
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

    //Check if logging is disabled and
    //configuration protection is being enabled
    if(!settings.value(SG_ENABLE_CONF_PROTECTION).toBool() &&
       !YubiKeyLogger::isLogging() &&
       ui->advConfigProtectionCombo->currentIndex() == CONFIG_PROTECTION_ENABLE) {
        //Confirm from client
        ConfirmBox confirm(this);
        confirm.setConfirmIndex(ConfirmBox::Confirm_ConfigurationProtection);
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

void OathPage::writeAdvConfig() {
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
    m_ykConfig->setProgrammingMode(YubiKeyConfig::Mode_OathHotp);

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
        QString pubIdTxt = YubiKeyUtil::qstrModhexEncode(m_pubId, OATH_HOTP_PUBLIC_ID_SIZE);
        m_ykConfig->setPubIdTxt(pubIdTxt);

        //OATH Public ID Type...
        switch(ui->advPubIdFormatCombo->currentIndex()) {
        case 1:
            m_ykConfig->setOathFixedModhex1(true);
            break;
        case 2:
            m_ykConfig->setOathFixedModhex2(true);
            break;
        case 3:
            m_ykConfig->setOathFixedModhex(true);
            break;
        }
    }

    //HOTP Len...
    m_ykConfig->setOathHotp8(ui->advHotpLen8Radio->isChecked());

    //HOTP Moving Factor Seed...
    if(YubiKeyFinder::getInstance()->checkFeatureSupport(
            YubiKeyFinder::Feature_MovingFactor)) {

        m_ykConfig->setOathMovingFactorSeed(
                ui->advMovingFactorSeedTxt->text().toUInt());
    }

    //Secret Key...
    m_ykConfig->setSecretKeyTxt(ui->advSecretKeyTxt->text());

    //Configuration protection...
    //Current Access Code...
    m_ykConfig->setCurrentAccessCodeTxt(ui->advCurrentAccessCodeTxt->text());

    //New Access Code...
    if(ui->advConfigProtectionCombo->currentIndex()
        == CONFIG_PROTECTION_DISABLE){
        m_ykConfig->setNewAccessCodeTxt(ACCESS_CODE_DEFAULT);
    } else {
        m_ykConfig->setNewAccessCodeTxt(ui->advNewAccessCodeTxt->text());
    }

    //Write
    connect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
            this, SLOT(advConfigWritten(bool, const QString &)));

    YubiKeyWriter::getInstance()->writeConfig(m_ykConfig);
}

void OathPage::advConfigWritten(bool written, const QString &msg) {
    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
               this, SLOT(advConfigWritten(bool, const QString &)));

    QString message;

    if(written && m_ykConfig != 0) {
        qDebug() << "Configuration written....";

        m_keysProgrammedCtr++;

        QString keyDetail("");
        if(ui->advPubIdCheck->isChecked()) {

            QString pubIdTxt = ui->advOMPTxt->text() +
                               ui->advTTTxt->text() +
                               ui->advMUITxt->text().replace(QRegExp("\\s"), QString(""));

            keyDetail = tr(" (OATH ID: %1)").arg(pubIdTxt);
        }

        if(m_state == State_Programming){
            message = KEY_CONFIGURED.arg(keyDetail);
        } else {
            message = tr("%1. %2").arg(KEY_CONFIGURED.arg(keyDetail)).arg(REMOVE_KEY);
        }

        showStatusMessage(message, 0);

        message = KEY_CONFIGURED.arg("");
    } else {
        qDebug() << "Configuration could not be written....";

        message = msg;
    }

    advUpdateResults(written, message);

    m_ready = false;
    stopAdvConfigWritting();
}

void OathPage::advUpdateResults(bool written, const QString &msg) {
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
    QString pubIdTxt;
    if(ui->advPubIdCheck->isChecked() && m_ykConfig != 0) {
        pubIdTxt = ui->advOMPTxt->text() +
                   ui->advTTTxt->text() +
                   ui->advMUITxt->text().replace(QRegExp("\\s"), QString(""));
    } else {
        pubIdTxt = NA;
    }
    QTableWidgetItem *idItem = new QTableWidgetItem(
            tr("%1").arg(pubIdTxt));
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
