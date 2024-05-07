/*
 * Copyright 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_checksum_fletcher32
 * @{
 *
 * @file
 * @brief       Fletcher32 implementation
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "unaligned.h"
#include "checksum/fletcher32.h"

static inline void _reduce(fletcher32_ctx_t *ctx)
{
    ctx->sum1 = (ctx->sum1 & 0xffff) + (ctx->sum1 >> 16);
    ctx->sum2 = (ctx->sum2 & 0xffff) + (ctx->sum2 >> 16);
}

void fletcher32_init(fletcher32_ctx_t *ctx)
{
    ctx->sum1 = 0xffff;
    ctx->sum2 = 0xffff;
}

uint32_t fletcher32_finish(fletcher32_ctx_t *ctx)
{
    /* Second reduction step to reduce sums to 8 bits */
    _reduce(ctx);
    return (ctx->sum2 << 16) | ctx->sum1;
}

void fletcher32_update(fletcher32_ctx_t *ctx, const void *data, size_t words)
{
    const uint16_t *u16_data = (const uint16_t*)data;
    while (words) {
        unsigned tlen = words > 359 ? 359 : words;
        words -= tlen;
        do {
            ctx->sum1 += unaligned_get_u16(u16_data++);
            ctx->sum2 += ctx->sum1;
        } while (--tlen);
        _reduce(ctx);
    }
}

uint32_t fletcher32(const uint16_t *data, size_t words)
{
    fletcher32_ctx_t ctx;
    fletcher32_init(&ctx);
    fletcher32_update(&ctx, data, words);
    return fletcher32_finish(&ctx);
}
