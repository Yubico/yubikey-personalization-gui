/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#include "yubikeyutil.h"
#ifdef Q_WS_WIN
#include "crandom.h"
#endif

YubiKeyUtil::~YubiKeyUtil() {
}

int YubiKeyUtil::hexModhexDecode(unsigned char *result, size_t *resultLen,
                                 const char *str, size_t strLen,
                                 size_t minSize, size_t maxSize,
                                 bool modhex)
{
    if ((strLen % 2 != 0) || (strLen < minSize) || (strLen > maxSize)) {
        return -1;
    }

    *resultLen = strLen / 2;
    if (modhex) {
        if (yubikey_modhex_p(str)) {
            yubikey_modhex_decode((char *)result, str, strLen);
            return 1;
        }
    } else {
        if (yubikey_hex_p(str)) {
            yubikey_hex_decode((char *)result, str, strLen);
            return 1;
        }
    }

    return 0;
}

int YubiKeyUtil::hexModhexEncode(char *result, size_t *resultLen,
                                 const unsigned char *str, size_t strLen,
                                 bool modhex)
{
    *resultLen = strLen * 2;
    if (modhex) {
        yubikey_modhex_encode((char *)result, (char *)str, strLen);
        return 1;
    } else {
        yubikey_hex_encode((char *)result, (char *)str, strLen);
        return 1;
    }

    return 0;
}

QString YubiKeyUtil::qstrHexEncode(const unsigned char *str, size_t strLen) {
    char result[strLen * 2 + 1];
    size_t resultLen = 0;
    memset(&result, 0, sizeof(result));

    int rc = hexModhexEncode(result, &resultLen, str, strLen, false);

    if(rc > 0) {
        qDebug("hex encoded string: -%s- (%u)", result, sizeof(result));
        return QString::fromLocal8Bit(result);
    }

    return QString("");
}

void YubiKeyUtil::qstrHexDecode(unsigned char *result, size_t *resultLen,
                                const QString &str) {

    if(str.size() % 2 != 0) {
        return;
    }

    char hex[MAX_SIZE];
    YubiKeyUtil::qstrToRaw(hex, sizeof(hex), str);
    size_t hexLen = strlen(hex);

    //Hex decode
    hexModhexDecode(result, resultLen,
                    hex, hexLen,
                    0, MAX_SIZE,
                    false);
}

QString YubiKeyUtil::qstrModhexEncode(const unsigned char *str, size_t strLen) {
    char result[strLen * 2 + 1];
    size_t resultLen = 0;
    memset(&result, 0, sizeof(result));

    int rc = hexModhexEncode(result, &resultLen, str, strLen, true);

    if(rc > 0) {
        qDebug("modhex encoded string: -%s- (%u)", result, sizeof(result));
        return QString::fromLocal8Bit(result);
    }

    return QString("");
}

void YubiKeyUtil::qstrModhexDecode(unsigned char *result, size_t *resultLen,
                                   const QString &str) {

    if(str.size() % 2 != 0) {
        return;
    }

    char modhex[MAX_SIZE];
    YubiKeyUtil::qstrToRaw(modhex, sizeof(modhex), str);
    size_t modhexLen = strlen(modhex);

    //Hex decode
    hexModhexDecode(result, resultLen,
                    modhex, modhexLen,
                    0, MAX_SIZE,
                    true);
}

void YubiKeyUtil::qstrDecDecode(unsigned char *result, size_t *resultLen,
                                const QString &str) {
    if(str.size() % 2 != 0) {
        return;
    }

    *resultLen = str.size() / 2;

    unsigned char val = 0;
    for(size_t i = 0; i < *resultLen; i++) {
        val = str.mid(i * 2, 2).toInt();
        result[i] = ((val / 10) << 4) | (val % 10);
    }
}

void YubiKeyUtil::qstrToRaw(char *result, size_t resultLen,
                            const QString &str) {
    QByteArray strByteArr = str.toLocal8Bit();

    size_t strLen = strByteArr.size() + 1;
    strLen = (resultLen < strLen)? resultLen : strLen;

    memset(result, 0, strLen);
    strncpy(result, (char *) strByteArr.data(), strLen);
}

void YubiKeyUtil::qstrClean(QString *str, size_t maxSize, bool reverse) {
    *str = str->toLower();

    QRegExp rx("[^0-9a-f]");
    *str = str->replace(rx, QString(""));

    if(maxSize > 0) {
        if(reverse) {
            *str = str->rightJustified(maxSize, '0', true);
        } else {
            *str = str->leftJustified(maxSize, '0', true);
        }
    }
}

void YubiKeyUtil::qstrModhexClean(QString *str, size_t maxSize, bool reverse) {
    *str = str->toLower();

    QRegExp rx("[^b-lnrt-v]");
    *str = str->replace(rx, QString(""));

    if(maxSize > 0) {
        if(reverse) {
            *str = str->rightJustified(maxSize, 'c', true);
        } else {
            *str = str->leftJustified(maxSize, 'c', true);
        }
    }
}

