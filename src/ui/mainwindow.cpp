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

#include "ui/mainwindow.h"
#include "ui/ui_mainwindow.h"
#include "yubikeyfinder.h"
#include "yubikeywriter.h"
#include "yubikeyutil.h"
#include "yubikeylogger.h"
#include "ui/otppage.h"
#include "ui/oathpage.h"
#include "ui/staticpage.h"
#include "ui/chalresppage.h"
#include "ui/toolpage.h"
#include "ui/settingpage.h"
#include "ui/aboutpage.h"
#include "ui/diagnostics.h"
#include "ui/helpbox.h"

#include <QMovie>
#include <QClipboard>
#include <QDebug>
#include <QSignalMapper>
#include <QSettings>
#include <QAction>
#include <QApplication>

#include "common.h"
#include "version.h"

#define TICKMAP         QPixmap(":/res/images/tick.png")
#define CROSSMAP        QPixmap(":/res/images/cross.png")
#define BLANKMAP        QPixmap(":/res/images/blank.png")
#define COPYMAP         QPixmap(":/res/images/clipboard.png")

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(900, 710);
    m_timer = 0;

    //Create pages
    createPages();

    //Connect other signals and slots
    connect(ui->exitMenuBtn, SIGNAL(clicked()), qApp, SLOT(quit()));

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));

    connect(YubiKeyFinder::getInstance(), SIGNAL(keyFound(bool, bool*, int)),
            this, SLOT(keyFound(bool, bool*, int)));
    connect(YubiKeyWriter::getInstance(), SIGNAL(errorOccurred(QString)),
            this, SLOT(showStatusMessage(QString)));

    connect(m_otpPage, SIGNAL(showStatusMessage(const QString &,int)),
            this, SLOT(showStatusMessage(const QString &,int)));
    connect(m_oathPage, SIGNAL(showStatusMessage(const QString &,int)),
            this, SLOT(showStatusMessage(const QString &,int)));
    connect(m_staticPage, SIGNAL(showStatusMessage(const QString &,int)),
            this, SLOT(showStatusMessage(const QString &,int)));
    connect(m_chalRespPage, SIGNAL(showStatusMessage(const QString &,int)),
            this, SLOT(showStatusMessage(const QString &,int)));
    connect(m_settingPage, SIGNAL(showStatusMessage(const QString &,int)),
            this, SLOT(showStatusMessage(const QString &,int)));
    connect(m_toolPage, SIGNAL(showStatusMessage(const QString &,int)),
            this, SLOT(showStatusMessage(const QString &,int)));
    connect(m_aboutPage, SIGNAL(changePage(int)),
            this, SLOT(setCurrentPage(int)));

    connect(m_otpPage, SIGNAL(showHelp(int)), this, SLOT(triggerHelp(int)));
    connect(m_oathPage, SIGNAL(showHelp(int)), this, SLOT(triggerHelp(int)));
    connect(m_staticPage, SIGNAL(showHelp(int)), this, SLOT(triggerHelp(int)));
    connect(m_chalRespPage, SIGNAL(showHelp(int)), this, SLOT(triggerHelp(int)));
    connect(m_settingPage, SIGNAL(showHelp(int)), this, SLOT(triggerHelp(int)));
    connect(m_toolPage, SIGNAL(showHelp(int)), this, SLOT(triggerHelp(int)));

    connect(m_settingPage, SIGNAL(settingsChanged()),
            m_otpPage, SLOT(loadSettings()));
    connect(m_settingPage, SIGNAL(settingsChanged()),
            m_oathPage, SLOT(loadSettings()));
    connect(m_settingPage, SIGNAL(settingsChanged()),
            m_chalRespPage, SLOT(loadSettings()));
    connect(m_settingPage, SIGNAL(settingsChanged()),
            m_staticPage, SLOT(loadSettings()));
    connect(m_settingPage, SIGNAL(settingsChanged()),
            m_toolPage, SLOT(loadSettings()));

    connect(m_toolPage, SIGNAL(reloadSettings()),
            m_settingPage, SLOT(reloadSettings()));

    connect(m_toolPage, SIGNAL(switchPage(int, int, int)),
            this, SLOT(setCurrentPage(int, int, int)));

    //Intialize settings
    m_settingPage->init();

    //Start YubiKey finder
    YubiKeyFinder::getInstance()->start();

    // add action for toggling animation
    animationAction = new QAction(this);
    animationAction->setText("Device image animated");
    animationAction->setCheckable(true);
    connect(animationAction, SIGNAL(triggered(bool)), this, SLOT(toggleAnimation(bool)));
    ui->deviceImage->addAction(animationAction);

    // add action for diagnostics
    diagnosticsAction = new QAction(this);
    diagnosticsAction->setText("Show detailed diagnostics");
    connect(diagnosticsAction, SIGNAL(triggered()), this, SLOT(triggerDiagnostics()));
    ui->logoImg->addAction(diagnosticsAction);

    QSettings settings;
    if(settings.value(SG_ANIMATIONS_PREFERENCE, true).toBool()) {
        animationAction->setChecked(true);
    } else {
        animationAction->setChecked(false);
    }
}

