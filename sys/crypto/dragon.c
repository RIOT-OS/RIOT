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
 * @brief       Implementation of the Dragon stream cipher
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Kevin Chen
 * @author      Matt Henricksen
 * @author      William Millan
 * @author      Joanne Fuller
 * @author      Leonie Simpson
 * @author      Ed Dawson
 * @author      HoonJae Lee
 * @author      SangJae Moon
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "crypto/dragon.h"

/* Dragon Key setup */
void dragon_keysetup(dragon_ctx *ctx, const uint8_t *key, uint32_t keysize)
{
    /* Supported key and IV sizes: * (128, 128) and (256, 256) bits */
    ctx->key_size = keysize;

    if (keysize == 128) {
        ctx->nlfsr_word[0 + 0]  = ctx->nlfsr_word[12 + 0] = ctx->nlfsr_word[20 + 0]
         = U8TO32_BIG( key + 0 * 4);
        ctx->nlfsr_word[0 + 1]  = ctx->nlfsr_word[12 + 1] = ctx->nlfsr_word[20 + 1]
         = U8TO32_BIG( key + 1 * 4);
        ctx->nlfsr_word[0 + 2]  = ctx->nlfsr_word[12 + 2] = ctx->nlfsr_word[20 + 2]
         = U8TO32_BIG( key + 2 * 4);
        ctx->nlfsr_word[0 + 3]  = ctx->nlfsr_word[12 + 3] = ctx->nlfsr_word[20 + 3]
         = U8TO32_BIG( key + 3 * 4);

        ctx->nlfsr_word[4 + 0] = ctx->nlfsr_word[16 + 0] = ctx->nlfsr_word[28 + 0]
         = U8TO32_BIG( key + 8 + 0 * 4);
        ctx->nlfsr_word[6 + 0] = ctx->nlfsr_word[18 + 0] = ctx->nlfsr_word[30 + 0]
         = U8TO32_BIG( key + 0 * 4);
        ctx->nlfsr_word[4 + 1] = ctx->nlfsr_word[16 + 1] = ctx->nlfsr_word[28 + 1]
         = U8TO32_BIG( key + 8 + 1 * 4);
        ctx->nlfsr_word[6 + 1] = ctx->nlfsr_word[18 + 1] = ctx->nlfsr_word[30 + 1]
         = U8TO32_BIG( key + 1 * 4);
    }
    else {
        ctx->nlfsr_word[0 + 0]  = ctx->nlfsr_word[8 + 0] = ctx->nlfsr_word[16 + 0]
         = U8TO32_BIG( key + 0 * 4);
        ctx->nlfsr_word[0 + 1]  = ctx->nlfsr_word[8 + 1] = ctx->nlfsr_word[16 + 1]
          = U8TO32_BIG( key + 1 * 4);
        ctx->nlfsr_word[0 + 2]  = ctx->nlfsr_word[8 + 2] = ctx->nlfsr_word[16 + 2]
         = U8TO32_BIG( key + 2 * 4);
        ctx->nlfsr_word[0 + 3]  = ctx->nlfsr_word[8 + 3] = ctx->nlfsr_word[16 + 3]
         = U8TO32_BIG( key + 3 * 4);
        ctx->nlfsr_word[0 + 4]  = ctx->nlfsr_word[8 + 4] = ctx->nlfsr_word[16 + 4]
         = U8TO32_BIG( key + 4 * 4);
        ctx->nlfsr_word[0 + 5]  = ctx->nlfsr_word[8 + 5] = ctx->nlfsr_word[16 + 5]
         = U8TO32_BIG( key + 5 * 4);
        ctx->nlfsr_word[0 + 6]  = ctx->nlfsr_word[8 + 6] = ctx->nlfsr_word[16 + 6]
         = U8TO32_BIG( key + 6 * 4);
        ctx->nlfsr_word[0 + 7]  = ctx->nlfsr_word[8 + 7] = ctx->nlfsr_word[16 + 7]
         = U8TO32_BIG( key + 7 * 4);
    }

    /* Preserve the state for the key-IV-IV */
    ctx->init_state[0] = ctx->nlfsr_word[0];
    ctx->init_state[1] = ctx->nlfsr_word[1];
    ctx->init_state[2] = ctx->nlfsr_word[2];
    ctx->init_state[3] = ctx->nlfsr_word[3];
    ctx->init_state[4] = ctx->nlfsr_word[4];
    ctx->init_state[5] = ctx->nlfsr_word[5];
    ctx->init_state[6] = ctx->nlfsr_word[6];
    ctx->init_state[7] = ctx->nlfsr_word[7];
    ctx->init_state[8] = ctx->nlfsr_word[8];
    ctx->init_state[9] = ctx->nlfsr_word[9];
    ctx->init_state[10] = ctx->nlfsr_word[10];
    ctx->init_state[11] = ctx->nlfsr_word[11];
    ctx->init_state[12] = ctx->nlfsr_word[12];
    ctx->init_state[13] = ctx->nlfsr_word[13];
    ctx->init_state[14] = ctx->nlfsr_word[14];
    ctx->init_state[15] = ctx->nlfsr_word[15];
    ctx->init_state[16] = ctx->nlfsr_word[16];
    ctx->init_state[17] = ctx->nlfsr_word[17];
    ctx->init_state[18] = ctx->nlfsr_word[18];
    ctx->init_state[19] = ctx->nlfsr_word[19];
    ctx->init_state[20] = ctx->nlfsr_word[20];
    ctx->init_state[21] = ctx->nlfsr_word[21];
    ctx->init_state[22] = ctx->nlfsr_word[22];
    ctx->init_state[23] = ctx->nlfsr_word[23];
    ctx->init_state[24] = ctx->nlfsr_word[24];
    ctx->init_state[25] = ctx->nlfsr_word[25];
    ctx->init_state[26] = ctx->nlfsr_word[26];
    ctx->init_state[27] = ctx->nlfsr_word[27];
    ctx->init_state[28] = ctx->nlfsr_word[28];
    ctx->init_state[29] = ctx->nlfsr_word[29];
    ctx->init_state[30] = ctx->nlfsr_word[30];
    ctx->init_state[31] = ctx->nlfsr_word[31];
}

