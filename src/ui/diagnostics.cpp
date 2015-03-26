/*
Copyright (C) 2014 Yubico AB.  All rights reserved.

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

#include "ui/diagnostics.h"
#include "ui_diagnostics.h"

#include "yubikeywriter.h"
#include "yubikeyfinder.h"
#include "version.h"

#include <ykpers-version.h>

#include <QDateTime>
#include <QDialogButtonBox>

Diagnostics::Diagnostics(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Diagnostics)
{
    ui->setupUi(this);

    ui->txtBrowser->clear();

    connect(YubiKeyWriter::getInstance(), SIGNAL(errorOccurred(QString)),
        this, SLOT(addDiagnostic(QString)));
    connect(YubiKeyWriter::getInstance(), SIGNAL(diagnostics(QString)),
        this, SLOT(addDiagnostic(QString)));
    connect(YubiKeyFinder::getInstance(), SIGNAL(errorOccurred(QString)),
        this, SLOT(addDiagnostic(QString)));
    connect(YubiKeyFinder::getInstance(), SIGNAL(diagnostics(QString)),
        this, SLOT(addDiagnostic(QString)));

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)),
        this, SLOT(clicked(QAbstractButton*)));

    setup();
}

Diagnostics::~Diagnostics() {
    delete ui;
}

void Diagnostics::addDiagnostic(QString text) {
    QDateTime ts = QDateTime::currentDateTime();
    ui->txtBrowser->append(ts.toString("yyyy-MM-ddThh:mm:ss; ") + text);
}

void Diagnostics::clicked(QAbstractButton *button) {
    if(ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
        setup();
    }
}

void Diagnostics::setup() {
    ui->txtBrowser->clear();
    addDiagnostic(QString("App_version: %1; Lib_version: %2; QT_version: %3; OS_version: %4; Word_size: %5")
        .arg(VER_PRODUCTVERSION_STR, ykpers_check_version(NULL), qVersion(), osVersion(), QString::number(QSysInfo::WordSize)));
}

QString Diagnostics::osVersion() {
#ifdef Q_OS_LINUX
    return "Linux";
#elif defined(Q_OS_MAC)
    QString mac = "OS X ";
    switch(QSysInfo::MacintoshVersion) {
        case QSysInfo::MV_LEOPARD:
            mac += "Leopard";
            break;
        case QSysInfo::MV_SNOWLEOPARD:
            mac += "SnowLeopard";
            break;
        case QSysInfo::MV_LION:
            mac += "Lion";
            break;
        case QSysInfo::MV_MOUNTAINLION:
            mac += "MountainLion";
            break;
        default:
            mac += "unknown";
            break;
    }
    return mac;
#elif defined(Q_OS_WIN32)
    QString win = "Windows ";
    switch(QSysInfo::WindowsVersion) {
        case QSysInfo::WV_2000:
            win += "2000";
            break;
        case QSysInfo::WV_XP:
            win += "XP";
            break;
        case QSysInfo::WV_2003:
            win += "2003";
            break;
        case QSysInfo::WV_VISTA:
            win += "Vista";
            break;
        case QSysInfo::WV_WINDOWS7:
            win += "7";
            break;
        case QSysInfo::WV_WINDOWS8:
            win += "8";
            break;
        default:
            win += "unknown";
            break;
    }
    return win;
#else
    return "Unknown OS";
#endif
}

