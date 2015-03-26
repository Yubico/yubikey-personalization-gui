/*
Copyright (C) 2011-2014 Yubico AB.  All rights reserved.

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
#include "yubikeyutil.h"

#include <ykcore.h>
#include <ykdef.h>

#include <QFileDialog>
#include <QDebug>
#include <QSettings>

#define EXPORT_FILENAME_DEF "export.ycfg"

YubiKeyWriter* YubiKeyWriter::_instance = 0;
QString YubiKeyWriter::m_filename = defaultExportFilename();

#define TKTFLAG(f, s) \
if(s) { if (!ykp_set_tktflag_##f(cfg, s)) { return 0; } }


#define CFGFLAG(f, s) \
if(s) { if (!ykp_set_cfgflag_##f(cfg, s)) { return 0; } }

#define EXTFLAG(f, s) \
if(s) { if (!ykp_set_extflag_##f(cfg, s)) { return 0; } }


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

QString YubiKeyWriter::reportError(bool chalresp = false) {
    QString errMsg;

    if (ykp_errno) {
        qDebug("Yubikey personalization error: %s\n", ykp_strerror(ykp_errno));
        emit diagnostics(ykp_strerror(ykp_errno));

        switch(ykp_errno) {
        case YKP_EYUBIKEYVER:
        case YKP_EOLDYUBIKEY:
            errMsg = tr(ERR_FEATURE_NOT_SUPPORTED);
            break;
        default:
            errMsg = tr(ERR_PROCESSING);
            break;
        }
        ykp_errno = 0;
    } else if (yk_errno) {
        if (yk_errno == YK_EUSBERR) {
            qDebug("USB error: %s\n", yk_usb_strerror());
            emit diagnostics(QString("USB error: %1").arg(yk_usb_strerror()));
        } else {
            qDebug("Yubikey core error: %s\n", yk_strerror(yk_errno));
            emit diagnostics(yk_strerror(yk_errno));
        }

        switch(yk_errno) {
        case YK_ENOKEY:
            errMsg = tr(ERR_KEY_NOT_FOUND);
            break;
        case YK_EFIRMWARE:
            errMsg = tr(ERR_FIRMWARE_NOT_SUPPORTED);
            break;
        default:
            if(chalresp) {
                errMsg = tr(ERR_PROCESSING_CHALRESP);
            } else {
                errMsg = tr(ERR_PROCESSING);
            }
            break;
        }
        yk_errno = 0;
    }


    if(!errMsg.isEmpty()) {
        emit errorOccurred(errMsg);
    }

    return errMsg;
}

int YubiKeyWriter::assembleConfig(YubiKeyConfig *ykConfig, YKP_CONFIG *cfg, bool *useAccessCode, unsigned char *accessCode) {
    // Check features support
    bool flagSrNoSupport = false;
    bool flagUpdateSupport = false;
    bool flagImfSupport = false;
    bool flagLedInvSupport = false;

    YubiKeyFinder *finder = YubiKeyFinder::getInstance();
    if(finder->checkFeatureSupport(
            YubiKeyFinder::Feature_SerialNumber)) {
        flagSrNoSupport = true;
    }
    if(finder->checkFeatureSupport(
          YubiKeyFinder::Feature_Updatable)) {
        flagUpdateSupport = true;
    }
    if(finder->checkFeatureSupport(
          YubiKeyFinder::Feature_MovingFactor)) {
        flagImfSupport = true;
    }
    if(finder->checkFeatureSupport(
          YubiKeyFinder::Feature_LedInvert)) {
        flagLedInvSupport = true;
    }

    //Programming Mode...
    bool longSecretKey = false;

    int command = ykConfig->configSlot() == 2 ? SLOT_CONFIG2 : SLOT_CONFIG;

    switch(ykConfig->programmingMode()) {
        case YubiKeyConfig::Mode_Update:
            // if we're doing an update it's other commands.
            command = ykConfig->configSlot() == 2 ? SLOT_UPDATE2 : SLOT_UPDATE1;
            break;

        case YubiKeyConfig::Mode_Swap:
            // swap has it's own command
            command = SLOT_SWAP;
            break;

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
            if(flagImfSupport && !ykp_set_oath_imf(cfg, ykConfig->oathMovingFactorSeed())) {
                return 0;
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

    //Configuration slot...
    if (!ykp_configure_command(cfg, command)) {
        return 0;
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
            return 0;
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
            return 0;
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
            return 0;
        }
    }


    //Configuration protection...
    //Current Access Code...
    size_t accessCodeLen = 0;

    if(ykConfig->currentAccessCodeTxt().length() > 0) {
        int rc = encodeAccessCode(ykConfig->currentAccessCodeTxt(), accessCode, &accessCodeLen);
        if (rc <= 0) {
            qDebug() << "Invalid current access code: " << ykConfig->currentAccessCodeTxt();
            return 0;
        }
    }

    //New Access Code...
    unsigned char newAccessCode[MAX_SIZE];
    size_t newAccessCodeLen = 0;

    if(ykConfig->newAccessCodeTxt().length() > 0) {
        int rc = encodeAccessCode(ykConfig->newAccessCodeTxt(), newAccessCode, &newAccessCodeLen);
        if (rc <= 0) {
            qDebug() << "Invalid new access code: " << ykConfig->newAccessCodeTxt();
            return 0;
        }
        if(ykConfig->accMode() > 0) {
            int accMode = 0;
            if(ykConfig->accMode() == Acc_None) {
                accMode = YKP_ACCCODE_NONE;
            } else if(ykConfig->accMode() == Acc_Serial) {
                accMode = YKP_ACCCODE_SERIAL;
            } else if(ykConfig->accMode() == Acc_Random) {
                accMode = YKP_ACCCODE_RANDOM;
            }
            ykp_set_acccode_type(cfg, accMode);
        }
    }

    if(accessCodeLen > 0) {
        *useAccessCode = true;
    }

    if(newAccessCodeLen > 0) {
        //Enable/change protection
        ykp_set_access_code(cfg, newAccessCode, newAccessCodeLen);

    } else if(accessCodeLen > 0) {
        //Keep same protection
        ykp_set_access_code(cfg, accessCode, accessCodeLen);
    }

    TKTFLAG(TAB_FIRST,          ykConfig->tabFirst());
    TKTFLAG(APPEND_TAB1,        ykConfig->appendTab1());
    TKTFLAG(APPEND_TAB2,        ykConfig->appendTab2());
    TKTFLAG(APPEND_CR,          ykConfig->appendCr());
    TKTFLAG(APPEND_DELAY1,      ykConfig->appendDelay1());
    TKTFLAG(APPEND_DELAY2,      ykConfig->appendDelay2());
    //TKTFLAG(PROTECT_CFG2,     ykConfig->protectCfg2());

    //CFGFLAG(SEND_REF,         ykConfig->sendRef());
    //CFGFLAG(TICKET_FIRST,     ykConfig->ticketFirst());
    CFGFLAG(PACING_10MS,        ykConfig->pacing10ms());
    CFGFLAG(PACING_20MS,        ykConfig->pacing20ms());
    //CFGFLAG(ALLOW_HIDTRIG,    ykConfig->allowHidtrig());

    //Serial # visibility...
    if(flagSrNoSupport) {
        EXTFLAG(SERIAL_BTN_VISIBLE, ykConfig->serialBtnVisible());
        EXTFLAG(SERIAL_USB_VISIBLE, ykConfig->serialUsbVisible());
        EXTFLAG(SERIAL_API_VISIBLE, ykConfig->serialApiVisible());
    }

    if(flagUpdateSupport) {
        EXTFLAG(ALLOW_UPDATE, ykConfig->allowUpdate());
        // XXX: let update support mean these as well..
        EXTFLAG(DORMANT, ykConfig->dormant());
        EXTFLAG(FAST_TRIG, ykConfig->fastTrig());
        EXTFLAG(USE_NUMERIC_KEYPAD, ykConfig->useNumericKeypad());
    }

    if(flagLedInvSupport) {
        EXTFLAG(LED_INV, ykConfig->ledInvert());
    }
    return 1;
}

void YubiKeyWriter::writeConfig(YubiKeyConfig *ykConfig) {

    YubiKeyFinder::getInstance()->stop();

    YK_KEY *yk = 0;
    YK_STATUS *ykst = YubiKeyFinder::getInstance()->status();
    YKP_CONFIG *cfg = ykp_alloc();

    bool error = false;

    qDebug() << "-------------------------";
    qDebug() << "Starting write config";
    qDebug() << "-------------------------";

    try {
        if (!(yk = yk_open_first_key())) {
            throw 0;
        } else if (!yk_get_status(yk, ykst)) {
            throw 0;
        }

        ykp_configure_version(cfg, ykst);

        qDebug() << "writer:configuration slot:" << ykConfig->configSlot();

        bool useAccessCode;
        unsigned char accessCode[MAX_SIZE];

        if(!assembleConfig(ykConfig, cfg, &useAccessCode, accessCode)) {
            throw 0;
        }

        //Log configuration...
        qDebug() << "-------------------------";
        qDebug() << "Config data to be written to key configuration...";

        char conf_buf[1024];
        ykp_export_config(cfg, conf_buf, 1024, YKP_FORMAT_LEGACY);
        qDebug() << conf_buf;

        qDebug() << "-------------------------";

        // Write configuration...
        if (!yk_write_command(yk,
                              ykp_core_config(cfg), ykp_command(cfg),
                              useAccessCode ? accessCode : NULL)) {
            qDebug() << "Failure";
            throw 0;
        }
        qDebug() << "Success... config written";
        emit diagnostics(QString("Successfully wrote config to slot %1").arg(ykp_config_num(cfg)));

        YubiKeyLogger::logConfig(ykConfig);
        emit configWritten(true, NULL);
    }
    catch(...) {
        error = true;
    }

    if (cfg) {
        ykp_free_config(cfg);
    }

    if (yk && !yk_close_key(yk)) {
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

void YubiKeyWriter::exportConfig(YubiKeyConfig *ykConfig) {
    YubiKeyFinder::getInstance()->stop();

    YK_KEY *yk = 0;
    YK_STATUS *ykst = YubiKeyFinder::getInstance()->status();
    YKP_CONFIG *cfg = ykp_alloc();

    bool error = false;

    qDebug() << "-------------------------";
    qDebug() << "Starting write config";
    qDebug() << "-------------------------";

    try {
        if (!(yk = yk_open_first_key())) {
            throw 0;
        }

        ykp_configure_version(cfg, ykst);

        qDebug() << "writer:configuration slot:" << ykConfig->configSlot();

        bool useAccessCode;
        unsigned char accessCode[MAX_SIZE];

        if(!assembleConfig(ykConfig, cfg, &useAccessCode, accessCode)) {
            throw 0;
        }

        m_filename = QFileDialog::getSaveFileName(NULL, tr("Export File"), m_filename, tr("Yubico cfg format (*.ycfg)"), NULL);

        char data[1024];
        int len = ykp_export_config(cfg, data, 1024, YKP_FORMAT_YCFG);
        if(!len) {
            throw 0;
        }

        QFile file(m_filename);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            throw 0;
        }
        if(!file.write(data, len)) {
            throw 0;
        }
        file.close();

        QSettings settings;
        settings.setValue(SG_EXPORT_FILENAME, m_filename);

        emit configWritten(true, NULL);
        emit diagnostics(QString("Exported config to file %1").arg(m_filename));
    }
    catch(...) {
        error = true;
    }

    if (cfg) {
        ykp_free_config(cfg);
    }

    if (yk && !yk_close_key(yk)) {
        error = true;
    }

    YubiKeyFinder::getInstance()->start();

    if(error) {
        qDebug() << "Config not exported";
        QString errMsg = reportError();
        emit configWritten(false, errMsg);
    }
}

void YubiKeyWriter::doChallengeResponse(const QString challenge, QString  &response, int slot, bool hmac) {
    YubiKeyFinder::getInstance()->stop();

    YK_KEY *yk = 0;

    bool error = false;

    qDebug() << "Challenge to slot " << slot << " with challenge: " << challenge;

    try {
        int yk_cmd;
        QByteArray chal_array = challenge.toLatin1();
        const unsigned char *chal = reinterpret_cast<const unsigned char*>(chal_array.constData());
        unsigned char resp[64];
        memset(resp, 0, sizeof(resp));
        if (!(yk = yk_open_first_key())) {
            throw 0;
        }

        switch(slot) {
          case 1:
            yk_cmd = (hmac == true) ? SLOT_CHAL_HMAC1 : SLOT_CHAL_OTP1;
            break;
          case 2:
            yk_cmd = (hmac == true) ? SLOT_CHAL_HMAC2 : SLOT_CHAL_OTP2;
            break;
          default:
            throw 0;
            break;
        }

        if(! yk_challenge_response(yk, yk_cmd, 1, challenge.length(),
              chal, sizeof(resp), resp)) {
            throw 0;
        }
        // 20 (160 bits) for hmac-sha1 and 16 (128 bits) for yubico mode
        if(hmac == true) {
            response = YubiKeyUtil::qstrHexEncode(resp, 20);
        } else {
            response = YubiKeyUtil::qstrModhexEncode(resp, 16);
        }
        emit diagnostics(QString("Successful challenge response with slot %1").arg(slot));
    } catch(...) {
        error = true;
    }

    if (yk && !yk_close_key(yk)) {
        error = true;
    }

    YubiKeyFinder::getInstance()->start();

    if(error) {
        qDebug() << "Challenge response failed.";
        QString errMsg = reportError(true);
    }
}

void YubiKeyWriter::writeNdef(bool uri, const QString language,
    const QString payload, const QString accCode, int slot) {
    YubiKeyFinder::getInstance()->stop();

    YK_KEY *yk = 0;
    YK_NDEF *ndef = ykp_alloc_ndef();

    bool error = false;

    qDebug() << "Writing ndef " << payload << " of type " << uri;

    try {

        if(accCode.length() > 0) {
            unsigned char accessCode[MAX_SIZE];
            size_t accessCodeLen = 0;
            int rc = encodeAccessCode(accCode, accessCode, &accessCodeLen);
            if (rc <= 0) {
                qDebug() << "Invalid access code: " << accCode;
                throw 0;
            }

            ykp_set_ndef_access_code(ndef, accessCode);
        }

        QByteArray payload_array = payload.toUtf8();
        const char *ndef_payload = payload_array.constData();
        qDebug() << "payload: " << ndef_payload;
        if (!(yk = yk_open_first_key())) {
            throw 0;
        }

        if(uri) {
            if(!ykp_construct_ndef_uri(ndef, ndef_payload)) {
                throw 0;
            }
        } else {
            QByteArray lang_array = language.toUtf8();
            const char *lang = lang_array.constData();
            if(!ykp_construct_ndef_text(ndef, ndef_payload, lang, false)) {
                throw 0;
            }
        }

        qDebug() << "writing the ndef.";
        if(! yk_write_ndef2(yk, ndef, slot)) {
            throw 0;
        }
        emit configWritten(true, NULL);
        emit diagnostics(QString("Wrote NDEF for slot %1").arg(slot));
    } catch(...) {
        error = true;
    }

    if(ndef && !ykp_free_ndef(ndef)) {
        error = true;
    }

    if (yk && !yk_close_key(yk)) {
        error = true;
    }

    YubiKeyFinder::getInstance()->start();


    if(error) {
        qDebug() << "Writing NDEF failed.";
        QString errMsg = reportError();
        emit configWritten(false, errMsg);
    }
}

void YubiKeyWriter::deleteConfig(int slot, const QString accCode) {
    bool error = false;
    YK_KEY *yk = NULL;
    YKP_CONFIG *cfg = ykp_alloc();

    YubiKeyFinder::getInstance()->stop();

    try {
        if (!(yk = yk_open_first_key())) {
            throw 0;
        }

        unsigned char accessCode[MAX_SIZE];
        size_t accessCodeLen = 0;

        if(accCode.length() > 0) {
            int rc = encodeAccessCode(accCode, accessCode, &accessCodeLen);
            if (rc <= 0) {
                qDebug() << "Invalid access code: " << accCode;
                throw 0;
            }
        }

        // write NULL to delete config
        if (!yk_write_config(yk,
                    NULL, slot,
                    accessCodeLen > 0 ? accessCode : NULL)) {
            qDebug() << "Failed to delete.";
            throw 0;
        }
        emit configWritten(true, NULL);
        qDebug() << "successfully deleted slot " << slot;
        emit diagnostics(QString("Deleted slot %1").arg(slot));
    } catch(...) {
        error = true;
    }

    if (cfg) {
        ykp_free_config(cfg);
    }

    if (yk && !yk_close_key(yk)) {
        error = true;
    }

    YubiKeyFinder::getInstance()->start();

    if(error) {
        qDebug() << "Failed to delete configuration in slot" << slot;
        QString errMsg = reportError();
        emit configWritten(false, errMsg);
    }
}

int YubiKeyWriter::encodeAccessCode(QString accCode, unsigned char* accessCode, size_t *accessCodeLen) {
    qDebug() << "access code: " << accCode
        << "length:" << accCode.length();

    char accessCodeStr[MAX_SIZE];
    YubiKeyUtil::qstrToRaw(accessCodeStr, sizeof(accessCodeStr),
            accCode);
    size_t accessCodeStrLen = strlen(accessCodeStr);

    int rc = YubiKeyUtil::hexModhexDecode(accessCode, accessCodeLen,
            accessCodeStr, accessCodeStrLen,
            ACC_CODE_SIZE * 2,
            ACC_CODE_SIZE * 2,
            false);
    return rc;
}

void YubiKeyWriter::setExportFilename(QString filename) {
    m_filename = filename;
}

QString YubiKeyWriter::defaultExportFilename() {
    return QDir::homePath() + "/" + EXPORT_FILENAME_DEF;
}