/* Dragon IV setup */
void dragon_ivsetup(dragon_ctx *ctx, const uint8_t *iv)
{
    uint32_t a, b, c, d;
    uint32_t e = 0x00004472;
    uint32_t f = 0x61676F6E;
    uint32_t iv_word;
    uint32_t idx;

    /* Restore post-keysetup state */
    ctx->nlfsr_word[0] = ctx->init_state[0];
    ctx->nlfsr_word[1] = ctx->init_state[1];
    ctx->nlfsr_word[2] = ctx->init_state[2];
    ctx->nlfsr_word[3] = ctx->init_state[3];
    ctx->nlfsr_word[4] = ctx->init_state[4];
    ctx->nlfsr_word[5] = ctx->init_state[5];
    ctx->nlfsr_word[6] = ctx->init_state[6];
    ctx->nlfsr_word[7] = ctx->init_state[7];
    ctx->nlfsr_word[8] = ctx->init_state[8];
    ctx->nlfsr_word[9] = ctx->init_state[9];
    ctx->nlfsr_word[10] = ctx->init_state[10];
    ctx->nlfsr_word[11] = ctx->init_state[11];
    ctx->nlfsr_word[12] = ctx->init_state[12];
    ctx->nlfsr_word[13] = ctx->init_state[13];
    ctx->nlfsr_word[14] = ctx->init_state[14];
    ctx->nlfsr_word[15] = ctx->init_state[15];
    ctx->nlfsr_word[16] = ctx->init_state[16];
    ctx->nlfsr_word[17] = ctx->init_state[17];
    ctx->nlfsr_word[18] = ctx->init_state[18];
    ctx->nlfsr_word[19] = ctx->init_state[19];
    ctx->nlfsr_word[20] = ctx->init_state[20];
    ctx->nlfsr_word[21] = ctx->init_state[21];
    ctx->nlfsr_word[22] = ctx->init_state[22];
    ctx->nlfsr_word[23] = ctx->init_state[23];
    ctx->nlfsr_word[24] = ctx->init_state[24];
    ctx->nlfsr_word[25] = ctx->init_state[25];
    ctx->nlfsr_word[26] = ctx->init_state[26];
    ctx->nlfsr_word[27] = ctx->init_state[27];
    ctx->nlfsr_word[28] = ctx->init_state[28];
    ctx->nlfsr_word[29] = ctx->init_state[29];
    ctx->nlfsr_word[30] = ctx->init_state[30];
    ctx->nlfsr_word[31] = ctx->init_state[31];

    if (ctx->key_size == 128) {
        /* Write IV first */
        iv_word = U8TO32_BIG(iv + 0 * 4);
        ctx->nlfsr_word[8 + 0]   = iv_word;
        ctx->nlfsr_word[20 + 0] ^= iv_word;
        ctx->nlfsr_word[28 + 0] ^= iv_word;

        iv_word = U8TO32_BIG(iv + 1 * 4);
        ctx->nlfsr_word[8 + 1]   = iv_word;
        ctx->nlfsr_word[20 + 1] ^= iv_word;
        ctx->nlfsr_word[28 + 1] ^= iv_word;

        iv_word = U8TO32_BIG(iv + 2 * 4);
        ctx->nlfsr_word[8 + 2]   = iv_word;
        ctx->nlfsr_word[20 + 2] ^= iv_word;
        ctx->nlfsr_word[28 + 2] ^= iv_word;

        iv_word = U8TO32_BIG(iv + 3 * 4);
        ctx->nlfsr_word[8 + 3]   = iv_word;
        ctx->nlfsr_word[20 + 3] ^= iv_word;
        ctx->nlfsr_word[28 + 3] ^= iv_word;

        /* Then IV' */
        iv_word = U8TO32_BIG(iv + 8 + 0 * 4);
        ctx->nlfsr_word[ 4 + 0] ^= iv_word;
        ctx->nlfsr_word[12 + 0] ^= iv_word;
        ctx->nlfsr_word[24 + 0]  = iv_word;

        iv_word = U8TO32_BIG(iv + 0 * 4);
        ctx->nlfsr_word[ 6 + 0] ^= iv_word;
        ctx->nlfsr_word[14 + 0] ^= iv_word;
        ctx->nlfsr_word[26 + 0]  = iv_word;

        iv_word = U8TO32_BIG(iv + 8 + 1 * 4);
        ctx->nlfsr_word[ 4 + 1] ^= iv_word;
        ctx->nlfsr_word[12 + 1] ^= iv_word;
        ctx->nlfsr_word[24 + 1]  = iv_word;

        iv_word = U8TO32_BIG(iv + 1 * 4);
        ctx->nlfsr_word[ 6 + 1] ^= iv_word;
        ctx->nlfsr_word[14 + 1] ^= iv_word;
        ctx->nlfsr_word[26 + 1]  = iv_word;
    }
    else {
        iv_word = U8TO32_BIG(iv + 0 * 4);
        ctx->nlfsr_word[ 8 + 0] ^= iv_word;
        ctx->nlfsr_word[16 + 0] ^= (iv_word ^ 0xFFFFFFFF);
        ctx->nlfsr_word[24 + 0]  = iv_word;
        iv_word = U8TO32_BIG(iv + 1 * 4);
        ctx->nlfsr_word[ 8 + 1] ^= iv_word;
        ctx->nlfsr_word[16 + 1] ^= (iv_word ^ 0xFFFFFFFF);
        ctx->nlfsr_word[24 + 1]  = iv_word;
        iv_word = U8TO32_BIG(iv + 2 * 4);
        ctx->nlfsr_word[ 8 + 2] ^= iv_word;
        ctx->nlfsr_word[16 + 2] ^= (iv_word ^ 0xFFFFFFFF);
        ctx->nlfsr_word[24 + 2]  = iv_word;
        iv_word = U8TO32_BIG(iv + 3 * 4);
        ctx->nlfsr_word[ 8 + 3] ^= iv_word;
        ctx->nlfsr_word[16 + 3] ^= (iv_word ^ 0xFFFFFFFF);
        ctx->nlfsr_word[24 + 3]  = iv_word;
        iv_word = U8TO32_BIG(iv + 4 * 4);
        ctx->nlfsr_word[ 8 + 4] ^= iv_word;
        ctx->nlfsr_word[16 + 4] ^= (iv_word ^ 0xFFFFFFFF);
        ctx->nlfsr_word[24 + 4]  = iv_word;
        iv_word = U8TO32_BIG(iv + 5 * 4);
        ctx->nlfsr_word[ 8 + 5] ^= iv_word;
        ctx->nlfsr_word[16 + 5] ^= (iv_word ^ 0xFFFFFFFF);
        ctx->nlfsr_word[24 + 5]  = iv_word;
        iv_word = U8TO32_BIG(iv + 6 * 4);
        ctx->nlfsr_word[ 8 + 6] ^= iv_word;
        ctx->nlfsr_word[16 + 6] ^= (iv_word ^ 0xFFFFFFFF);
        ctx->nlfsr_word[24 + 6]  = iv_word;
        iv_word = U8TO32_BIG(iv + 7 * 4);
        ctx->nlfsr_word[ 8 + 7] ^= iv_word;
        ctx->nlfsr_word[16 + 7] ^= (iv_word ^ 0xFFFFFFFF);
        ctx->nlfsr_word[24 + 7]  = iv_word;
    }

    ctx->nlfsr_offset = 0;

    /* Iterate mixing process */
    for (idx = 0; idx < DRAGON_MIXING_STAGES; idx++) {
        a = DRAGON_NLFSR_WORD(ctx, 0)  ^
            DRAGON_NLFSR_WORD(ctx, 24) ^
            DRAGON_NLFSR_WORD(ctx, 28);

        b = DRAGON_NLFSR_WORD(ctx, 1)  ^
            DRAGON_NLFSR_WORD(ctx, 25) ^
            DRAGON_NLFSR_WORD(ctx, 29);

        c = DRAGON_NLFSR_WORD(ctx, 2)  ^
            DRAGON_NLFSR_WORD(ctx, 26) ^
            DRAGON_NLFSR_WORD(ctx, 30);

        d = DRAGON_NLFSR_WORD(ctx, 3)  ^
            DRAGON_NLFSR_WORD(ctx, 27) ^
            DRAGON_NLFSR_WORD(ctx, 31);

        DRAGON_UPDATE(a, b, c, d, e, f);

        ctx->nlfsr_offset += (DRAGON_NLFSR_SIZE - 4);

        DRAGON_NLFSR_WORD(ctx, 0) = a ^ DRAGON_NLFSR_WORD(ctx, 20);
        DRAGON_NLFSR_WORD(ctx, 1) = b ^ DRAGON_NLFSR_WORD(ctx, 21);
        DRAGON_NLFSR_WORD(ctx, 2) = c ^ DRAGON_NLFSR_WORD(ctx, 22);
        DRAGON_NLFSR_WORD(ctx, 3) = d ^ DRAGON_NLFSR_WORD(ctx, 23);
    }

    ctx->state_counter = ((uint64_t)e << 32) | (uint64_t)f;

    /* Reset buffer index */
    ctx->buffer_index = 0;
}

