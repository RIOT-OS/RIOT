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
 * @brief       Implementation of the HC-256 stream cipher
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Hongjun Wu
 *
 */

#include <stdio.h>
#include <stdint.h>

#include "crypto/hc256.h"

/* 16 steps of HC-256, generate 512 bits keystream */
static void generate_keystream(hc256_ctx *ctx, uint32_t *keystream)
{
    uint32_t cc = ctx->counter2048 & 0x3ff;
    uint32_t dd = (cc + 16) & 0x3ff;

    if (ctx->counter2048 < 1024) {
        ctx->counter2048 = (ctx->counter2048 + 16) & 0x7ff;
        HC256_STEP_P(ctx, cc + 0, cc + 1, 0, 6, 13, 4, keystream[0]);
        HC256_STEP_P(ctx, cc + 1, cc + 2, 1, 7, 14, 5, keystream[1]);
        HC256_STEP_P(ctx, cc + 2, cc + 3, 2, 8, 15, 6, keystream[2]);
        HC256_STEP_P(ctx, cc + 3, cc + 4, 3, 9, 0, 7, keystream[3]);
        HC256_STEP_P(ctx, cc + 4, cc + 5, 4, 10, 1, 8, keystream[4]);
        HC256_STEP_P(ctx, cc + 5, cc + 6, 5, 11, 2, 9, keystream[5]);
        HC256_STEP_P(ctx, cc + 6, cc + 7, 6, 12, 3, 10, keystream[6]);
        HC256_STEP_P(ctx, cc + 7, cc + 8, 7, 13, 4, 11, keystream[7]);
        HC256_STEP_P(ctx, cc + 8, cc + 9, 8, 14, 5, 12, keystream[8]);
        HC256_STEP_P(ctx, cc + 9, cc + 10, 9, 15, 6, 13, keystream[9]);
        HC256_STEP_P(ctx, cc + 10, cc + 11, 10, 0, 7, 14, keystream[10]);
        HC256_STEP_P(ctx, cc + 11, cc + 12, 11, 1, 8, 15, keystream[11]);
        HC256_STEP_P(ctx, cc + 12, cc + 13, 12, 2, 9, 0, keystream[12]);
        HC256_STEP_P(ctx, cc + 13, cc + 14, 13, 3, 10, 1, keystream[13]);
        HC256_STEP_P(ctx, cc + 14, cc + 15, 14, 4, 11, 2, keystream[14]);
        HC256_STEP_P(ctx, cc + 15, dd + 0, 15, 5, 12, 3, keystream[15]);
    }
    else {
        ctx->counter2048 = (ctx->counter2048 + 16) & 0x7ff;
        HC256_STEP_Q(ctx, 1024 + cc + 0, 1024 + cc + 1, 0, 6, 13, 4, keystream[0]);
        HC256_STEP_Q(ctx, 1024 + cc + 1, 1024 + cc + 2, 1, 7, 14, 5, keystream[1]);
        HC256_STEP_Q(ctx, 1024 + cc + 2, 1024 + cc + 3, 2, 8, 15, 6, keystream[2]);
        HC256_STEP_Q(ctx, 1024 + cc + 3, 1024 + cc + 4, 3, 9, 0, 7, keystream[3]);
        HC256_STEP_Q(ctx, 1024 + cc + 4, 1024 + cc + 5, 4, 10, 1, 8, keystream[4]);
        HC256_STEP_Q(ctx, 1024 + cc + 5, 1024 + cc + 6, 5, 11, 2, 9, keystream[5]);
        HC256_STEP_Q(ctx, 1024 + cc + 6, 1024 + cc + 7, 6, 12, 3, 10, keystream[6]);
        HC256_STEP_Q(ctx, 1024 + cc + 7, 1024 + cc + 8, 7, 13, 4, 11, keystream[7]);
        HC256_STEP_Q(ctx, 1024 + cc + 8, 1024 + cc + 9, 8, 14, 5, 12, keystream[8]);
        HC256_STEP_Q(ctx, 1024 + cc + 9, 1024 + cc + 10, 9, 15, 6, 13, keystream[9]);
        HC256_STEP_Q(ctx, 1024 + cc + 10, 1024 + cc + 11, 10, 0, 7, 14, keystream[10]);
        HC256_STEP_Q(ctx, 1024 + cc + 11, 1024 + cc + 12, 11, 1, 8, 15, keystream[11]);
        HC256_STEP_Q(ctx, 1024 + cc + 12, 1024 + cc + 13, 12, 2, 9, 0, keystream[12]);
        HC256_STEP_Q(ctx, 1024 + cc + 13, 1024 + cc + 14, 13, 3, 10, 1, keystream[13]);
        HC256_STEP_Q(ctx, 1024 + cc + 14, 1024 + cc + 15, 14, 4, 11, 2, keystream[14]);
        HC256_STEP_Q(ctx, 1024 + cc + 15, 1024 + dd + 0, 15, 5, 12, 3, keystream[15]);
    }
}

