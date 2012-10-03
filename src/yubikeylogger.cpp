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

#include "yubikeylogger.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDateTime>

#define LOG_FILENAME_DEF   "configuration_log.csv"
#define LOG_SEPARATOR      ","

QString YubiKeyLogger::m_filename   = defaultLogFilename();
bool YubiKeyLogger::m_enabled       = true;
bool YubiKeyLogger::m_started       = true;
YubiKeyLogger::Format YubiKeyLogger::m_format = Format_Traditional;

YubiKeyLogger::~YubiKeyLogger() {
}

void YubiKeyLogger::logConfig(YubiKeyConfig *ykConfig) {
    //Check if logging is enabled
    if(!m_enabled) {
        return;
    }

    QFile file(m_filename);

    qDebug() << "Log file name:" << m_filename;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qDebug() << "File could not be opened for writing";
        return;
    }

    QTextStream out(&file);


    if(m_format == Format_Traditional) {
        logConfigTraditional(ykConfig, out);
    } else {
        logConfigYubico(ykConfig, out);
    }
    file.flush();
    file.close();
}

void YubiKeyLogger::logConfigTraditional(YubiKeyConfig *ykConfig, QTextStream &out) {
    if(m_started) {
        QDateTime ts = QDateTime::currentDateTime();
        out << tr("LOGGING START")
                << LOG_SEPARATOR
                << ts.toString(Qt::SystemLocaleDate)
                << endl;

        m_started = false;
    }

    //Event type...
    QString eventType;

    switch(ykConfig->programmingMode()) {
    case YubiKeyConfig::Mode_YubicoOtp:
        eventType = tr("Yubico OTP");
        break;

    case YubiKeyConfig::Mode_Static:
        eventType = tr("Static Password");

        if(ykConfig->shortTicket()) {
            if(ykConfig->staticTicket()) {
                eventType.append(tr(": Short"));
            } else {
                eventType.append(tr(": Scan Code"));
            }
        }
        break;

    case YubiKeyConfig::Mode_OathHotp:
        eventType = tr("OATH-HOTP");
        break;

    case YubiKeyConfig::Mode_ChalRespYubico:
        eventType = tr("Challenge-Response: Yubico OTP");
        break;

    case YubiKeyConfig::Mode_ChalRespHmac:
        eventType = tr("Challenge-Response: HMAC-SHA1");
        break;

    case YubiKeyConfig::Mode_Update:
        eventType = tr("Configuration Update");
        break;

    case YubiKeyConfig::Mode_Swap:
        eventType = tr("Configuration Swap");
        break;
    }
    out << eventType;

    //Timestamp...
    QDateTime timstamp = QDateTime::currentDateTime();
    out << LOG_SEPARATOR << timstamp.toString(Qt::SystemLocaleDate);

    //Configuration slot...
    out << LOG_SEPARATOR << ykConfig->configSlot();

    //Public ID...
    out << LOG_SEPARATOR << ykConfig->pubIdTxt();

    //Private ID...
    out << LOG_SEPARATOR << ykConfig->pvtIdTxt();

    //Secret Key...
    out << LOG_SEPARATOR << ykConfig->secretKeyTxt();

    //Current Access Code...
    out << LOG_SEPARATOR << ykConfig->currentAccessCodeTxt();

    //New Access Code...
    out << LOG_SEPARATOR << ykConfig->newAccessCodeTxt();

    //OATH-HOTP specific...
    out << LOG_SEPARATOR << (ykConfig->oathFixedModhex1()? 1: 0);
    out << LOG_SEPARATOR << (ykConfig->oathFixedModhex2()? 1: 0);
    out << LOG_SEPARATOR << (ykConfig->oathFixedModhex()? 1: 0);
    if(ykConfig->programmingMode() == YubiKeyConfig::Mode_OathHotp) {
        out << LOG_SEPARATOR << (ykConfig->oathHotp8()? 8: 6);
    } else {
        out << LOG_SEPARATOR << 0;
    }
    out << LOG_SEPARATOR << ykConfig->oathMovingFactorSeed();

    //Static Password specific...
    out << LOG_SEPARATOR << (ykConfig->strongPw1()? 1: 0);
    out << LOG_SEPARATOR << (ykConfig->strongPw2()? 1: 0);
    out << LOG_SEPARATOR << (ykConfig->sendRef()? 1: 0);

    //Challenge-Response specific
    out << LOG_SEPARATOR << (ykConfig->chalBtnTrig()? 1: 0);
    if(ykConfig->programmingMode() == YubiKeyConfig::Mode_ChalRespHmac) {
        out << LOG_SEPARATOR << (ykConfig->hmacLT64()? 1: 0);
    } else {
        out << LOG_SEPARATOR << 0;
    }

    out << endl;
}

void YubiKeyLogger::logConfigYubico(YubiKeyConfig *ykConfig, QTextStream &out) {

}

void YubiKeyLogger::enableLogging() {
    m_enabled = true;
}

void YubiKeyLogger::disableLogging() {
    m_enabled = false;
}

bool YubiKeyLogger::isLogging() {
    return m_enabled;
}

void YubiKeyLogger::setLogFilename(const QString &filename) {
    m_filename = filename;
}

QString YubiKeyLogger::logFilename() {
    return m_filename;
}

QString YubiKeyLogger::defaultLogFilename() {
    return QDir::homePath() + "/" + LOG_FILENAME_DEF;
}

void YubiKeyLogger::setLogFormat(Format format) {
    m_format = format;
}

