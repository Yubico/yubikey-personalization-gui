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

#ifndef HELP_H
#define HELP_H

#define HELP_CONFIG_SLOT            "<p>YubiKey 2.x and later comes with two configuration slots. Each configuration slot works independent of each other and can be programmed in any of the following modes:<ol><li>Yubico OTP</li><li>OATH-HOTP</li><li>Static Password</li><li>Challenge-Response (YubiKey 2.2 and onwards)</li></ol></p><p>The first slot is used to generate the output when the YubiKey button is touched between 0.3 to 1.5 seconds and released and the second slot is used if the button is touched between 2 to 5 seconds.</p><p>When the YubiKey 2.X is shipped, it's first configuration slot is factory programmed for Yubico OTP mode (which works with online Yubico OTP validation server) and the second configuration slot is left blank.</p>"

#define HELP_PUBLIC_ID              "<p>Before the OTP string, a fixed optional Public Identity can be used as a prefix which identifies a particular YubiKey.</p><p><b>Important Note:</b> Yubico OTP validation server requires Public Identity to be of 12 characters (6 bytes) in order to correctly extract the Secret Key. If you change the Public Identity to any other length, the Yubico OTP validation server won't be able to extract the Secret Key and the OTP validation will fail.</p><p>For additional information, please refer to YubiKey Personalization Tool user guide.</p>"

#define HELP_PRIVATE_ID             "<p>Private Identity is an optional secret field that is included as an input parameter in the OTP generation algorithm.</p><p>For additional information, please refer to YubiKey Personalization Tool user guide.</p>"

#define HELP_SECRET_KEY             "<p>Secret Key is a mandatory cryptographic key needed to compute the OTP or the Response code for a given Challenge.</p>"

#define HELP_PARAM_GENERATION       "<p>Parameter Generation Scheme defines how the Public Identity, Private Identity and Secret Key parameters are generated when multiple programming YubiKeys are to be programmed based on the initial inputs and then just inserting YubiKeys one by one.</p><p>The following options are available:<ol><li><b>Increment Identities; Randomize Secret:</b> If this option is selected, the Public Identity and Private Identity values are incremented by one and the Secret Key is randomly generated for each new YubiKey to be programmed.</li><li><b>Randomize all parameters:</b> If this option is selected, all 3 parameters i.e. the Public Identity, Private Identity and Secret Key are randomly generated for each new YubiKey to be programmed.</li><li><b>Fixed parameters:</b> if this option is selected, all 3 parameters i.e. the Public Identity, Private Identity and Secret Key remain unchanged for each new YubiKey to be programmed.</li></p>"

#define HELP_CONFIG_PROTECTION      "<p>For security reasons and for avoiding accidental reprogramming, YubiKeys can be protected using configuration protection access code.</p><p>If the configuration protection access code is set, no one can reprogram the YubiKey unless the correct access code is provided during reprogramming.</p><p>The following operations are supported:<ol><li><b>YubiKey(s) unprotected - Keep it that way:</b> Use this option if the YubiKey is currently unprotected and you want to keep it that way</li><li><b>YubiKey(s) unprotected - Enable protection:</b> Use this option if the YubiKey is currently unprotected and you want to enable the protection. You are required to provide the New Access Code if you select this option.</li><li><b>YubiKey(s) protected - Disable protection:</b> Use this option if the YubiKey is currently protected and you want to disable the protection. You are required to provide your Current Access Code if you select this option.</li><li><b>YubiKey(s) protected - Keep it that way:</b> Use this option if the YubiKey is currently protected and you want to keep it that way. You are required to provide your Current Access Code if you select this option.</li><li><b>YubiKey(s) protected - Change access code:</b> Use this option if the YubiKey is currently protected and you want to chagne the access code. You are required to provide your Current Access Code and New Access Code if you select this option.</li></ol></p><p><b>Important Note:</b> It is not possible to retrieve the access code from the YubiKey. Yubico highly recommends users to record the access code for each YubiKey programmed. This can be achieved easily by ensuring that logging is enabled and archiving the relevant log records.</p>"

#define HELP_OUTPUT_FORMAT          "<p>When programmed in OTP mode, this option can be used to insert Tab and Enter characters at various positions in the OTP output string.</p><p>The following options are available:<ol><li>A Tab before Public ID</li><li>A Tab between Public ID and OTP</li><li>A Tab after OTP</li><li>An Enter at the end of the output string</li></p><p>Each \"pressed\" button indicates the corresponding character is enabled at the specified position.</p>"

#define HELP_OUTPUT_SPEED           "<p>When programmed in OTP mode, this option can be used to introduce a small delay of 20, 40 or 60ms between the characters emitted by the YubiKey.</p><p>This may be required under very rare and exceptional circumstances if some characters of the YubiKey output string are missed.</p><p>Also, if there is some parsing - or GUI rendering delay for a particular application, an additional 500 ms delay can be inserted before and after sending the OTP.</p>"

