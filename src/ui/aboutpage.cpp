/*
Copyright (C) 2011-2013 Yubico AB.  All rights reserved.

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

#include "aboutpage.h"
#include "ui_aboutpage.h"
#include "ui/mainwindow.h"
#include "yubikeyfinder.h"

#include "common.h"
#include "version.h"

#include <ykpers-version.h>

#include <QUrl>
#include <QDesktopServices>
#include <QSignalMapper>

AboutPage::AboutPage(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::AboutPage)
{
    ui->setupUi(this);

    //Set application version
    QString appVersion = tr("%1 %2").
                         arg(ui->appVersionLbl->text()).
                         arg(VER_PRODUCTVERSION_STR);

    ui->appVersionLbl->setText(appVersion);

    // Set the library version
    QString libVersion = tr("%1 %2").
                         arg(ui->libVersionLbl->text()).
                         arg(ykpers_check_version(NULL));

    ui->libVersionLbl->setText(libVersion);

    //Set copyright
    QString copyright = tr("%1. %2.").
                        arg(trUtf8(VER_LEGALCOPYRIGHT)).
                        arg(VER_LEGALTRADEMARKS1_STR);

    ui->copyrightLbl->setText(copyright);

    connectButtons();

    //Connect other signals and slots
    connect(YubiKeyFinder::getInstance(), SIGNAL(keyFound(bool, bool*, int)),
            this, SLOT(keyFound(bool, bool*)));
}

AboutPage::~AboutPage() {
    delete ui;
}

void AboutPage::connectButtons() {
    //Map the values of the buttons

    //Create a QMapper
    QSignalMapper *mapper = new QSignalMapper(this);

    //Connect the clicked signal with the QSignalMapper
    connect(ui->otpBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->oathBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->staticBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->chalRespBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(ui->settingsBtn, SIGNAL(clicked()), mapper, SLOT(map()));

    //Set a value for each button
    mapper->setMapping(ui->otpBtn, MainWindow::Page_Otp);
    mapper->setMapping(ui->oathBtn, MainWindow::Page_Oath);
    mapper->setMapping(ui->staticBtn, MainWindow::Page_Static);
    mapper->setMapping(ui->chalRespBtn, MainWindow::Page_ChalResp);
    mapper->setMapping(ui->settingsBtn, MainWindow::Page_Settings);

    //Connect the mapper
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(btn_pressed(int)));
}

void AboutPage::keyFound(bool found, bool* featuresMatrix) {
    QString enabledLinkBtnSS = QString::fromUtf8(SS_LINK_ENABLED);
    QString disabledLinkBtnSS = QString::fromUtf8(SS_LINK_DISABLED);

    if(found) {
        if(featuresMatrix[YubiKeyFinder::Feature_OathHotp]) {
            ui->oathBtn->setEnabled(true);
            ui->oathBtn->setStyleSheet(enabledLinkBtnSS);
        } else {
            ui->oathBtn->setEnabled(false);
            ui->oathBtn->setStyleSheet(disabledLinkBtnSS);
        }

        if(featuresMatrix[YubiKeyFinder::Feature_StaticPassword]) {
            ui->staticBtn->setEnabled(true);
            ui->staticBtn->setStyleSheet(enabledLinkBtnSS);
        } else {
            ui->staticBtn->setEnabled(false);
            ui->staticBtn->setStyleSheet(disabledLinkBtnSS);
        }

        if(featuresMatrix[YubiKeyFinder::Feature_ChallengeResponse]) {
            ui->chalRespBtn->setEnabled(true);
            ui->chalRespBtn->setStyleSheet(enabledLinkBtnSS);
        } else {
            ui->chalRespBtn->setEnabled(false);
            ui->chalRespBtn->setStyleSheet(disabledLinkBtnSS);
        }
    } else {
        ui->oathBtn->setEnabled(true);
        ui->oathBtn->setStyleSheet(enabledLinkBtnSS);

        ui->staticBtn->setEnabled(true);
        ui->staticBtn->setStyleSheet(enabledLinkBtnSS);

        ui->chalRespBtn->setEnabled(true);
        ui->chalRespBtn->setStyleSheet(enabledLinkBtnSS);
    }
}

void AboutPage::btn_pressed(int pageIndex) {
    emit changePage(pageIndex);
}

void AboutPage::on_supportBtn_clicked() {
    QDesktopServices::openUrl(QUrl(tr(SUPPORT_LINK)));
}
