/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        sosemanuk.c
 * @brief       Implementation of the SOSEMANUK stream cipher
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Come Berbain
 * @author      Olivier Billet
 * @author      Anne Canteaut
 * @author      Nicolas Courtois
 * @author      Henri Gilbert
 * @author      Louis Goubin
 * @author      Aline Gouget
 * @author      Louis Granboulan
 * @author      Cédric Lauradoux
 * @author      Marine Minier
 * @author      Thomas Pornin
 * @author      Hervé Sibert
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "crypto/sosemanuk.h"

/* Compute the next block of bits of output stream */
static void sosemanuk_internal(sosemanuk_ctx *rc, const uint32_t *src, uint32_t *dst)
{
    uint32_t s00 = rc->s00;
    uint32_t s01 = rc->s01;
    uint32_t s02 = rc->s02;
    uint32_t s03 = rc->s03;
    uint32_t s04 = rc->s04;
    uint32_t s05 = rc->s05;
    uint32_t s06 = rc->s06;
    uint32_t s07 = rc->s07;
    uint32_t s08 = rc->s08;
    uint32_t s09 = rc->s09;
    uint32_t r1 = rc->r1;
    uint32_t r2 = rc->r2;
    //cppcheck-suppress variableScope unreadVariable
    uint32_t u0, u1, u2, u3, u4;
    uint32_t v0, v1, v2, v3;

    STEP(00, 01, 02, 03, 04, 05, 06, 07, 08, 09, v0, u0);
    STEP(01, 02, 03, 04, 05, 06, 07, 08, 09, 00, v1, u1);
    STEP(02, 03, 04, 05, 06, 07, 08, 09, 00, 01, v2, u2);
    STEP(03, 04, 05, 06, 07, 08, 09, 00, 01, 02, v3, u3);
    SRD(S2, 2, 3, 1, 4, 0);
    STEP(04, 05, 06, 07, 08, 09, 00, 01, 02, 03, v0, u0);
    STEP(05, 06, 07, 08, 09, 00, 01, 02, 03, 04, v1, u1);
    STEP(06, 07, 08, 09, 00, 01, 02, 03, 04, 05, v2, u2);
    STEP(07, 08, 09, 00, 01, 02, 03, 04, 05, 06, v3, u3);
    SRD(S2, 2, 3, 1, 4, 16);
    STEP(08, 09, 00, 01, 02, 03, 04, 05, 06, 07, v0, u0);
    STEP(09, 00, 01, 02, 03, 04, 05, 06, 07, 08, v1, u1);
    STEP(00, 01, 02, 03, 04, 05, 06, 07, 08, 09, v2, u2);
    STEP(01, 02, 03, 04, 05, 06, 07, 08, 09, 00, v3, u3);
    SRD(S2, 2, 3, 1, 4, 32);
    STEP(02, 03, 04, 05, 06, 07, 08, 09, 00, 01, v0, u0);
    STEP(03, 04, 05, 06, 07, 08, 09, 00, 01, 02, v1, u1);
    STEP(04, 05, 06, 07, 08, 09, 00, 01, 02, 03, v2, u2);
    STEP(05, 06, 07, 08, 09, 00, 01, 02, 03, 04, v3, u3);
    SRD(S2, 2, 3, 1, 4, 48);
    STEP(06, 07, 08, 09, 00, 01, 02, 03, 04, 05, v0, u0);
    STEP(07, 08, 09, 00, 01, 02, 03, 04, 05, 06, v1, u1);
    STEP(08, 09, 00, 01, 02, 03, 04, 05, 06, 07, v2, u2);
    STEP(09, 00, 01, 02, 03, 04, 05, 06, 07, 08, v3, u3);
    SRD(S2, 2, 3, 1, 4, 64);

    rc->s00 = s00;
    rc->s01 = s01;
    rc->s02 = s02;
    rc->s03 = s03;
    rc->s04 = s04;
    rc->s05 = s05;
    rc->s06 = s06;
    rc->s07 = s07;
    rc->s08 = s08;
    rc->s09 = s09;
    rc->r1 = r1;
    rc->r2 = r2;
}

/* SOSEMANUK Key setup */
void sosemanuk_keysetup(sosemanuk_ctx *kc, const uint8_t *key, uint32_t keysize, uint32_t ivsize)
{
    unsigned char wbuf[32];
    //cppcheck-suppress variableScope unreadVariable
    register uint32_t w0, w1, w2, w3, w4, w5, w6, w7;
    //cppcheck-suppress variableScope unreadVariable
    int i = 0;
    size_t key_len = keysize / 8;

    kc->ivlen = ivsize / 8;
    memcpy(wbuf, key, key_len);

    if (key_len < 32) {
        wbuf[key_len] = 0x01;

        if (key_len < 31) {
            memset(wbuf + key_len + 1, 0, 31 - key_len);
        }
    }

    w0 = U8TO32_LITTLE(wbuf);
    w1 = U8TO32_LITTLE(wbuf + 4);
    w2 = U8TO32_LITTLE(wbuf + 8);
    w3 = U8TO32_LITTLE(wbuf + 12);
    w4 = U8TO32_LITTLE(wbuf + 16);
    w5 = U8TO32_LITTLE(wbuf + 20);
    w6 = U8TO32_LITTLE(wbuf + 24);
    w7 = U8TO32_LITTLE(wbuf + 28);

    WUP0(0); SKS3;
    WUP1(4); SKS2;
    WUP0(8); SKS1;
    WUP1(12); SKS0;
    WUP0(16); SKS7;
    WUP1(20); SKS6;
    WUP0(24); SKS5;
    WUP1(28); SKS4;
    WUP0(32); SKS3;
    WUP1(36); SKS2;
    WUP0(40); SKS1;
    WUP1(44); SKS0;
    WUP0(48); SKS7;
    WUP1(52); SKS6;
    WUP0(56); SKS5;
    WUP1(60); SKS4;
    WUP0(64); SKS3;
    WUP1(68); SKS2;
    WUP0(72); SKS1;
    WUP1(76); SKS0;
    WUP0(80); SKS7;
    WUP1(84); SKS6;
    WUP0(88); SKS5;
    WUP1(92); SKS4;
    WUP0(96); SKS3;
}

