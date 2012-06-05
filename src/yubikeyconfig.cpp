/*
Copyright (C) 2011-2012 Yubico AB.  All rights reserved.

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

#include "yubikeyconfig.h"
#include "yubikeyutil.h"

YubiKeyConfig::YubiKeyConfig() {
    m_mode = Mode_YubicoOtp;
    m_configSlot = 1;
    m_pubIdInHex = false;
    m_pubIdTxt.clear();
    m_pvtIdTxt.clear();
    m_secretKeyTxt.clear();
    m_currentAccessCodeTxt.clear();
    m_newAccessCodeTxt.clear();

    m_tabFirst = false;
    m_appendTab1 = false;
    m_appendTab2 = false;
    m_appendCr = false;
    m_appendDelay1 = false;
    m_appendDelay2= false;
    m_protectCfg2 = false;

    m_sendRef = false;
    m_ticketFirst = false;
    m_pacing10ms = false;
    m_pacing20ms = false;
    m_allowHidtrig = false;
    m_staticTicket = false;
    m_shortTicket = false;
    m_strongPw1 = false;
    m_strongPw2 = false;
    m_manUpdate = false;

    m_oathFixedModhex1 = false;
    m_oathFixedModhex2 = false;
    m_oathFixedModhex = false;
    m_oathHotp8 = false;
    m_oathMovingFactorSeed = 0;

    m_chalYubico = false;
    m_chalHmac = false;
    m_chalBtnTrig = false;
    m_serialBtnVisible = false;
    m_serialUsbVisible = false;
    m_serialApiVisible = false;
}

void YubiKeyConfig::setPubIdTxt(const QString &pubIdTxt) {
    m_pubIdTxt = pubIdTxt;
    if(m_pubIdInHex) {
        YubiKeyUtil::qstrClean(&m_pubIdTxt, 0);
    } else {
        YubiKeyUtil::qstrModhexClean(&m_pubIdTxt, 0);
    }
}

void YubiKeyConfig::setPvtIdTxt(const QString &pvtIdTxt) {
    m_pvtIdTxt = pvtIdTxt;
    YubiKeyUtil::qstrClean(&m_pvtIdTxt, 0);
}

void YubiKeyConfig::setSecretKeyTxt(const QString &secretKeyTxt) {
    m_secretKeyTxt = secretKeyTxt;
    YubiKeyUtil::qstrClean(&m_secretKeyTxt, 0);
}

void YubiKeyConfig::setCurrentAccessCodeTxt(const QString &currentAccessCodeTxt) {
    m_currentAccessCodeTxt = currentAccessCodeTxt;
    YubiKeyUtil::qstrClean(&m_currentAccessCodeTxt, 0);
}

void YubiKeyConfig::setNewAccessCodeTxt(const QString &newAccessCodeTxt) {
    m_newAccessCodeTxt = newAccessCodeTxt;
    YubiKeyUtil::qstrClean(&m_newAccessCodeTxt, 0);
}
