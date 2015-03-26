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

#include "chalresppage.h"
#include "ui_chalresppage.h"
#include "ui/helpbox.h"
#include "ui/confirmbox.h"
#include "yubikeywriter.h"
#include "yubikeyfinder.h"
#include "yubikeyutil.h"
#include "common.h"

#include <QSettings>
#include <QDateTime>
#include <QDebug>
#include <QSignalMapper>
#include <QTableWidgetItem>

ChalRespPage::ChalRespPage(QWidget *parent) :
        QStackedWidget(parent),
        ui(new Ui::ChalRespPage)
{
    ui->setupUi(this);

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

    connect(ui->quickResetBtn, SIGNAL(clicked()),
            this, SLOT(resetQuickPage()));
    connect(ui->advResetBtn, SIGNAL(clicked()),
            this, SLOT(resetAdvPage()));

    connect(ui->advRequireUserInputCheck, SIGNAL(clicked(bool)),
            this, SLOT(requireInput_clicked(bool)));
    connect(ui->quickRequireUserInputCheck, SIGNAL(clicked(bool)),
            this, SLOT(requireInput_clicked(bool)));

    connect(ui->advHmacVarInputRadio, SIGNAL(clicked()),
            this, SLOT(hmac_lt64_clicked()));
    connect(ui->advHmacFixedInputRadio, SIGNAL(clicked()),
            this, SLOT(hmac_lt64_clicked()));

    ui->quickResultsWidget->resizeColumnsToContents();
    ui->advResultsWidget->resizeColumnsToContents();

    loadSettings();
}

ChalRespPage::~ChalRespPage() {
    if(m_ykConfig != 0) {
        delete m_ykConfig;
        m_ykConfig = 0;
    }
    delete ui;
}

void ChalRespPage::loadSettings() {
    QSettings settings;

    ui->advExportConfigBtn->setVisible(settings.value(SG_EXPORT_PREFERENCE).toBool());
    ui->quickExportConfigBtn->setVisible(settings.value(SG_EXPORT_PREFERENCE).toBool());

    bool reqInput = settings.value(SG_REQUIRE_INPUT).toBool();
    ui->quickRequireUserInputCheck->setChecked(reqInput);
    ui->advRequireUserInputCheck->setChecked(reqInput);

    bool hmaclt64 = true;
    if(settings.contains(SG_HMAC_LT64)) {
      hmaclt64 = settings.value(SG_HMAC_LT64).toBool();
    }
    ui->advHmacVarInputRadio->setChecked(hmaclt64);
    ui->advHmacFixedInputRadio->setChecked(!hmaclt64);
}

/*
 Common
*/

