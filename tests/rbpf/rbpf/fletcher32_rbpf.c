/*
 * Copyright 2015 Eistec AB
 * Copyright 2021 Koen Zandberg <koen@bergzand.net>
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
 * @brief       Fletcher32 implementation for rBPF
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * Implementation is adapted from our own in-tree application
 *
 * @}
 */

#include <stddef.h>
#include "rbpf/shared.h"
#include "unaligned.h"

typedef struct {
    __bpf_shared_ptr(const uint16_t *, data);
    uint32_t words;
} fletcher32_ctx_t;

uint32_t fletcher32(fletcher32_ctx_t *ctx)
{
    uint32_t words = ctx->words;
    const uint16_t *data = ctx->data;

    uint32_t sum1 = 0xffff, sum2 = 0xffff;

    while (words) {
        unsigned tlen = words > 359 ? 359 : words;
        words -= tlen;
        do {
            sum2 += sum1 += unaligned_get_u16(data++);
        } while (--tlen);
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }
    /* Second reduction step to reduce sums to 16 bits */
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return (sum2 << 16) | sum1;
}
