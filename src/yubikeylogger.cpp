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

#include "yubikeylogger.h"
#include "yubikeyutil.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QFileDialog>
#include <QSettings>

#define LOG_FILENAME_DEF   "configuration_log.csv"
#define LOG_SEPARATOR      ","

QString YubiKeyLogger::m_filename   = defaultLogFilename();
bool YubiKeyLogger::m_enabled       = true;
bool YubiKeyLogger::m_started       = true;
YubiKeyLogger::Format YubiKeyLogger::m_format = Format_Traditional;
QFile* YubiKeyLogger::m_logFile     = NULL;
QString YubiKeyLogger::m_flexibleFormat = "";

#define STRING 1
#define BOOL   2
#define INT    3
#define UINT   4

struct logging_st YubiKeyLogger::logging_map[] = {
    { "eventType", NULL, STRING, YubiKeyLogger::resolve_eventType },
    { "timestampLocal", NULL, STRING, YubiKeyLogger::resolve_timestamp },
    { "configSlot", "configSlot", INT, NULL },
    { "pubIdTxt", "pubIdTxt", STRING, NULL },
    { "oathIdTxt", NULL, STRING, YubiKeyLogger::resolve_oathId },
    { "pvtIdTxt", "pvtIdTxt", STRING, NULL },
    { "secretKeyTxt", "secretKeyTxt", STRING, NULL },
    { "secretKeyB64", NULL, STRING, YubiKeyLogger::resolve_secretKeyB64 },
    { "currentAccessCodeTxt", "currentAccessCodeTxt", STRING, NULL },
    { "currentAccessCodeTxtPadded", NULL, STRING, YubiKeyLogger::resolve_accessCode },
    { "newAccessCodeTxt", "newAccessCodeTxt", STRING, NULL },
    { "newAccessCodeTxtPadded", NULL, STRING, YubiKeyLogger::resolve_accessCode },
    { "hotpDigits", NULL, STRING, YubiKeyLogger::resolve_hotpDigits },
    { "oathMovingFactorSeed", "oathMovingFactorSeed", UINT, NULL },
    { "strongPw1", "strongPw1", BOOL, NULL },
    { "strongPw2", "strongPw2", BOOL, NULL },
    { "sendRef", "sendRef", BOOL, NULL },
    { "chalBtnTrig", "chalBtnTrig", BOOL, NULL },
    { "hmacLT64", "hmacLT64", BOOL, NULL },
    { "timestampFixed", NULL, STRING, YubiKeyLogger::resolve_timestamp },
    { "oathFixedModhex1", "oathFixedModhex1", BOOL, NULL },
    { "oathFixedModhex2", "oathFixedModhex2", BOOL, NULL },
    { "oathFixedModhex", "oathFixedModhex", BOOL, NULL },
    { "tokenLength", NULL, STRING, YubiKeyLogger::resolve_tokenLength },
    { "serial", "serial", STRING, NULL },
    { "endl", NULL, STRING, YubiKeyLogger::resolve_symbol },
    { "tab", NULL, STRING, YubiKeyLogger::resolve_symbol },
};

YubiKeyLogger::~YubiKeyLogger() {
}

QFile *YubiKeyLogger::getLogFile() {
    if(m_logFile == NULL) {
        m_filename = QFileDialog::getSaveFileName(NULL, tr("Select Log File"), m_filename, tr("Comma-sepparated values (*.csv)"), NULL, QFileDialog::DontConfirmOverwrite);
        if(m_filename.isEmpty()) {
            qDebug() << "No filename returned for logging";
            return NULL;
        }
        m_logFile = new QFile(m_filename);
        if(!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
            qDebug() << "File could not be opened for writing";
            return NULL;
        }
        QSettings settings;
        settings.setValue(SG_LOG_FILENAME, m_filename);
    }
    return m_logFile;
}

void YubiKeyLogger::closeLogFile() {
    if(m_logFile != NULL) {
        if(m_format == Format_PSKC) {
            endPSKC();
        }
        m_logFile->close();
        delete(m_logFile);
        m_logFile = NULL;
        m_started = true;
    }
}

QString YubiKeyLogger::formatLog(YubiKeyConfig *ykConfig, QString format) {
    for(unsigned long i = 0; i < (sizeof(logging_map) / sizeof(logging_st)); i++) {
        QString token = QString("{") + logging_map[i].name + QString("}");
        if(!format.contains(token)) {
            continue;
        }
        QString out;
        int type = logging_map[i].returnType;
        const char *configName = logging_map[i].configName;
        if(logging_map[i].resolver != NULL) {
            out = logging_map[i].resolver(ykConfig, logging_map[i].name);
        } else if(type == STRING) {
            QMetaObject::invokeMethod(ykConfig, configName,
                    Qt::DirectConnection, Q_RETURN_ARG(QString, out));
        } else if(type == BOOL) {
            bool ret;
            QMetaObject::invokeMethod(ykConfig, configName,
                    Qt::DirectConnection, Q_RETURN_ARG(bool, ret));
            out = ret ? "1" : "0";
        } else if(type == INT) {
            int ret;
            QMetaObject::invokeMethod(ykConfig, configName,
                    Qt::DirectConnection, Q_RETURN_ARG(int, ret));
            out = QString::number(ret);
        } else if(type == UINT) {
            unsigned int ret;
            QMetaObject::invokeMethod(ykConfig, configName,
                    Qt::DirectConnection, Q_RETURN_ARG(unsigned int, ret));
            out = QString::number(ret);
        }

        qDebug() << "replacing " << token << " with " << out << " from " << configName << " type " << type;
        format.replace(token, out);
    }
    return format;
}

