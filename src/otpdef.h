/*
 * Copyright (c) 2011 Yubico AB
 * See the file COPYING for licence statement.
 *
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

#define YUBICO_OTP_CUSTOMER_PREFIX_CODE     0x28
#define OATH_HOTP_CUSTOMER_PREFIX_START     190

// openauthentication.org assigned manufacturer prefix
#define YUBICO_OATH_MFG_PREFIX              "e1"
#define YUBICO_OMP_CODE                     0xe1    // Modhex equivalent

#define MAX_SCAN_EDIT_SIZE                  38

#endif // OTPDEF_H
