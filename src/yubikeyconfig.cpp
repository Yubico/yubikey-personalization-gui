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

#include "yubikeyconfig.h"
#include "yubikeyutil.h"

#include <QSettings>

YubiKeyConfig::YubiKeyConfig() {
    m_mode = Mode_YubicoOtp;
    m_accMode = Acc_None;
    m_configSlot = 1;
    m_pubIdInHex = false;
    m_pubIdTxt.clear();
    m_pvtIdTxt.clear();
    m_secretKeyTxt.clear();
    m_currentAccessCodeTxt.clear();
    m_newAccessCodeTxt.clear();
    m_serial.clear();

    QSettings settings;

    m_tabFirst = settings.value(SG_TAB_FIRST).toBool();
    m_appendTab1 = settings.value(SG_APPEND_TAB1).toBool();
    m_appendTab2 = settings.value(SG_APPEND_TAB2).toBool();
    m_appendCr = settings.value(SG_APPEND_CR).toBool();
    m_appendDelay1 = settings.value(SG_APPEND_DELAY1).toBool();
    m_appendDelay2= settings.value(SG_APPEND_DELAY2).toBool();
    m_protectCfg2 = false; // XXX Not implemented

    m_sendRef = false;
    m_ticketFirst = false;
    m_pacing10ms = settings.value(SG_PACING_10MS).toBool();
    m_pacing20ms = settings.value(SG_PACING_20MS).toBool();
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
    m_hmacLT64 = false;
    m_serialBtnVisible = settings.value(SG_SR_BTN_VISIBLE).toBool();
    m_serialUsbVisible = settings.value(SG_SR_USB_VISIBLE).toBool();
    m_serialApiVisible = settings.value(SG_SR_API_VISIBLE).toBool();

    if(settings.contains(SG_ALLOW_UPDATE)) {
        m_allowUpdate = settings.value(SG_ALLOW_UPDATE).toBool();
    } else {
        m_allowUpdate = true;
    }

    m_dormant = false;
    m_fastTrig = settings.value(SG_FAST_TRIG).toBool();
    m_useNumericKeypad = settings.value(SG_USE_NUMERIC_KEYPAD).toBool();

    m_ledInvert = settings.value(SG_LED_INVERT).toBool();
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

void YubiKeyConfig::setNewAccessCodeTxt(const QString &newAccessCodeTxt, AccMode accMode) {
    m_newAccessCodeTxt = newAccessCodeTxt;
    YubiKeyUtil::qstrClean(&m_newAccessCodeTxt, 0);
    m_accMode = accMode;
}
