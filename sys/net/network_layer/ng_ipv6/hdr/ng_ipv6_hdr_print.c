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

#include "net/ng_ipv6/hdr.h"

void ng_ipv6_hdr_print(ng_ipv6_hdr_t *hdr)
{
    char addr_str[NG_IPV6_ADDR_MAX_STR_LEN];

    if (!ng_ipv6_hdr_is(hdr)) {
        printf("illegal version field: %" PRIu8 "\n", ng_ipv6_hdr_get_version(hdr));
    }

    printf("traffic class: 0x%02" PRIx8 " (ECN: 0x%" PRIx8 ", DSCP: 0x%02" PRIx8 ")\n",
           ng_ipv6_hdr_get_tc(hdr), ng_ipv6_hdr_get_tc_ecn(hdr),
           ng_ipv6_hdr_get_tc_dscp(hdr));
    printf("flow label: 0x%05" PRIx32 "\n", ng_ipv6_hdr_get_fl(hdr));
    printf("length: %" PRIu16 "  next header: %" PRIu8 "  hop limit: %" PRIu8 "\n",
           byteorder_ntohs(hdr->len), hdr->nh, hdr->hl);
    printf("source address: %s\n", ng_ipv6_addr_to_str(addr_str, &hdr->src,
            sizeof(addr_str)));
    printf("destination address: %s\n", ng_ipv6_addr_to_str(addr_str, &hdr->dst,
            sizeof(addr_str)));

}

/** @} */
