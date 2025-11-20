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

/* Use the same code for 32 bit and 64 bit sum */
#define _BCD_CONVERT(sum, len)                  \
    for (int i = len * 2 - 1; i >= 0; --i) {    \
        uint8_t digit = i & 1                   \
                      ? bcd[i >> 1] >> 4        \
                      : bcd[i >> 1] & 0xF;      \
        sum = sum * 10 + digit;                 \
    }

uint32_t bcd_buf_to_u32(const void *src, size_t len)
{
    const uint8_t *bcd = src;
    uint32_t sum = 0;

    _BCD_CONVERT(sum, len);

    return sum;
}

uint64_t bcd_buf_to_u64(const void *src, size_t len)
{
    const uint8_t *bcd = src;
    uint64_t sum = 0;

    _BCD_CONVERT(sum, len);

    return sum;
}

static bool _is_digit(char c)
{
    return c >= '0' && c <= '9';
}

int bcd_buf_from_str(const char *str, size_t len, void *dst, size_t dst_len)
{
    uint8_t *bcd = dst;
    memset(dst, 0, dst_len);
    dst_len *= 2;

    uint8_t i = 0;
    for (int j = len; j >= 0; --j) {
        if (i == dst_len) {
            return -ENOBUFS;
        }
        if (!_is_digit(str[j])) {
            continue;
        }
        uint8_t d = str[j] & 0xF;
        bcd[i >> 1] |= i & 1
                    ? d << 4
                    : d;
        ++i;
    }

    return (i & 1) + (i >> 1);
}
