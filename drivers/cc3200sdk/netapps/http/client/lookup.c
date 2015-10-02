/*
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* C code produced by gperf version 3.0.4 */
/* Computed positions: -k'1,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 8 "lookup.gperf"

#include "lookup.h"
#line 11 "lookup.gperf"
struct Lookup_Field;

#define TOTAL_KEYWORDS 25
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 18
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 53
/* maximum key range = 50, duplicates = 0 */

#ifndef GPERF_DOWNCASE
#define GPERF_DOWNCASE 1
static unsigned char gperf_downcase[256] =
  {
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
     30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
     45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
     60,  61,  62,  63,  64,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106,
    107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
    122,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
    135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
    165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
    195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
    225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254,
    255
  };
#endif

#ifndef GPERF_CASE_STRCMP
#define GPERF_CASE_STRCMP 1
static int
gperf_case_strcmp (s1, s2)
     register const char *s1;
     register const char *s2;
{
  for (;;)
    {
      unsigned char c1 = gperf_downcase[(unsigned char)*s1++];
      unsigned char c2 = gperf_downcase[(unsigned char)*s2++];
      if (c1 != 0 && c1 == c2)
        continue;
      return (int)c1 - (int)c2;
    }
}
#endif

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static const unsigned char asso_values[] =
    {
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 30, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54,  0, 54,  0,  5,  5,
      54, 10, 10, 54, 54, 54, 15, 54,  0, 54,
      30, 54,  0,  0,  0, 54,  0, 10, 54,  0,
      54, 54, 54, 54, 54, 54, 54,  0, 54,  0,
       5,  5, 54, 10, 10, 54, 54, 54, 15, 54,
       0, 54, 30, 54,  0,  0,  0, 54,  0, 10,
      54,  0, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
      54, 54, 54, 54, 54, 54
    };
  return len + asso_values[(unsigned char)str[len - 1]] + asso_values[(unsigned char)str[0]];
}

static const struct Lookup_Field wordlist[] =
  {
#line 36 "lookup.gperf"
    {"Vary",               24},
#line 33 "lookup.gperf"
    {"Server",             21},
#line 34 "lookup.gperf"
    {"Trailer",            22},
#line 14 "lookup.gperf"
    {"Age",                2},
#line 17 "lookup.gperf"
    {"Connection",         5},
#line 32 "lookup.gperf"
    {"Retry-After",        20 },
#line 27 "lookup.gperf"
    {"Expires",            15},
#line 13 "lookup.gperf"
    {"Accept-Ranges",      1},
#line 25 "lookup.gperf"
    {"Date",               13 },
#line 15 "lookup.gperf"
    {"Allow",              3},
#line 21 "lookup.gperf"
    {"Content-Location",   9},
#line 24 "lookup.gperf"
    {"Content-Type",       12},
#line 23 "lookup.gperf"
    {"Content-Range",      11},
#line 26 "lookup.gperf"
    {"ETag",               14},
#line 19 "lookup.gperf"
    {"Content-Language",   7},
#line 29 "lookup.gperf"
    {"Location",           17},
#line 20 "lookup.gperf"
    {"Content-Length",     8},
#line 18 "lookup.gperf"
    {"Content-Encoding",   6},
#line 35 "lookup.gperf"
    {"Transfer-Encoding",  23},
#line 16 "lookup.gperf"
    {"Cache-Control",      4},
#line 37 "lookup.gperf"
    {"WWW-Authenticate",   25},
#line 28 "lookup.gperf"
    {"Last-Modified",      16},
#line 30 "lookup.gperf"
    {"Pragma",             18},
#line 22 "lookup.gperf"
    {"Content-MD5",        10 },
#line 31 "lookup.gperf"
    {"Proxy-Authenticate", 19}
  };

#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
const struct Lookup_Field *
Lookup_getField (str, len)
     register const char *str;
     register unsigned int len;
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          register const struct Lookup_Field *resword;

          switch (key - 4)
            {
              case 0:
                resword = &wordlist[0];
                goto compare;
              case 2:
                resword = &wordlist[1];
                goto compare;
              case 3:
                resword = &wordlist[2];
                goto compare;
              case 4:
                resword = &wordlist[3];
                goto compare;
              case 6:
                resword = &wordlist[4];
                goto compare;
              case 7:
                resword = &wordlist[5];
                goto compare;
              case 8:
                resword = &wordlist[6];
                goto compare;
              case 9:
                resword = &wordlist[7];
                goto compare;
              case 10:
                resword = &wordlist[8];
                goto compare;
              case 11:
                resword = &wordlist[9];
                goto compare;
              case 12:
                resword = &wordlist[10];
                goto compare;
              case 13:
                resword = &wordlist[11];
                goto compare;
              case 14:
                resword = &wordlist[12];
                goto compare;
              case 15:
                resword = &wordlist[13];
                goto compare;
              case 17:
                resword = &wordlist[14];
                goto compare;
              case 19:
                resword = &wordlist[15];
                goto compare;
              case 20:
                resword = &wordlist[16];
                goto compare;
              case 22:
                resword = &wordlist[17];
                goto compare;
              case 23:
                resword = &wordlist[18];
                goto compare;
              case 24:
                resword = &wordlist[19];
                goto compare;
              case 27:
                resword = &wordlist[20];
                goto compare;
              case 29:
                resword = &wordlist[21];
                goto compare;
              case 32:
                resword = &wordlist[22];
                goto compare;
              case 37:
                resword = &wordlist[23];
                goto compare;
              case 49:
                resword = &wordlist[24];
                goto compare;
            }
          return 0;
        compare:
          {
            register const char *s = resword->name;

            if ((((unsigned char)*str ^ (unsigned char)*s) & ~32) == 0 && !gperf_case_strcmp (str, s))
              return resword;
          }
        }
    }
  return 0;
}
