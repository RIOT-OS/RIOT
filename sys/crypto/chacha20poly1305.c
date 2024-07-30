/*
 * Copyright (C) 2008  D. J. Bernstein  (dedicated to the public domain)
 * Copyright (C) 2015  René Kijewski  <rene.kijewski@fu-berlin.de>
 * Copyright (C) 2018  Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_crypto_chacha20poly1305
 * @{
 * @file
 * @brief   Implementation of the chacha20poly1305 aead cipher
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @see     https://tools.ietf.org/html/rfc8439
 * @}
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "crypto/helper.h"
#include "crypto/chacha20poly1305.h"
#include "crypto/poly1305.h"
#include "unaligned.h"

/* Missing operations to convert numbers to little endian prevents this from
 * working on big endian systems */
#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#   error "This code is implementented in a way that it will only work for little-endian systems!"
#endif

/* Nothing to hide here, Literally "expand 32-byte k" */
static const uint32_t constant[] = {0x61707865,
                                    0x3320646e,
                                    0x79622d32,
                                    0x6b206574};

/* Padding to add to the poly1305 authentication tag */
static const uint8_t padding[15] = {0};

/**
 * @brief   Calculates a single round of the ChaCha20 block algorithm on a 32b word.
 */
static void _r(uint32_t *a, uint32_t *b, uint32_t *d, unsigned c)
{
    *a += *b;
    uint32_t tmp = *a ^ *d;
    *d = (tmp << c) | (tmp >> (32 - c));
}

/**
 * @brief   Initialization function, that initializes the ChaCha20 state with
 *          the given key, nonce and counter
 */
static void _create_initial(chacha20_ctx_t *ctx, const uint8_t *key,
                         const uint8_t *nonce, uint32_t blk)
{
    for (unsigned i = 0; i < 4; i++) {
        ctx->split.constant[i] = constant[i];
    }
    for (unsigned i = 0; i < 8; i++) {
        ctx->split.key[i] = unaligned_get_u32(key + 4*i);
    }
    ctx->split.counter[0] = unaligned_get_u32((uint8_t*)&blk);
    ctx->split.nonce[0] = unaligned_get_u32(nonce);
    ctx->split.nonce[1] = unaligned_get_u32(nonce+4);
    ctx->split.nonce[2] = unaligned_get_u32(nonce+8);
}

/**
 * @brief   Does 32bit uint addition of the initial state to the key_stream buffer.
 */
static void _add_initial(chacha20_ctx_t *ctx, uint32_t *key_stream)
{
    for (unsigned i = 0; i < 16; i++) {
        key_stream[i] += ctx->words[i];
    }
}

/**
 * @brief   Calculates the ChaCha20 Block function with the given state/context
 *          and returns it in the key_stream buffer (64B).
 */
static void _keystream(chacha20_ctx_t *ctx, uint32_t *key_stream)
{
    /* Initialize block state */
    memset(key_stream, 0, 16 * 4);
    _add_initial(ctx, key_stream);

    /* perform rounds */
    for (unsigned i = 0; i < 80; ++i) {
        uint32_t *a = &key_stream[((i                    ) & 3)          ];
        uint32_t *b = &key_stream[((i + ((i & 4) ? 1 : 0)) & 3) + (4 * 1)];
        uint32_t *c = &key_stream[((i + ((i & 4) ? 2 : 0)) & 3) + (4 * 2)];
        uint32_t *d = &key_stream[((i + ((i & 4) ? 3 : 0)) & 3) + (4 * 3)];
        _r(a, b, d, 16);
        _r(c, d, b, 12);
        _r(a, b, d, 8);
        _r(c, d, b, 7);
    }
    /* add initial state */
    _add_initial(ctx, key_stream);
}

static void _poly1305_padded(poly1305_ctx_t *pctx, const uint8_t *data, size_t len)
{
    poly1305_update(pctx, data, len);
    const size_t padlen = (16 - len) & 0xF;
    poly1305_update(pctx, padding, padlen);
}