#define HELP_SR_NO_VISIBILITY       "<p>Yubikey 2.2 or later has a non-alterable, factory programmed unique serial number which is different than the public identities configured.</p><p>The following options are available for serial number reading:<ol><li><b>Button at startup:</b> This option allows the serial number to be read at device powerup.</li><li><b>USB descriptor:</b> This option makes the device serial number visible in the serial number field in the USB device descriptor. Please note that the YubiKey must be removed and reinserted after this option is enabled in order for the operating system to recognize the updated device descriptor.</li><li><b>API call:</b> This option allows the device serial number to be read via a client-side application via a software interface, such as the Yubikey Client API.</li></p><p>For additional information, please refer to YubiKey Personalization Tool user guide.</p>"

#define HELP_OATH_PUBLIC_ID         "<p>The YubiKey supports the Class A Token Identifier Specification as outlined by <a href=\"http://www.openauthentication.org/oath-id\">openauthentication.org</a>.</p><p>The general format of the 12 character (6 bytes) OATH Token Identifier is as follows:<br/><b>[MM][TT][UUUUUUUU]</b>,where - <br/><table border=1 cellspacing=1 cellpadding=1 style=\"border-width:1px;border-style:dotted;\"><tr><td width=20%><b>MM</b></td><td width=20%>OATH Manufacturer prefix (OMP)</td><td width=60%>A 2 character prefix, assigned by OATH.<br/>Yubico's OMP is <b>\"ub\"</b></td></tr><tr><td width=20%><b>TT</b></td><td width=20%>Token Type (TT)</td><td width=60%>A 2 character token type, assigned by the manufacturer</td></tr><tr><td width=20%><b>UUUUUUUU</b></td><td width=20%>Manufacturer Unique Identifier (MUI)</td><td width=60%></td>8 characters that uniquely identify the token for a given manufacturer and token type</tr></table></p><p>The Token Identifier can be configured to be all numeric, OMP Modhex, OMP + TT Modhex or all Modhex.</p>"

#define HELP_HOTP_LEN               "<p>According to the OATH-HOTP standard <a href=\"http://www.ietf.org/rfc/rfc4226.txt\">RFC 4226</a>, the YubiKey supports 6 or 8 digit HOTP values.</p>"

#define HELP_HOTP_PARAM             "<p>According to the OATH-HOTP standard <a href=\"http://www.ietf.org/rfc/rfc4226.txt\">RFC 4226</a>, moving factor is a counter value that must be synchronized between the HOTP generator (YubiKey) and the HOTP validator (server).</p>"

#define HELP_STATIC_SCAN_CODE       "<p>The Scan Code mode allows a YubiKey to be programmed to emit the desired static password.</p><p>Type in the characters of your static password in the \"Password\" field using the keyboard and the tool will automatically fill-in the corresponding scan codes of characters in the field.</p><p>The tool maps the scan codes assuming a US standard keyboard layout, so the results might differ depending on the layout you use. In such cases, try setting the keyboard layout to US standard and then typing the password as you normally would on your keyboard - the characters in the password field will not match but the eventual output will.</p>"

#define HELP_CHALRESP_YUBICO        "<p>The Challenge-Response mode allows programmatic interaction between a client-side application and the YubiKey by support of a client-side interface software, such as the YubiKey Client API.</p><p>The challenge-response mode can either be Yubico OTP compatible mode or HMAC-SHA1.</p><p>The Yubico OTP Challenge-Response mode takes a 6 byte challenge and creates a response using Yubico OTP algorithm. In this mode, the YubiKey makes use of some variable fields and hence generates a different response every time even if the challenge is same.</p>"

#define HELP_CHALRESP_HMAC          "<p>The Challenge-Response mode allows programmatic interaction between a client-side application and the YubiKey by support of a client-side interface software, such as the YubiKey Client API.</p><p>The challenge-response mode can either be Yubico OTP compatible mode or HMAC-SHA1.</p><p>The HMAC-SHA1 Challenge-Response mode takes a 0-64 byte challenge and creates a HMAC using HMAC-SHA1 algorithm in combination with a 20 byte secret key. In this mode, the YubiKey does not make use of any variable field and hence generates an identical response if the challenge is same.</p>"

#define HELP_CHALRESP_OPTION        "<p>If this option is selected, when a client-side interface software presents a Challenge to the YubiKey, the light starts flashing and the user has to touch the YubiKey button to acknowledge the challenge-response operation before the computed Response is sent back.</p>"

#define HELP_ALLOW_UPDATE_OPTION       "<p>If this option is selected the YubiKey configuration will be updatable, without rewriting the aes-key. Only non-security related settings are changeable with this.</p>"

#define HELP_SWAP_OPTION            "<p>If both slots are configured to allow updates they can be swapped with this option. That is slot 1 and slot 2 will change places.</p>"

#define HELP_MAN_UPDATE_OPTION      "<p>The manual update setting is to allow the static password in the YubiKey to be changed without reprogramming the key. If it is set it can be triggered by holding the button for 10 seconds, releasing and then tapping it again, the YubiKey will then generate a new static password.</p>"

#define HELP_LOG_FORMAT             "<p>With the flexible logging format the format can be specified manually, this is done by adding a number of variables enclosed in {}, newline and tab can be added with variables endl and tab. Currently supported variables are: "

#endif // HELP_H