/* Encrypt or decrypt a "blocks" number of blocks */
void dragon_process_blocks(dragon_ctx *ctx, const uint8_t *input, uint8_t *output, uint32_t blocks)
{
    uint32_t a, b, c, d, e, f;

    while (blocks > 0) {
        DRAGON_ROUND(ctx, a, b, c, d, e, f)
        *(output++) = U32TO32_BIG(a) ^ *(input++);
        *(output++) = U32TO32_BIG(e) ^ *(input++);
        blocks--;
    }
}

/* Generate "blocks" number of keystream blocks */
void dragon_keystream_blocks(dragon_ctx *ctx, uint8_t *keystream, uint32_t blocks)
{
    uint32_t *k_ptr = (uint32_t *)keystream;
    uint32_t a, b, c, d, e, f;

    while (blocks > 0) {
        DRAGON_ROUND(ctx, a, b, c, d, e, f)
        *(k_ptr++) = U32TO32_BIG(a);
        *(k_ptr++) = U32TO32_BIG(e);
        blocks--;
    }
}

/* Generate "length" number of keystream bytes */
void dragon_keystream_bytes(dragon_ctx *ctx, uint8_t *keystream, uint32_t length)
{
    while ((length--) > 0) {
        if (ctx->buffer_index == 0) {
            dragon_keystream_blocks(ctx, ctx->keystream_buffer, DRAGON_BUFFER_SIZE);
        }

        *(keystream++) = ctx->keystream_buffer[ctx->buffer_index];
        ctx->buffer_index = ((ctx->buffer_index + 1) % DRAGON_BUFFER_BYTES);
    }
}

/* Encrypt or decrypt "msglen" number of message bytes */
void dragon_process_bytes(dragon_ctx *ctx, const uint8_t *input, uint8_t *output, uint32_t msglen)
{
    uint32_t i;

    dragon_keystream_bytes(ctx, output, msglen);

    for (i = 0; i < msglen; i++) {
        output[i] ^= input[i];
    }
}
/** @} */
