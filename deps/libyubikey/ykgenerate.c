/* ykgenerate.c --- Example command line interface for generating OTPs.
 *
 * Written by Olov Danielson <olov@yubico.com>.
 * Copyright (c) 2006, 2007, 2009, 2010 Yubico AB
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

#include "yubikey.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <limits.h>

int
main (int argc, char *argv[])
{
  uint8_t key[YUBIKEY_KEY_SIZE];
  char otp[YUBIKEY_OTP_SIZE];
  char *aeskey, *yk_internalname, *yk_counter, *yk_low, *yk_high, *yk_use;
  yubikey_token_st tok;
  int i;

  /* Initiate pseudo-random generator */
  srand (time (NULL));

  /* Parse command line parameters. */
  if (argc < 7)
    {
      printf ("Usage: %s <aeskey> <yk_internalname> <yk_counter> <yk_low> "
	      "<yk_high> <yk_use>\n", argv[0]);
      printf (" AESKEY:\t\tHex encoded AES-key.\n");
      printf (" YK_INTERNALNAME:\tHex encoded yk_internalname (48 bit).\n");
      printf (" YK_COUNTER:\t\tHex encoded counter (16 bit).\n");
      printf (" YK_LOW:\t\tHex encoded timestamp low (16 bit).\n");
      printf (" YK_HIGH:\t\tHex encoded timestamp high (8bit).\n");
      printf (" YK_USE:\t\tHex encoded use (8 bit).\n");
      return EXIT_FAILURE;
    }

  aeskey = argv[1];
  if (strlen (aeskey) != 32)
    {
      printf ("error: Hex encoded AES-key must be 32 characters.\n");
      return EXIT_FAILURE;
    }

  yk_internalname = argv[2];
  if (strlen (yk_internalname) != 12)
    {
      printf ("error: Hex encoded yk_internalname must be 12 characters.\n");
      return EXIT_FAILURE;
    }

  yk_counter = argv[3];
  if (strlen (yk_counter) != 4)
    {
      printf ("error: Hex encoded yk_counter must be 4 characters.\n");
      return EXIT_FAILURE;
    }

  yk_low = argv[4];
  if (strlen (yk_low) != 4)
    {
      printf ("error: Hex encoded yk_low must be 4 characters.\n");
      return EXIT_FAILURE;
    }

  yk_high = argv[5];
  if (strlen (yk_high) != 2)
    {
      printf ("error: Hex encoded yk_high must be 2 characters.\n");
      return EXIT_FAILURE;
    }

  yk_use = argv[6];
  if (strlen (yk_use) != 2)
    {
      printf ("error: Hex encoded yk_use must be 2 characters.\n");
      return EXIT_FAILURE;
    }

  yubikey_hex_decode ((char *) key, aeskey, YUBIKEY_KEY_SIZE);

  /* Fill upp tok with values */
  yubikey_hex_decode ((char *) &tok.uid, yk_internalname, 6);
  {
    unsigned long i;
    char *endptr;

    i = strtoul (yk_counter, &endptr, 16);
    if (*endptr != '\0' || i == ULONG_MAX)
      {
	printf ("error: Hex encoded yk_counter must be 4 hex characters.\n");
	return EXIT_FAILURE;
      }
    tok.ctr = i;

    i = strtoul (yk_low, &endptr, 16);
    if (*endptr != '\0' || i == ULONG_MAX)
      {
	printf ("error: Hex encoded yk_low must be 4 hex characters.\n");
	return EXIT_FAILURE;
      }
    tok.tstpl = i;
  }
  yubikey_hex_decode ((char *) &tok.tstph, yk_high, 1);
  yubikey_hex_decode ((char *) &tok.use, yk_use, 1);
  tok.rnd = rand ();
  tok.crc = ~yubikey_crc16 ((void *) &tok, sizeof (tok) - sizeof (tok.crc));

  yubikey_generate ((void *) &tok, key, otp);

  for (i = 0; i < 32; i++)
    {
      printf ("%c", otp[i]);
    }
  printf ("\n");

  return EXIT_SUCCESS;
}