MainWindow::~MainWindow()
{
    if (m_timer != 0) {
        delete m_timer;
    }
    delete m_otpPage;
    delete m_oathPage;
    delete m_staticPage;
    delete m_chalRespPage;
    delete m_settingPage;
    delete m_toolPage;
    delete m_aboutPage;

    delete m_diagnostics;
    delete m_help;

    delete animationAction;
    delete diagnosticsAction;

    delete ui;
}

/*
 Menu Widget handling
*/

void MainWindow::createPages() {
    //Create pages
    m_otpPage = new OtpPage(this);
    m_oathPage = new OathPage(this);
    m_staticPage = new StaticPage(this);
    m_chalRespPage = new ChalRespPage(this);
    m_settingPage = new SettingPage(this);
    m_toolPage = new ToolPage(this);
    m_aboutPage = new AboutPage(this);

    m_diagnostics = new Diagnostics(this);
    m_help = new HelpBox(this);

    //Add pages to the pages widget
    ui->pagesWidget->addWidget(m_otpPage);
    ui->pagesWidget->addWidget(m_oathPage);
    ui->pagesWidget->addWidget(m_staticPage);
    ui->pagesWidget->addWidget(m_chalRespPage);
    ui->pagesWidget->addWidget(m_settingPage);
    ui->pagesWidget->addWidget(m_toolPage);
    ui->pagesWidget->addWidget(m_aboutPage);

    //Map the values of menu buttons with the indexes of the pages widget

    //Create a QMapper
    QSignalMapper *mapper = new QSignalMapper(this);

    //Connect the clicked signal with the QSignalMapper
    connect(ui->otpMenuBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->oathHotpMenuBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->staticMenuBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->chalRespMenuBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->settingsMenuBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->toolsMenuBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->aboutMenuBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    //Set a value for each button
    mapper->setMapping(ui->otpMenuBtn, Page_Otp);
    mapper->setMapping(ui->oathHotpMenuBtn, Page_Oath);
    mapper->setMapping(ui->staticMenuBtn, Page_Static);
    mapper->setMapping(ui->chalRespMenuBtn, Page_ChalResp);
    mapper->setMapping(ui->settingsMenuBtn, Page_Settings);
    mapper->setMapping(ui->toolsMenuBtn, Page_Tools);
    mapper->setMapping(ui->aboutMenuBtn, Page_About);

    //Connect the mapper to the pages widget
    //The mapper will set a value to each button and set that value to
    //the pages widget
    //connect(mapper, SIGNAL(mapped(int)), ui->pagesWidget, SLOT(setCurrentIndex(int)));
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(setCurrentPage(int)));

    //Set the current page
    m_currentPage = 0;
    setCurrentPage(Page_About);
}