void ChalRespPage::connectPages() {
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

void ChalRespPage::setCurrentPage(int pageIndex) {
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

void ChalRespPage::connectHelpButtons() {
    //Map the values of the help buttons

    //Create a QMapper
    QSignalMapper *mapper = new QSignalMapper(this);

    //Connect the clicked signal with the QSignalMapper
    connect(ui->quickHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    connect(ui->quickConfigHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->quickParamGenSchemeHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->quickChalRespOptionsHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->quickPvtIdHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->quickSecretKeyHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    connect(ui->advConfigHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advChalRespOptionsHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->advSecretKeyHelpBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    //Set a value for each button
    mapper->setMapping(ui->quickHelpBtn, HelpBox::Help_ChalRespYubico);
    mapper->setMapping(ui->advHelpBtn, HelpBox::Help_ChalRespHmac);

    mapper->setMapping(ui->quickConfigHelpBtn, HelpBox::Help_ConfigurationSlot);
    mapper->setMapping(ui->quickParamGenSchemeHelpBtn, HelpBox::Help_ParameterGeneration);
    mapper->setMapping(ui->quickChalRespOptionsHelpBtn, HelpBox::Help_ChalRespOption);
    mapper->setMapping(ui->quickPvtIdHelpBtn, HelpBox::Help_PrivateID);
    mapper->setMapping(ui->quickSecretKeyHelpBtn, HelpBox::Help_SecretKey);

    mapper->setMapping(ui->advConfigHelpBtn, HelpBox::Help_ConfigurationSlot);
    mapper->setMapping(ui->advChalRespOptionsHelpBtn, HelpBox::Help_ChalRespOption);
    mapper->setMapping(ui->advSecretKeyHelpBtn, HelpBox::Help_SecretKey);

    //Connect the mapper
    connect(mapper, SIGNAL(mapped(int)), this, SIGNAL(showHelp(int)));
    connect(ui->quickConfigProtectionBox, SIGNAL(showHelp(int)), this, SIGNAL(showHelp(int)));
    connect(ui->advConfigProtectionBox, SIGNAL(showHelp(int)), this, SIGNAL(showHelp(int)));
}

void ChalRespPage::keyFound(bool found, bool* featuresMatrix) {
    if(found) {
        if(m_state == State_Initial) {
            ui->quickWriteConfigBtn->setEnabled(true);
            ui->quickExportConfigBtn->setEnabled(true);
            ui->advWriteConfigBtn->setEnabled(true);
            ui->advExportConfigBtn->setEnabled(true);

            if(!featuresMatrix[YubiKeyFinder::Feature_MultipleConfigurations]) {
                ui->quickConfigSlot2Radio->setEnabled(false);
                ui->advConfigSlot2Radio->setEnabled(false);
            } else {
                ui->quickConfigSlot2Radio->setEnabled(true);
                ui->advConfigSlot2Radio->setEnabled(true);
            }
            if(!featuresMatrix[YubiKeyFinder::Feature_ChallengeResponseFixed]) {
                ui->advHmacFixedInputRadio->setEnabled(false);
                ui->advHmacVarInputRadio->setChecked(true);
            } else {
                ui->advHmacFixedInputRadio->setEnabled(true);
            }

            if(!featuresMatrix[YubiKeyFinder::Feature_ChallengeResponse]) {
                this->setEnabled(false);
            }
        } else if(m_state >= State_Programming_Multiple && m_keyPresent == false) {
            if(this->currentIndex() == Page_Quick) {
                if(m_state == State_Programming_Multiple) {
                    ui->quickWriteConfigBtn->setEnabled(true);
                    ui->quickExportConfigBtn->setEnabled(true);
                } else {
                    writeQuickConfig(WRITE_CONFIG);
                }

            } else {
                if(m_state == State_Programming_Multiple) {
                    ui->advWriteConfigBtn->setEnabled(true);
                    ui->advExportConfigBtn->setEnabled(true);
                } else {
                    writeAdvConfig(WRITE_CONFIG);
                }
            }
        }
        m_keyPresent = true;
    } else {
        ui->quickWriteConfigBtn->setEnabled(false);
        ui->quickExportConfigBtn->setEnabled(false);
        ui->advWriteConfigBtn->setEnabled(false);
        ui->advExportConfigBtn->setEnabled(false);
        ui->advHmacFixedInputRadio->setEnabled(true);
        m_keyPresent = false;

        if(m_state == State_Initial) {
            ui->quickConfigSlot2Radio->setEnabled(true);
            ui->advConfigSlot2Radio->setEnabled(true);

            this->setEnabled(true);
        } else if(m_state >= State_Programming_Multiple) {
            if(m_keysProgrammedCtr > 0 && !m_ready) {
                if(this->currentIndex() == Page_Quick) {
                    changeQuickConfigParams();
                } else {

                    changeAdvConfigParams();
                }
            }
        }
    }
}

void ChalRespPage::clearState() {
    m_state = State_Initial;
    m_ready = true;

    if(m_ykConfig != 0) {
        delete m_ykConfig;
        m_ykConfig = 0;
    }
}


/*
 Quick Page handling
*/
void ChalRespPage::resetQuickPage() {
    if(ui->quickConfigSlot1Radio->isChecked()) {
        ui->quickConfigSlot2Radio->setChecked(true);
    }

    ui->quickConfigParamsCombo->setCurrentIndex(0);
    ui->quickAutoProgramKeysCheck->setChecked(false);
    ui->quickProgramMulKeysBox->setChecked(false);

    ui->quickConfigProtectionBox->reset();

    ui->quickPvtIdCheck->setChecked(true);
    ui->quickPvtIdTxt->clear();
    on_quickPvtIdTxt_editingFinished();

    ui->quickSecretKeyTxt->clear();
    on_quickSecretKeyTxt_editingFinished();

    ui->quickStopBtn->setEnabled(false);
    ui->quickResetBtn->setEnabled(false);
}

void ChalRespPage::freezeQuickPage(bool freeze) {
    bool disable = !freeze;
    ui->quickConfigBox->setEnabled(disable);
    ui->quickProgramMulKeysBox->setEnabled(disable);
    ui->quickConfigProtectionBox->setEnabled(disable);
    ui->quickKeyParamsBox->setEnabled(disable);

    ui->quickWriteConfigBtn->setEnabled(disable);
    ui->quickExportConfigBtn->setEnabled(disable);
    ui->quickStopBtn->setEnabled(!disable);
    ui->quickResetBtn->setEnabled(disable);
    ui->quickBackBtn->setEnabled(disable);
}

void ChalRespPage::on_quickProgramMulKeysBox_clicked(bool checked) {
    if(checked) {
        changeQuickConfigParams();
    }
}

void ChalRespPage::on_quickConfigParamsCombo_currentIndexChanged(__attribute__((unused)) int index) {
    changeQuickConfigParams();
}

void ChalRespPage::on_quickPvtIdCheck_stateChanged(int state) {
    if(ui->quickConfigParamsCombo->currentIndex() != SCHEME_RAND_ALL) {
        bool disable = (state != 0);
        ui->quickPvtIdTxt->setEnabled(disable);
        ui->quickPvtIdGenerateBtn->setEnabled(disable);
    }
}

void ChalRespPage::on_quickPvtIdTxt_editingFinished() {
    QString txt = ui->quickPvtIdTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)UID_SIZE * 2);
    ui->quickPvtIdTxt->setText(txt);
}

void ChalRespPage::on_quickPvtIdGenerateBtn_clicked() {
    ui->quickPvtIdTxt->setText(
            YubiKeyUtil::generateRandomHex((size_t)UID_SIZE * 2));
}

void ChalRespPage::on_quickSecretKeyTxt_editingFinished() {
    QString txt = ui->quickSecretKeyTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)KEY_SIZE * 2);
    ui->quickSecretKeyTxt->setText(txt);
}

void ChalRespPage::on_quickSecretKeyGenerateBtn_clicked() {
    ui->quickSecretKeyTxt->setText(
            YubiKeyUtil::generateRandomHex((size_t)KEY_SIZE * 2));
}

void ChalRespPage::on_quickWriteConfigBtn_clicked() {
    emit showStatusMessage(NULL, -1);

    if(!ui->quickProgramMulKeysBox->isChecked()) {
        m_keysProgrammedCtr = 0;
    }

    //Validate settings
    if(!validateQuickSettings()) {
        return;
    }

    clearState();

    freezeQuickPage(true);

    // Change state
    if(ui->quickProgramMulKeysBox->isChecked()) {
        if(ui->quickAutoProgramKeysCheck->isChecked()) {
            m_keysProgrammedCtr = 0;
            m_state = State_Programming_Multiple_Auto;
        } else {
            m_state = State_Programming_Multiple;
        }
    } else {
        m_keysProgrammedCtr = 0;
        m_state = State_Programming;
    }

    writeQuickConfig(WRITE_CONFIG);
}

void ChalRespPage::on_quickExportConfigBtn_clicked() {
    emit showStatusMessage(NULL, -1);

    //Validate settings
    if(!validateQuickSettings()) {
        return;
    }

    clearState();

    freezeQuickPage(true);

    writeQuickConfig(EXPORT_CONFIG);
}
void ChalRespPage::on_quickStopBtn_clicked() {
    ui->quickStopBtn->setEnabled(false);
    m_state = State_Initial;
    stopQuickConfigWritting();
}

void ChalRespPage::stopQuickConfigWritting() {
    qDebug() << "Stopping quick configuration writing...";

    if(m_state >= State_Programming_Multiple) {
        ui->quickStopBtn->setEnabled(true);
        return;
    }

    m_keysProgrammedCtr = 0;
    clearState();

    freezeQuickPage(false);
    ui->quickResetBtn->setEnabled(true);
}

void ChalRespPage::changeQuickConfigParams() {
    int index = ui->quickConfigParamsCombo->currentIndex();
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

    //Private ID...
    if(ui->quickPvtIdCheck->isChecked()) {
        QString pvtIdTxt = YubiKeyUtil::getNextHex(
                UID_SIZE * 2,
                ui->quickPvtIdTxt->text(), idScheme);
        ui->quickPvtIdTxt->setText(pvtIdTxt);
    }

    //Secret Key...
    QString secretKeyTxt = YubiKeyUtil::getNextHex(
            KEY_SIZE * 2,
            ui->quickSecretKeyTxt->text(), secretScheme);
    ui->quickSecretKeyTxt->setText(secretKeyTxt);

    m_ready = true;
}

bool ChalRespPage::validateQuickSettings() {
    if(!(ui->quickConfigSlot1Radio->isChecked() ||
         ui->quickConfigSlot2Radio->isChecked())) {
        emit showStatusMessage(tr(ERR_CONF_SLOT_NOT_SELECTED), 1);
        return false;
    }

    QSettings settings;

    //Check if configuration slot 1 is being programmed
    if (!settings.value(SG_OVERWRITE_CONF_SLOT1).toBool() &&
        ui->quickConfigSlot1Radio->isChecked() &&
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

    if(!ui->quickConfigProtectionBox->checkConfirm()) {
        return false;
    }

    return true;
}

void ChalRespPage::writeQuickConfig(int mode) {
    qDebug() << "Writing configuration...";

    //Disable stop button while configuration is being written
    ui->quickStopBtn->setEnabled(false);

    //Write configuration
    if(m_ykConfig != 0) {
        qDebug() << "ykConfig destroyed";
        delete m_ykConfig;
        m_ykConfig = 0;
    }
    m_ykConfig = new YubiKeyConfig();

    //Programming mode...
    m_ykConfig->setProgrammingMode(YubiKeyConfig::Mode_ChalRespYubico);

    // set serial
    m_ykConfig->setSerial(QString::number(YubiKeyFinder::getInstance()->serial()));

    //Configuration slot...
    int configSlot = 1;
    if(ui->quickConfigSlot2Radio->isChecked()) {
        configSlot = 2;
    }
    m_ykConfig->setConfigSlot(configSlot);

    //Private ID...
    if(ui->quickPvtIdCheck->isChecked()) {
        m_ykConfig->setPvtIdTxt(ui->quickPvtIdTxt->text());
    }

    //Secret Key...
    m_ykConfig->setSecretKeyTxt(ui->quickSecretKeyTxt->text());

    //Configuration protection...
    //Current Access Code...
    m_ykConfig->setCurrentAccessCodeTxt(
            ui->quickConfigProtectionBox->currentAccessCode());
    m_ykConfig->setNewAccessCodeTxt(
            ui->quickConfigProtectionBox->newAccessCode(),
            ui->quickConfigProtectionBox->newAccMode());

    //Challenge-Response Options...
    m_ykConfig->setChalYubico(true);
    m_ykConfig->setChalBtnTrig(ui->quickRequireUserInputCheck->isChecked());

    if(mode == WRITE_CONFIG) {
        //Write
        connect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
                this, SLOT(quickConfigWritten(bool, const QString &)));

        YubiKeyWriter::getInstance()->writeConfig(m_ykConfig);
    } else if(mode == EXPORT_CONFIG) {
        //Write
        connect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
                this, SLOT(quickConfigExported(bool, const QString &)));

        YubiKeyWriter::getInstance()->exportConfig(m_ykConfig);
    }
}

