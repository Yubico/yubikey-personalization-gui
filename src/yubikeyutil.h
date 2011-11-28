/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef YUBIKEYUTIL_H
#define YUBIKEYUTIL_H

#include <QtCore/QString>
#include "common.h"

class YubiKeyUtil {

public:
    ~YubiKeyUtil();

    static int hexModhexDecode
            (unsigned char *result, size_t *resultLen,
             const char *str, size_t strLen,
             size_t minSize, size_t maxSize,
             bool modhex);

    static int hexModhexEncode
            (char *result, size_t *resultLen,
             const unsigned char *str, size_t strLen,
             bool modhex);

    static QString qstrHexEncode
            (const unsigned char *str, size_t strLen);

    static void qstrHexDecode
            (unsigned char *result, size_t *resultLen,
             const QString &str);

    static QString qstrModhexEncode
            (const unsigned char *str, size_t strLen);

    static void qstrModhexDecode
            (unsigned char *result, size_t *resultLen,
             const QString &str);

    static void qstrDecDecode
            (unsigned char *result, size_t *resultLen,
             const QString &str);

    static void qstrToRaw
            (char *result, size_t resultLen,
             const QString &str);

    static void qstrClean
            (QString *str, size_t maxSize, bool reverse = false);

    static void qstrModhexClean
            (QString *str, size_t maxSize, bool reverse = false);

    static int generateRandom
            (unsigned char *result, size_t resultLen);

    static QString generateRandomHex
            (size_t resultLen);

    static QString generateRandomModhex(
            size_t resultLen);

    static QString getNextHex
            (size_t resultLen,
             const QString &str, int scheme);

    static QString getNextModhex(
            size_t resultLen,
            const QString &str, int scheme);

    static void hexdump
            (void *buffer, int size);
};

#endif // YUBIKEYUTIL_H
