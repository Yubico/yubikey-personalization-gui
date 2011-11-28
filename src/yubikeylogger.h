/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef YUBIKEYLOGGER_H
#define YUBIKEYLOGGER_H

#include <QObject>
#include <QtCore/QString>

#include "yubikeyconfig.h"

class YubiKeyLogger : public QObject {

public:
    ~YubiKeyLogger();
    static void logConfig(YubiKeyConfig *ykConfig);

    static void enableLogging();
    static void disableLogging();
    static bool isLogging();

    static void setLogFilename(const QString &filename);
    static QString logFilename();
    static QString defaultLogFilename();

private:
    static bool m_enabled;
    static QString m_filename;
    static bool m_started;
};

#endif // YUBIKEYLOGGER_H
