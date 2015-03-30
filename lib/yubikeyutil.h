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
};

#endif // YUBIKEYUTIL_H
