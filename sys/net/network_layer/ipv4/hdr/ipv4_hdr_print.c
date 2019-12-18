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

#include "net/ipv4/addr.h"
#include "net/ipv4/hdr.h"

void ipv4_hdr_print(ipv4_hdr_t *hdr)
{
    char addr_str[IPV4_ADDR_MAX_STR_LEN];

    if (!ipv4_hdr_is(hdr)) {
        printf("illegal version field: %" PRIu8 "\n", ipv4_hdr_get_version(hdr));
    }

    // TODO : Add more infos

    printf("source address: %s\n", ipv4_addr_to_str(addr_str, &hdr->src,
            sizeof(addr_str)));
    printf("destination address: %s\n", ipv4_addr_to_str(addr_str, &hdr->dst,
            sizeof(addr_str)));

}

/** @} */