/* 16 steps of HC-256, without generating keystream */
static void setup_update(hc256_ctx *ctx)
{
    uint32_t cc = ctx->counter2048 & 0x3ff;
    uint32_t dd = (cc + 16) & 0x3ff;

    if (ctx->counter2048 < 1024) {
        ctx->counter2048 = (ctx->counter2048 + 16) & 0x7ff;
        HC256_UPDATE_P(ctx, cc + 0, cc + 1, 0, 6, 13);
        HC256_UPDATE_P(ctx, cc + 1, cc + 2, 1, 7, 14);
        HC256_UPDATE_P(ctx, cc + 2, cc + 3, 2, 8, 15);
        HC256_UPDATE_P(ctx, cc + 3, cc + 4, 3, 9, 0);
        HC256_UPDATE_P(ctx, cc + 4, cc + 5, 4, 10, 1);
        HC256_UPDATE_P(ctx, cc + 5, cc + 6, 5, 11, 2);
        HC256_UPDATE_P(ctx, cc + 6, cc + 7, 6, 12, 3);
        HC256_UPDATE_P(ctx, cc + 7, cc + 8, 7, 13, 4);
        HC256_UPDATE_P(ctx, cc + 8, cc + 9, 8, 14, 5);
        HC256_UPDATE_P(ctx, cc + 9, cc + 10, 9, 15, 6);
        HC256_UPDATE_P(ctx, cc + 10, cc + 11, 10, 0, 7);
        HC256_UPDATE_P(ctx, cc + 11, cc + 12, 11, 1, 8);
        HC256_UPDATE_P(ctx, cc + 12, cc + 13, 12, 2, 9);
        HC256_UPDATE_P(ctx, cc + 13, cc + 14, 13, 3, 10);
        HC256_UPDATE_P(ctx, cc + 14, cc + 15, 14, 4, 11);
        HC256_UPDATE_P(ctx, cc + 15, dd + 0, 15, 5, 12);
    }
    else {
        ctx->counter2048 = (ctx->counter2048 + 16) & 0x7ff;
        HC256_UPDATE_Q(ctx, 1024 + cc + 0, 1024 + cc + 1, 0, 6, 13);
        HC256_UPDATE_Q(ctx, 1024 + cc + 1, 1024 + cc + 2, 1, 7, 14);
        HC256_UPDATE_Q(ctx, 1024 + cc + 2, 1024 + cc + 3, 2, 8, 15);
        HC256_UPDATE_Q(ctx, 1024 + cc + 3, 1024 + cc + 4, 3, 9, 0);
        HC256_UPDATE_Q(ctx, 1024 + cc + 4, 1024 + cc + 5, 4, 10, 1);
        HC256_UPDATE_Q(ctx, 1024 + cc + 5, 1024 + cc + 6, 5, 11, 2);
        HC256_UPDATE_Q(ctx, 1024 + cc + 6, 1024 + cc + 7, 6, 12, 3);
        HC256_UPDATE_Q(ctx, 1024 + cc + 7, 1024 + cc + 8, 7, 13, 4);
        HC256_UPDATE_Q(ctx, 1024 + cc + 8, 1024 + cc + 9, 8, 14, 5);
        HC256_UPDATE_Q(ctx, 1024 + cc + 9, 1024 + cc + 10, 9, 15, 6);
        HC256_UPDATE_Q(ctx, 1024 + cc + 10, 1024 + cc + 11, 10, 0, 7);
        HC256_UPDATE_Q(ctx, 1024 + cc + 11, 1024 + cc + 12, 11, 1, 8);
        HC256_UPDATE_Q(ctx, 1024 + cc + 12, 1024 + cc + 13, 12, 2, 9);
        HC256_UPDATE_Q(ctx, 1024 + cc + 13, 1024 + cc + 14, 13, 3, 10);
        HC256_UPDATE_Q(ctx, 1024 + cc + 14, 1024 + cc + 15, 14, 4, 11);
        HC256_UPDATE_Q(ctx, 1024 + cc + 15, 1024 + dd + 0, 15, 5, 12);
    }
}

/* HC-256 Key setup */
void hc256_keysetup(hc256_ctx *ctx, const uint8_t *key, uint32_t keysize, uint32_t ivsize)
{
    uint32_t i;

    ctx->keysize = keysize;
    ctx->ivsize = ivsize;

    /* Key size in bits (128+128*i) */
    for (i = 0; i < (keysize >> 5); i++) {
        ctx->key[i] = U32TO32_LITTLE(((uint32_t *)key)[i]);
    }

    for (; i < 8 ; i++) {
        ctx->key[i] = ctx->key[i - 4];
    }
}

