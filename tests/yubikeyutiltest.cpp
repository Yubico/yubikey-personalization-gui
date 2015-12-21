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

#include "yubikeyutiltest.h"
#include "yubikeyutil.h"


void TestYubikeyUtil::testHexDecode() {
   unsigned char result[3];
   size_t len = 3;
   unsigned char expect[] = "foo";
   YubiKeyUtil::qstrHexDecode(result, &len, QString("666f6f"));
   QVERIFY(memcmp(expect, result, 3) == 0);
}

void TestYubikeyUtil::testHexEncode() {
   unsigned char string[] = "foo";
   QString result = YubiKeyUtil::qstrHexEncode(string, 3);
   QCOMPARE(result, QString("666f6f"));
}

void TestYubikeyUtil::testModhexDecode() {
   unsigned char result[3];
   size_t len = 3;
   unsigned char expect[] = "foo";
   YubiKeyUtil::qstrModhexDecode(result, &len, QString("hhhvhv"));
   QVERIFY(memcmp(expect, result, 3) == 0);
}

void TestYubikeyUtil::testModhexEncode() {
   unsigned char string[] = "foo";
   QString result = YubiKeyUtil::qstrModhexEncode(string, 3);
   QCOMPARE(result, QString("hhhvhv"));
}

void TestYubikeyUtil::testGetNextHex1() {
   QString result = YubiKeyUtil::getNextHex(6, "000000", GEN_SCHEME_INCR);
   QCOMPARE(result, QString("000001"));
}

void TestYubikeyUtil::testGetNextHex2() {
   QString result = YubiKeyUtil::getNextHex(6, "0fffff", GEN_SCHEME_INCR);
   QCOMPARE(result, QString("100000"));
}

void TestYubikeyUtil::testGetNextHex3() {
   QString result = YubiKeyUtil::getNextHex(6, "ffffff", GEN_SCHEME_INCR);
   QCOMPARE(result, QString("000000"));
}

void TestYubikeyUtil::testGetNextHex4() {
   QString result = YubiKeyUtil::getNextHex(6, "  1 fff f f  ", GEN_SCHEME_INCR);
   QCOMPARE(result, QString("200000"));
}

void TestYubikeyUtil::testGetNextHexRand() {
   QString result = YubiKeyUtil::getNextHex(6, "cccccc", GEN_SCHEME_RAND);
   QVERIFY(result != "");
}

void TestYubikeyUtil::testGetNextModhex1() {
   QString result = YubiKeyUtil::getNextModhex(6, "cccccc", GEN_SCHEME_INCR);
   QCOMPARE(result, QString("cccccb"));
}

void TestYubikeyUtil::testGetNextModhex2() {
   QString result = YubiKeyUtil::getNextModhex(6, "cvvvvv", GEN_SCHEME_INCR);
   QCOMPARE(result, QString("bccccc"));
}

void TestYubikeyUtil::testGetNextModhex3() {
   QString result = YubiKeyUtil::getNextModhex(6, "vvvvvv", GEN_SCHEME_INCR);
   QCOMPARE(result, QString("cccccc"));
}

void TestYubikeyUtil::testGetNextModhex4() {
   QString result = YubiKeyUtil::getNextModhex(6, "vvvvv", GEN_SCHEME_INCR);
   QCOMPARE(result, QString("vvvvvb"));
}

void TestYubikeyUtil::testGetNextModhex5() {
   QString result = YubiKeyUtil::getNextModhex(6, " cc cc cc    ", GEN_SCHEME_INCR);
   QCOMPARE(result, QString("cccccb"));
}

void TestYubikeyUtil::testGetNextModhexRand() {
   QString result = YubiKeyUtil::getNextModhex(6, "bbbbbb", GEN_SCHEME_RAND);
   QVERIFY(result != "");
}

void TestYubikeyUtil::testGenerateRandomHex() {
   QString first = YubiKeyUtil::generateRandomHex(16);
   QString second = YubiKeyUtil::generateRandomHex(16);
   QVERIFY(first != second);
}

void TestYubikeyUtil::testGenerateRandomModhex() {
   QString first = YubiKeyUtil::generateRandomModhex(16);
   QString second = YubiKeyUtil::generateRandomModhex(16);
   QVERIFY(first != second);
}
