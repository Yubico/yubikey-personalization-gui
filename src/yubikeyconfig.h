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

#ifndef YUBIKEYCONFIG_H
#define YUBIKEYCONFIG_H

#include <QString>
#include "common.h"

class YubiKeyConfig
{
public:
    YubiKeyConfig();

    enum Mode {
        Mode_YubicoOtp,
        Mode_Static,
        Mode_OathHotp,
        Mode_ChalRespYubico,
        Mode_ChalRespHmac,
        Mode_Update,
        Mode_Swap,
    };

    void setProgrammingMode(Mode mode)
    {m_mode = mode;}
    Mode programmingMode() const
    {return m_mode;}

    AccMode accMode() const
    {return m_accMode;}

    void setConfigSlot(int configSlot)
    {m_configSlot = configSlot;}
    int configSlot() const
    {return m_configSlot;}

    void setPubIdInHex(bool pubIdInHex)
    {m_pubIdInHex = pubIdInHex;}
    bool pubIdInHex() const
    {return m_pubIdInHex;}

    void setPubIdTxt(const QString &pubIdTxt);
    QString pubIdTxt() const
    {return m_pubIdTxt;}

    void setPvtIdTxt(const QString &pvtIdTxt);
    QString pvtIdTxt() const
    {return m_pvtIdTxt;}

    void setSecretKeyTxt(const QString &secretKeyTxt);
    QString secretKeyTxt() const
    {return m_secretKeyTxt;}

    void setCurrentAccessCodeTxt(const QString &currentAccessCodeTxt);
    QString currentAccessCodeTxt() const
    {return m_currentAccessCodeTxt;}

    void setNewAccessCodeTxt(const QString &newAccessCodeTxt, AccMode accMode);
    QString newAccessCodeTxt() const
    {return m_newAccessCodeTxt;}

    void setSerial(const QString &serial)
    {m_serial = serial;}
    QString serial() const
    {return m_serial;}

    void setTabFirst(bool tabFirst)
    {m_tabFirst = tabFirst;}
    bool tabFirst() const
    {return m_tabFirst;}

    void setAppendTab1(bool appendTab1)
    {m_appendTab1 = appendTab1;}
    bool appendTab1() const
    {return m_appendTab1;}

    void setAppendTab2(bool appendTab2)
    {m_appendTab2 = appendTab2;}
    bool appendTab2() const
    {return m_appendTab2;}

    void setAppendCr(bool appendCr)
    {m_appendCr = appendCr;}
    bool appendCr() const
    {return m_appendCr;}

    void setAppendDelay1(bool appendDelay1)
    {m_appendDelay1 = appendDelay1;}
    bool appendDelay1() const
    {return m_appendDelay1;}

    void setAppendDelay2(bool appendDelay2)
    {m_appendDelay2 = appendDelay2;}
    bool appendDelay2() const
    {return m_appendDelay2;}

    void setProtectCfg2(bool protectCfg2)
    {m_protectCfg2 = protectCfg2;}
    bool protectCfg2() const
    {return m_protectCfg2;}

    void setSendRef(bool sendRef)
    {m_sendRef = sendRef;}
    bool sendRef() const
    {return m_sendRef;}

    void setTicketFirst(bool ticketFirst)
    {m_ticketFirst = ticketFirst;}
    bool ticketFirst() const
    {return m_ticketFirst;}

    void setPacing10ms(bool pacing10ms)
    {m_pacing10ms = pacing10ms;}
    bool pacing10ms() const
    {return m_pacing10ms;}

    void setPacing20ms(bool pacing20ms)
    {m_pacing20ms = pacing20ms;}
    bool pacing20ms() const
    {return m_pacing20ms;}

    void setAllowHidtrig(bool allowHidtrig)
    {m_allowHidtrig = allowHidtrig;}
    bool allowHidtrig() const
    {return m_allowHidtrig;}

    void setStaticTicket(bool staticTicket)
    {m_staticTicket = staticTicket;}
    bool staticTicket() const
    {return m_staticTicket;}

    void setShortTicket(bool shortTicket)
    {m_shortTicket = shortTicket;}
    bool shortTicket() const
    {return m_shortTicket;}

    void setStrongPw1(bool strongPw1)
    {m_strongPw1 = strongPw1;}
    bool strongPw1() const
    {return m_strongPw1;}

    void setStrongPw2(bool strongPw2)
    {m_strongPw2 = strongPw2;}
    bool strongPw2() const
    {return m_strongPw2;}

    void setManUpdate(bool manUpdate)
    {m_manUpdate = manUpdate;}
    bool manUpdate() const
    {return m_manUpdate;}

