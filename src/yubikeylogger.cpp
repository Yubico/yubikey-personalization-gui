/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#include "yubikeylogger.h"
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>

#define LOG_FILENAME_DEF   "configuration_log.csv"
#define LOG_SEPARATOR      ","

QString YubiKeyLogger::m_filename   = defaultLogFilename();
bool YubiKeyLogger::m_enabled       = true;
bool YubiKeyLogger::m_started       = true;

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

    file.flush();
    file.close();
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