void MainWindow::setCurrentPage(int pageIndex, int tab, int slot) {
    // Page changed...

    // Clear status message
    clearStatusMessage();

    // Toggle menu buttons
    QString disabledMenuBtnSS = QString::fromUtf8(SS_MENU_DISABLED);
    QString checkedMenuBtnSS = QString::fromUtf8(SS_MENU_CHECKED);
    QString uncheckedMenuBtnSS = QString::fromUtf8(SS_MENU_UNCHECKED);

    switch(m_currentPage){
    case Page_Otp:
        ui->otpMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
        break;
    case Page_Oath:
        if(m_oathPage->isEnabled()) {
            ui->oathHotpMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
        } else {
            ui->oathHotpMenuBtn->setStyleSheet(disabledMenuBtnSS);
        }
        break;
    case Page_Static:
        if(m_staticPage->isEnabled()) {
            ui->staticMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
        } else {
            ui->staticMenuBtn->setStyleSheet(disabledMenuBtnSS);
        }
        break;
    case Page_ChalResp:
        if(m_chalRespPage->isEnabled()) {
            ui->chalRespMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
        } else {
            ui->chalRespMenuBtn->setStyleSheet(disabledMenuBtnSS);
        }
        break;
    case Page_Settings:
        ui->settingsMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
        break;
    case Page_Tools:
        ui->toolsMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
        break;
    case Page_About:
        ui->aboutMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
        break;
    }

    switch(pageIndex){
    case Page_Otp:
        ui->otpMenuBtn->setStyleSheet(checkedMenuBtnSS);
        if(tab) m_otpPage->setCurrentPage(tab);
        if(slot) m_otpPage->setCurrentSlot(slot);
        break;
    case Page_Oath:
        ui->oathHotpMenuBtn->setStyleSheet(checkedMenuBtnSS);
        if(tab) m_oathPage->setCurrentPage(tab);
        if(slot) m_oathPage->setCurrentSlot(slot);
        break;
    case Page_Static:
        ui->staticMenuBtn->setStyleSheet(checkedMenuBtnSS);
        if(tab) m_staticPage->setCurrentPage(tab);
        if(slot) m_staticPage->setCurrentSlot(slot);
        break;
    case Page_ChalResp:
        ui->chalRespMenuBtn->setStyleSheet(checkedMenuBtnSS);
        if(tab) m_chalRespPage->setCurrentPage(tab);
        if(slot) m_chalRespPage->setCurrentSlot(slot);
        break;
    case Page_Settings:
        ui->settingsMenuBtn->setStyleSheet(checkedMenuBtnSS);
        if(tab) m_settingPage->setCurrentPage(tab);
        break;
    case Page_Tools:
        ui->toolsMenuBtn->setStyleSheet(checkedMenuBtnSS);
        if(tab) m_toolPage->setCurrentPage(tab);
        break;
    case Page_About:
        ui->aboutMenuBtn->setStyleSheet(checkedMenuBtnSS);
        break;
    }

    m_currentPage = pageIndex;

    ui->pagesWidget->setCurrentIndex(pageIndex);
}

/*
 Device Widget handling
*/

void MainWindow::resetDeviceInfo() {
    ui->statusLbl->clear();
    ui->versionLbl->setText(tr(NA));

    QString blankBtnSS = QString::fromUtf8(SS_BLANK_BTN);
    ui->serialNoDecLbl->setText(tr(NA));
    ui->serialNoHexLbl->setText(tr(NA));
    ui->serialNoModhexLbl->setText(tr(NA));
    ui->serialNoDecCopyBtn->setStyleSheet(blankBtnSS);
    ui->serialNoHexCopyBtn->setStyleSheet(blankBtnSS);
    ui->serialNoModhexCopyBtn->setStyleSheet(blankBtnSS);

    ui->otpSupportLbl->setPixmap(QPixmap());
    ui->otpSupportLbl->setText(tr(NA));
    ui->multiConfigSupportLbl->setPixmap(QPixmap());
    ui->multiConfigSupportLbl->setText(tr(NA));
    ui->oathHotpSupportLbl->setPixmap(QPixmap());
    ui->oathHotpSupportLbl->setText(tr(NA));
    ui->staticPwdSupportLbl->setPixmap(QPixmap());
    ui->staticPwdSupportLbl->setText(tr(NA));
    ui->scanCodeSupportLbl->setPixmap(QPixmap());
    ui->scanCodeSupportLbl->setText(tr(NA));
    ui->chalRespSupportLbl->setPixmap(QPixmap());
    ui->chalRespSupportLbl->setText(tr(NA));
    ui->updatableSupportLbl->setPixmap(QPixmap());
    ui->updatableSupportLbl->setText(tr(NA));
    ui->ndefSupportLbl->setPixmap(QPixmap());
    ui->ndefSupportLbl->setText(tr(NA));
    ui->u2fSupportLbl->setPixmap(QPixmap());
    ui->u2fSupportLbl->setText(tr(NA));
}