/* Generate a poly1305 tag */
static void _poly1305_gentag(uint8_t *mac, const uint8_t *key, const uint8_t *nonce,
                             const uint8_t *cipher, size_t cipherlen,
                             const uint8_t *aad, size_t aadlen)
{
    chacha20poly1305_ctx_t ctx;
    /* generate one time key */
    _create_initial(&ctx.chacha20, key, nonce, 0);
    uint32_t key_stream[16];
    _keystream(&ctx.chacha20, key_stream);
    poly1305_init(&ctx.poly, (uint8_t*)key_stream);
    /* Add aad */
    _poly1305_padded(&ctx.poly, aad, aadlen);
    /* Add ciphertext */
    _poly1305_padded(&ctx.poly, cipher, cipherlen);
    /* Add aad length */
    const uint64_t lengths[2] = {aadlen, cipherlen};
    poly1305_update(&ctx.poly, (uint8_t*)lengths, sizeof(lengths));
    poly1305_finish(&ctx.poly, mac);
    crypto_secure_wipe(&ctx, sizeof(ctx));
}

void chacha20poly1305_encrypt(uint8_t *cipher, const uint8_t *msg,
                              size_t msglen, const uint8_t *aad, size_t aadlen,
                              const uint8_t *key, const uint8_t *nonce)
{

    chacha20_encrypt_decrypt(key, nonce, 1, msg, msglen, cipher);
    /* Generate tag */
    _poly1305_gentag(&cipher[msglen], key, nonce,
                    cipher, msglen, aad, aadlen);
    /* Wipe structures */
}

int chacha20poly1305_decrypt(const uint8_t *cipher, size_t cipherlen,
                             uint8_t *msg, size_t *msglen,
                             const uint8_t *aad, size_t aadlen,
                             const uint8_t *key, const uint8_t *nonce)
{
    *msglen = cipherlen - CHACHA20POLY1305_TAG_BYTES;
    uint8_t mac[16];
    _poly1305_gentag(mac, key, nonce, cipher,
                     cipherlen - CHACHA20POLY1305_TAG_BYTES, aad, aadlen);
    if (crypto_equals(cipher+*msglen, mac, CHACHA20POLY1305_TAG_BYTES) == 0) {
        return 0;
    }
    chacha20_encrypt_decrypt(key, nonce, 1, cipher, cipherlen, msg);
    return 1;
}

void chacha20_encrypt_decrypt(  const uint8_t *key,
                                const uint8_t *nonce,
                                uint32_t counter,
                                const uint8_t *input,
                                size_t input_length,
                                uint8_t *output)
{
    chacha20_ctx_t ctx;
    chacha20_setup(&ctx, key, nonce, counter);
    const size_t num_blocks = input_length >> 6;
    size_t pos = 0;
    /* xcrypt full blocks */
    for (size_t i = 0; i < num_blocks; i++, pos += 64) {
        chacha20_update(&ctx, &input[pos], &output[pos]);
    }
    /* xcrypt remaining bytes */
    if (input_length - pos) {
        chacha20_finish(&ctx, &input[pos], input_length - pos, &output[pos]);
    }
}

void chacha20_setup(chacha20_ctx_t *ctx,
                    const uint8_t *key,
                    const uint8_t *nonce,
                    const uint32_t counter)
{
    _create_initial(ctx, key, nonce, counter);
}

void chacha20_update(   chacha20_ctx_t *ctx,
                        const uint8_t *input,
                        uint8_t *output)
{

    uint32_t key_stream[16];
    /* xcrypt full block */
    _keystream(ctx, key_stream);
    for (size_t j = 0; j < 64; j++) {
        output[j] = input[j] ^ ((uint8_t*)key_stream)[j];
    }
    ctx->split.counter[0]++;
}

void chacha20_finish(   chacha20_ctx_t *ctx,
                        const uint8_t *input,
                        size_t input_length,
                        uint8_t *output)
{
    uint32_t key_stream[16];
    _keystream(ctx, key_stream);
    for (size_t j = 0; j < input_length; j++) {
        output[j] = input[j] ^ ((uint8_t*)key_stream)[j];
    }
    crypto_secure_wipe(ctx, sizeof(*ctx));
}
