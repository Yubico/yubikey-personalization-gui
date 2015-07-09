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

#include "toolpage.h"
#include "yubikeyfinder.h"
#include "yubikeywriter.h"
#include "yubikeyutil.h"
#include "ui_toolpage.h"
#include "ui/helpbox.h"
#include "ui/confirmbox.h"
#include "mainwindow.h"
#include "otppage.h"
#include "chalresppage.h"
#include "oathpage.h"
#include "staticpage.h"

#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <QSignalMapper>
#include <QClipboard>
#include <QApplication>

#include <ykpers.h>
#include <ykdef.h>

#include "common.h"

#define IMPORT_FILENAME_DEF "import.ycfg"

QString ToolPage::m_filename = defaultImportFilename();

ToolPage::ToolPage(QWidget *parent) :
        QStackedWidget(parent),
        ui(new Ui::ToolPage)
{
    ui->setupUi(this);

    //Connect pages
    connectPages();

    //Connect help buttons
    connectHelpButtons();

    //Connect other signals and slots
    connect(ui->converterResetBtn, SIGNAL(clicked()),
            this, SLOT(resetConverterPage()));
    connect(ui->chalRespResetBtn, SIGNAL(clicked()),
            this, SLOT(resetChalRespPage()));
    connect(ui->chalRespPerformBtn, SIGNAL(clicked()),
            this, SLOT(performChallengeResponse()));
    connect(ui->chalRespChallenge, SIGNAL(editingFinished()),
            this, SLOT(on_chalRespChallenge_editingFinished()));
    connect(ui->ndefResetBtn, SIGNAL(clicked()),
            this, SLOT(resetNdefPage()));
    connect(ui->ndefProgramBtn, SIGNAL(clicked()),
            this, SLOT(programNdef()));

    connect(YubiKeyFinder::getInstance(), SIGNAL(keyFound(bool, bool*, int)),
            this, SLOT(keyFound(bool, bool*)));

    ui->zapAccCodeEdit->setEnabled(false);
    ui->ndefAccCodeEdit->setEnabled(false);

    ui->importBox->setVisible(false);

    loadSettings();
}

ToolPage::~ToolPage() {
    delete ui;
}

/*
 Common
*/

