/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @addtogroup  ipv6_addr
 * @{
 *
 * @file        addr.c
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdlib.h>

#include "ipv6/addr.h"

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

/**
 * @}
 */
