/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
 */

#ifndef CRANDOM_H
#define CRANDOM_H

#include <windows.h>
#include <wincrypt.h>

class CRandom
{
private:
    HCRYPTPROV m_crypt;

public:
    CRandom();
    ~CRandom(void);
    bool getRand(BYTE *buf, int size);

};

#endif // CRANDOM_H
