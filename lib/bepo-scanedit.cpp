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

#include "bepo-scanedit.h"

#define	SHIFT   0x80

BepoScanEdit::BepoScanEdit() {
    keyMap = QMap<int, int>();
    keyMap[0x09] = 0x2b | SHIFT; /* '\t' */
    keyMap[0x0a] = 0x28 | SHIFT; /* '\n' */
    keyMap[0x20] = 0x2c; /* ' ' */
    keyMap[0x21] = 0x1c | SHIFT; /* '!' */
    keyMap[0x22] = 0x1e; /* '"' */
    keyMap[0x23] = 0x35 | SHIFT; /* '#' */
    keyMap[0x24] = 0x35; /* '$' */
    keyMap[0x25] = 0x2e; /* '%' */
    keyMap[0x27] = 0x11; /* "'" */
    keyMap[0x28] = 0x21; /* '(' */
    keyMap[0x29] = 0x22; /* ')' */
    keyMap[0x2a] = 0x27; /* '*' */
    keyMap[0x2b] = 0x24; /* '+' */
    keyMap[0x2c] = 0x0a; /* ',' */
    keyMap[0x2d] = 0x25; /* '-' */
    keyMap[0x2e] = 0x19; /* '.' */
    keyMap[0x2f] = 0x26; /* '/' */
    keyMap[0x30] = 0x27 | SHIFT; /* '0' */
    keyMap[0x31] = 0x1e | SHIFT; /* '1' */
    keyMap[0x32] = 0x1f | SHIFT; /* '2' */
    keyMap[0x33] = 0x20 | SHIFT; /* '3' */
    keyMap[0x34] = 0x21 | SHIFT; /* '4' */
    keyMap[0x35] = 0x22 | SHIFT; /* '5' */
    keyMap[0x36] = 0x23 | SHIFT; /* '6' */
    keyMap[0x37] = 0x24 | SHIFT; /* '7' */
    keyMap[0x38] = 0x25 | SHIFT; /* '8' */
    keyMap[0x39] = 0x26 | SHIFT; /* '9' */
    keyMap[0x3a] = 0x19 | SHIFT; /* ':' */
    keyMap[0x3b] = 0x0a | SHIFT; /* ';' */
    keyMap[0x3d] = 0x2d; /* '=' */
    keyMap[0x3f] = 0x11 | SHIFT; /* '?' */
    keyMap[0x40] = 0x23; /* '@' */
    keyMap[0x41] = 0x04 | SHIFT; /* 'A' */
    keyMap[0x42] = 0x14 | SHIFT; /* 'B' */
    keyMap[0x43] = 0x0b | SHIFT; /* 'C' */
    keyMap[0x44] = 0x0c | SHIFT; /* 'D' */
    keyMap[0x45] = 0x09 | SHIFT; /* 'E' */
    keyMap[0x46] = 0x38 | SHIFT; /* 'F' */
    keyMap[0x47] = 0x36 | SHIFT; /* 'G' */
    keyMap[0x48] = 0x37 | SHIFT; /* 'H' */
    keyMap[0x49] = 0x07 | SHIFT; /* 'I' */
    keyMap[0x4a] = 0x13 | SHIFT; /* 'J' */
    keyMap[0x4b] = 0x05 | SHIFT; /* 'K' */
    keyMap[0x4c] = 0x12 | SHIFT; /* 'L' */
    keyMap[0x4d] = 0x34 | SHIFT; /* 'M' */
    keyMap[0x4e] = 0x33 | SHIFT; /* 'N' */
    keyMap[0x4f] = 0x15 | SHIFT; /* 'O' */
    keyMap[0x50] = 0x08 | SHIFT; /* 'P' */
    keyMap[0x51] = 0x10 | SHIFT; /* 'Q' */
    keyMap[0x52] = 0x0f | SHIFT; /* 'R' */
    keyMap[0x53] = 0x0e | SHIFT; /* 'S' */
    keyMap[0x54] = 0x0d | SHIFT; /* 'T' */
    keyMap[0x55] = 0x16 | SHIFT; /* 'U' */
    keyMap[0x56] = 0x18 | SHIFT; /* 'V' */
    keyMap[0x57] = 0x30 | SHIFT; /* 'W' */
    keyMap[0x58] = 0x06 | SHIFT; /* 'X' */
    keyMap[0x59] = 0x1b | SHIFT; /* 'Y' */
    keyMap[0x5a] = 0x2f | SHIFT; /* 'Z' */
    keyMap[0x60] = 0x2e | SHIFT; /* '`' */
    keyMap[0x61] = 0x04; /* 'a' */
    keyMap[0x62] = 0x14; /* 'b' */
    keyMap[0x63] = 0x0b; /* 'c' */
    keyMap[0x64] = 0x0c; /* 'd' */
    keyMap[0x65] = 0x09; /* 'e' */
    keyMap[0x66] = 0x38; /* 'f' */
    keyMap[0x67] = 0x36; /* 'g' */
    keyMap[0x68] = 0x37; /* 'h' */
    keyMap[0x69] = 0x07; /* 'i' */
    keyMap[0x6a] = 0x13; /* 'j' */
    keyMap[0x6b] = 0x05; /* 'k' */
    keyMap[0x6c] = 0x12; /* 'l' */
    keyMap[0x6d] = 0x34; /* 'm' */
    keyMap[0x6e] = 0x33; /* 'n' */
    keyMap[0x6f] = 0x15; /* 'o' */
    keyMap[0x70] = 0x08; /* 'p' */
    keyMap[0x71] = 0x10; /* 'q' */
    keyMap[0x72] = 0x0f; /* 'r' */
    keyMap[0x73] = 0x0e; /* 's' */
    keyMap[0x74] = 0x0d; /* 't' */
    keyMap[0x75] = 0x16; /* 'u' */
    keyMap[0x76] = 0x18; /* 'v' */
    keyMap[0x77] = 0x30; /* 'w' */
    keyMap[0x78] = 0x06; /* 'x' */
    keyMap[0x79] = 0x1b; /* 'y' */
    keyMap[0x7a] = 0x2f; /* 'z' */
    keyMap[0xa0] = 0x2c | SHIFT; /* '\xa0' non-breaking space */
    keyMap[0xab] = 0x1f; /* '«' */
    keyMap[0xb0] = 0x2d | SHIFT; /* '°' */
    keyMap[0xbb] = 0x20; /* '»' */
    keyMap[0xc0] = 0x1d | SHIFT; /* 'À' */
    keyMap[0xc7] = 0x31 | SHIFT; /* 'Ç' */
    keyMap[0xc8] = 0x17 | SHIFT; /* 'È' */
    keyMap[0xc9] = 0x1a | SHIFT; /* 'É' */
    keyMap[0xca] = 0x64 | SHIFT; /* 'Ê' */
    keyMap[0xe0] = 0x1d; /* 'à' */
    keyMap[0xe7] = 0x31; /* 'ç' */
    keyMap[0xe8] = 0x17; /* 'è' */
    keyMap[0xe9] = 0x1a; /* 'é' */
    keyMap[0xea] = 0x64; /* 'ê' */
}
