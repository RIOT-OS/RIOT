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
 * @file
 * @brief       Implementation of the NLSv2 stream cipher
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Gregory Rose
 * @author      Philip Hawkes
 * @author      Michael Paddon
 * @author      Miriam Wiggers de Vries
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "crypto/nlsv2.h"

/* Return a non-linear function of some parts of the register */
static uint32_t nltap(nlsv2_ctx *ctx)
{
    return (ctx->R[0] + ctx->R[16]) ^ (ctx->R[1] + ctx->R[13]) ^ (ctx->R[6] + ctx->konst);
}

/* Cycle the contents of the shift register */
static void cycle(nlsv2_ctx *ctx)
{
    uint32_t t;
    int i;

    /* Nonlinear feedback function */
    t = ROTL(ctx->R[0], 19) + ROTL(ctx->R[15], 9) + ctx->konst;
    t ^= nlsv2_sbox[(t >> 24) & 0xFF];
    t ^= ctx->R[4];

    /* Shift register */
    for (i = 1; i < N; ++i) {
        ctx->R[i - 1] = ctx->R[i];
    }

    ctx->R[N - 1] = t;
}

/* NLSv2 Key setup */
void nlsv2_keysetup(nlsv2_ctx *ctx, const uint8_t *key, uint32_t keylen, uint32_t ivlen)
{
    uint32_t i, j;
    uint32_t k;
    uint8_t xtra[4];

    /* Register initialised to Fibonacci numbers; Counter zeroed */
    ctx->R[0] = 1;
    ctx->R[1] = 1;

    for (i = 2; i < N; ++i) {
        ctx->R[i] = ctx->R[i - 1] + ctx->R[i - 2];
    }

    ctx->konst = INITKONST;

    keylen = (keylen + 7) / 8;

    /* Start folding in key */
    for (i = 0; i < (keylen & ~0x3); i += 4) {
        k = BYTE2WORD(&key[i]);
        /* Add key */
        ctx->R[KEYP] += k;

        cycle(ctx);
        /* XOR nltap */
        ctx->R[FOLDP] ^= nltap(ctx);
    }

    /* If there were any extra key bytes, zero pad to a word */
    if (i < keylen) {
        for (j = 0 /* i unchanged */; i < keylen; ++i) {
            xtra[j++] = key[i];
        }

        for (/* j unchanged */; j < 4; ++j) {
            xtra[j] = 0;
        }

        k = BYTE2WORD(xtra);
        /* Add key */
        ctx->R[KEYP] += k;

        cycle(ctx);
        /* XOR nltap */
        ctx->R[FOLDP] ^= nltap(ctx);
    }

    /* Also fold in the length of the key */
    ctx->R[KEYP] += keylen;

    /* Nonlinear diffusion of register for key and MAC */
    for (i = 0; i < N; ++i) {
        cycle(ctx);
        /* XOR nltap */
        ctx->R[FOLDP] ^= nltap(ctx);
    }

    /* Initialise "konst" */
    cycle(ctx);
    ctx->konst = nltap(ctx);

    /* Save the current register state*/
    for (i = 0; i < N; ++i) {
        ctx->initR[i] = ctx->R[i];
    }

    ctx->nbuf = 0;
    ctx->ivsize = (ivlen + 7) / 8;
}

/* NLSv2 IV setup */
void nlsv2_ivsetup(nlsv2_ctx *ctx, const uint8_t *iv)
{
    uint32_t i, j;
    uint32_t k;
    uint8_t xtra[4];

    /* Initialise to previously saved register state */
    for (i = 0; i < N; ++i) {
        ctx->R[i] = ctx->initR[i];
    }

    /* TWEAK */
    ctx->konst = INITKONST;

    /* Start folding in iv */
    for (i = 0; i < (ctx->ivsize & ~0x3); i += 4) {
        k = BYTE2WORD(&iv[i]);
        /* Add key */
        ctx->R[KEYP] += k;

        cycle(ctx);
        /* XOR nltap */
        ctx->R[FOLDP] ^= nltap(ctx);
    }

    /* If there were any extra key bytes, zero pad to a word */
    if (i < ctx->ivsize) {
        for (j = 0 /* i unchanged */; i < ctx->ivsize; ++i) {
            xtra[j++] = iv[i];
        }

        for (/* j unchanged */; j < 4; ++j) {
            xtra[j] = 0;
        }

        k = BYTE2WORD(xtra);
        /* Add key */
        ctx->R[KEYP] += k;

        cycle(ctx);
        /* XOR nltap */
        ctx->R[FOLDP] ^= nltap(ctx);
    }

    /* Also fold in the length of the key */
    ctx->R[KEYP] += ctx->ivsize;

    /* Nonlinear diffusion of register for key and MAC */
    for (i = 0; i < N; ++i) {
        cycle(ctx);
        /* XOR nltap */
        ctx->R[FOLDP] ^= nltap(ctx);
    }

    /* Initialise "konst" */
    cycle(ctx);
    ctx->konst = nltap(ctx);

    ctx->nbuf = 0;
}

/* Encrypt or decrypt "msglen" number of message bytes */
void nlsv2_process_bytes(nlsv2_ctx *ctx, const uint8_t *input, uint8_t *output, uint32_t msglen)
{
    uint8_t *endbuf;
    uint32_t t = 0;

    /* Prepare output buffer */
    memmove(output, input, msglen);

    /* Handle any previously buffered bytes */
    while (ctx->nbuf != 0 && msglen != 0) {
        *output++ ^= ctx->sbuf & 0xFF;
        ctx->sbuf >>= 8;
        ctx->nbuf -= 8;
        --msglen;
    }

    /* Handle whole words */
    endbuf = &output[msglen & ~((uint32_t)0x03)];

    while (output < endbuf) {
        cycle(ctx);
        t = nltap(ctx);
        XORWORD(t, output);
        output += 4;
    }

    /* Handle any trailing bytes */
    msglen &= 0x03;

    if (msglen != 0) {
        cycle(ctx);
        ctx->sbuf = nltap(ctx);
        ctx->nbuf = 32;

        while (ctx->nbuf != 0 && msglen != 0) {
            *output++ ^= ctx->sbuf & 0xFF;
            ctx->sbuf >>= 8;
            ctx->nbuf -= 8;
            --msglen;
        }
    }
}
/** @} */