void MainWindow::keyFound(bool found, bool* featuresMatrix, int error) {
    QString disabledMenuBtnSS = QString::fromUtf8(SS_MENU_DISABLED);
    QString checkedMenuBtnSS = QString::fromUtf8(SS_MENU_CHECKED);
    QString uncheckedMenuBtnSS = QString::fromUtf8(SS_MENU_UNCHECKED);
    QString copyBtnSS = QString::fromUtf8(SS_COPY_BTN);
    QString blankBtnSS = QString::fromUtf8(SS_BLANK_BTN);

    resetDeviceInfo();

    if(ui->deviceImage->movie()) {
      delete(ui->deviceImage->movie());
      ui->deviceImage->setMovie(NULL);
    }

    if(found) {
        YubiKeyFinder *finder = YubiKeyFinder::getInstance();
        int touchLevel = finder->touchLevel();
        qDebug() << "touchLevel is " << touchLevel;
        if(touchLevel & CONFIG1_VALID && touchLevel & CONFIG2_VALID) {
            ui->programLbl->setText(tr("Slot 1 and 2 configured"));
        } else if(touchLevel & CONFIG1_VALID) {
            ui->programLbl->setText(tr("Slot 1 configured"));
        } else if(touchLevel & CONFIG2_VALID) {
            ui->programLbl->setText(tr("Slot 2 configured"));
        } else {
            ui->programLbl->setText(tr("Not configured"));
        }

        unsigned int version = finder->version();
        if(error == ERR_UNKNOWN_FIRMWARE) {
            ui->statusLbl->setStyleSheet(QString::fromUtf8(SS_YKSTATUS_ERROR));
            ui->statusLbl->setText(tr(UNKNOWN_FIRMWARE));
        } else {
            ui->statusLbl->setStyleSheet(QString::fromUtf8(SS_YKSTATUS_SUCCESS));
            ui->statusLbl->setText(tr(KEY_FOUND));
        }

        ui->versionLbl->setText(finder->versionStr());
        qDebug() << "version is" << finder->versionStr();

        QPixmap pixmap;
        QMovie *movie = new QMovie();
        if(version == 0) {
            pixmap.load(":/res/images/blank.png");
        } else if(version < YK_VERSION(2,0,0)) {
            pixmap.load(":/res/images/v1-3-not-animated.png");
        } else if(version < YK_VERSION(2,1,4)) {
            movie->setFileName(":/res/images/V2-0-2-1-animated.mng");
        } else if(version < YK_VERSION(2,2,0)) { // NEO Beta
            pixmap.load(":/res/images/neo_transparent.png");
        } else if(version < YK_VERSION(2,3,0)){
            movie->setFileName(":/res/images/v2-2-animated.mng");
        } else if(version < YK_VERSION(3,0,0)){
            movie->setFileName(":/res/images/v2-3-animated.mng");
        } else if(version < YK_VERSION(3,3,0)){
            pixmap.load(":/res/images/neo_production.png");
        } else if(version < YK_VERSION(4,0,0)) {
            movie->setFileName(":/res/images/neo_production_33.mng");
        } else if(version < YK_VERSION(4,1,0)) {
            movie->setFileName(":/res/images/plus.mng");
        } else if(version < YK_VERSION(4,9,9)) {
            movie->setFileName(":/res/images/edge.mng");
        } else {
            pixmap.load(":/res/images/blank.png");
        }
        if(pixmap.isNull()) {
            ui->deviceImage->setMovie(movie);
            movie->start();
            if(!animationAction->isChecked()) {
                movie->stop();
            }
        } else {
            delete movie;
            ui->deviceImage->setPixmap(pixmap);
        }

        unsigned int serial = 0;
        if(featuresMatrix[YubiKeyFinder::Feature_SerialNumber]) {
            serial = YubiKeyFinder::getInstance()->serial();
        }
        if(serial != 0) {
            ui->serialNoDecLbl->setText(QString::number(serial));

            //Convert serial number in hex and modhex
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

            QString hex = YubiKeyUtil::qstrHexEncode(buf, bufLen);
            QString modhex = YubiKeyUtil::qstrModhexEncode(buf, bufLen);

            ui->serialNoHexLbl->setText(hex);
            ui->serialNoModhexLbl->setText(modhex);

            ui->serialNoDecCopyBtn->setStyleSheet(copyBtnSS);
            ui->serialNoHexCopyBtn->setStyleSheet(copyBtnSS);
            ui->serialNoModhexCopyBtn->setStyleSheet(copyBtnSS);
        } else {
            ui->serialNoDecLbl->setText(tr(NA));
            ui->serialNoHexLbl->setText(tr(NA));
            ui->serialNoModhexLbl->setText(tr(NA));
            ui->serialNoDecCopyBtn->setStyleSheet(blankBtnSS);
            ui->serialNoHexCopyBtn->setStyleSheet(blankBtnSS);
            ui->serialNoModhexCopyBtn->setStyleSheet(blankBtnSS);
        }

        ui->otpSupportLbl->setPixmap(TICKMAP);

        if(featuresMatrix[YubiKeyFinder::Feature_MultipleConfigurations]) {
            ui->multiConfigSupportLbl->setPixmap(TICKMAP);
        } else {
            ui->multiConfigSupportLbl->setPixmap(CROSSMAP);
        }

        if(featuresMatrix[YubiKeyFinder::Feature_OathHotp]) {
            ui->oathHotpSupportLbl->setPixmap(TICKMAP);
            if(m_currentPage == Page_Oath) {
                ui->oathHotpMenuBtn->setStyleSheet(checkedMenuBtnSS);
            } else {
                ui->oathHotpMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
            }
            ui->oathHotpMenuBtn->setEnabled(true);
        } else {
            ui->oathHotpSupportLbl->setPixmap(CROSSMAP);
            ui->oathHotpMenuBtn->setStyleSheet(disabledMenuBtnSS);
            ui->oathHotpMenuBtn->setEnabled(false);
        }

        if(featuresMatrix[YubiKeyFinder::Feature_StaticPassword]) {
            ui->staticPwdSupportLbl->setPixmap(TICKMAP);
            if(m_currentPage == Page_Static) {
                ui->staticMenuBtn->setStyleSheet(checkedMenuBtnSS);
            } else {
                ui->staticMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
            }
            ui->staticMenuBtn->setEnabled(true);
        } else {
            ui->staticPwdSupportLbl->setPixmap(CROSSMAP);
            ui->staticMenuBtn->setStyleSheet(disabledMenuBtnSS);
            ui->staticMenuBtn->setEnabled(false);
        }

        if(featuresMatrix[YubiKeyFinder::Feature_ScanCodeMode]) {
            ui->scanCodeSupportLbl->setPixmap(TICKMAP);
        } else {
            ui->scanCodeSupportLbl->setPixmap(CROSSMAP);
        }

        if(featuresMatrix[YubiKeyFinder::Feature_ChallengeResponse]) {
            ui->chalRespSupportLbl->setPixmap(TICKMAP);
            if(m_currentPage == Page_ChalResp) {
                ui->chalRespMenuBtn->setStyleSheet(checkedMenuBtnSS);
            } else {
                ui->chalRespMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
            }
            ui->chalRespMenuBtn->setEnabled(true);
        } else {
            ui->chalRespSupportLbl->setPixmap(CROSSMAP);
            ui->chalRespMenuBtn->setStyleSheet(disabledMenuBtnSS);
            ui->chalRespMenuBtn->setEnabled(false);
        }

        if(featuresMatrix[YubiKeyFinder::Feature_Updatable]) {
            ui->updatableSupportLbl->setPixmap(TICKMAP);
        } else {
            ui->updatableSupportLbl->setPixmap(CROSSMAP);
        }

        if(featuresMatrix[YubiKeyFinder::Feature_Ndef]) {
            ui->ndefSupportLbl->setPixmap(TICKMAP);
        } else {
            ui->ndefSupportLbl->setPixmap(CROSSMAP);
        }
        if(featuresMatrix[YubiKeyFinder::Feature_U2F]) {
            ui->u2fSupportLbl->setPixmap(TICKMAP);
        } else {
            ui->u2fSupportLbl->setPixmap(CROSSMAP);
        }
    } else {
        ui->programLbl->clear();
        if(ui->deviceImage->pixmap()) {
            ui->deviceImage->setPixmap(QPixmap());
        }
        ui->deviceImage->clear();
        if(error == ERR_NOKEY) {
            ui->statusLbl->setText(tr(NO_KEY_FOUND));
        } else if(error == ERR_MORETHANONE) {
            ui->statusLbl->setText(tr(MORE_THAN_ONE));
        } else {
            ui->statusLbl->setText(tr(OTHER_ERROR));
        }
        ui->statusLbl->setStyleSheet(QString::fromUtf8(SS_YKSTATUS_ERROR));

        if(m_currentPage == Page_Oath) {
            ui->oathHotpMenuBtn->setStyleSheet(checkedMenuBtnSS);
        } else {
            ui->oathHotpMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
        }
        ui->oathHotpMenuBtn->setEnabled(true);

        if(m_currentPage == Page_Static) {
            ui->staticMenuBtn->setStyleSheet(checkedMenuBtnSS);
        } else {
            ui->staticMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
        }
        ui->staticMenuBtn->setEnabled(true);

        if(m_currentPage == Page_ChalResp) {
            ui->chalRespMenuBtn->setStyleSheet(checkedMenuBtnSS);
        } else {
            ui->chalRespMenuBtn->setStyleSheet(uncheckedMenuBtnSS);
        }
        ui->chalRespMenuBtn->setEnabled(true);
    }
}

