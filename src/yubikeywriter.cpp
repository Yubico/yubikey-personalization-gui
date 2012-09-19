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

#include "yubikeywriter.h"
#include "yubikeyfinder.h"
#include "yubikeylogger.h"

YubiKeyWriter* YubiKeyWriter::_instance = 0;

#define TKTFLAG(f, s) \
if(s) { if (!ykp_set_tktflag_##f(cfg, s)) { error = true; throw 0; } }


#define CFGFLAG(f, s) \
if(s) { if (!ykp_set_cfgflag_##f(cfg, s)) { error = true; throw 0; } }

#define EXTFLAG(f, s) \
if(s) { if (!ykp_set_extflag_##f(cfg, s)) { error = true; throw 0; } }


static int writer(const char *buf, size_t count, void *stream) {
    return (int)fwrite(buf, 1, count, (FILE *)stream);
}

YubiKeyWriter::YubiKeyWriter() {
}

YubiKeyWriter::~YubiKeyWriter() {
    if(_instance) {
        delete _instance;
    }
}

YubiKeyWriter* YubiKeyWriter::getInstance() {
    if(_instance == NULL) {
        _instance = new YubiKeyWriter();
    }
    return _instance;
}

QString YubiKeyWriter::reportError() {
    QString errMsg;

    if (ykp_errno) {
        qDebug("Yubikey personalization error: %s\n", ykp_strerror(ykp_errno));

        switch(ykp_errno) {
        case YKP_EYUBIKEYVER:
        case YKP_EOLDYUBIKEY:
            errMsg = ERR_FEATURE_NOT_SUPPORTED;
            break;
        default:
            errMsg = ERR_PROCESSING;
            break;
        }
        ykp_errno = 0;
    } else if (yk_errno) {
        if (yk_errno == YK_EUSBERR) {
            qDebug("USB error: %s\n", yk_usb_strerror());
        } else {
            qDebug("Yubikey core error: %s\n", yk_strerror(yk_errno));
        }

        switch(yk_errno) {
        case YK_ENOKEY:
            errMsg = ERR_KEY_NOT_FOUND;
            break;
        case YK_EFIRMWARE:
            errMsg = ERR_FIRMWARE_NOT_SUPPORTED;
            break;
        default:
            errMsg = ERR_PROCESSING;
            break;
        }
        yk_errno = 0;
    }


    if(!errMsg.isEmpty()) {
        emit errorOccurred(errMsg);
    }

    return errMsg;
}

void YubiKeyWriter::writeConfig(YubiKeyConfig *ykConfig) {
    // Check features support
    bool flagSrNoSupport = false;
    if(YubiKeyFinder::getInstance()->checkFeatureSupport(
            YubiKeyFinder::Feature_SerialNumber)) {
        flagSrNoSupport = true;
    }

    YubiKeyFinder::getInstance()->stop();

    YK_KEY *yk = 0;
    YK_STATUS *ykst = ykds_alloc();
    YKP_CONFIG *cfg = ykp_alloc();

    bool error = false;

    qDebug() << "-------------------------";
    qDebug() << "Starting write config";
    qDebug() << "-------------------------";

    try {
        if (!yk_init()) {
            throw 0;
        } else if (!(yk = yk_open_first_key())) {
            throw 0;
        } else if (!yk_get_status(yk, ykst)) {
            throw 0;
        }

        if (!(yk_check_firmware_version(yk))) {
            throw 0;
        }

        ykp_configure_version(cfg, ykst);

        qDebug() << "writer:configuration slot:" << ykConfig->configSlot();

        //Configuration slot...
        if (!ykp_configure_command(cfg,
              ykConfig->configSlot() == 2 ? SLOT_CONFIG2 : SLOT_CONFIG)) {
            throw 0;
        }

        //Programming Mode...
        bool longSecretKey = false;

        switch(ykConfig->programmingMode()) {
        case YubiKeyConfig::Mode_YubicoOtp:
            break;

        case YubiKeyConfig::Mode_Static:
            CFGFLAG(STATIC_TICKET,      ykConfig->staticTicket());
            CFGFLAG(SHORT_TICKET,       ykConfig->shortTicket());
            CFGFLAG(STRONG_PW1,         ykConfig->strongPw1());
            CFGFLAG(STRONG_PW2,         ykConfig->strongPw2());
            CFGFLAG(MAN_UPDATE,         ykConfig->manUpdate());
            CFGFLAG(SEND_REF,           ykConfig->sendRef());
            break;

        case YubiKeyConfig::Mode_OathHotp:
            TKTFLAG(OATH_HOTP, true);

            CFGFLAG(OATH_HOTP8,         ykConfig->oathHotp8());
            CFGFLAG(OATH_FIXED_MODHEX1, ykConfig->oathFixedModhex1());
            CFGFLAG(OATH_FIXED_MODHEX2, ykConfig->oathFixedModhex2());
            CFGFLAG(OATH_FIXED_MODHEX,  ykConfig->oathFixedModhex());

            //Moving Factor Seed...
            if(!ykp_set_oath_imf(cfg, ykConfig->oathMovingFactorSeed())) {
              throw 0;
            }

            //For OATH-HOTP, 160 bits key is also valid
            longSecretKey = true;
            break;

        case YubiKeyConfig::Mode_ChalRespYubico:
            TKTFLAG(CHAL_RESP, true);

            CFGFLAG(CHAL_YUBICO,        ykConfig->chalYubico());
            CFGFLAG(CHAL_BTN_TRIG,      ykConfig->chalBtnTrig());
            break;

        case YubiKeyConfig::Mode_ChalRespHmac:
            TKTFLAG(CHAL_RESP, true);

            CFGFLAG(CHAL_HMAC,          ykConfig->chalHmac());
            CFGFLAG(HMAC_LT64,          ykConfig->hmacLT64());
            CFGFLAG(CHAL_BTN_TRIG,      ykConfig->chalBtnTrig());

            //For HMAC (not Yubico) challenge-response, 160 bits key is also valid
            longSecretKey = true;
            break;
        }

        //Public ID...
        if(ykConfig->pubIdTxt().length() > 0) {
            qDebug() << "Pub id: " << ykConfig->pubIdTxt()
                    << "length:" << ykConfig->pubIdTxt().length();

            char pubIdStr[MAX_SIZE];
            YubiKeyUtil::qstrToRaw(pubIdStr, sizeof(pubIdStr),
                                   ykConfig->pubIdTxt());
            size_t pubIdStrLen = strlen(pubIdStr);

            unsigned char pubId[MAX_SIZE];
            size_t pubIdLen = 0;

            int rc = YubiKeyUtil::hexModhexDecode(pubId, &pubIdLen,
                                                  pubIdStr, pubIdStrLen,
                                                  0, FIXED_SIZE * 2,
                                                  !ykConfig->pubIdInHex());

            if (rc <= 0) {
                qDebug("Invalid public id: %s", pubIdStr);
                throw 0;
            }
            ykp_set_fixed(cfg, pubId, pubIdLen);
        }

        //Private ID...
        if(ykConfig->pvtIdTxt().length() > 0) {
            qDebug() << "Pvt id: " << ykConfig->pvtIdTxt()
                    << "length:" << ykConfig->pvtIdTxt().length();

            char pvtIdStr[MAX_SIZE];
            YubiKeyUtil::qstrToRaw(pvtIdStr, sizeof(pvtIdStr),
                                   ykConfig->pvtIdTxt());
            size_t pvtIdStrLen = strlen(pvtIdStr);

            unsigned char pvtId[MAX_SIZE];
            size_t pvtIdLen = 0;

            int rc = YubiKeyUtil::hexModhexDecode(pvtId, &pvtIdLen,
                                                  pvtIdStr, pvtIdStrLen,
                                                  UID_SIZE * 2,
                                                  UID_SIZE * 2,
                                                  false);

            if (rc <= 0) {
                qDebug("Invalid private id: %s", pvtIdStr);
                throw 0;
            }
            ykp_set_uid(cfg, pvtId, pvtIdLen);
        }

        //Secret Key...
        if(ykConfig->secretKeyTxt().length() > 0) {
            qDebug() << "Secret key: " << ykConfig->secretKeyTxt()
                    << "length:" << ykConfig->secretKeyTxt().length();

            char secretKeyStr[MAX_SIZE];
            YubiKeyUtil::qstrToRaw(secretKeyStr, sizeof(secretKeyStr),
                                   ykConfig->secretKeyTxt());

            int res = 0;

            if(longSecretKey && strlen(secretKeyStr) == 40) {
                res = ykp_HMAC_key_from_hex(cfg, secretKeyStr);
            } else {
                res = ykp_AES_key_from_hex(cfg, secretKeyStr);
            }

            if (res) {
                qDebug() << "Bad secret key: " << secretKeyStr;
                throw 0;
            }
        }


        //Configuration protection...
        //Current Access Code...
        unsigned char accessCode[MAX_SIZE];
        size_t accessCodeLen = 0;

        if(ykConfig->currentAccessCodeTxt().length() > 0) {
            qDebug() << "Current access code: " << ykConfig->currentAccessCodeTxt()
                    << "length:" << ykConfig->currentAccessCodeTxt().length();

            char accessCodeStr[MAX_SIZE];
            YubiKeyUtil::qstrToRaw(accessCodeStr, sizeof(accessCodeStr),
                                   ykConfig->currentAccessCodeTxt());
            size_t accessCodeStrLen = strlen(accessCodeStr);

            int rc = YubiKeyUtil::hexModhexDecode(accessCode, &accessCodeLen,
                                                  accessCodeStr, accessCodeStrLen,
                                                  ACC_CODE_SIZE * 2,
                                                  ACC_CODE_SIZE * 2,
                                                  false);

            if (rc <= 0) {
                qDebug() << "Invalid current access code: " << accessCodeStr;
                throw 0;
            }
        }

        //New Access Code...
        unsigned char newAccessCode[MAX_SIZE];
        size_t newAccessCodeLen = 0;

        if(ykConfig->newAccessCodeTxt().length() > 0) {
            qDebug() << "New access code: " << ykConfig->newAccessCodeTxt()
                    << "length:" << ykConfig->newAccessCodeTxt().length();

            char newAccessCodeStr[MAX_SIZE];
            YubiKeyUtil::qstrToRaw(newAccessCodeStr, sizeof(newAccessCodeStr),
                                   ykConfig->newAccessCodeTxt());
            size_t newAccessCodeStrLen = strlen(newAccessCodeStr);

            int rc = YubiKeyUtil::hexModhexDecode(newAccessCode, &newAccessCodeLen,
                                                  newAccessCodeStr, newAccessCodeStrLen,
                                                  ACC_CODE_SIZE * 2,
                                                  ACC_CODE_SIZE * 2,
                                                  false);

            if (rc <= 0) {
                qDebug() << "Invalid new access code: " << newAccessCodeStr;
                throw 0;
            }
        }

        bool useAccessCode = false;
        if(accessCodeLen > 0) {
            useAccessCode = true;
        }

        if(newAccessCodeLen > 0) {
            //Enable/change protection
            ykp_set_access_code(cfg, newAccessCode, newAccessCodeLen);

        } else if(accessCodeLen > 0) {
            //Keep same protection
            ykp_set_access_code(cfg, accessCode, accessCodeLen);
        }

        //Output parameters...
        QSettings settings;

        TKTFLAG(TAB_FIRST,          settings.value(SG_TAB_FIRST).toBool());
        TKTFLAG(APPEND_TAB1,        settings.value(SG_APPEND_TAB1).toBool());
        TKTFLAG(APPEND_TAB2,        settings.value(SG_APPEND_TAB2).toBool());
        TKTFLAG(APPEND_CR,          settings.value(SG_APPEND_CR).toBool());
        TKTFLAG(APPEND_DELAY1,      settings.value(SG_APPEND_DELAY1).toBool());
        TKTFLAG(APPEND_DELAY2,      settings.value(SG_APPEND_DELAY2).toBool());
        //TKTFLAG(PROTECT_CFG2,     ykConfig->protectCfg2());

        //CFGFLAG(SEND_REF,         ykConfig->sendRef());
        //CFGFLAG(TICKET_FIRST,     ykConfig->ticketFirst());
        CFGFLAG(PACING_10MS,        settings.value(SG_PACING_10MS).toBool());
        CFGFLAG(PACING_20MS,        settings.value(SG_PACING_20MS).toBool());
        //CFGFLAG(ALLOW_HIDTRIG,    ykConfig->allowHidtrig());

        //Serial # visibility...
        if(flagSrNoSupport) {
            EXTFLAG(SERIAL_BTN_VISIBLE, settings.value(SG_SR_BTN_VISIBLE).toBool());
            EXTFLAG(SERIAL_USB_VISIBLE, settings.value(SG_SR_USB_VISIBLE).toBool());
            EXTFLAG(SERIAL_API_VISIBLE, settings.value(SG_SR_API_VISIBLE).toBool());
        }

        //Log configuration...
        qDebug() << "-------------------------";
        qDebug() << "Config data to be written to key configuration...";

        ykp_write_config(cfg, writer, stderr);

        qDebug() << "-------------------------";

        // Write configuration...
        if (!yk_write_command(yk,
                              ykp_core_config(cfg), ykp_command(cfg),
                              useAccessCode ? accessCode : NULL)) {
            qDebug() << "Failure";
            throw 0;
        }
        qDebug() << "Success... config written";

        YubiKeyLogger::logConfig(ykConfig);
        emit configWritten(true, NULL);
    }
    catch(...) {
        error = true;
    }

    if (cfg) {
        ykp_free_config(cfg);
    }

    if(ykst) {
        ykds_free(ykst);
    }

    if (yk && !yk_close_key(yk)) {
        error = true;
    }

    if (!yk_release()) {
        error = true;
    }

    YubiKeyFinder::getInstance()->start();

    if(error) {
        qDebug() << "Config not written";
        QString errMsg = reportError();
        emit configWritten(false, errMsg);
    }

    qDebug() << "-------------------------";
    qDebug() << "Stopping write config";
    qDebug() << "-------------------------";
}
