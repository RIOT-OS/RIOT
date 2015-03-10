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
 * @brief       Implementation of the Salsa20 stream cipher
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      D. J. Bernstein
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "crypto/salsa20.h"

/* Convert word to byte array */
static void salsa20_wordtobyte(uint8_t output[64], const uint32_t input[16])
{
    uint32_t x[16];
    int i;

    x[0] = input[0];
    x[1] = input[1];
    x[2] = input[2];
    x[3] = input[3];
    x[4] = input[4];
    x[5] = input[5];
    x[6] = input[6];
    x[7] = input[7];
    x[8] = input[8];
    x[9] = input[9];
    x[10] = input[10];
    x[11] = input[11];
    x[12] = input[12];
    x[13] = input[13];
    x[14] = input[14];
    x[15] = input[15];

    for (i = 20; i > 0; i -= 2) {
        x[ 4] = XOR(x[ 4], ROTL32(PLUS(x[ 0], x[12]), 7));
        x[ 8] = XOR(x[ 8], ROTL32(PLUS(x[ 4], x[ 0]), 9));
        x[12] = XOR(x[12], ROTL32(PLUS(x[ 8], x[ 4]), 13));
        x[ 0] = XOR(x[ 0], ROTL32(PLUS(x[12], x[ 8]), 18));
        x[ 9] = XOR(x[ 9], ROTL32(PLUS(x[ 5], x[ 1]), 7));
        x[13] = XOR(x[13], ROTL32(PLUS(x[ 9], x[ 5]), 9));
        x[ 1] = XOR(x[ 1], ROTL32(PLUS(x[13], x[ 9]), 13));
        x[ 5] = XOR(x[ 5], ROTL32(PLUS(x[ 1], x[13]), 18));
        x[14] = XOR(x[14], ROTL32(PLUS(x[10], x[ 6]), 7));
        x[ 2] = XOR(x[ 2], ROTL32(PLUS(x[14], x[10]), 9));
        x[ 6] = XOR(x[ 6], ROTL32(PLUS(x[ 2], x[14]), 13));
        x[10] = XOR(x[10], ROTL32(PLUS(x[ 6], x[ 2]), 18));
        x[ 3] = XOR(x[ 3], ROTL32(PLUS(x[15], x[11]), 7));
        x[ 7] = XOR(x[ 7], ROTL32(PLUS(x[ 3], x[15]), 9));
        x[11] = XOR(x[11], ROTL32(PLUS(x[ 7], x[ 3]), 13));
        x[15] = XOR(x[15], ROTL32(PLUS(x[11], x[ 7]), 18));
        x[ 1] = XOR(x[ 1], ROTL32(PLUS(x[ 0], x[ 3]), 7));
        x[ 2] = XOR(x[ 2], ROTL32(PLUS(x[ 1], x[ 0]), 9));
        x[ 3] = XOR(x[ 3], ROTL32(PLUS(x[ 2], x[ 1]), 13));
        x[ 0] = XOR(x[ 0], ROTL32(PLUS(x[ 3], x[ 2]), 18));
        x[ 6] = XOR(x[ 6], ROTL32(PLUS(x[ 5], x[ 4]), 7));
        x[ 7] = XOR(x[ 7], ROTL32(PLUS(x[ 6], x[ 5]), 9));
        x[ 4] = XOR(x[ 4], ROTL32(PLUS(x[ 7], x[ 6]), 13));
        x[ 5] = XOR(x[ 5], ROTL32(PLUS(x[ 4], x[ 7]), 18));
        x[11] = XOR(x[11], ROTL32(PLUS(x[10], x[ 9]), 7));
        x[ 8] = XOR(x[ 8], ROTL32(PLUS(x[11], x[10]), 9));
        x[ 9] = XOR(x[ 9], ROTL32(PLUS(x[ 8], x[11]), 13));
        x[10] = XOR(x[10], ROTL32(PLUS(x[ 9], x[ 8]), 18));
        x[12] = XOR(x[12], ROTL32(PLUS(x[15], x[14]), 7));
        x[13] = XOR(x[13], ROTL32(PLUS(x[12], x[15]), 9));
        x[14] = XOR(x[14], ROTL32(PLUS(x[13], x[12]), 13));
        x[15] = XOR(x[15], ROTL32(PLUS(x[14], x[13]), 18));
    }

    x[0] = PLUS(x[0], input[0]);
    x[1] = PLUS(x[1], input[1]);
    x[2] = PLUS(x[2], input[2]);
    x[3] = PLUS(x[3], input[3]);
    x[4] = PLUS(x[4], input[4]);
    x[5] = PLUS(x[5], input[5]);
    x[6] = PLUS(x[6], input[6]);
    x[7] = PLUS(x[7], input[7]);
    x[8] = PLUS(x[8], input[8]);
    x[9] = PLUS(x[9], input[9]);
    x[10] = PLUS(x[10], input[10]);
    x[11] = PLUS(x[11], input[11]);
    x[12] = PLUS(x[12], input[12]);
    x[13] = PLUS(x[13], input[13]);
    x[14] = PLUS(x[14], input[14]);
    x[15] = PLUS(x[15], input[15]);

    U32TO8_LITTLE(output + 4 * 0, x[0]);
    U32TO8_LITTLE(output + 4 * 1, x[1]);
    U32TO8_LITTLE(output + 4 * 2, x[2]);
    U32TO8_LITTLE(output + 4 * 3, x[3]);
    U32TO8_LITTLE(output + 4 * 4, x[4]);
    U32TO8_LITTLE(output + 4 * 5, x[5]);
    U32TO8_LITTLE(output + 4 * 6, x[6]);
    U32TO8_LITTLE(output + 4 * 7, x[7]);
    U32TO8_LITTLE(output + 4 * 8, x[8]);
    U32TO8_LITTLE(output + 4 * 9, x[9]);
    U32TO8_LITTLE(output + 4 * 10, x[10]);
    U32TO8_LITTLE(output + 4 * 11, x[11]);
    U32TO8_LITTLE(output + 4 * 12, x[12]);
    U32TO8_LITTLE(output + 4 * 13, x[13]);
    U32TO8_LITTLE(output + 4 * 14, x[14]);
    U32TO8_LITTLE(output + 4 * 15, x[15]);
}