void YubiKeyLogger::logConfig(YubiKeyConfig *ykConfig) {
    //Check if logging is enabled
    if(!m_enabled) {
        return;
    }

    QString format = "";
    if(m_format == Format_Traditional) {
        if(m_started) {
            format += "LOGGING START,{timestampLocal}{endl}";
            m_started = false;
        }

        format += "{eventType},{timestampLocal},{configSlot},{pubIdTxt},{pvtIdTxt},{secretKeyTxt},{currentAccessCodeTxt},{newAccessCodeTxt},{oathFixedModhex1},{oathFixedModhex2},{oathFixedModhex},{hotpDigits},{oathMovingFactorSeed},{strongPw1},{strongPw2},{sendRef},{chalBtnTrig},{hmacLT64}";
    } else if(m_format == Format_Yubico) {
        format = "{serial},";
        if(ykConfig->programmingMode() == YubiKeyConfig::Mode_YubicoOtp) {
            format += "{pubIdTxt},{pvtIdTxt},";
        } else if(ykConfig->programmingMode() == YubiKeyConfig::Mode_OathHotp) {
            format += "{pubIdTxt},{oathMovingFactorSeed},";
        } else if(ykConfig->programmingMode() == YubiKeyConfig::Mode_ChalRespHmac) {
            format += ",0,";
        } else {
            format += ",,";
        }
        format += "{secretKeyTxt},{newAccessCodeTxtPadded},{timestampFixed},";
    } else if(m_format == Format_PSKC) {
        if(m_started) {
            format += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>{endl}<KeyContainer Version=\"1.0\" xmlns=\"urn:ietf:params:xml:ns:keyprov:pskc\">{endl}";
            m_started = false;
        }
        format += "<KeyPackage><DeviceInfo><Manufacturer>Yubico</Manufacturer><SerialNo>{serial}</SerialNo></DeviceInfo>";
        QString id = "\"{serial}:{configSlot}\"";
        if(ykConfig->programmingMode() == YubiKeyConfig::Mode_OathHotp && ykConfig->pubIdTxt().length() > 0) {
            id = "\"{oathIdTxt}\"";
        }
        format += "<CryptoModuleInfo><Id>{configSlot}</Id></CryptoModuleInfo><Key Id=" + id;
        if(ykConfig->programmingMode() == YubiKeyConfig::Mode_YubicoOtp) {
            format += " Algorithm=\"http://www.yubico.com/#yubikey-aes\"><AlgorithmParameters><ResponseFormat Length=\"{tokenLength}\" Encoding=\"ALPHANUMERIC\"/></AlgorithmParameters>";
        } else if(ykConfig->programmingMode() == YubiKeyConfig::Mode_OathHotp) {
            format += " Algorithm=\"urn:ietf:params:xml:ns:keyprov:pskc:hotp\"><AlgorithmParameters><ResponseFormat Length=\"{tokenLength}\" Encoding=\"DECIMAL\"/></AlgorithmParameters>";
        } else {
            qDebug() << "PSKC is primarily for Oath-HOTP and Yubico-OTP";
            format += ">";
        }
        format += "<Data><Secret><PlainValue>{secretKeyB64}</PlainValue></Secret>";
        if(ykConfig->programmingMode() == YubiKeyConfig::Mode_OathHotp) {
            format += "<Counter><PlainValue>{oathMovingFactorSeed}</PlainValue></Counter>";
        }
        format += "</Data>";
        if(ykConfig->programmingMode() == YubiKeyConfig::Mode_YubicoOtp) {
            format += "<UserId>CN={pubIdTxt}, UID={pvtIdTxt}</UserId>";
        }
        format += "</Key></KeyPackage>";
    } else if(m_format == Format_Flexible) {
        format = m_flexibleFormat;
    } else {
        qDebug() << "unknown format" << m_format;
        return;
    }
    format = formatLog(ykConfig, format);
    logString(format);
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
    closeLogFile();
    m_filename = filename;
}

QString YubiKeyLogger::logFilename() {
    return m_filename;
}

QString YubiKeyLogger::defaultLogFilename() {
    return QDir::homePath() + "/" + LOG_FILENAME_DEF;
}

