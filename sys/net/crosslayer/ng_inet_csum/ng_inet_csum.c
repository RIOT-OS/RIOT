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

#include <stdio.h>
#include "net/ng_inet_csum.h"

uint16_t ng_inet_csum(uint16_t sum, const uint8_t *buf, uint16_t len)
{
    uint32_t csum = sum;

    for (int i = 0; i < (len >> 1); buf += 2, i++) {
        csum += (*buf << 8) + *(buf + 1);   /* group bytes by 16-byte words
                                             * and add them*/
    }

    if (len & 1) {                      /* if len is odd */
        csum += (*buf << 8);            /* add last byte as top half of 16-byte word */
    }

    csum += csum >> 16;

    return (csum & 0xffff);
}

/** @} */