/*
 Status Widget handling
*/

void MainWindow::showStatusMessage(const QString &text, int status) {
    qDebug() << "status: " << status << "\n";
    if(status == -1) {
        clearStatusMessage();
    }

    QString statuSS;

    switch(status) {
    case 0:
        ui->statusIconLbl->setPixmap(TICKMAP);
        statuSS = QString::fromUtf8(SS_STATUS_SUCCESS);

        break;
    case 1:
        ui->statusIconLbl->setPixmap(CROSSMAP);
        statuSS = QString::fromUtf8(SS_STATUS_ERROR);

        break;
    }

    ui->statusMsgLbl->setStyleSheet(statuSS);
    ui->statusMsgLbl->setText(text);

    if (m_timer != 0) {
        delete m_timer;
        m_timer = 0;
    }

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(clearStatusMessage()));
    m_timer->start(TIMEOUT_STATUS);
}

void MainWindow::clearStatusMessage() {
    if (m_timer != 0) {
        delete m_timer;
        m_timer = 0;
    }
    ui->statusIconLbl->clear();
    ui->statusMsgLbl->clear();
}

void MainWindow::copyToClipboard(const QString &str) {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(str);

    showStatusMessage(tr(VALUE_COPIED), 0);
}

void MainWindow::on_serialNoDecCopyBtn_clicked() {
    copyToClipboard(ui->serialNoDecLbl->text());
}

void MainWindow::on_serialNoHexCopyBtn_clicked() {
    copyToClipboard(ui->serialNoHexLbl->text());
}

void MainWindow::on_serialNoModhexCopyBtn_clicked() {
    copyToClipboard(ui->serialNoModhexLbl->text());
}

void MainWindow::toggleAnimation(bool checked) {
    QSettings settings;
    settings.setValue(SG_ANIMATIONS_PREFERENCE, checked);
    animationAction->setChecked(checked);
    if(ui->deviceImage->movie()) {
        if(checked) {
            ui->deviceImage->movie()->start();
        } else {
            ui->deviceImage->movie()->stop();
        }
    }
}

void MainWindow::triggerDiagnostics() {
    m_diagnostics->show();
}

void MainWindow::triggerHelp(int index) {
    m_help->setHelpIndex((HelpBox::Help)index);
    m_help->show();
}

void MainWindow::aboutToQuit() {
    YubiKeyLogger::closeLogFile();
}