/* Salsa20 Key setup */
void salsa20_keysetup(salsa20_ctx *ctx, const uint8_t *key, uint32_t keysize)
{
    const char *constants;

    static const char sigma[16] = "expand 32-byte k";
    static const char tau[16] = "expand 16-byte k";

    ctx->input[1] = U8TO32_LITTLE(key + 0);
    ctx->input[2] = U8TO32_LITTLE(key + 4);
    ctx->input[3] = U8TO32_LITTLE(key + 8);
    ctx->input[4] = U8TO32_LITTLE(key + 12);

    /* recommended */
    if (keysize == 256) {
        key += 16;
        constants = sigma;
        /* kbits == 128 */
    }
    else {
        constants = tau;
    }

    ctx->input[11] = U8TO32_LITTLE(key + 0);
    ctx->input[12] = U8TO32_LITTLE(key + 4);
    ctx->input[13] = U8TO32_LITTLE(key + 8);
    ctx->input[14] = U8TO32_LITTLE(key + 12);
    ctx->input[0] = U8TO32_LITTLE(constants + 0);
    ctx->input[5] = U8TO32_LITTLE(constants + 4);
    ctx->input[10] = U8TO32_LITTLE(constants + 8);
    ctx->input[15] = U8TO32_LITTLE(constants + 12);
}

/* Salsa20 IV setup */
void salsa20_ivsetup(salsa20_ctx *x, const uint8_t *iv)
{
    x->input[6] = U8TO32_LITTLE(iv + 0);
    x->input[7] = U8TO32_LITTLE(iv + 4);
    x->input[8] = 0;
    x->input[9] = 0;
}

/* Encrypt or decrypt "msglen" number of message bytes */
void salsa20_process_bytes(salsa20_ctx *ctx, const uint8_t *input, uint8_t *output, uint32_t msglen)
{
    uint8_t tmp[64];
    uint32_t i;

    for (;;) {
        salsa20_wordtobyte(tmp, ctx->input);
        ctx->input[8] = PLUSONE(ctx->input[8]);

        if (!ctx->input[8]) {
            ctx->input[9] = PLUSONE(ctx->input[9]);
            /* stopping at 2^70 bytes per nonce is user's responsibility */
        }

        if (msglen <= 64) {
            for (i = 0; i < msglen; ++i) {
                output[i] = input[i] ^ tmp[i];
            }

            return;
        }

        for (i = 0; i < 64; ++i) {
            output[i] = input[i] ^ tmp[i];
        }

        msglen -= 64;
        output += 64;
        input += 64;
    }
}
/** @} */
