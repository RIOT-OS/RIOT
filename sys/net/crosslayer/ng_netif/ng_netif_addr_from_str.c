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

static uint8_t _parse_byte(char *str)
{
    uint8_t res = 0;

    for (int i = 0; i < 2; i++) {   /* iterate over half-bytes */
        if ((str[i] >= '0') && (str[i] <= '9')) {       /* if '0'-'9' */
            res |= (str[i] - '0') << (4 * (1 - i));     /* set half-byte to 0-9 */
        }
        else if ((str[i] >= 'a') && (str[i] <= 'f')) {  /* if 'a'-'f' */
            res |= (str[i] - 'a' + 10) << (4 * (1 - i));/* set half-byte to 10-15 */
        }

        /* interpret any other character as 0 */
    }

    return res;
}

size_t ng_netif_addr_from_str(uint8_t *out, size_t out_len, char *str)
{
    size_t res = 0;
    char *byte_str = str;
    int end = 0;

    while (end == 0) {
        str++;

        if ((res >= out_len) || ((str - byte_str) > 2)) {
            /* no space left or byte_str has become > 2 chars */
            return 0;
        }

        if ((*str == ':') || (*str == '\0')) {
            end = (*str == '\0');
            *str = '\0';
            out[res++] = _parse_byte(byte_str);
            byte_str = ++str;
        }
    }

    return res;
}

/** @} */
