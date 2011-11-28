/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#include "toolpage.h"
#include "ui_toolpage.h"
#include "ui/helpbox.h"

#include "common.h"

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
    //connect(ui->converterBackBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    //Set a value for each button
    mapper->setMapping(ui->converterBtn, Page_Converter);
    //mapper->setMapping(ui->converterBackBtn, Page_Base);

    //Connect the mapper to the widget
    //The mapper will set a value to each button and
    //set that value to the widget
    //connect(pageMapper, SIGNAL(mapped(int)), this, SLOT(setCurrentIndex(int)));
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(setCurrentPage(int)));

    //Set the current page
    m_currentPage = 0;
    setCurrentIndex(Page_Converter);
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

void ToolPage::helpBtn_pressed(int helpIndex) {
    HelpBox help(this);
    help.setHelpIndex((HelpBox::Help)helpIndex);
    help.exec();
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
    unsigned char buf[16];
    memset(buf, 0, sizeof(buf));
    size_t bufLen = 0;

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
        ui->converterDecErrLbl->setText(TOVERFLOW);
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
        ui->converterDecErrLbl->setText(TOVERFLOW);
    }

}

void ToolPage::copyToClipboard(const QString &str) {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(str);

    showStatusMessage(VALUE_COPIED, 0);
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
