/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdlib.h>
#include <string.h>

#include "net/ipv6/addr.h"

uint8_t ipv6_addr_match_prefix(const ipv6_addr_t *a, const ipv6_addr_t *b)
{
    uint8_t prefix_len = 0;

    if ((a == NULL) || (b == NULL)) {
        return 0;
    }

    if (a == b) {
        return 128;
    }

    for (int i = 0; i < 16; i++) {
        /* if bytes are equal add 8 */
        if (a->u8[i] == b->u8[i]) {
            prefix_len += 8;
        }
        else {
            uint8_t xor = (a->u8[i] ^ b->u8[i]);

            /* while bits from byte equal add 1 */
            for (int j = 0; j < 8; j++) {
                if ((xor & 0x80) == 0) {
                    prefix_len++;
                    xor = xor << 1;
                }
                else {
                    break;
                }
            }

            break;
        }
    }

    return prefix_len;
}

void ipv6_addr_init_prefix(ipv6_addr_t *out, const ipv6_addr_t *prefix,
                           uint8_t bits)
{
    uint8_t bytes;

    if (bits > 128) {
        bits = 128;
    }

    bytes = bits / 8;

    memcpy(out, prefix, bytes);

    if (bits % 8) {
        uint8_t mask = 0xff << (8 - (bits - (bytes * 8)));

        out->u8[bytes] &= ~mask;
        out->u8[bytes] |= (prefix->u8[bytes] & mask);
    }
}
/**
 * @}
 */
