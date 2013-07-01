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

#include "common.h"
#include "yubikeyconfig.h"

class YubiKeyLogger : public QObject {

public:
    ~YubiKeyLogger();

    enum Format {
        Format_Traditional,
        Format_Yubico,
    };
    static void logConfig(YubiKeyConfig *ykConfig);

    static void enableLogging();
    static void disableLogging();
    static bool isLogging();

    static void setLogFilename(const QString &filename);
    static QString logFilename();
    static QString defaultLogFilename();
    static void setLogFormat(Format format);

private:
    static bool m_enabled;
    static QString m_filename;
    static bool m_started;
    static Format m_format;

    static void logConfigTraditional(YubiKeyConfig *ykConfig, QTextStream &out);
    static void logConfigYubico(YubiKeyConfig *ykConfig, QTextStream &out);

    static QFile *m_logFile;
    static QFile *getLogFile(void);
};

#endif // YUBIKEYLOGGER_H
