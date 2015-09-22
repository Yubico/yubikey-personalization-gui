/*
Copyright (C) 2011-2015 Yubico AB.  All rights reserved.

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

#include "scanedit.h"
#include "yubikeyutil.h"
#include "common.h"

#include <QDebug>

ScanEdit::ScanEdit() {
}

QString ScanEdit::textToScanCodes(const QString text) {
    QString scanCode;
    for(int i = 0; i < text.length(); i++) {
        QChar ch = text.at(i);
        unsigned char code = 0;
        if(ch == '\\') {
            if(i + 1 != text.length()) {
                QChar next = text.at(i + 1);
                if(next == '\\') {
                    i++;
                } else if(next == 't') {
                    i++;
                    ch = '\t';
                } else if(next == 'n') {
                    i++;
                    ch = '\n';
                }
            }
        }
        code = keyMap.value(ch.toLatin1() & 0xff, 0);
        if(code == 0) {
            qDebug() << "no mapping for" << ch;
        }
        QString hexTxt = YubiKeyUtil::qstrHexEncode(&code, 1);
        scanCode += hexTxt;
    }
    return scanCode;
}

QString ScanEdit::scanCodesToText(const QString scanCode) {
    QString text;
    for(int i = 0; i < scanCode.length(); i += 2) {
        bool ok;
        unsigned int code = scanCode.mid(i, 2).toUInt(&ok, 16);
        if(ok == true) {
            QString key;
            key += keyMap.key(code, 0);
            if(key == "\n") key = "\\n";
            if(key == "\t") key = "\\t";
            if(key == "\\") key = "\\\\";
            text += key;
        }
    }
    return text;
}
