/* yubikey-test.c --- Self-tests for authentication token functions.
 *
 * Written by Simon Josefsson <simon@josefsson.org>.
 * Copyright (c) 2006, 2007, 2008, 2009, 2010 Yubico AB
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <yubikey.h>
#include <stdio.h>

int
main (void)
{
  char buf[1024];
  size_t i;
  int rc;
  yubikey_token_st tok;


  /* Test Modhex */
  yubikey_modhex_encode (buf, "test", 4);
  printf ("modhex-encode(\"test\") = %s\n", buf);
  if (strcmp (buf, "ifhgieif") != 0)
    {
      printf ("ModHex failure\n");
      return 1;
    }
  printf ("Modhex-1 success\n");

  printf ("modhex-decode(\"%s\") = ", buf);
  yubikey_modhex_decode (buf, buf, strlen ((char *) buf));
  printf ("%.*s\n", 4, buf);
  if (memcmp (buf, "test", 4) != 0)
    {
      printf ("ModHex failure\n");
      return 1;
    }
  printf ("Modhex-2 success\n");

  strcpy (buf, "cbdefghijklnrtuv");
  rc = yubikey_modhex_p (buf);
  printf ("hex-p(\"%s\") = %d\n", buf, rc);
  if (!rc)
    {
      printf ("Hex_p failure\n");
      return 1;
    }
  printf ("Hex-3 success\n");

  strcpy (buf, "0123Xabc");
  rc = yubikey_hex_p (buf);
  printf ("hex-p(\"%s\") = %d\n", buf, rc);
  if (rc)
    {
      printf ("Hex_p failure\n");
      return 1;
    }
  printf ("Hex-3 success\n");

  /* Test Hex */

  yubikey_hex_encode (buf, "test", 4);
  printf ("hex-encode(\"test\") = %s\n", buf);
  if (strcmp (buf, "74657374") != 0)
    {
      printf ("Hex failure\n");
      return 1;
    }
  printf ("Hex-1 success\n");

  printf ("hex-decode(\"%s\") = ", buf);
  yubikey_hex_decode (buf, buf, strlen ((char *) buf));
  printf ("%.*s\n", 4, buf);
  if (memcmp (buf, "test", 4) != 0)
    {
      printf ("Hex failure\n");
      return 1;
    }
  printf ("Hex-2 success\n");

  strcpy (buf, "0123456789abcdef");
  rc = yubikey_hex_p (buf);
  printf ("hex-p(\"%s\") = %d\n", buf, rc);
  if (!rc)
    {
      printf ("Hex_p failure\n");
      return 1;
    }
  printf ("Hex-3 success\n");

  strcpy (buf, "0123Xabc");
  rc = yubikey_hex_p (buf);
  printf ("hex-p(\"%s\") = %d\n", buf, rc);
  if (rc)
    {
      printf ("Hex_p failure\n");
      return 1;
    }
  printf ("Hex-3 success\n");

  /* Test AES */

  {
    uint8_t buf[1024];
    uint8_t key[16 + 1];

    memcpy (buf, "0123456789abcdef\0", 17);
    memcpy (key, "abcdef0123456789\0", 17);
    printf ("aes-decrypt (data=%s, key=%s)\n => ", (char *) buf, (char *) key);
    yubikey_aes_decrypt (buf, key);
    for (i = 0; i < 16; i++)
      printf ("%02x", buf[i] & 0xFF);
    printf ("\n");

    if (memcmp (buf,
		"\x83\x8a\x46\x7f\x34\x63\x95\x51"
		"\x75\x5b\xd3\x2a\x4a\x2f\x15\xe1", 16) != 0)
      {
	printf ("AES failure\n");
	return 1;
      }
    printf ("AES-1 success\n");

    yubikey_aes_encrypt (buf, key);
    if (memcmp (buf, "0123456789abcdef", 16) != 0)
      {
	printf ("AES encryption failure\n");
	return 1;
      }
    printf ("AES-2 success\n");

    /* Test OTP */

    memcpy ((void *) &tok,
	    "\x16\xe1\xe5\xd9\xd3\x99\x10\x04\x45\x20\x07\xe3\x02\x00\x00", 16);
    memcpy (key, "abcdef0123456789", 16);

    yubikey_generate ((void *) &tok, key, buf);
    yubikey_parse ((uint8_t *) buf, key, &tok);

    if (memcmp
	(&tok, "\x16\xe1\xe5\xd9\xd3\x99\x10\x04\x45\x20\x07\xe3\x02\x00\x00",
	 16) != 0)
      {
	printf ("OTP generation - parse failure\n");
	return 1;
      }
    printf ("OTP-1 success\n");
  }

  return 0;
}
