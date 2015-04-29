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

#include "net/ng_netif.h"

static int _dehex(char c)
{
    return ('0' <= c && c <= '9') ? c - '0'
         : ('A' <= c && c <= 'F') ? c - 'A' + 10
         : ('a' <= c && c <= 'f') ? c - 'a' + 10
         : -1;
}

size_t ng_netif_addr_from_str(uint8_t *out, size_t out_len, const char *str)
{
    size_t len = 0;
    while (1) {
        ++len;
        if (len > out_len) {
            return 0;
        }

        int a = _dehex(*str++);
        if (a < 0) {
            return 0;
        }

        int b = _dehex(*str++);
        if (b < 0) {
            return 0;
        }

        *out++ = (a << 4) | b;

        if (*str == ':') {
            ++str;
        }
        else if (*str == '\0') {
            break;
        }
        else {
            return 0;
        }
    }

    return len;
}

/** @} */
