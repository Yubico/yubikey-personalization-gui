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

#include <QtGui>

#include "otpdef.h"
#include "yubikeylogger.h"
#include "yubikeyutil.h"
#include "yubikeyfinder.h"
#include "yubikeywriter.h"

typedef std::vector<int>::size_type size_t;

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

#define CONFIG_PROTECTION_DISABLED  0
#define CONFIG_PROTECTION_ENABLE    1
#define CONFIG_PROTECTION_DISABLE   2
#define CONFIG_PROTECTION_ENABLED   3
#define CONFIG_PROTECTION_CHANGE    4

#define OUT_CHAR_RATE_STD           0
#define OUT_CHAR_RATE_SLOWDOWN_20   1
#define OUT_CHAR_RATE_SLOWDOWN_40   2
#define OUT_CHAR_RATE_SLOWDOWN_60   3

#define WRITE_CONFIG                0
#define EXPORT_CONFIG               1

#define SUPPORT_EMAIL               tr("mailto:support@yubico.com")
#define UPLOAD_URL                  tr("<!doctype html><html><title>upload</title></head><body><form action='https://upload.yubico.com/' method='post' id='upload'>serial:<input name='serial' value='%1'/><br/>prefix:<input name='prefix' value='%2'/><br/>uid:<input name='uid' value='%3'/><br/>aeskey:<input name='aeskey' value='%4'/><br/><input type='submit'/><br/></form><script type='text/javascript'>window.onload=function(){document.getElementById('upload').submit();}</script></body></html>")


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

#define SUCCESS                     tr("Success")
#define ERR                         tr("Error")
#define NA                          tr("N/A")
#define TOVERFLOW                   tr("Overflow")

#define KEY_FOUND                   tr("YubiKey is inserted")
#define NO_KEY_FOUND                tr("No YubiKey inserted")

#define KEY_CONFIGURED              tr("YubiKey%1 has been successfully configured")
#define KEY_EXPORTED                tr("YubiKey configuration has been successfully exported")
#define REMOVE_KEY                  tr("Please remove this YubiKey")
#define SETTINGS_SAVED              tr("Settings have been successfully saved")
#define SETTINGS_RESTORED           tr("Default settings have been restored")
#define VALUE_COPIED                tr("Value copied to clipboard")

#define ERR_KEY_NOT_FOUND           tr("YubiKey not found. Please insert YubiKey in USB port")
#define ERR_FEATURE_NOT_SUPPORTED   tr("Selected configuration is not supported by the YubiKey being programmed")
#define ERR_FIRMWARE_NOT_SUPPORTED  tr("This YubiKey firmware version is not supported")
#define ERR_PROCESSING              tr("YubiKey could not be configured. Perhaps protected with configuration protection access code?")
#define ERR_PROCESSING_CHALRESP     tr("Challenge response could not be performed. Perhaps the YubiKey is not configured for challenge-response?")
#define ERR_CONF_SLOT_NOT_SELECTED  tr("No configuration slot selected. Please select the configuration slot to be programmed")
#define ERR_CHAL_TYPE_NOT_SELECTED  tr("No Challenge-Response Type selected. Please select the one to use")

//Warnings

#define OVERWRITE_CONF_SLOT1        tr("Overwrite configuration slot 1?")
#define WARN_OVERWRITE_CONF_SLOT1   tr("By re-programming Slot 1 of your YubiKey, you will lose ALL abilities to authenticate against Yubico online severs, forum, Wiki, etc. (unless you upload your new configuration to Yubico server)\n\nAre you sure you want to overwrite the configuration slot 1?")

#define RESTORE_SETTING             tr("Restore default settings?")
#define WARN_RESTORE_SETTING        tr("Are you sure you want to restore the default settings?")

#define WARN_EMPTY_PASS             tr("No password set. Please set the password")

