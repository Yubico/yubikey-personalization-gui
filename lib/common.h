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

#ifndef COMMON_H
#define COMMON_H

#include "otpdef.h"

#define YK_VERSION(MAJOR, MINOR, BUILD) (MAJOR * 10000 + MINOR * 100 + BUILD)

//typedef std::vector<int>::size_type size_t;

enum AccMode {
  Acc_None,
  Acc_Serial,
  Acc_Random,
};

#define MAX_SIZE                    256

#define SCHEME_INCR_ID_RAND_SECRET  0
#define SCHEME_RAND_SECRET          0
#define SCHEME_RAND_ALL             1
#define SCHEME_ID_FROM_SERIAL_RAND_SECRET 2
#define SCHEME_FIXED                2

#define GEN_SCHEME_FIXED            0
#define GEN_SCHEME_INCR             1
#define GEN_SCHEME_RAND             2
#define GEN_SCHEME_SERIAL           3

#define WRITE_CONFIG                0
#define EXPORT_CONFIG               1

#define SUPPORT_LINK                "https://yubi.co/forum"
#define UPLOAD_URL                  "<!doctype html><html><head><title>upload</title></head><body><form action='https://upload.yubico.com/' method='post' id='upload'>serial:<input name='serial' value='%1'/><br/>prefix:<input name='prefix' value='%2'/><br/>uid:<input name='uid' value='%3'/><br/>aeskey:<input name='aeskey' value='%4'/><br/><input type='submit'/><br/></form><script type='text/javascript'>window.onload=function(){document.getElementById('upload').submit();}</script></body></html>"

#define ERR_NOERROR                 0
#define ERR_OTHER                   1
#define ERR_NOKEY                   2
#define ERR_MORETHANONE             3
#define ERR_UNKNOWN_FIRMWARE        4

#define KEYMAP_NONE                 0
#define KEYMAP_US                   1
#define KEYMAP_DE                   2
#define KEYMAP_BEPO                 3
#define KEYMAP_FR                   4

//Timeouts

#define TIMEOUT_STATUS              10000
#define TIMEOUT_FINDER              1000


//Stylesheets

#define SS_MENU_CHECKED             "color:rgb(0,0,0);border:2px groove rgb(255,255,255);"
#define SS_MENU_UNCHECKED           "color:rgb(122,171,32);border:2px groove rgb(255,255,255);"
#define SS_MENU_DISABLED            "color:rgb(118,116,108);border:2px groove rgb(255,255,255);"

#define SS_STATUS_SUCCESS           "font:12px \"Verdana\";color:green;"
#define SS_STATUS_ERROR             "font:12px \"Verdana\";color:rgb(221,97,99);"

#define SS_YKSTATUS_SUCCESS         "font:11px \"Verdana\";font-weight:bold;color:rgb(122,171,32);"
#define SS_YKSTATUS_ERROR           "font:11px \"Verdana\";font-weight:bold;color:rgb(221, 97, 99);"

#define SS_COPY_BTN                 "background-image: url(:/res/images/clipboard.png); border-radius: 2px;"
#define SS_BLANK_BTN                "background-image: url(:/res/images/blank.png); border-radius: 2px;"

#define SS_LINK_ENABLED             "font: 12px \"Verdana\"; text-decoration: underline; text-align: left; color: rgb(5, 67, 168);"
#define SS_LINK_DISABLED            "font: 12px \"Verdana\"; text-decoration: underline; text-align: left; color: rgb(118,116,108);"


//Messages

#define ERR                         "Error"
#define NA                          "N/A"
#define TOVERFLOW                   "Overflow"

#define KEY_FOUND                   "YubiKey is inserted"
#define NO_KEY_FOUND                "No YubiKey inserted"
#define MORE_THAN_ONE               "More than one\nYubiKey inserted"
#define OTHER_ERROR                 "Unknown error occurred"
#define UNKNOWN_FIRMWARE            "Unknown firmware"

#define KEY_CONFIGURED              "YubiKey%1 has been successfully configured"
#define KEY_EXPORTED                "YubiKey configuration has been successfully exported"
#define REMOVE_KEY                  "Please remove this YubiKey"
#define SETTINGS_SAVED              "Settings have been successfully saved"
#define SETTINGS_RESTORED           "Default settings have been restored"
#define VALUE_COPIED                "Value copied to clipboard"

#define ERR_KEY_NOT_FOUND           "YubiKey not found. Please insert YubiKey in USB port"
#define ERR_FEATURE_NOT_SUPPORTED   "Selected configuration is not supported by the YubiKey being programmed"
#define ERR_FIRMWARE_NOT_SUPPORTED  "This YubiKey firmware version is not supported"
#define ERR_PROCESSING              "YubiKey could not be configured. Perhaps protected with configuration protection access code?"
#define ERR_PROCESSING_CHALRESP     "Challenge response could not be performed. Perhaps the YubiKey is not configured for challenge-response?"
#define ERR_CONF_SLOT_NOT_SELECTED  "No configuration slot selected. Please select the configuration slot to be programmed"
#define ERR_CHAL_TYPE_NOT_SELECTED  "No Challenge-Response Type selected. Please select the one to use"