void ChalRespPage::quickConfigWritten(bool written, const QString &msg) {
    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
               this, SLOT(quickConfigWritten(bool, const QString &)));

    QString message;

    if(written) {
        qDebug() << "Configuration written....";

        m_keysProgrammedCtr++;

        if(m_state == State_Programming){
            message = tr(KEY_CONFIGURED).arg("");
        } else {
            message = tr("%1. %2").arg(tr(KEY_CONFIGURED).arg(m_keysProgrammedCtr)).arg(tr(REMOVE_KEY));
        }

        showStatusMessage(message, 0);

        message = tr(KEY_CONFIGURED).arg("");
    } else {
        qDebug() << "Configuration could not be written....";

        message = msg;
    }

    quickUpdateResults(written, message);

    m_ready = false;
    stopQuickConfigWritting();
}

void ChalRespPage::quickConfigExported(bool written, const QString &msg) {
    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
               this, SLOT(quickConfigExported(bool, const QString &)));

    QString message;

    if(written) {
        qDebug() << "Configuration exported....";

        message = tr(KEY_EXPORTED);

        showStatusMessage(message, 0);
    } else {
        qDebug() << "Configuration could not be exported....";

        message = msg;
    }

    m_ready = false;
    stopQuickConfigWritting();
}

void ChalRespPage::quickUpdateResults(bool written, const QString &msg) {
    int row = 0;

    ui->quickResultsWidget->insertRow(row);

    //Sr. No....
    QTableWidgetItem *srnoItem = new QTableWidgetItem(
            tr("%1").arg(ui->quickResultsWidget->rowCount()));
    if(written) {
        srnoItem->setIcon(QIcon(QPixmap(tr(":/res/images/tick.png"))));
    } else {
        srnoItem->setIcon(QIcon(QPixmap(tr(":/res/images/cross.png"))));
    }
    srnoItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->quickResultsWidget->setItem(row, 0, srnoItem);


    //Status...
    QTableWidgetItem *statusItem = new QTableWidgetItem(
            tr("%1").arg(msg));
    statusItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->quickResultsWidget->setItem(row, 1, statusItem);


    //Timestamp...
    QDateTime timstamp = QDateTime::currentDateTime();
    QTableWidgetItem *timeItem = new QTableWidgetItem(
            tr("%1").arg(timstamp.toString(Qt::SystemLocaleShortDate)));
    timeItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->quickResultsWidget->setItem(row, 2, timeItem);


    ui->quickResultsWidget->resizeColumnsToContents();
    ui->quickResultsWidget->resizeRowsToContents();
}