void YubiKeyLogger::setLogFormat(Format format) {
    closeLogFile();
    m_format = format;
}

void YubiKeyLogger::setFlexibleFormat(QString format) {
    m_flexibleFormat = format;
}

QString YubiKeyLogger::resolve_hotpDigits(YubiKeyConfig *ykConfig, QString name __attribute__((unused))) {
    QString ret = "0";
    if(ykConfig->programmingMode() == YubiKeyConfig::Mode_OathHotp) {
        ret = (ykConfig->oathHotp8()? "8": "6");
    }
    return ret;
}

QString YubiKeyLogger::resolve_eventType(YubiKeyConfig *ykConfig, QString name __attribute__((unused))) {
    switch(ykConfig->programmingMode()) {
        case YubiKeyConfig::Mode_YubicoOtp:
            return(tr("Yubico OTP"));
        case YubiKeyConfig::Mode_Static:
            {
                QString eventType = tr("Static Password");
                if(ykConfig->shortTicket()) {
                    if(ykConfig->staticTicket()) {
                        eventType.append(tr(": Short"));
                    } else {
                        eventType.append(tr(": Scan Code"));
                    }
                }
                return eventType;
            }
        case YubiKeyConfig::Mode_OathHotp:
            return(tr("OATH-HOTP"));
        case YubiKeyConfig::Mode_ChalRespYubico:
            return(tr("Challenge-Response: Yubico OTP"));
        case YubiKeyConfig::Mode_ChalRespHmac:
            return(tr("Challenge-Response: HMAC-SHA1"));
        case YubiKeyConfig::Mode_Update:
            return(tr("Configuration Update"));
        case YubiKeyConfig::Mode_Swap:
            return(tr("Configuration Swap"));
    }
    return(tr("Unknown programming mode"));
}

QString YubiKeyLogger::resolve_timestamp(YubiKeyConfig *ykConfig __attribute__((unused)), QString name) {
        QDateTime ts = QDateTime::currentDateTime();
        if(name == "timestampLocal") {
            return ts.toString(Qt::SystemLocaleDate);
        } else {
            return ts.toString("yyyy-MM-ddThh:mm:ss");
        }
}

QString YubiKeyLogger::resolve_symbol(YubiKeyConfig *ykConfig __attribute__((unused)), QString name) {
    if(name == "endl") {
        return "\n";
    } else if(name == "tab") {
        return "\t";
    }
    return "";
}

QString YubiKeyLogger::resolve_secretKeyB64(YubiKeyConfig *ykConfig, QString name __attribute__((unused))) {
    QByteArray decodedSecret = QByteArray::fromHex(ykConfig->secretKeyTxt().toLatin1());
    return decodedSecret.toBase64();
}

QString YubiKeyLogger::resolve_tokenLength(YubiKeyConfig *ykConfig, QString name __attribute__((unused))) {
    int len = ykConfig->pubIdTxt().length();

    if(ykConfig->programmingMode() == YubiKeyConfig::Mode_YubicoOtp) {
        len += 32;
    } else if(ykConfig->programmingMode() == YubiKeyConfig::Mode_OathHotp) {
        len += ykConfig->oathHotp8() ? 8 : 6;
    }
    return QString::number(len);
}

QString YubiKeyLogger::resolve_accessCode(YubiKeyConfig *ykConfig, QString name) {
    QString code;
    if(name == "currentAccessCodeTxtPadded") {
        code = ykConfig->currentAccessCodeTxt();
    } else {
        code = ykConfig->newAccessCodeTxt();
    }
    return code.rightJustified(12, '0');
}

QString YubiKeyLogger::resolve_oathId(YubiKeyConfig *ykConfig, QString name __attribute__((unused))) {
    QString id;
    QString pubid = ykConfig->pubIdTxt();
    int modhex = 0;
    if(ykConfig->oathFixedModhex()) {
        modhex = pubid.length();
    } else if(ykConfig->oathFixedModhex1()) {
        modhex = 2;
    } else if(ykConfig->oathFixedModhex2()) {
        modhex = 4;
    }
    id = pubid.left(modhex);
    if(modhex != pubid.length()) {
        unsigned char buf[16];
        size_t len = sizeof(buf);
        YubiKeyUtil::qstrModhexDecode(buf, &len, pubid.right(pubid.length() - modhex));
        id += YubiKeyUtil::qstrHexEncode(buf, len);
    }
    return id;
}

QStringList YubiKeyLogger::getLogNames() {
    QStringList list;
    for(unsigned int i = 0; i < (sizeof(logging_map) / sizeof(logging_st)); i++) {
        list << logging_map[i].name;
    }
    return list;
}

void YubiKeyLogger::endPSKC() {
    logString("</KeyContainer>");
}

void YubiKeyLogger::logString(QString string) {
    QFile *file = getLogFile();
    if(file == NULL) {
        qDebug() << "Failed to get logfile";
        return;
    }

    QTextStream out(file);
    out << string << "\n";
    file->flush();
}