void ToolPage::connectPages() {
    //Map the values of the navigation buttons with the indexes of
    //the stacked widget

    //Create a QMapper
    QSignalMapper *mapper = new QSignalMapper(this);

    //Connect the clicked signal with the QSignalMapper
    connect(ui->converterBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->converterBackBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    connect(ui->chalRespBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->chalRespBackBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    connect(ui->ndefBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->ndefBackBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    connect(ui->zapBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->zapBackBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    connect(ui->importBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->importBackBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    //Set a value for each button
    mapper->setMapping(ui->converterBtn, Page_Converter);
    mapper->setMapping(ui->converterBackBtn, Page_Base);

    mapper->setMapping(ui->chalRespBtn, Page_ChalResp);
    mapper->setMapping(ui->chalRespBackBtn, Page_Base);

    mapper->setMapping(ui->ndefBtn, Page_Ndef);
    mapper->setMapping(ui->ndefBackBtn, Page_Base);

    mapper->setMapping(ui->zapBtn, Page_Zap);
    mapper->setMapping(ui->zapBackBtn, Page_Base);

    mapper->setMapping(ui->importBtn, Page_Import);
    mapper->setMapping(ui->importBackBtn, Page_Base);

    //Connect the mapper to the widget
    //The mapper will set a value to each button and
    //set that value to the widget
    //connect(pageMapper, SIGNAL(mapped(int)), this, SLOT(setCurrentIndex(int)));
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(setCurrentPage(int)));

    //Set the current page
    m_currentPage = 0;
    setCurrentIndex(Page_Base);
}

void ToolPage::loadSettings() {
    QSettings settings;
    ui->importBox->setVisible(settings.value(SG_EXPORT_PREFERENCE).toBool());
}

void ToolPage::setCurrentPage(int pageIndex) {
    //Page changed...

    m_currentPage = pageIndex;

    switch(pageIndex){
    case Page_Converter:
        resetConverterPage();
        break;
    }

    setCurrentIndex(pageIndex);
}

void ToolPage::connectHelpButtons() {
}

void ToolPage::resetChalRespPage() {
    ui->chalRespChallenge->clear();
    ui->chalRespResponse->clear();
}

void ToolPage::on_chalRespChallenge_editingFinished() {
    QString challenge = ui->chalRespChallenge->text().trimmed();
    ui->chalRespChallenge->setText(challenge);
}

void ToolPage::performChallengeResponse() {
    QString challenge = ui->chalRespChallenge->text();
    QString response = "";
    bool hmac;
    int slot;
    if(ui->chalRespHmacRadio->isChecked()) {
        hmac = true;
    } else if(ui->chalRespYubicoRadio->isChecked()) {
        hmac = false;
    } else {
      emit showStatusMessage(tr(ERR_CHAL_TYPE_NOT_SELECTED), 1);
      return;
    }
    if(ui->chalRespSlot1Radio->isChecked()) {
        slot = 1;
    } else if(ui->chalRespSlot2Radio->isChecked()) {
        slot = 2;
    } else {
      emit showStatusMessage(tr(ERR_CONF_SLOT_NOT_SELECTED), 1);
      return;
    }
    YubiKeyWriter::getInstance()->doChallengeResponse(challenge, response, slot, hmac);
    qDebug() << "response was: " << response;
    ui->chalRespResponse->setText(response);
}
/*
 Quick Page handling
*/
void ToolPage::resetConverterPage() {
    convert(0, "");
    ui->converterHexTxt->setCursorPosition(0);
    ui->converterHexTxt->setFocus();
}

void ToolPage::convert(int updatedIndex, QString txt) {
    unsigned char buf[32];
    memset(buf, 0, sizeof(buf));
    size_t bufLen;

    switch(updatedIndex) {
    case 0: //Hex
        YubiKeyUtil::qstrHexDecode(buf, &bufLen, txt);
        break;

    case 1: //Modhex
        YubiKeyUtil::qstrModhexDecode(buf, &bufLen, txt);
        break;

    case 2: //Decimal
        QString tmp = QString::number(txt.toULongLong(), 16);
        size_t len = tmp.length();
        if(len % 2 != 0) {
            len++;
        }
        YubiKeyUtil::qstrClean(&tmp, (size_t)len, true);
        YubiKeyUtil::qstrHexDecode(buf, &bufLen, tmp);
        break;
    }

    QString hex = YubiKeyUtil::qstrHexEncode(buf, bufLen);
    QString modhex = YubiKeyUtil::qstrModhexEncode(buf, bufLen);
    bool ok = false;
    qulonglong dec = hex.toULongLong(&ok, 16);

    int hexLen = hex.length();
    int modhexLen = modhex.length();

    ui->converterHexTxt->setText(hex);
    ui->converterModhexTxt->setText(modhex);
    ui->converterDecTxt->setText(QString::number(dec));

    ui->converterHexCopyBtn->setEnabled(hexLen > 0);
    ui->converterModhexCopyBtn->setEnabled(modhexLen > 0);
    ui->converterDecCopyBtn->setEnabled(
            ui->converterDecTxt->text().length() > 0);

    ui->converterHexLenLbl->setText(tr("(%1 chars)").arg(hexLen));
    ui->converterModhexLenLbl->setText(tr("(%1 chars)").arg(modhexLen));

    if(hexLen != 0 && !ok) {
        ui->converterDecErrLbl->setText(tr(TOVERFLOW));
    } else {
        ui->converterDecErrLbl->setText(tr(""));
    }
}

void ToolPage::on_converterHexTxt_editingFinished() {
    QString txt = ui->converterHexTxt->text();
    YubiKeyUtil::qstrClean(&txt, 0, true);

    size_t len = txt.length();
    if(len > 0) {
        if(len % 2 != 0) {
            len++;
        }
        YubiKeyUtil::qstrClean(&txt, (size_t)len, true);
        convert(0, txt);
    }
    ui->converterHexTxt->setCursorPosition(len + len/2);
}

void ToolPage::on_converterModhexTxt_editingFinished() {
    QString txt = ui->converterModhexTxt->text();
    YubiKeyUtil::qstrModhexClean(&txt, 0, true);

    size_t len = txt.length();
    if(len > 0) {
        if(len % 2 != 0) {
            len++;
        }
        YubiKeyUtil::qstrModhexClean(&txt, (size_t)len, true);
        convert(1, txt);
    }
    ui->converterModhexTxt->setCursorPosition(len + len/2);
}

void ToolPage::on_converterDecTxt_editingFinished() {
    QString txt = ui->converterDecTxt->text();
    bool ok = false;
    qulonglong dec = txt.toULongLong(&ok);
    if(ok) {
        if(dec > 0) {
            size_t len = txt.length();
            YubiKeyUtil::qstrClean(&txt, (size_t)len, true);
            convert(2, txt);
            ui->converterDecTxt->setCursorPosition(len);
        }
    } else {
        ui->converterDecErrLbl->setText(tr(TOVERFLOW));
    }

}

void ToolPage::copyToClipboard(const QString &str) {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(str);

    showStatusMessage(tr(VALUE_COPIED), 0);
}

void ToolPage::on_converterHexCopyBtn_clicked() {
    QString txt = ui->converterHexTxt->text();
    YubiKeyUtil::qstrClean(&txt, 0, true);

    copyToClipboard(txt);
}

void ToolPage::on_converterModhexCopyBtn_clicked() {
    QString txt = ui->converterModhexTxt->text();
    YubiKeyUtil::qstrModhexClean(&txt, 0, true);

    copyToClipboard(txt);
}

void ToolPage::on_converterDecCopyBtn_clicked() {
    copyToClipboard(ui->converterDecTxt->text());
}

void ToolPage::resetNdefPage() {
    ui->ndefEdit->setText("https://my.yubico.com/neo/");
    ui->ndefTextLangEdit->setText("en-US");
    ui->ndefUriRadio->setChecked(true);
    ui->ndefAccCodeCheckbox->setChecked(false);
    ui->ndefSlot1Radio->setChecked(false);
    ui->ndefSlot2Radio->setChecked(false);
}

void ToolPage::programNdef() {
    QSettings settings;
    YubiKeyWriter *writer = YubiKeyWriter::getInstance();
    bool uri = true;
    QString language;
    QString payload;
    int slot;
    if(ui->ndefTextRadio->isChecked()) {
        uri = false;
        language = ui->ndefTextLangEdit->text().trimmed();
        if(language.isEmpty()) {
            return;
        }
    }
    payload = ui->ndefEdit->text().trimmed();
    if(payload.isEmpty()) {
        return;
    }

    if(uri && !settings.value(SG_NDEF_WITHOUT_HTTP).toBool()) {
        if(!payload.startsWith("http")) {
            ConfirmBox confirm(this);
            confirm.setConfirmIndex(ConfirmBox::Confirm_NdefWithoutHttp);
            int ret = confirm.exec();
            if(ret != 1) // 1 is yes
                return;
        }
    }

    if(ui->ndefSlot1Radio->isChecked()) {
        slot = 1;
    } else if(ui->ndefSlot2Radio->isChecked()) {
        slot = 2;
    } else {
        emit showStatusMessage(tr(ERR_CONF_SLOT_NOT_SELECTED), 1);
        return;
    }

    connect(writer, SIGNAL(configWritten(bool, const QString &)),
            this, SLOT(ndefWritten(bool, const QString &)));
    writer->writeNdef(uri, language, payload, ui->ndefAccCodeEdit->text().remove(" "), slot);
}

void ToolPage::ndefWritten(bool written, __attribute__((unused)) const QString &msg) {
    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
            this, SLOT(ndefWritten(bool, const QString &)));
    if(written) {
        showStatusMessage(tr("NDEF successfully written"));
    }
}

void ToolPage::on_ndefTextRadio_toggled(bool checked) {
    if(checked) {
        ui->ndefTextLangEdit->setEnabled(true);
        ui->ndefEdit->setText("");
    } else {
        ui->ndefTextLangEdit->setText("en-US");
        ui->ndefTextLangEdit->setEnabled(false);
        ui->ndefEdit->setText("https://my.yubico.com/neo/");
    }
}

void ToolPage::on_ndefAccCodeCheckbox_toggled(bool checked) {
    ui->ndefAccCodeEdit->setText("00 00 00 00 00 00");
    ui->ndefAccCodeEdit->setEnabled(checked);
    ui->ndefUseSerial->setChecked(false);
    ui->ndefUseSerial->setEnabled(checked);
}

void ToolPage::on_ndefUseSerial_toggled(bool checked) {
    if(checked) {
        setSerial(ui->ndefAccCodeEdit);
    }
}

void ToolPage::on_zapPerformBtn_clicked() {
    int slot;
    if(ui->zapSlot1Radio->isChecked()) {
        slot = 1;
    } else if(ui->zapSlot2Radio->isChecked()) {
        slot = 2;
    } else {
      emit showStatusMessage(tr(ERR_CONF_SLOT_NOT_SELECTED), 1);
      return;
    }

    YubiKeyWriter *writer = YubiKeyWriter::getInstance();
    connect(writer, SIGNAL(configWritten(bool, const QString &)),
            this, SLOT(zapDone(bool, const QString &)));
    writer->deleteConfig(slot, ui->zapAccCodeEdit->text().remove(" "));
}

void ToolPage::zapDone(bool written, __attribute__((unused)) const QString &msg) {
    disconnect(YubiKeyWriter::getInstance(), SIGNAL(configWritten(bool, const QString &)),
            this, SLOT(zapDone(bool, const QString &)));
    if(written) {
        showStatusMessage(tr("Configuration successfully deleted."));
    }
}

void ToolPage::on_zapAccCodeCheckbox_toggled(bool checked) {
    ui->zapAccCodeEdit->setText("00 00 00 00 00 00");
    ui->zapAccCodeEdit->setEnabled(checked);
    ui->zapUseSerial->setChecked(false);
    ui->zapUseSerial->setEnabled(checked);
}

void ToolPage::on_zapUseSerial_toggled(bool checked) {
    if(checked) {
        setSerial(ui->zapAccCodeEdit);
    }
}

void ToolPage::on_importPerformBtn_clicked() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Open file for import"), m_filename, tr("Yubico cfg format (*.ycfg);;All Files (*.*)"));
    if(filename.isEmpty()) {
        return;
    }
    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        showStatusMessage(tr("Failed to open selected file."), 1);
    }

    char data[1024];
    int len = file.read(data, 1024);
    if(!len) {
        showStatusMessage(tr("Failed to read from selected file."), 1);
    }
    file.close();

    YKP_CONFIG *cfg = ykp_alloc();
    YK_STATUS *ykds = YubiKeyFinder::getInstance()->status();
    ykp_configure_version(cfg, ykds);
    int ret = ykp_import_config(cfg, data, len, YKP_FORMAT_YCFG);
    if(ret) {
        QSettings settings;
        m_filename = filename;
        settings.setValue(SG_IMPORT_FILENAME, filename);

        MainWindow::Page page = MainWindow::Page_Otp;
        int tab = OtpPage::Page_Advanced;
        if(ykp_get_tktflag_OATH_HOTP(cfg)) {
            if(ykp_get_cfgflag_CHAL_HMAC(cfg)) {
                qDebug() << "importing mode chal-resp hmac";
                page = MainWindow::Page_ChalResp;
                tab = ChalRespPage::Page_Advanced;
                settings.setValue(SG_REQUIRE_INPUT, ykp_get_cfgflag_CHAL_BTN_TRIG(cfg));
                settings.setValue(SG_HMAC_LT64, ykp_get_cfgflag_HMAC_LT64(cfg));
            } else if(ykp_get_cfgflag_CHAL_YUBICO(cfg)) {
                qDebug() << "importing mode chal-resp yubico";
                page = MainWindow::Page_ChalResp;
                tab = ChalRespPage::Page_Quick;
                settings.setValue(SG_REQUIRE_INPUT, ykp_get_cfgflag_CHAL_BTN_TRIG(cfg));
            } else {
                qDebug() << "importing mode oath hotp";
                page = MainWindow::Page_Oath;
                tab = OathPage::Page_Advanced;
                settings.setValue(SG_OATH_HOTP8, ykp_get_cfgflag_OATH_HOTP8(cfg));
                // XXX: handle seed and fixed_modhex
            }
        } else if(ykp_get_cfgflag_STATIC_TICKET(cfg)) {
            qDebug() << "importing mode static";
            page = MainWindow::Page_Static;
            tab = StaticPage::Page_Advanced;
            settings.setValue(SG_STRONG_PW1, ykp_get_cfgflag_STRONG_PW1(cfg));
            if(ykp_get_cfgflag_STRONG_PW2(cfg)) {
                settings.setValue(SG_STRONG_PW2, true);
                settings.setValue(SG_STRONG_PW3, ykp_get_cfgflag_SEND_REF(cfg));
            } else {
                settings.setValue(SG_STRONG_PW2, false);
            }
        } else {
            qDebug() << "importing yubico otp";
        }

        settings.setValue(SG_MAN_UPDATE, ykp_get_cfgflag_MAN_UPDATE(cfg));
        settings.setValue(SG_PACING_10MS, ykp_get_cfgflag_PACING_10MS(cfg));
        settings.setValue(SG_PACING_20MS, ykp_get_cfgflag_PACING_20MS(cfg));
        settings.setValue(SG_APPEND_CR, ykp_get_tktflag_APPEND_CR(cfg));
        settings.setValue(SG_APPEND_DELAY1, ykp_get_tktflag_APPEND_DELAY1(cfg));
        settings.setValue(SG_APPEND_DELAY2, ykp_get_tktflag_APPEND_DELAY2(cfg));
        settings.setValue(SG_APPEND_TAB1, ykp_get_tktflag_APPEND_TAB1(cfg));
        settings.setValue(SG_APPEND_TAB2, ykp_get_tktflag_APPEND_TAB2(cfg));
        settings.setValue(SG_TAB_FIRST, ykp_get_tktflag_TAB_FIRST(cfg));
        settings.setValue(SG_SR_BTN_VISIBLE, ykp_get_extflag_SERIAL_BTN_VISIBLE(cfg));
        settings.setValue(SG_SR_USB_VISIBLE, ykp_get_extflag_SERIAL_USB_VISIBLE(cfg));
        settings.setValue(SG_SR_API_VISIBLE, ykp_get_extflag_SERIAL_API_VISIBLE(cfg));
        settings.setValue(SG_USE_NUMERIC_KEYPAD, ykp_get_extflag_USE_NUMERIC_KEYPAD(cfg));
        settings.setValue(SG_FAST_TRIG, ykp_get_extflag_FAST_TRIG(cfg));
        settings.setValue(SG_ALLOW_UPDATE, ykp_get_extflag_ALLOW_UPDATE(cfg));
        settings.setValue(SG_LED_INVERT, ykp_get_extflag_LED_INV(cfg));

        int config = ykp_config_num(cfg);

        emit switchPage(page, tab, config);
        emit reloadSettings();
    } else {
        showStatusMessage(tr("Failed to parse the configuration."), 1);
    }
    ykp_free_config(cfg);
}

void ToolPage::keyFound(bool found, bool* featuresMatrix) {
    if(found && featuresMatrix[YubiKeyFinder::Feature_ChallengeResponse]) {
        ui->chalRespPerformBtn->setEnabled(true);
    } else {
        ui->chalRespPerformBtn->setEnabled(false);
    }
    if(found && featuresMatrix[YubiKeyFinder::Feature_Ndef]) {
        ui->ndefProgramBtn->setEnabled(true);
    } else {
        ui->ndefProgramBtn->setEnabled(false);
    }
    if(found && featuresMatrix[YubiKeyFinder::Feature_MultipleConfigurations]) {
        ui->chalRespSlot2Radio->setEnabled(true);
        ui->zapSlot2Radio->setEnabled(true);
        ui->ndefSlot2Radio->setEnabled(true);
    } else {
        ui->chalRespSlot2Radio->setEnabled(false);
        ui->zapSlot2Radio->setEnabled(false);
        ui->ndefSlot2Radio->setEnabled(false);
    }
    ui->zapPerformBtn->setEnabled(found);
    ui->importPerformBtn->setEnabled(found);

    if(found) {
        m_serial = QString::number(YubiKeyFinder::getInstance()->serial());
        int num = 12 - m_serial.length();
        for(int i = 0; i < num; i++) {
            m_serial.prepend("0");
        }
        if(!m_serial.isEmpty()) {
            if(ui->ndefUseSerial->isChecked()) {
                setSerial(ui->ndefAccCodeEdit);
            }
            if(ui->zapUseSerial->isChecked()) {
                setSerial(ui->zapAccCodeEdit);
            }
        }
    } else {
        m_serial.clear();
    }
}

void ToolPage::setImportFilename(QString filename) {
        m_filename = filename;
}

QString ToolPage::defaultImportFilename() {
    return QDir::homePath() + "/" + IMPORT_FILENAME_DEF;
}

void ToolPage::setSerial(QLineEdit *line) {
    if(!m_serial.isEmpty()) {
        line->setText(m_serial);
    }
}
