/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#include "aboutpage.h"
#include "ui_aboutpage.h"
#include "ui/mainwindow.h"

#include "common.h"
#include "version.h"

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

    //Set build timestamp
    QString buildTS = tr("%1 %2 %3").
                      arg(ui->buildTimestampLbl->text()).
                      arg(__DATE__).
                      arg(__TIME__);

    ui->buildTimestampLbl->setText(buildTS);

    //Set copyright
    QString copyright = tr("%1. %2.").
                        arg(VER_LEGALCOPYRIGHT_STR).
                        arg(VER_LEGALTRADEMARKS1_STR);

    ui->copyrightLbl->setText(copyright);

    connectButtons();

    //Connect other signals and slots
    connect(YubiKeyFinder::getInstance(), SIGNAL(keyFound(bool, bool*)),
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

    //Set a value for each button
    mapper->setMapping(ui->otpBtn, MainWindow::Page_Otp);
    mapper->setMapping(ui->oathBtn, MainWindow::Page_Oath);
    mapper->setMapping(ui->staticBtn, MainWindow::Page_Static);
    mapper->setMapping(ui->chalRespBtn, MainWindow::Page_ChalResp);

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
    QDesktopServices::openUrl(QUrl(SUPPORT_EMAIL));
}
