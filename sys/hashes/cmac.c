/*
 * Copyright (C) 2016 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_hashes_cmac
 * @{
 *
 * @file
 * @brief       AES_CMAC implementation
 *
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "crypto/ciphers.h"
#include "hashes/cmac.h"

#define MIN(a, b) a < b ? a : b

static void _xor128(uint8_t *x, uint8_t *y)
{
    for (unsigned i = 0; i < 16; i++) {
        y[i] = x[i] ^ y[i];
    }
}

static void _leftshift(uint8_t *x, uint8_t *y)
{
    for (unsigned i = 0; i < 15; i++) {
        y[i] = (x[i] << 1) | (x[i + 1] >> 7);
    }
    y[15] = x[15] << 1;
}

int cmac_init(cmac_context_t *ctx, const uint8_t *key, uint8_t key_size)
{
    if (key_size != CMAC_BLOCK_SIZE) {
        return CIPHER_ERR_INVALID_KEY_SIZE;
    }

    memset(ctx, 0, sizeof(cmac_context_t));
    return cipher_init(&(ctx->aes_ctx), CIPHER_AES, key, key_size);
}

void cmac_update(cmac_context_t *ctx, const void *data, size_t len)
{
    uint8_t d[16];

    while (len) {
        uint8_t c;
        if (ctx->M_n == 16) {
            ctx->M_n = 0;
            _xor128(ctx->M_last, ctx->X);
            cipher_encrypt(&ctx->aes_ctx, ctx->X, d);
            memcpy(ctx->X, d, CMAC_BLOCK_SIZE);
        }
        c = MIN(CMAC_BLOCK_SIZE - ctx->M_n, len);
        memcpy(ctx->M_last + ctx->M_n, data, c);
        ctx->M_n += c;
        len -= c;
        data = (void *) (((uint8_t *) data) + c);

        if (ctx->M_n < CMAC_BLOCK_SIZE) {
            break;
        }
    }
}

void cmac_final(cmac_context_t *ctx, void *digest)
{
    /* Generate subkeys */
    uint8_t K[CMAC_BLOCK_SIZE];
    uint8_t L[CMAC_BLOCK_SIZE];

    memset(K, 0, CMAC_BLOCK_SIZE);
    cipher_encrypt(&ctx->aes_ctx, K, L);

    if (L[0] & 0x80) {
        _leftshift(L, K);
        K[15] ^= 0x87;
    }
    else {
        _leftshift(L, K);
    }

    if (ctx->M_n != 16) {
        /* Generate K2 */
        if (K[0] & 0x80) {
            _leftshift(K, K);
            K[15] ^= 0x87;
        }
        else {
            _leftshift(K, K);
        }
        /* Padding */
        memset(ctx->M_last + ctx->M_n, 0, CMAC_BLOCK_SIZE - ctx->M_n);
        ctx->M_last[ctx->M_n] = 0x80;
    }
    _xor128(K, ctx->M_last);
    _xor128(ctx->M_last, ctx->X);
    cipher_encrypt(&ctx->aes_ctx, ctx->X, L);
    memcpy(digest, L, CMAC_BLOCK_SIZE);
}
