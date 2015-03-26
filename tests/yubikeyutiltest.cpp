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
   QString hex = "666f6f";
   unsigned char result[3];
   size_t len = 3;
   unsigned char expect[] = "foo";
   YubiKeyUtil::qstrHexDecode(result, &len, hex);
   QVERIFY(memcmp(expect, result, 3) == 0);
}

void TestYubikeyUtil::testHexEncode() {
   unsigned char string[] = "foo";
   QString expect = "666f6f";
   QString result = YubiKeyUtil::qstrHexEncode(string, 3);
   QCOMPARE(expect, result);
}

void TestYubikeyUtil::testModhexDecode() {
   QString modhex = "hhhvhv";
   unsigned char result[3];
   size_t len = 3;
   unsigned char expect[] = "foo";
   YubiKeyUtil::qstrModhexDecode(result, &len, modhex);
   QVERIFY(memcmp(expect, result, 3) == 0);
}

void TestYubikeyUtil::testModhexEncode() {
   unsigned char string[] = "foo";
   QString expect = "hhhvhv";
   QString result = YubiKeyUtil::qstrModhexEncode(string, 3);
   QCOMPARE(expect, result);
}