#define DIFF_PUBLIC_ID_LEN          tr("Change Public Identity length?")
#define WARN_DIFF_PUBLIC_ID_LEN     tr("Yubico OTP validation server requires Public Identity to be of 12 characters (6 bytes) in order to correctly extract the Secret Key. If you change the Public Identity to any other length, the Yubico OTP validation server won't be able to extract the Secret Key and the OTP validation will fail.\n\nAre you sure you want to change the default Public Idenity length?")

#define ENABLE_CONF_PROTECTION      tr("Enable configuration protection?")
#define WARN_ENABLE_CONF_PROTECTION tr("YubiKey Personalization Tool logging is currently disabled. It is not possible to retrieve the configuration protection access code from the YubiKey. If protection access code is forgotten, you may not be able to program the YubiKey again. Yubico highly recommends to enable logging and archive the relevant log records. \n\nAre you sure you still want to enable the configuration protection?")

#define NDEF_WITHOUT_HTTP           tr("Write NDEF without HTTP?")
#define WARN_NDEF_WITHOUT_HTTP      tr("The NDEF URI entered does not start with http, are you sure that is what you want?")

//Settings

#define SG_CUSTOMER_PREFIX_USED     "Customer/Used"
#define SG_CUSTOMER_PREFIX          "Customer/Prefix"

#define SG_LOG_DISABLED             "Log/Disabled"
#define SG_LOG_FILENAME             "Log/Filename"
#define SG_LOG_FORMAT               "Log/Format"

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

#define SG_OVERWRITE_CONF_SLOT1     "DontAsk/Slot1Overwriting"
#define SG_DIFF_PUBLIC_ID_LEN       "DontAsk/PublicIdLen"
#define SG_ENABLE_CONF_PROTECTION   "DontAsk/ConfProtection"
#define SG_NDEF_WITHOUT_HTTP        "DontAsk/NdefWithoutHttp"

#define SG_ANIMATIONS_PREFERENCE    "Preference/Animations"
#define SG_EXPORT_PREFERENCE        "Preference/Export"

//Help messages

#define HELP_CONFIG_SLOT            tr("<p>YubiKey 2.x and later comes with two configuration slots. Each configuration slot works independent of each other and can be programmed in any of the following modes:<ol><li>Yubico OTP</li><li>OATH-HOTP</li><li>Static Password</li><li>Challenge-Response (YubiKey 2.2 and onwards)</li></ol></p><p>The first slot is used to generate the output when the YubiKey button is touched between 0.3 to 1.5 seconds and released and the second slot is used if the button is touched between 2 to 5 seconds.</p><p>When the YubiKey 2.X is shipped, it's first configuration slot is factory programmed for Yubico OTP mode (which works with online Yubico OTP validation server) and the second configuration slot is left blank.</p>");

#define HELP_PUBLIC_ID              tr("<p>Before the OTP string, a fixed optional Public Identity can be used as a prefix which identifies a particular YubiKey.</p><p><b>Important Note:</b> Yubico OTP validation server requires Public Identity to be of 12 characters (6 bytes) in order to correctly extract the Secret Key. If you change the Public Identity to any other length, the Yubico OTP validation server won't be able to extract the Secret Key and the OTP validation will fail.</p><p>For additional information, please refer to YubiKey Personalization Tool user guide</p>")

#define HELP_PRIVATE_ID             tr("<p>Private Identity is an optional secret field that is included as an input parameter in the OTP generation algorithm.</p><p>For additional information, please refer to YubiKey Personalization Tool user guide</p>")

#define HELP_SECRET_KEY             tr("<p>Secret Key is a mandatory cryptographic key needed to compute the OTP or the Response code for a given Challenge.</p>")

