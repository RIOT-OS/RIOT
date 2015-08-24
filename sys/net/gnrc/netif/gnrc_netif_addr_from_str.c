/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2015 René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include "net/gnrc/netif.h"

static inline int _dehex(char c, int default_)
{
    if ('0' <= c && c <= '9') {
        return c - '0';
    }
    else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    }
    else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    }
    else {
        return default_;
    }
}

size_t gnrc_netif_addr_from_str(uint8_t *out, size_t out_len, const char *str)
{
    /* Walk over str from the end. */
    /* Take two chars a time as one hex value (%hhx). */
    /* Leading zeros can be omitted. */
    /* Every non-hexadimal character is a delimiter. */
    /* Leading, tailing and adjacent delimiters are forbidden. */

    const char *end_str = str;
    uint8_t *out_end = out;
    size_t count = 0;
    int assert_cell = 1;

    if (!str || !*str) {
        return 0;
    }
    while (end_str[1]) {
        ++end_str;
    }

    while (end_str >= str) {
        int a = 0, b = _dehex(*end_str--, -1);
        if (b < 0) {
            if (assert_cell) {
                return 0;
            }
            else {
                assert_cell = 1;
                continue;
            }
        }
        assert_cell = 0;

        if (end_str >= str) {
            a = _dehex(*end_str--, 0);
        }

        if (++count > out_len) {
            return 0;
        }
        *out_end++ = (a << 4) | b;
    }
    if (assert_cell) {
        return 0;
    }
    /* out is reversed */

    while (out < --out_end) {
        uint8_t tmp = *out_end;
        *out_end = *out;
        *out++ = tmp;
    }

    return count;
}

/** @} */
