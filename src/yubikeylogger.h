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

#ifndef YUBIKEYLOGGER_H
#define YUBIKEYLOGGER_H

#include <QObject>
#include <QString>
#include <QTextStream>
#include <QFile>
#include <QStringList>

#include "common.h"
#include "yubikeyconfig.h"

struct logging_st {
    const char *name;
    const char *configName;
    int returnType;
    QString (*resolver)(YubiKeyConfig*, QString);
};

class YubiKeyLogger : public QObject {
    Q_OBJECT
public:
    ~YubiKeyLogger();

    enum Format {
        Format_Traditional,
        Format_Yubico,
        Format_Flexible,
        Format_PSKC,
    };
    static void logConfig(YubiKeyConfig *ykConfig);

    static void enableLogging();
    static void disableLogging();
    static bool isLogging();

    static void setLogFilename(const QString &filename);
    static QString logFilename();
    static QString defaultLogFilename();
    static void setLogFormat(Format format);
    static void setFlexibleFormat(QString);
    static QStringList getLogNames();
    static void closeLogFile(void);

private:
    static bool m_enabled;
    static QString m_filename;
    static bool m_started;
    static Format m_format;
    static QString m_flexibleFormat;
    static logging_st logging_map[];

    static QString formatLog(YubiKeyConfig *ykConfig, QString format);

    static QFile *m_logFile;
    static QFile *getLogFile(void);

    static void endPSKC();
    static void logString(QString);

    static QString resolve_eventType(YubiKeyConfig*, QString);
    static QString resolve_timestamp(YubiKeyConfig*, QString);
    static QString resolve_hotpDigits(YubiKeyConfig*, QString);
    static QString resolve_symbol(YubiKeyConfig*, QString);
    static QString resolve_secretKeyB64(YubiKeyConfig*, QString);
    static QString resolve_tokenLength(YubiKeyConfig*, QString);
    static QString resolve_accessCode(YubiKeyConfig*, QString);
    static QString resolve_oathId(YubiKeyConfig*, QString);
};

#endif // YUBIKEYLOGGER_H
