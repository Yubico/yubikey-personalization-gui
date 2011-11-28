/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#include "crandom.h"

CRandom::CRandom(void) {
    m_crypt = 0;

    if (!CryptAcquireContext(&m_crypt, 0, 0, PROV_RSA_FULL, 0))
        if (!CryptAcquireContext(&m_crypt, 0, 0, PROV_RSA_FULL, CRYPT_NEWKEYSET)) m_crypt = 0;
}

CRandom::~CRandom(void) {
    if (m_crypt) CryptReleaseContext(m_crypt, 0);
}

bool CRandom::getRand(BYTE *buf, int size) {
    if (!m_crypt) return false;

    return CryptGenRandom(m_crypt, size, buf) != 0;
}
