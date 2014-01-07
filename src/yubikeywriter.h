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

#ifndef YUBIKEYWRITER_H
#define YUBIKEYWRITER_H

#include <QObject>

extern "C" {
#include <ykpers.h>
}
#include "common.h"
#include "yubikeyconfig.h"

class YubiKeyConfig;

class YubiKeyWriter : public QObject {
    Q_OBJECT

public:
    static YubiKeyWriter* getInstance();
    ~YubiKeyWriter();
    static void setExportFilename(QString filename);
    static QString defaultExportFilename(void);

private:
    static QString m_filename;

    YubiKeyWriter();
    static YubiKeyWriter* _instance;

    QString reportError(bool chalresp);
    int encodeAccessCode(const QString accCode, unsigned char *accessCode, size_t *accessCodeLen);

    int assembleConfig(YubiKeyConfig *ykConfig, YKP_CONFIG *ykp, bool *useAccessCode, unsigned char *accessCode);

public slots:
    void writeConfig(YubiKeyConfig *ykConfig);
    void exportConfig(YubiKeyConfig *ykConfig);
    void doChallengeResponse(const QString challenge,
        QString &response, int slot, bool hmac);
    void writeNdef(bool uri, const QString language,
        const QString payload, const QString accCode, int slot);
    void deleteConfig(int slot, const QString accCode);

signals:
    void configWritten(bool written, const QString &msg);
    void errorOccurred(const QString &text);
    void diagnostics(const QString &text);
};

#endif // YUBIKEYWRITER_H
