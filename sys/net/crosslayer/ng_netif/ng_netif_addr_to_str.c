/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
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

static inline char _half_byte_to_char(uint8_t half_byte)
{
    return (half_byte < 10) ? ('0' + half_byte) : ('a' + (half_byte - 10));
}

char *ng_netif_addr_to_str(char *out, size_t out_len, uint8_t *addr,
                           size_t addr_len)
{
    size_t i;

    if (out_len < (3 * addr_len)) { /* 2 for every byte, 1 for ':' or '\0' */
        return NULL;
    }

    for (i = 0; i < addr_len; i++) {
        out[(3 * i)] = _half_byte_to_char(addr[i] >> 4);
        out[(3 * i) + 1] = _half_byte_to_char(addr[i] & 0xf);

        if (i != (addr_len - 1)) {
            out[(3 * i) + 2] = ':';
        }
        else {
            out[(3 * i) + 2] = '\0';
        }
    }

    return out;
}

/** @} */
