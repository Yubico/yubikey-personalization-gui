/*
Copyright (C) 2015 Yubico AB.  All rights reserved.

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

#include "it-scanedit.h"

#define	SHIFT   0x80

ItScanEdit::ItScanEdit() {
    keyMap = QMap<int, int>();
    keyMap[0x09] = 0x2b; /* \t */
    keyMap[0x0a] = 0x28; /* \n */
    keyMap[0x20] = 0x2c; /*   */
    keyMap[0x21] = 0x1e | SHIFT; /* ! */
    keyMap[0x22] = 0x1f | SHIFT; /* " */
    keyMap[0x23] = 0x32; /* # */
    keyMap[0x24] = 0x21 | SHIFT; /* $ */
    keyMap[0x25] = 0x22 | SHIFT; /* % */
    keyMap[0x26] = 0x23 | SHIFT; /* & */
    keyMap[0x27] = 0x2d; /* ' */
    keyMap[0x28] = 0x25 | SHIFT; /* ( */
    keyMap[0x29] = 0x26 | SHIFT; /* ) */
    keyMap[0x2a] = 0x55; /* * */
    keyMap[0x2b] = 0x30; /* + */
    keyMap[0x2c] = 0x36; /* , */
    keyMap[0x2d] = 0x38; /* - */
    keyMap[0x2e] = 0x63; /* . */
    keyMap[0x2f] = 0x24 | SHIFT; /* / */
    keyMap[0x30] = 0x27; /* 0 */
    keyMap[0x31] = 0x1e; /* 1 */
    keyMap[0x32] = 0x1f; /* 2 */
    keyMap[0x33] = 0x20; /* 3 */
    keyMap[0x34] = 0x21; /* 4 */
    keyMap[0x35] = 0x22; /* 5 */
    keyMap[0x36] = 0x23; /* 6 */
    keyMap[0x37] = 0x24; /* 7 */
    keyMap[0x38] = 0x25; /* 8 */
    keyMap[0x39] = 0x26; /* 9 */
    keyMap[0x3a] = 0xb7; /* : */
    keyMap[0x3b] = 0xb6; /* ; */
    keyMap[0x3c] = 0x64; /* < */
    keyMap[0x3d] = 0x27 | SHIFT; /* = */
    keyMap[0x3e] = 0x64 | SHIFT; /* > */
    keyMap[0x3f] = 0x2d | SHIFT; /* ? */
    keyMap[0x40] = 0x24; /* @ */
    keyMap[0x41] = 0x04 | SHIFT; /* A */
    keyMap[0x42] = 0x05 | SHIFT; /* B */
    keyMap[0x43] = 0x06 | SHIFT; /* C */
    keyMap[0x44] = 0x07 | SHIFT; /* D */
    keyMap[0x45] = 0x08 | SHIFT; /* E */
    keyMap[0x46] = 0x09 | SHIFT; /* F */
    keyMap[0x47] = 0x0a | SHIFT; /* G */
    keyMap[0x48] = 0x0b | SHIFT; /* H */
    keyMap[0x49] = 0x0c | SHIFT; /* I */
    keyMap[0x4a] = 0x0d | SHIFT; /* J */
    keyMap[0x4b] = 0x0e | SHIFT; /* K */
    keyMap[0x4c] = 0x0f | SHIFT; /* L */
    keyMap[0x4d] = 0x10 | SHIFT; /* M */
    keyMap[0x4e] = 0x11 | SHIFT; /* N */
    keyMap[0x4f] = 0x12 | SHIFT; /* O */
    keyMap[0x50] = 0x13 | SHIFT; /* P */
    keyMap[0x51] = 0x14 | SHIFT; /* Q */
    keyMap[0x52] = 0x15 | SHIFT; /* R */
    keyMap[0x53] = 0x16 | SHIFT; /* S */
    keyMap[0x54] = 0x17 | SHIFT; /* T */
    keyMap[0x55] = 0x18 | SHIFT; /* U */
    keyMap[0x56] = 0x19 | SHIFT; /* V */
    keyMap[0x57] = 0x1a | SHIFT; /* W */
    keyMap[0x58] = 0x1b | SHIFT; /* X */
    keyMap[0x59] = 0x1c | SHIFT; /* Y */
    keyMap[0x5a] = 0x1d | SHIFT; /* Z */
    keyMap[0x5b] = 0; /* [ */
    keyMap[0x5c] = 0x35; /* \ */
    keyMap[0x5d] = 0; /* ] */
    keyMap[0x5e] = 0xae; /* ^ */
    keyMap[0x5f] = 0xb8; /* _ */
    keyMap[0x60] = 0x2d | SHIFT; /* ` */
    keyMap[0x61] = 0x04; /* a */
    keyMap[0x62] = 0x05; /* b */
    keyMap[0x63] = 0x06; /* c */
    keyMap[0x64] = 0x07; /* d */
    keyMap[0x65] = 0x08; /* e */
    keyMap[0x66] = 0x09; /* f */
    keyMap[0x67] = 0x0a; /* g */
    keyMap[0x68] = 0x0b; /* h */
    keyMap[0x69] = 0x0c; /* i */
    keyMap[0x6a] = 0x0d; /* j */
    keyMap[0x6b] = 0x0e; /* k */
    keyMap[0x6c] = 0x0f; /* l */
    keyMap[0x6d] = 0x10; /* m */
    keyMap[0x6e] = 0x11; /* n */
    keyMap[0x6f] = 0x12; /* o */
    keyMap[0x70] = 0x13; /* p */
    keyMap[0x71] = 0x14; /* q */
    keyMap[0x72] = 0x15; /* r */
    keyMap[0x73] = 0x16; /* s */
    keyMap[0x74] = 0x17; /* t */
    keyMap[0x75] = 0x18; /* u */
    keyMap[0x76] = 0x19; /* v */
    keyMap[0x77] = 0x1a; /* w */
    keyMap[0x78] = 0x1b; /* x */
    keyMap[0x79] = 0x1c; /* y */
    keyMap[0x7a] = 0x1d; /* z */
    keyMap[0x7c] = 0xb5; /* | */
    keyMap[0xa3] = 0xa0; /* £ */
    keyMap[0xa7] = 0xb2; /* § */
    keyMap[0xb0] = 0xb4; /* ° */
    keyMap[0xe7] = 0xb3; /* ç */
    keyMap[0xe8] = 0x2f; /* è */
    keyMap[0xe9] = 0x2f | SHIFT; /* é */
    keyMap[0xe0] = 0x34; /* à */
    keyMap[0xec] = 0x2e; /* ì */
    keyMap[0xf2] = 0x33; /* ò */
    keyMap[0xf9] = 0x31; /* ù */
}
