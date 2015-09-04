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

#ifndef YUBIKEYFINDER_H
#define YUBIKEYFINDER_H

#include <QObject>
#include <QTimer>

extern "C" {
#include <ykpers.h>
}

#include "common.h"

class YubiKeyFinder : public QObject {
    Q_OBJECT

public:
    static YubiKeyFinder* getInstance();
    ~YubiKeyFinder();

    enum State {
        State_Absent,
        State_Present,
        State_Processing,
        State_NoFocus,
    };

    enum Feature {
        Feature_MultipleConfigurations,
        Feature_ProtectConfiguration2,
        Feature_StaticPassword,
        Feature_ScanCodeMode,
        Feature_ShortTicket,
        Feature_StrongPwd,
        Feature_OathHotp,
        Feature_ChallengeResponse,
        Feature_SerialNumber,
        Feature_MovingFactor,
        Feature_ChallengeResponseFixed,
        Feature_Updatable,
        Feature_Ndef,
        Feature_LedInvert,
        Feature_U2F,
    };

    State state() const
    {return m_state;}

    unsigned int version() const
    {return m_version;}

    unsigned int serial() const
    {return m_serial;}

    int touchLevel() const
    {return m_touchLevel;}

    YK_STATUS *status() const
    { return m_ykds;}

    QString versionStr();
    void reportError();

    bool checkFeatureSupport(Feature feature);

private:
    YubiKeyFinder();
    void init();
    bool openKey();
    bool closeKey();

    static YubiKeyFinder* _instance;
    static const unsigned int FEATURE_MATRIX[][2];
    static const unsigned int FEATURE_MATRIX_EXCLUDE[][4];

    State m_state;
    YK_KEY *m_yk;
    YK_STATUS *m_ykds;
    unsigned int m_version;
    unsigned int m_versionMinor;
    unsigned int m_versionMajor;
    unsigned int m_versionBuild;
    unsigned int m_serial;

    int m_touchLevel;

    QTimer *m_timer;

public slots:
    void findKey();
    void start();
    void stop();

signals:
    void keyFound(bool found, bool* featuresMatrix, int error);
    void errorOccurred(const QString &text);
    void diagnostics(const QString &text);
};

#endif // YUBIKEYFINDER_H
