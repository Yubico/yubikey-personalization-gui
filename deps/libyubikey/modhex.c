/* modhex.c --- Simple ModHex conversion command line tool.
 *
 * Written by Simon Josefsson <simon@josefsson.org>.
 * Copyright (c) 2006, 2007, 2008, 2009 Yubico AB
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
#include <stdbool.h>

int
main (int argc, char *argv[])
{
  int argi;
  bool decode_p = false;
  bool hex_p = false;
  char *data;
  size_t inlen;

  /* Usage. */
  if (argc < 2)
    {
      printf ("Usage: %s [-dh] <data>\n", argv[0]);
      printf ("\n");
      printf
	("Convert input DATA as specified and print output to STDOUT.\n");
      printf ("\n");
      printf (" -d: Decode data (the default is to encode data).\n");
      printf (" -h: Use hex encoding for all non-modhex data.\n");
      printf (" DATA: string with data to encode\n");
      printf ("\n");
      printf ("Examples:\n");
      printf ("\n");
      printf ("  ModHex encode ASCII-string \"test\":\n");
      printf ("    %s test\n", argv[0]);
      printf ("\n");
      printf ("  Decode ModHex data \"ifhgieif\" into ASCII string:\n");
      printf ("    %s -d ifhgieif\n", argv[0]);
      printf ("\n");
      printf ("  ModHex encode hex-encoded data \"b565716f\":\n");
      printf ("    %s -h b565716f\n", argv[0]);
      printf ("\n");
      printf
	("  Decode ModHex data \"nghgibhv\" and print hex-encode data:\n");
      printf ("    %s -d -h nghgibhv\n", argv[0]);
      return 1;
    }

  /* Command line handling. */

  argi = 1;
  while (argc > argi && (!decode_p || !hex_p))
    {
      if (strcmp (argv[argi], "-d") == 0)
	{
	  decode_p = true;
	  argi++;
	}
      else if (strcmp (argv[argi], "-h") == 0)
	{
	  hex_p = true;
	  argi++;
	}
      else
	break;
    }

  if (argc < argi + 1)
    {
      printf ("error: missing argument\n");
      return 1;
    }

  data = argv[argi];
  inlen = strlen (data);

  if (!decode_p && hex_p)
    {
      size_t i;
      char *tmp;

      if (inlen & 1)
	{
	  printf ("error: size of hex encoded input not even\n");
	  return 1;
	}

      tmp = malloc (inlen / 2 + 1);
      if (!tmp)
	{
	  perror ("malloc");
	  return 1;
	}

      for (i = 0; i < inlen; i++)
	{
	  const char *hex = "0123456789abcdef";
	  char *p;

	  if ((p = strchr (hex, data[i])) == NULL)
	    {
	      printf ("error: input not hex encoded at position %d\n", i);
	      return 1;
	    }

	  if (i & 1)
	    tmp[i / 2] |= p - hex;
	  else
	    tmp[i / 2] = (p - hex) << 4;
	}
      tmp[i / 2] = '\0';

      inlen = inlen / 2;

      data = tmp;
    }

  /* Doit. */

  {
    size_t i;
    char *buf = malloc (2 * inlen + 1);

    if (!buf)
      {
	perror ("malloc");
	return 1;
      }

    if (decode_p)
      yubikey_modhex_decode (buf, data, inlen / 2);
    else
      yubikey_modhex_encode (buf, data, inlen);

    if (decode_p && hex_p)
      {
	for (i = 0; i < inlen / 2; i++)
	  printf ("%02x", buf[i] & 0xFF);
	printf ("\n");
      }
    else if (decode_p)
      fwrite (buf, inlen / 2, 1, stdout);
    else
      printf ("%s\n", buf);

    free (buf);
  }

  if (!decode_p && hex_p)
    free (data);

  return 0;
}
