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
#include <inttypes.h>

#include "net/ipv6/hdr.h"

void ipv6_hdr_print(ipv6_hdr_t *hdr)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    if (!ipv6_hdr_is(hdr)) {
        printf("illegal version field: %" PRIu8 "\n", ipv6_hdr_get_version(hdr));
    }

    printf("traffic class: 0x%02x (ECN: 0x%x, DSCP: 0x%02x)\n",
           (unsigned)ipv6_hdr_get_tc(hdr), (unsigned)ipv6_hdr_get_tc_ecn(hdr),
           (unsigned)ipv6_hdr_get_tc_dscp(hdr));
    printf("flow label: 0x%05" PRIx32 "\n", ipv6_hdr_get_fl(hdr));
    printf("length: %" PRIu16 "  next header: %u  hop limit: %u\n",
           byteorder_ntohs(hdr->len), (unsigned)hdr->nh, (unsigned)hdr->hl);
    printf("source address: %s\n", ipv6_addr_to_str(addr_str, &hdr->src,
            sizeof(addr_str)));
    printf("destination address: %s\n", ipv6_addr_to_str(addr_str, &hdr->dst,
            sizeof(addr_str)));

}

/** @} */
