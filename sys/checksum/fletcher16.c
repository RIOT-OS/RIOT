/*
 * Copyright 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_checksum_fletcher16
 * @{
 *
 * @file
 * @brief       Fletcher16 implementation
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "checksum/fletcher16.h"

static inline void _reduce(fletcher16_ctx_t *ctx)
{
    ctx->sum1 = (ctx->sum1 & 0xff) + (ctx->sum1 >> 8);
    ctx->sum2 = (ctx->sum2 & 0xff) + (ctx->sum2 >> 8);
}

void fletcher16_init(fletcher16_ctx_t *ctx)
{
    ctx->sum1 = 0xff;
    ctx->sum2 = 0xff;
}

void fletcher16_update(fletcher16_ctx_t *ctx, const uint8_t *data, size_t len)
{
    while (len) {
        size_t tlen = len > 20 ? 20 : len;
        len -= tlen;
        do {
            ctx->sum2 += ctx->sum1 += *data++;
        } while (--tlen);
        _reduce(ctx);
    }
}

uint16_t fletcher16_finish(fletcher16_ctx_t *ctx)
{
    /* Second reduction step to reduce sums to 8 bits */
    _reduce(ctx);
    return (ctx->sum2 << 8) | ctx->sum1;
}

uint16_t fletcher16(const uint8_t *data, size_t bytes)
{
    fletcher16_ctx_t ctx;
    fletcher16_init(&ctx);
    fletcher16_update(&ctx, data, bytes);
    return fletcher16_finish(&ctx);
}
