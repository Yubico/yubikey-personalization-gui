/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef YUBIKEYFINDER_H
#define YUBIKEYFINDER_H

#include <QObject>

extern "C" {
#include "deps/libykpers/ykcore/ykcore.h"
#include "deps/libykpers/ykcore/ykdef.h"
#include "deps/libykpers/ykpers.h"
#include "deps/libyubikey/yubikey.h"
}
#include "common.h"
#include "qtimer.h"

class YubiKeyFinder : public QObject {
    Q_OBJECT

public:
    static YubiKeyFinder* getInstance();
    ~YubiKeyFinder();

    enum State {
        State_Absent,
        State_Preset,
        State_Processing
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
    };

    State state() const
    {return m_state;}

    unsigned int version() const
    {return m_version;}

    unsigned int serial() const
    {return m_serial;}

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

    State m_state;
    YK_KEY *m_yk;
    unsigned int m_version;
    unsigned int m_versionMinor;
    unsigned int m_versionMajor;
    unsigned int m_versionBuild;
    unsigned int m_serial;

    QTimer *m_timer;

public slots:
    void findKey();
    void start();
    void stop();

signals:
    void keyFound(bool found, bool* featuresMatrix);
    void errorOccurred(const QString &text);
};

#endif // YUBIKEYFINDER_H