/*
 Advanced Page handling
*/

void ChalRespPage::resetAdvPage() {
    if(ui->advConfigSlot1Radio->isChecked()) {
        ui->advConfigSlot2Radio->setChecked(true);
    }

    ui->advConfigParamsCombo->setCurrentIndex(0);
    ui->advAutoProgramKeysCheck->setChecked(false);
    ui->advProgramMulKeysBox->setChecked(false);

    ui->advConfigProtectionBox->reset();

    ui->advSecretKeyTxt->clear();
    on_advSecretKeyTxt_editingFinished();

    ui->advStopBtn->setEnabled(false);
    ui->advResetBtn->setEnabled(false);
}

void ChalRespPage::freezeAdvPage(bool freeze) {
    bool disable = !freeze;
    ui->advConfigBox->setEnabled(disable);
    ui->advProgramMulKeysBox->setEnabled(disable);
    ui->advConfigProtectionBox->setEnabled(disable);
    ui->advKeyParamsBox->setEnabled(disable);

    ui->advWriteConfigBtn->setEnabled(disable);
    ui->advExportConfigBtn->setEnabled(disable);
    ui->advStopBtn->setEnabled(!disable);
    ui->advResetBtn->setEnabled(disable);
    ui->advBackBtn->setEnabled(disable);
}

