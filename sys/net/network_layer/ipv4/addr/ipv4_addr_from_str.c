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

#include <stdlib.h>
#include <string.h>

#include "net/ipv4/addr.h"

#define DEC     "0123456789"

/* based on inet_pton4() by Paul Vixie */
ipv4_addr_t *ipv4_addr_from_str(ipv4_addr_t *result, const char *addr)
{
    uint8_t saw_digit, octets, ch;
    uint8_t tmp[sizeof(ipv4_addr_t)], *tp;

    if ((result == NULL) || (addr == NULL)) {
        return NULL;
    }

    saw_digit = 0;
    octets = 0;
    *(tp = tmp) = 0;

    while ((ch = *addr++) != '\0') {
        const char *pch;

        if ((pch = strchr(DEC, ch)) != NULL) {
            uint16_t new = *tp * 10 + (uint16_t)(pch - DEC);

            if (new > 255) {
                return NULL;
            }

            *tp = new;

            if (!saw_digit) {
                if (++octets > 4) {
                    return NULL;
                }

                saw_digit = 1;
            }
        }
        else if (ch == '.' && saw_digit) {
            if (octets == 4) {
                return NULL;
            }

            *++tp = 0;
            saw_digit = 0;
        }
        else {
            return NULL;
        }
    }

    if (octets < 4) {
        return NULL;
    }

    memcpy(result, tmp, sizeof(ipv4_addr_t));
    return result;
}


/** @} */