/* HC-256 IV setup */
void hc256_ivsetup(hc256_ctx *ctx, const uint8_t *iv)
{
    uint32_t i;

    /* Initialize the iv (128+128*i) bits*/
    for (i = 0; i < (ctx->ivsize >> 5); i++) {
        ctx->iv[i] = U32TO32_LITTLE(((uint32_t *)iv)[i]);
    }

    for (; i < 8; i++) {
        ctx->iv[i] = ctx->iv[i - 4];
    }

    /* Expand the key and IV into the table T */
    for (i = 0; i < 8;  i++) {
        ctx->T[i] = ctx->key[i];
    }

    for (i = 8; i < 16; i++) {
        ctx->T[i] = ctx->iv[i - 8];
    }

    for (i = 16; i < 528; i++) {
        ctx->T[i] = HC256_F2(ctx->T[i - 2]) + ctx->T[i - 7] + HC256_F1(ctx->T[i - 15]) + ctx->T[i - 16] + i;
    }

    for (i = 0; i < 16;  i++) {
        ctx->T[i] = ctx->T[512 + i];
    }

    for (i = 16; i < 2048; i++) {
        ctx->T[i] = HC256_F2(ctx->T[i - 2]) + ctx->T[i - 7] + HC256_F1(ctx->T[i - 15]) + ctx->T[i - 16] + 512 + i;
    }

    /* Initialize counter2048, X and Y */
    ctx->counter2048 = 0;

    for (i = 0; i < 16; i++) {
        ctx->X[i] = ctx->T[1008 + i];
    }

    for (i = 0; i < 16; i++) {
        ctx->Y[i] = ctx->T[1024 + 1008 + i];
    }

    /* Run the cipher 4096 steps before generating the output */
    for (i = 0; i < 256; i++) {
        setup_update(ctx);
    }
}

/* Encrypt or decrypt "msglen" number of message bytes */
void hc256_process_bytes(hc256_ctx *ctx, const uint8_t *input, uint8_t *output, uint32_t msglen)
{
    uint32_t i, keystream[16];

    for (; msglen >= 64; msglen -= 64, input += 64, output += 64) {
        generate_keystream(ctx, keystream);

        ((uint32_t *)output)[0]  = ((uint32_t *)input)[0]  ^ U32TO32_LITTLE(keystream[0]);
        ((uint32_t *)output)[1]  = ((uint32_t *)input)[1]  ^ U32TO32_LITTLE(keystream[1]);
        ((uint32_t *)output)[2]  = ((uint32_t *)input)[2]  ^ U32TO32_LITTLE(keystream[2]);
        ((uint32_t *)output)[3]  = ((uint32_t *)input)[3]  ^ U32TO32_LITTLE(keystream[3]);
        ((uint32_t *)output)[4]  = ((uint32_t *)input)[4]  ^ U32TO32_LITTLE(keystream[4]);
        ((uint32_t *)output)[5]  = ((uint32_t *)input)[5]  ^ U32TO32_LITTLE(keystream[5]);
        ((uint32_t *)output)[6]  = ((uint32_t *)input)[6]  ^ U32TO32_LITTLE(keystream[6]);
        ((uint32_t *)output)[7]  = ((uint32_t *)input)[7]  ^ U32TO32_LITTLE(keystream[7]);
        ((uint32_t *)output)[8]  = ((uint32_t *)input)[8]  ^ U32TO32_LITTLE(keystream[8]);
        ((uint32_t *)output)[9]  = ((uint32_t *)input)[9]  ^ U32TO32_LITTLE(keystream[9]);
        ((uint32_t *)output)[10] = ((uint32_t *)input)[10] ^ U32TO32_LITTLE(keystream[10]);
        ((uint32_t *)output)[11] = ((uint32_t *)input)[11] ^ U32TO32_LITTLE(keystream[11]);
        ((uint32_t *)output)[12] = ((uint32_t *)input)[12] ^ U32TO32_LITTLE(keystream[12]);
        ((uint32_t *)output)[13] = ((uint32_t *)input)[13] ^ U32TO32_LITTLE(keystream[13]);
        ((uint32_t *)output)[14] = ((uint32_t *)input)[14] ^ U32TO32_LITTLE(keystream[14]);
        ((uint32_t *)output)[15] = ((uint32_t *)input)[15] ^ U32TO32_LITTLE(keystream[15]);
    }

    if (msglen > 0) {
        generate_keystream(ctx, keystream);

        for (i = 0; i < msglen; i ++) {
            output[i] = input[i] ^ ((uint8_t *)keystream)[i];
        }
    }
}
/** @} */