/* SOSEMANUK IV setup */
void sosemanuk_ivsetup(sosemanuk_ctx *ctx, const uint8_t *iv)
{
    //cppcheck-suppress variableScope unreadVariable
    register uint32_t r0, r1, r2, r3, r4;
    unsigned char ivtmp[16];

    if (iv_len >= sizeof ivtmp) {
        memcpy(ivtmp, iv, sizeof ivtmp);
    }
    else
    {
        if (iv_len > 0) {
            memcpy(ivtmp, iv, iv_len);
        }
        memset(ivtmp + iv_len, 0, (sizeof ivtmp) - iv_len);
    }

    /* Decode IV into four 32-bit words (little-endian) */
    r0 = U8TO32_LITTLE(ivtmp);
    r1 = U8TO32_LITTLE(ivtmp + 4);
    r2 = U8TO32_LITTLE(ivtmp + 8);
    r3 = U8TO32_LITTLE(ivtmp + 12);

    /* Encrypt IV with Serpent24. Some values are extracted */
    FSS(0, S0, 0, 1, 2, 3, 4, 1, 4, 2, 0);
    FSS(4, S1, 1, 4, 2, 0, 3, 2, 1, 0, 4);
    FSS(8, S2, 2, 1, 0, 4, 3, 0, 4, 1, 3);
    FSS(12, S3, 0, 4, 1, 3, 2, 4, 1, 3, 2);
    FSS(16, S4, 4, 1, 3, 2, 0, 1, 0, 4, 2);
    FSS(20, S5, 1, 0, 4, 2, 3, 0, 2, 1, 4);
    FSS(24, S6, 0, 2, 1, 4, 3, 0, 2, 3, 1);
    FSS(28, S7, 0, 2, 3, 1, 4, 4, 1, 2, 0);
    FSS(32, S0, 4, 1, 2, 0, 3, 1, 3, 2, 4);
    FSS(36, S1, 1, 3, 2, 4, 0, 2, 1, 4, 3);
    FSS(40, S2, 2, 1, 4, 3, 0, 4, 3, 1, 0);
    FSS(44, S3, 4, 3, 1, 0, 2, 3, 1, 0, 2);
    rc->s09 = r3;
    rc->s08 = r1;
    rc->s07 = r0;
    rc->s06 = r2;

    FSS(48, S4, 3, 1, 0, 2, 4, 1, 4, 3, 2);
    FSS(52, S5, 1, 4, 3, 2, 0, 4, 2, 1, 3);
    FSS(56, S6, 4, 2, 1, 3, 0, 4, 2, 0, 1);
    FSS(60, S7, 4, 2, 0, 1, 3, 3, 1, 2, 4);
    FSS(64, S0, 3, 1, 2, 4, 0, 1, 0, 2, 3);
    FSS(68, S1, 1, 0, 2, 3, 4, 2, 1, 3, 0);
    rc->r1  = r2;
    rc->s04 = r1;
    rc->r2  = r3;
    rc->s05 = r0;

    FSS(72, S2, 2, 1, 3, 0, 4, 3, 0, 1, 4);
    FSS(76, S3, 3, 0, 1, 4, 2, 0, 1, 4, 2);
    FSS(80, S4, 0, 1, 4, 2, 3, 1, 3, 0, 2);
    FSS(84, S5, 1, 3, 0, 2, 4, 3, 2, 1, 0);
    FSS(88, S6, 3, 2, 1, 0, 4, 3, 2, 4, 1);
    FSF(92, S7, 3, 2, 4, 1, 0, 0, 1, 2, 3);
    rc->s03 = r0;
    rc->s02 = r1;
    rc->s01 = r2;
    rc->s00 = r3;
}

/* Encrypt or decrypt "msglen" number of message bytes */
void sosemanuk_process_bytes(sosemanuk_ctx *ctx, const uint8_t *input, uint8_t *output,
                             uint32_t msglen)
{
    while (msglen > 0) {
        uint32_t ibuf[SOSEMANUK_BLOCKLENGTH / 4];
        uint32_t obuf[SOSEMANUK_BLOCKLENGTH / 4];
        size_t len;

        len = sizeof ibuf;

        if (len > msglen) {
            len = msglen;
        }

        memcpy(ibuf, input, len);
        sosemanuk_internal(ctx, ibuf, obuf);
        memcpy(output, obuf, len);
        input += len;
        output += len;
        msglen -= len;
    }
}

/* Encrypt or decrypt a "blocks" number of blocks */
void sosemanuk_process_blocks(sosemanuk_ctx *ctx, const uint8_t *input, uint8_t *output,
                              uint32_t blocks)
{
    while (blocks -- > 0) {
        sosemanuk_internal(ctx, (uint32_t *)input, (uint32_t *)output);
        input += SOSEMANUK_BLOCKLENGTH;
        output += SOSEMANUK_BLOCKLENGTH;
    }
}
/** @} */
