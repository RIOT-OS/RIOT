/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_bcd
 * @{
 *
 * @file
 * @brief       Library to de- and encode binary coded decimals
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "bcd.h"
#include <errno.h>
#include <string.h>

static inline uint8_t swap_nibbles(uint8_t b)
{
    return (b << 4) | (b >> 4);
}

int bcd_buf_from_u32(uint32_t val, void *dst, size_t len)
{
    uint8_t *tgt = dst;
    uint8_t hex = 0;
    uint8_t idx = 0;

    memset(dst, 0, len);
    len *= 2;

    do {
        hex <<= 4;
        hex += val % 10;
        val /= 10;
        if (++idx % 2 == 0) {
            *tgt++ = swap_nibbles(hex);
            hex = 0;
        }
    } while (val && idx <= len);

    if (idx > len) {
        return -ENOBUFS;
    }

    if (idx % 2) {
        *tgt++ = hex;
    }

    return (uintptr_t)tgt - (uintptr_t)dst;
}