    void setOathFixedModhex1(bool oathFixedModhex1)
    {m_oathFixedModhex1 = oathFixedModhex1;}
    bool oathFixedModhex1() const
    {return m_oathFixedModhex1;}

    void setOathFixedModhex2(bool oathFixedModhex2)
    {m_oathFixedModhex2 = oathFixedModhex2;}
    bool oathFixedModhex2() const
    {return m_oathFixedModhex2;}

    void setOathFixedModhex(bool oathFixedModhex)
    {m_oathFixedModhex = oathFixedModhex;}
    bool oathFixedModhex() const
    {return m_oathFixedModhex;}

    void setOathHotp8(bool oathHotp8)
    {m_oathHotp8 = oathHotp8;}
    bool oathHotp8() const
    {return m_oathHotp8;}

    void setOathMovingFactorSeed(unsigned int oathMovingFactorSeed)
    {m_oathMovingFactorSeed = oathMovingFactorSeed;}
    unsigned int oathMovingFactorSeed() const
    {return m_oathMovingFactorSeed;}

    void setChalYubico(bool chalYubico)
    {m_chalYubico = chalYubico;}
    bool chalYubico() const
    {return m_chalYubico;}

    void setChalHmac(bool chalHmac)
    {m_chalHmac = chalHmac;}
    bool chalHmac() const
    {return m_chalHmac;}

    void setHmacLT64(bool hmacLT64)
    {m_hmacLT64 = hmacLT64;}
    bool hmacLT64() const
    {return m_hmacLT64;}

    void setChalBtnTrig(bool chalBtnTrig)
    {m_chalBtnTrig = chalBtnTrig;}
    bool chalBtnTrig() const
    {return m_chalBtnTrig;}

    void setSerialBtnVisible(bool serialBtnVisible)
    {m_serialBtnVisible = serialBtnVisible;}
    bool serialBtnVisible() const
    {return m_serialBtnVisible;}

    void setSerialUsbVisible(bool serialUsbVisible)
    {m_serialUsbVisible = serialUsbVisible;}
    bool serialUsbVisible() const
    {return m_serialUsbVisible;}

    void setSerialApiVisible(bool serialApiVisible)
    {m_serialApiVisible = serialApiVisible;}
    bool serialApiVisible() const
    {return m_serialApiVisible;}

    void setAllowUpdate(bool allowUpdate)
    {m_allowUpdate = allowUpdate;}
    bool allowUpdate() const
    {return m_allowUpdate;}

    void setDormant(bool dormant)
    {m_dormant = dormant;}
    bool dormant() const
    {return m_dormant;}

    void setFastTrig(bool fastTrig)
    {m_fastTrig = fastTrig;}
    bool fastTrig() const
    {return m_fastTrig;}

    void setUseNumericKeypad(bool useNumericKeypad)
    {m_useNumericKeypad = useNumericKeypad;}
    bool useNumericKeypad() const
    {return m_useNumericKeypad;}

    void setLedInvert(bool ledInvert)
    {m_ledInvert = ledInvert;}
    bool ledInvert() const
    {return m_ledInvert;}

private:
    Mode m_mode;
    AccMode m_accMode;
    int m_configSlot;
    bool m_pubIdInHex;
    QString m_pubIdTxt;
    QString m_pvtIdTxt;
    QString m_secretKeyTxt;
    QString m_currentAccessCodeTxt;
    QString m_newAccessCodeTxt;

    QString m_serial;

    //Ticket flags
    bool m_tabFirst;
    bool m_appendTab1;
    bool m_appendTab2;
    bool m_appendCr;
    bool m_appendDelay1;
    bool m_appendDelay2;
    bool m_protectCfg2;

    //Configuration flags
    bool m_sendRef;
    bool m_ticketFirst;
    bool m_pacing10ms;
    bool m_pacing20ms;
    bool m_allowHidtrig;

    bool m_staticTicket;
    bool m_shortTicket;
    bool m_strongPw1;
    bool m_strongPw2;
    bool m_manUpdate;

    bool m_oathFixedModhex1;
    bool m_oathFixedModhex2;
    bool m_oathFixedModhex;
    bool m_oathHotp8;
    unsigned int m_oathMovingFactorSeed;

    bool m_chalYubico;
    bool m_chalHmac;
    bool m_hmacLT64;
    bool m_chalBtnTrig;

    bool m_serialBtnVisible;
    bool m_serialUsbVisible;
    bool m_serialApiVisible;

    // v2.3 settings
    bool m_allowUpdate;
    bool m_dormant;
    bool m_fastTrig;
    bool m_useNumericKeypad;

    bool m_ledInvert;
};

#endif // YUBIKEYCONFIG_H