void ChalRespPage::on_advProgramMulKeysBox_clicked(bool checked) {
    if(checked) {
        changeAdvConfigParams();
    }
}

void ChalRespPage::on_advConfigParamsCombo_currentIndexChanged(__attribute__((unused)) int index) {
    changeAdvConfigParams();
}

void ChalRespPage::on_advSecretKeyTxt_editingFinished() {
    QString txt = ui->advSecretKeyTxt->text();
    YubiKeyUtil::qstrClean(&txt, (size_t)KEY_SIZE_OATH * 2);
    ui->advSecretKeyTxt->setText(txt);
}

void ChalRespPage::on_advSecretKeyGenerateBtn_clicked() {
    ui->advSecretKeyTxt->setText(
            YubiKeyUtil::generateRandomHex((size_t)KEY_SIZE_OATH * 2));
    ui->advSecretKeyTxt->setCursorPosition(0);
}

void ChalRespPage::on_advWriteConfigBtn_clicked() {
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

void ChalRespPage::on_advExportConfigBtn_clicked() {
    emit showStatusMessage(NULL, -1);

    //Validate settings
    if(!validateAdvSettings()) {
        return;
    }

    clearState();

    freezeAdvPage(true);

    writeAdvConfig(EXPORT_CONFIG);
}

void ChalRespPage::on_advStopBtn_clicked() {
    ui->advStopBtn->setEnabled(false);
    m_state = State_Initial;
    stopAdvConfigWritting();
}

void ChalRespPage::stopAdvConfigWritting() {
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

void ChalRespPage::changeAdvConfigParams() {
    int index = ui->advConfigParamsCombo->currentIndex();
    // else we use a static secret.
    if(index == SCHEME_INCR_ID_RAND_SECRET) {
        //Secret Key...
        QString secretKeyTxt = YubiKeyUtil::getNextHex(
            KEY_SIZE_OATH * 2,
            ui->advSecretKeyTxt->text(), GEN_SCHEME_RAND);
        ui->advSecretKeyTxt->setText(secretKeyTxt);
    }
    on_advSecretKeyTxt_editingFinished();
    m_ready = true;
}

bool ChalRespPage::validateAdvSettings() {
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

    if(!ui->advConfigProtectionBox->checkConfirm()) {
        return false;
    }

    return true;
}

void ChalRespPage::writeAdvConfig(int mode) {
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
    m_ykConfig->setProgrammingMode(YubiKeyConfig::Mode_ChalRespHmac);

    // set serial
    m_ykConfig->setSerial(QString::number(YubiKeyFinder::getInstance()->serial()));

    //Configuration slot...
    int configSlot = 1;
    if(ui->advConfigSlot2Radio->isChecked()) {
        configSlot = 2;
    }
    m_ykConfig->setConfigSlot(configSlot);

    //Secret Key...
    m_ykConfig->setSecretKeyTxt(ui->advSecretKeyTxt->text());

    //Configuration protection...
    m_ykConfig->setCurrentAccessCodeTxt(
            ui->advConfigProtectionBox->currentAccessCode());
    m_ykConfig->setNewAccessCodeTxt(
            ui->advConfigProtectionBox->newAccessCode(),
            ui->advConfigProtectionBox->newAccMode());

    //Challenge-Response Options...
    m_ykConfig->setChalHmac(true);
    m_ykConfig->setHmacLT64(ui->advHmacVarInputRadio->isChecked());
    m_ykConfig->setChalBtnTrig(ui->advRequireUserInputCheck->isChecked());

    if(mode == WRITE_CONFIG) {
        //Write
        connect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
                this, SLOT(advConfigWritten(bool, const QString &)));

        YubiKeyWriter::getInstance()->writeConfig(m_ykConfig);
    } else if(mode == EXPORT_CONFIG) {
        //Write
        connect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
                this, SLOT(advConfigExported(bool, const QString &)));

        YubiKeyWriter::getInstance()->exportConfig(m_ykConfig);
    }
}