#define HELP_PARAM_GENERATION       tr("<p>Parameter Generation Scheme defines how the Public Identity, Private Identity and Secret Key parameters are generated when multiple programming YubiKeys are to be programmed based on the initial inputs and then just inserting YubiKeys one by one.</p><p>The following options are available:<ol><li><b>Increment Identities; Randomize Secret:</b> If this option is selected, the Public Identity and Private Identity values are incremented by one and the Secret Key is randomly generated for each new YubiKey to be programmed.</li><li><b>Randomize all parameters:</b> If this option is selected, all 3 parameters i.e. the Public Identity, Private Identity and Secret Key are randomly generated for each new YubiKey to be programmed.</li><li><b>Fixed parameters:</b> if this option is selected, all 3 parameters i.e. the Public Identity, Private Identity and Secret Key remain unchanged for each new YubiKey to be programmed.</li></p>")

#define HELP_CONFIG_PROTECTION      tr("<p>For security reasons and for avoiding accidental reprogramming, YubiKeys can be protected using configuration protection access code.</p><p>If the configuration protection access code is set, no one can reprogram the YubiKey unless the correct access code is provided during reprogramming.</p><p>The following operations are supported:<ol><li><b>YubiKey(s) unprotected - Keep it that way:</b> Use this option if the YubiKey is currently unprotected and you want to keep it that way</li><li><b>YubiKey(s) unprotected - Enable protection:</b> Use this option if the YubiKey is currently unprotected and you want to enable the protection. You are required to provide the New Access Code if you select this option.</li><li><b>YubiKey(s) protected - Disable protection:</b> Use this option if the YubiKey is currently protected and you want to disable the protection. You are required to provide your Current Access Code if you select this option.</li><li><b>YubiKey(s) protected - Keep it that way:</b> Use this option if the YubiKey is currently protected and you want to keep it that way. You are required to provide your Current Access Code if you select this option.</li><li><b>YubiKey(s) protected - Change access code:</b> Use this option if the YubiKey is currently protected and you want to chagne the access code. You are required to provide your Current Access Code and New Access Code if you select this option.</li></ol></p><p><b>Important Note:</b> It is not possible to retrieve the access code from the YubiKey. Yubico highly recommends users to record the access code for each YubiKey programmed. This can be achieved easily by ensuring that logging is enabled and archiving the relevant log records.</p>")

#define HELP_OUTPUT_FORMAT          tr("<p>When programmed in OTP mode, this option can be used to insert Tab and Enter characters at various positions in the OTP output string.</p><p>The following options are available:<ol><li>A Tab before Public ID</li><li>A Tab between Public ID and OTP</li><li>A Tab after OTP</li><li>An Enter at the end of the output string</li></p><p>Each \"pressed\" button indicates the corresponding character is enabled at the specified position.</p>")

#define HELP_OUTPUT_SPEED           tr("<p>When programmed in OTP mode, this option can be used to introduce a small delay of 20, 40 or 60ms between the characters emitted by the YubiKey.</p><p>This may be required under very rare and exceptional circumstances if some characters of the YubiKey output string are missed.</p><p>Also, if there is some parsing - or GUI rendering delay for a particular application, an additional 500 ms delay can be inserted before and after sending the OTP.</p>")

#define HELP_SR_NO_VISIBILITY       tr("<p>Yubikey 2.2 or later has a non-alterable, factory programmed unique serial number which is different than the public identities configured.</p><p>The following options are available for serial number reading:<ol><li><b>Button at startup:</b> This option allows the serial number to be read at device powerup.</li><li><b>USB descriptor:</b> This option makes the device serial number visible in the serial number field in the USB device descriptor. Please note that the YubiKey must be removed and reinserted after this option is enabled in order for the operating system to recognize the updated device descriptor.</li><li><b>API call:</b> This option allows the device serial number to be read via a client-side application via a software interface, such as the Yubikey Client API.</li></p><p>For additional information, please refer to YubiKey Personalization Tool user guide</p>")

