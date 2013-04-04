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

#ifndef OTPDEF_H
#define OTPDEF_H

#define YUBICO_OTP_SERVER_PUBLIC_ID_PREFIX  "vv"
#define YUBICO_OTP_SERVER_PUBLIC_ID_SIZE    2

#define ACCESS_CODE_DEFAULT                 "000000000000"
#define PUBLIC_ID_DEFAULT_SIZE              6

#define OATH_HOTP_PUBLIC_ID_SIZE            6
#define OATH_HOTP_OMP_SIZE                  1
#define OATH_HOTP_TT_SIZE                   1
#define OATH_HOTP_MUI_SIZE                  4

#define MIN_CUSTOMER_PREFIX                 1
#define MAX_CUSTOMER_PREFIX                 99999

#define MAX_YUBICO_SERIAL                   0x00ffffffUL
#define MAX_OATH_SERIAL                     99999UL

#define OATH_HOTP_CUSTOMER_PREFIX_START     190

// openauthentication.org assigned manufacturer prefix
#define YUBICO_OATH_MFG_PREFIX              "e1"
#define YUBICO_OMP_CODE                     0xe1    // Modhex equivalent

#define MAX_SCAN_EDIT_SIZE                  38

#endif // OTPDEF_H