//Warnings

#define OVERWRITE_CONF_SLOT1        "Overwrite configuration slot 1?"
#define WARN_OVERWRITE_CONF_SLOT1   "If you have a YubiKey VIP, this slot should contain a Symantec VIP credential by default. You should refrain from rewriting this Slot as a Symantec VIP credential can only be programmed during manufacture - you will thus not be able to get a new one.\n\nIf you have any other YubiKey, this slot should contain a Yubico OTP credential by default. This credential is required for using the YubiKey with many of our services and that of our partners. If you overwrite this and want to use it again later, you will need to reprogram it and upload the new one to our YubiCloud validation service.\n\nAre you sure you want to overwrite the configuration slot 1?"

#define RESTORE_SETTING             "Restore default settings?"
#define WARN_RESTORE_SETTING        "Are you sure you want to restore the default settings?"

#define WARN_EMPTY_PASS             "No password set. Please set the password"

#define DIFF_PUBLIC_ID_LEN          "Change Public Identity length?"
#define WARN_DIFF_PUBLIC_ID_LEN     "Yubico OTP validation server requires Public Identity to be of 12 characters (6 bytes) in order to correctly extract the Secret Key. If you change the Public Identity to any other length, the Yubico OTP validation server won't be able to extract the Secret Key and the OTP validation will fail.\n\nAre you sure you want to change the default Public Idenity length?"

#define ENABLE_CONF_PROTECTION      "Enable configuration protection?"
#define WARN_ENABLE_CONF_PROTECTION "YubiKey Personalization Tool logging is currently disabled. It is not possible to retrieve the configuration protection access code from the YubiKey. If protection access code is forgotten, you may not be able to program the YubiKey again. Yubico highly recommends to enable logging and archive the relevant log records. \n\nAre you sure you still want to enable the configuration protection?"

#define NDEF_WITHOUT_HTTP           "Write NDEF without HTTP?"
#define WARN_NDEF_WITHOUT_HTTP      "The NDEF URI entered does not start with http, are you sure that is what you want?"

//Settings

#define SG_CUSTOMER_PREFIX_USED     "Customer/Used"
#define SG_CUSTOMER_PREFIX          "Customer/Prefix"

#define SG_LOG_DISABLED             "Log/Disabled"
#define SG_LOG_FILENAME             "Log/Filename"
#define SG_LOG_FORMAT               "Log/Format"
#define SG_LOG_FLEXIBLE             "Log/Flexible"

#define SG_EXPORT_FILENAME          "Export/Filename"
#define SG_IMPORT_FILENAME          "Import/Filename"

#define SG_TAB_FIRST                "Flag/TabTirst"
#define SG_APPEND_TAB1              "Flag/AppendTab1"
#define SG_APPEND_TAB2              "Flag/AppendTab2"
#define SG_APPEND_CR                "Flag/AppendCr"
#define SG_APPEND_DELAY1            "Flag/AppendDelay1"
#define SG_APPEND_DELAY2            "Flag/AppendDelay2"

#define SG_SR_BTN_VISIBLE           "Flag/SerialBtnVisible"
#define SG_SR_USB_VISIBLE           "Flag/serialUsbVisible"
#define SG_SR_API_VISIBLE           "Flag/serialApiVisible"

#define SG_PACING_10MS              "Flag/Pacing10ms"
#define SG_PACING_20MS              "Flag/Pacing20ms"

#define SG_MAN_UPDATE               "Flag/ManUpdate"

#define SG_ALLOW_UPDATE             "Flag/AllowUpdate"
#define SG_FAST_TRIG                "Flag/FastTrig"
#define SG_USE_NUMERIC_KEYPAD       "Flag/UseNumericKeypad"
#define SG_LED_INVERT               "Flag/LedInvert"

#define SG_OATH_HOTP8               "Flag/OathHotp8"
#define SG_REQUIRE_INPUT            "Flag/RequireInput"
#define SG_HMAC_LT64                "Flag/HmacLt64"

#define SG_STRONG_PW1               "Flag/StrongPw1"
#define SG_STRONG_PW2               "Flag/StrongPw2"
#define SG_STRONG_PW3               "Flag/StrongPw3"

#define SG_OVERWRITE_CONF_SLOT1     "DontAsk/Slot1Overwriting"
#define SG_DIFF_PUBLIC_ID_LEN       "DontAsk/PublicIdLen"
#define SG_ENABLE_CONF_PROTECTION   "DontAsk/ConfProtection"
#define SG_NDEF_WITHOUT_HTTP        "DontAsk/NdefWithoutHttp"

#define SG_ANIMATIONS_PREFERENCE    "Preference/Animations"
#define SG_EXPORT_PREFERENCE        "Preference/Export"

#endif // COMMON_H
