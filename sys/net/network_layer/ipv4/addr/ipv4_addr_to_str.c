/*
 * SPDX-FileCopyrightText: 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */

#include <stdlib.h>

#include "net/ipv4/addr.h"

/* based on inet_ntop4() by Paul Vixie */
char *ipv4_addr_to_str(char *result, const ipv4_addr_t *addr, uint8_t result_len)
{
    uint8_t n = 0;
    char *next = result;

    if ((result == NULL) || (addr == NULL) || (result_len < IPV4_ADDR_MAX_STR_LEN)) {
        return NULL;
    }

    do {
        uint8_t u = addr->u8[n];

        if (u > 99) {
            *next++ = '0' + u / 100;
            u %= 100;
            *next++ = '0' + u / 10;
            u %= 10;
        }
        else if (u > 9) {
            *next++ = '0' + u / 10;
            u %= 10;
        }

        *next++ = '0' + u;
        *next++ = '.';
        n++;
    } while (n < 4);

    *--next = '\0';
    return result;
}

/** @} */
