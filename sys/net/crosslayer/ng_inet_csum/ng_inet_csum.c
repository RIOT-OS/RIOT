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

#include <inttypes.h>
#include <stdio.h>
#include "od.h"
#include "net/ng_inet_csum.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

uint16_t ng_inet_csum(uint16_t sum, const uint8_t *buf, uint16_t len)
{
    uint32_t csum = sum;

    DEBUG("inet_sum: sum = 0x%04" PRIx16 ", len = %" PRIu16, sum, len);
#if ENABLE_DEBUG
#ifdef MODULE_OD
    DEBUG(", buf:\n");
    od_hex_dump(buf, len, OD_WIDTH_DEFAULT);
#else
    DEBUG(", buf output only with od module\n");
#endif
#endif

    for (int i = 0; i < (len >> 1); buf += 2, i++) {
        csum += (*buf << 8) + *(buf + 1);   /* group bytes by 16-byte words
                                             * and add them*/
    }

    if (len & 1) {                      /* if len is odd */
        csum += (*buf << 8);            /* add last byte as top half of 16-byte word */
    }

    while (csum >> 16) {
        uint16_t carry = csum >> 16;
        csum = (csum & 0xffff) + carry;
    }

    DEBUG("inet_sum: new sum = 0x%04" PRIx32 "\n", csum);

    return csum;
}

/** @} */
