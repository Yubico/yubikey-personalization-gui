/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
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
