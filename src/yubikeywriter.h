/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef YUBIKEYWRITER_H
#define YUBIKEYWRITER_H

#include <QObject>

extern "C" {
#include <ykcore.h>
#include <ykdef.h>
#include <ykpers.h>
#include <yubikey.h>
}
#include "common.h"
#include "yubikeyconfig.h"

class YubiKeyConfig;

class YubiKeyWriter : public QObject {
    Q_OBJECT

public:
    static YubiKeyWriter* getInstance();
    ~YubiKeyWriter();

private:
    YubiKeyWriter();
    static YubiKeyWriter* _instance;

    QString reportError();

public slots:
    void writeConfig(YubiKeyConfig *ykConfig);

signals:
    void configWritten(bool written, const QString &msg);
    void errorOccurred(const QString &text);

};

#endif // YUBIKEYWRITER_H
