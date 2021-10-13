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
#include "net/inet_csum.h"

#define ENABLE_DEBUG 0
#include "debug.h"

uint16_t inet_csum_slice(uint16_t sum, const uint8_t *buf, uint16_t len, size_t accum_len)
{
    uint32_t csum = sum;

    DEBUG("inet_sum: sum = 0x%04" PRIx16 ", len = %" PRIu16, sum, len);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (IS_USED(MODULE_OD)) {
            DEBUG(", buf:\n");
            od_hex_dump(buf, len, OD_WIDTH_DEFAULT);
        } else {
            DEBUG(", buf output only with od module\n");
        }
    }

    if (len == 0)
        return csum;

    if (accum_len & 1) {      /* if accumulated length is odd */
        csum += *buf;         /* add first byte as bottom half of 16-byte word */
        buf++;
        len--;
        accum_len++;
    }

    for (unsigned i = 0; i < (len >> 1); buf += 2, i++) {
        csum += (uint16_t)(*buf << 8) + *(buf + 1); /* group bytes by 16-byte words */
                                                    /* and add them */
    }

    if ((accum_len + len) & 1)          /* if accumulated length is odd */
        csum += (uint16_t)(*buf << 8);  /* add last byte as top half of 16-byte word */

    while (csum >> 16) {
        uint16_t carry = csum >> 16;
        csum = (csum & 0xffff) + carry;
    }

    DEBUG("inet_sum: new sum = 0x%04" PRIx32 "\n", csum);

    return csum;
}

/** @} */