#define HELP_OATH_PUBLIC_ID         tr("<p>The YubiKey supports the Class A Token Identifier Specification as outlined by <a href=\"http://www.openauthentication.org/oath-id\">openauthentication.org</a>.</p><p>The general format of the 12 character (6 bytes) OATH Token Identifier is as follows:<br/><b>[MM][TT][UUUUUUUU]</b>,where - <br/><table border=1 cellspacing=1 cellpadding=1 style=\"border-width:1px;border-style:dotted;\"><tr><td width=20%><b>MM</b></td><td width=20%>OATH Manufacturer prefix (OMP)</td><td width=60%>A 2 character prefix, assigned by OATH.<br/>Yubico's OMP is <b>\"ub\"</b></td></tr><tr><td width=20%><b>TT</b></td><td width=20%>Token Type (TT)</td><td width=60%>A 2 character token type, assigned by the manufacturer</td></tr><tr><td width=20%><b>UUUUUUUU</b></td><td width=20%>Manufacturer Unique Identifier (MUI)</td><td width=60%></td>8 characters that uniquely identify the token for a given manufacturer and token type</tr></table></p><p>The Token Identifier can be configured to be all numeric, OMP Modhex, OMP + TT Modhex or all Modhex.</p>")

#define HELP_HOTP_LEN               tr("<p>According to the OATH-HOTP standard <a href=\"http://www.ietf.org/rfc/rfc4226.txt\">RFC 4226</a>, the YubiKey supports 6 or 8 digit HOTP values.</p>")

#define HELP_HOTP_PARAM             tr("<p>According to the OATH-HOTP standard <a href=\"http://www.ietf.org/rfc/rfc4226.txt\">RFC 4226</a>, moving factor is a counter value that must be synchronized between the HOTP generator (YubiKey) and the HOTP validator (server).</p>")

#define HELP_STATIC_SCAN_CODE       tr("<p>The Scan Code mode allows a YubiKey to be programmed to emit the desired static password.</p><p>Type in the characters of your static password in the \"Password\" field using the keyboard and the tool will automatically fill-in the corresponding scan codes of characters in the field.</p>")

#define HELP_CHALRESP_YUBICO        tr("<p>The Challenge-Response mode allows programmatic interaction between a client-side application and the YubiKey by support of a client-side interface software, such as the YubiKey Client API.</p><p>The challenge-response mode can either be Yubico OTP compatible mode or HMAC-SHA1.</p><p>The Yubico OTP Challenge-Response mode takes a 6 byte challenge and creates a response using Yubico OTP algorithm. In this mode, the YubiKey makes use of some variable fields and hence generates a different response every time even if the challenge is same.</p>")

#define HELP_CHALRESP_HMAC          tr("<p>The Challenge-Response mode allows programmatic interaction between a client-side application and the YubiKey by support of a client-side interface software, such as the YubiKey Client API.</p><p>The challenge-response mode can either be Yubico OTP compatible mode or HMAC-SHA1.</p><p>The HMAC-SHA1 Challenge-Response mode takes a 0-64 byte challenge and creates a HMAC using HMAC-SHA1 algorithm in combination with a 20 byte secret key. In this mode, the YubiKey does not make use of any variable field and hence generates an identical response if the challenge is same.</p>")

#define HELP_CHALRESP_OPTION        tr("<p>If this option is selected, when a client-side interface software presents a Challenge to the YubiKey, the light starts flashing and the user has to touch the YubiKey button to acknowledge the challenge-response operation before the computed Response is sent back.</p>")

#define HELP_ALLOW_UPDATE_OPTION       tr("<p>If this option is selected the YubiKey configuration will be updatable, without rewriting the aes-key. Only non-security related settings are changeable with this.</p>")

#define HELP_SWAP_OPTION            tr("<p>If both slots are configured to allow updates they can be swapped with this option. That is slot 1 and slot 2 will change places.</p>")

#define HELP_MAN_UPDATE_OPTION      tr("<p>The manual update setting is to allow the static password in the YubiKey to be changed without reprogramming the key. If it is set it can be triggered by holding the button for 10 seconds, releasing and then tapping it again, the YubiKey will then generate a new static password.</p>")
#endif // COMMON_H
