/*
 * Copyright (C) 2016 Andrew Moon (dedicated to the public domain)
 * Copyright Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_crypto_poly1305
 * @{
 * @file
 * @brief   Implementation of Poly1305. Based on Floodberry's and Loup
 *          Valliant's implementation. Optimized for small flash size.
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <string.h>
#include "crypto/poly1305.h"

static void poly1305_block(poly1305_ctx_t *ctx, uint8_t c4);

static uint32_t u8to32(const uint8_t *p)
{
    return
        ((uint32_t)p[0] |
         ((uint32_t)p[1] <<  8) |
         ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24));
}

static void u32to8(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v);
    p[1] = (uint8_t)(v >>  8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

static void _clear_c(poly1305_ctx_t *ctx)
{
    ctx->c[0] = 0;
    ctx->c[1] = 0;
    ctx->c[2] = 0;
    ctx->c[3] = 0;
    ctx->c_idx = 0;
}

static void poly1305_block(poly1305_ctx_t *ctx, uint8_t c4)
{
    /* Local copies */
    const uint32_t r0 = ctx->r[0];
    const uint32_t r1 = ctx->r[1];
    const uint32_t r2 = ctx->r[2];
    const uint32_t r3 = ctx->r[3];

    const uint32_t rr0 = (r0 >> 2) * 5;
    const uint32_t rr1 = (r1 >> 2) + r1;
    const uint32_t rr2 = (r2 >> 2) + r2;
    const uint32_t rr3 = (r3 >> 2) + r3;

    /* s = h + c, without carry propagation */
    const uint64_t s0 = ctx->h[0] + (uint64_t)ctx->c[0];
    const uint64_t s1 = ctx->h[1] + (uint64_t)ctx->c[1];
    const uint64_t s2 = ctx->h[2] + (uint64_t)ctx->c[2];
    const uint64_t s3 = ctx->h[3] + (uint64_t)ctx->c[3];
    const uint32_t s4 = ctx->h[4] + c4;

    /* (h + c) * r, without carry propagation */
    const uint64_t x0 = s0 * r0 + s1 * rr3 + s2 * rr2 + s3 * rr1 + s4 * rr0;
    const uint64_t x1 = s0 * r1 + s1 * r0  + s2 * rr3 + s3 * rr2 + s4 * rr1;
    const uint64_t x2 = s0 * r2 + s1 * r1  + s2 * r0  + s3 * rr3 + s4 * rr2;
    const uint64_t x3 = s0 * r3 + s1 * r2  + s2 * r1  + s3 * r0  + s4 * rr3;
    const uint32_t x4 = s4 * (r0 & 3);

    /* partial reduction modulo 2^130 - 5 */
    const uint32_t u5 = x4 + (x3 >> 32); // u5 <= 7ffffff5
    const uint64_t u0 = (u5 >>  2) * 5 + (x0 & 0xffffffff);
    const uint64_t u1 = (u0 >> 32)     + (x1 & 0xffffffff) + (x0 >> 32);
    const uint64_t u2 = (u1 >> 32)     + (x2 & 0xffffffff) + (x1 >> 32);
    const uint64_t u3 = (u2 >> 32)     + (x3 & 0xffffffff) + (x2 >> 32);
    const uint64_t u4 = (u3 >> 32)     + (u5 & 3);

    /* Update the hash */
    ctx->h[0] = (uint32_t)u0;
    ctx->h[1] = (uint32_t)u1;
    ctx->h[2] = (uint32_t)u2;
    ctx->h[3] = (uint32_t)u3;
    ctx->h[4] = (uint32_t)u4;
}

static void _take_input(poly1305_ctx_t *ctx, uint8_t input)
{
    size_t word = ctx->c_idx >> 2;
    size_t byte = ctx->c_idx & 3;

    ctx->c[word] |= (uint32_t)input << (byte * 8);
    ctx->c_idx++;
}

void poly1305_update(poly1305_ctx_t *ctx, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        _take_input(ctx, data[i]);
        if (ctx->c_idx == 16) {
            poly1305_block(ctx, 1);
            _clear_c(ctx);
        }
    }
}

void poly1305_init(poly1305_ctx_t *ctx, const uint8_t *key)
{
    /* load and clamp key */
    ctx->r[0] = u8to32(key) & 0x0fffffff;
    for (size_t i = 1; i < 4; i++) {
        ctx->r[i] = u8to32(&key[4 * i]) & 0x0ffffffc;
    }
    for (size_t i = 0; i < 4; i++) {
        ctx->pad[i] = u8to32(&key[16 + i * 4]);
    }

    /* Zero the hash */
    memset(ctx->h, 0, sizeof(ctx->h));
    _clear_c(ctx);
}

void poly1305_finish(poly1305_ctx_t *ctx, uint8_t *mac)
{
    /* Process the last block if there is data remaining */
    if (ctx->c_idx) {
        /* move the final 1 according to remaining input length */
        /* (We may add less than 2^130 to the last input block) */
        _take_input(ctx, 1);
        /* And update hash */
        poly1305_block(ctx, 0);
    }

    /* check if we should subtract 2^130-5 by performing the
     * corresponding carry propagation. */
    const uint64_t u0 = (uint64_t)5 + ctx->h[0];    // <= 1_00000004
    const uint64_t u1 = (u0 >> 32)  + ctx->h[1];    // <= 1_00000000
    const uint64_t u2 = (u1 >> 32)  + ctx->h[2];    // <= 1_00000000
    const uint64_t u3 = (u2 >> 32)  + ctx->h[3];    // <= 1_00000000
    const uint64_t u4 = (u3 >> 32)  + ctx->h[4];    // <=          5
    /* u4 indicates how many times we should subtract 2^130-5 (0 or 1) */

    /* h + pad, minus 2^130-5 if u4 exceeds 3 */
    const uint64_t uu0 = (u4 >> 2) * 5 + ctx->h[0] + ctx->pad[0];
    u32to8(mac, uu0);

    const uint64_t uu1 = (uu0 >> 32)   + ctx->h[1] + ctx->pad[1];
    u32to8(mac + 4, uu1);

    const uint64_t uu2 = (uu1 >> 32)   + ctx->h[2] + ctx->pad[2];
    u32to8(mac + 8, uu2);

    const uint64_t uu3 = (uu2 >> 32)   + ctx->h[3] + ctx->pad[3];
    u32to8(mac + 12, uu3);

}

void poly1305_auth(uint8_t *mac, const uint8_t *data, size_t len,
                   const uint8_t *key)
{
    poly1305_ctx_t ctx;

    poly1305_init(&ctx, key);
    poly1305_update(&ctx, data, len);
    poly1305_finish(&ctx, mac);
}