void ChalRespPage::advConfigWritten(bool written, const QString &msg) {
    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
               this, SLOT(advConfigWritten(bool, const QString &)));

    QString message;

    if(written) {
        qDebug() << "Configuration written....";

        m_keysProgrammedCtr++;

        if(m_state == State_Programming){
            message = tr(KEY_CONFIGURED).arg("");
        } else {
            message = tr("%1. %2").arg(tr(KEY_CONFIGURED).arg(m_keysProgrammedCtr)).arg(tr(REMOVE_KEY));
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

void ChalRespPage::advConfigExported(bool written, const QString &msg) {
    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
               this, SLOT(advConfigExported(bool, const QString &)));

    QString message;

    if(written) {
        qDebug() << "Configuration written....";

        message = tr(KEY_EXPORTED);

        showStatusMessage(message, 0);
    } else {
        qDebug() << "Configuration could not be exported....";

        message = msg;
    }

    m_ready = false;
    stopAdvConfigWritting();
}

void ChalRespPage::advUpdateResults(bool written, const QString &msg) {
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


    //Status...
    QTableWidgetItem *statusItem = new QTableWidgetItem(
            tr("%1").arg(msg));
    statusItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->advResultsWidget->setItem(row, 1, statusItem);


    //Timestamp...
    QDateTime timstamp = QDateTime::currentDateTime();
    QTableWidgetItem *timeItem = new QTableWidgetItem(
            tr("%1").arg(timstamp.toString(Qt::SystemLocaleShortDate)));
    timeItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    ui->advResultsWidget->setItem(row, 2, timeItem);


    ui->advResultsWidget->resizeColumnsToContents();
    ui->advResultsWidget->resizeRowsToContents();
}

void ChalRespPage::requireInput_clicked(bool checked) {
    QSettings settings;
    settings.setValue(SG_REQUIRE_INPUT, checked);
    if(m_currentPage == Page_Quick) {
        ui->advRequireUserInputCheck->setChecked(checked);
    } else {
        ui->quickRequireUserInputCheck->setChecked(checked);
    }
}

void ChalRespPage::hmac_lt64_clicked() {
    QSettings settings;
    settings.setValue(SG_HMAC_LT64, ui->advHmacVarInputRadio->isChecked());
}


void ChalRespPage::setCurrentSlot(int slot) {
    if(m_currentPage == Page_Advanced) {
        ui->advConfigSlot1Radio->setChecked(slot == 1);
        ui->advConfigSlot2Radio->setChecked(slot == 2);
    } else if(m_currentPage == Page_Quick) {
        ui->quickConfigSlot1Radio->setChecked(slot == 1);
        ui->quickConfigSlot2Radio->setChecked(slot == 2);
    }
}