int YubiKeyUtil::generateRandom(unsigned char *result, size_t resultLen) {
    size_t bufSize = resultLen;

    unsigned char buf[bufSize];
    memset(&buf, 0, sizeof(buf));

    size_t bufLen = 0;

#ifdef Q_WS_WIN
    CRandom random;
    random.getRand(buf, bufSize);

    bufLen = sizeof(buf);
#else
    char *random_places[] = {
        "/dev/srandom",
        "/dev/urandom",
        "/dev/random",
        0
    };

    char **random_place;

    for (random_place = random_places; *random_place; random_place++) {
        FILE *random_file = fopen(*random_place, "r");
        if (random_file) {
            size_t read_bytes = 0;

            while (read_bytes < bufSize) {
                size_t n = fread(&buf[read_bytes],
                                 1, bufSize - read_bytes,
                                 random_file);
                read_bytes += n;
            }

            fclose(random_file);

            bufLen = sizeof(buf);

            break; /* from for loop */
        }
    }
#endif

    if(bufLen > 0) {
        memcpy(result, buf, bufLen);
        return 1;
    }

    return 0;
}

QString YubiKeyUtil::generateRandomHex(size_t resultLen) {
    QString result("");

    if (resultLen % 2 != 0) {
        return result;
    }

    size_t bufSize = resultLen / 2;
    unsigned char buf[bufSize];
    memset(&buf, 0, sizeof(buf));

    if(generateRandom(buf, bufSize) > 0) {
        result = qstrHexEncode(buf, bufSize);
    }

    return result;
}

QString YubiKeyUtil::generateRandomModhex(size_t resultLen) {
    QString result("");

    if (resultLen % 2 != 0) {
        return result;
    }

    size_t bufSize = resultLen / 2;
    unsigned char buf[bufSize];
    memset(&buf, 0, sizeof(buf));

    if(generateRandom(buf, bufSize) > 0) {
        result = qstrModhexEncode(buf, bufSize);
    }

    return result;
}

QString YubiKeyUtil::getNextHex(size_t resultLen,
                                const QString &str, int scheme) {
    QString result("");

    qDebug() << "str = " << str
            << " len = " << str.length();

    switch(scheme) {
    case GEN_SCHEME_FIXED:
        result = str;
        break;

    case GEN_SCHEME_INCR:
        {
            //Hex clean
            QString hexStr(str);
            qstrClean(&hexStr, resultLen);

            //Hex decode
            unsigned char hexDecoded[MAX_SIZE];
            size_t hexDecodedLen = 0;
            memset(&hexDecoded, 0, sizeof(hexDecoded));

            qstrHexDecode(hexDecoded, &hexDecodedLen, hexStr);
            if(hexDecodedLen <= 0) {
                break;
            }

            qDebug() << "hexDecoded = " << QString((char*)hexDecoded)
                    << " len = " << hexDecodedLen;

            //Increment
            for (int i = hexDecodedLen; i--; ) {
                if (++hexDecoded[i]) {
                    break;
                }
            }

            //Hex encode
            result = qstrHexEncode(hexDecoded, hexDecodedLen);

            qDebug() << "hexEncoded = " << result
                    << " len = " << result.size();
        }
        break;

    case GEN_SCHEME_RAND:
        result = generateRandomHex(resultLen);
        break;
    }

    return result;
}

QString YubiKeyUtil::getNextModhex(size_t resultLen,
                                   const QString &str, int scheme) {
    QString result("");

    qDebug() << "str = " << str
            << " len = " << str.length();

    switch(scheme) {
    case GEN_SCHEME_FIXED:
        result = str;
        break;

    case GEN_SCHEME_INCR:
        {
            //Modhex clean
            QString modhexStr(str);
            qstrModhexClean(&modhexStr, resultLen);

            //Modhex decode
            unsigned char modhexDecoded[MAX_SIZE];
            size_t modhexDecodedLen = 0;
            memset(&modhexDecoded, 0, sizeof(modhexDecoded));

            qstrModhexDecode(modhexDecoded, &modhexDecodedLen, modhexStr);
            if(modhexDecodedLen <= 0) {
                break;
            }

            qDebug() << "modhexDecoded = " << QString((char*)modhexDecoded)
                    << " len = " << modhexDecodedLen;

            //Increment
            for (int i = modhexDecodedLen; i--; ) {
                if (++modhexDecoded[i]) {
                    break;
                }
            }

            //Modhex encode
            result = qstrModhexEncode(modhexDecoded, modhexDecodedLen);

            qDebug() << "modhexEncoded = " << result
                    << " len = " << result.size();
        }
        break;

    case GEN_SCHEME_RAND:
        result = generateRandomModhex(resultLen);
        break;
    }

    return result;
}

void YubiKeyUtil::hexdump(void *buffer, int size) {
    unsigned char *p = (unsigned char *)buffer;
    int i;
    for (i = 0; i < size; i++) {
        fprintf(stderr, "\\x%02x", *p);
        p++;
    }
    fprintf(stderr, "\n");
    fflush(stderr);
}
