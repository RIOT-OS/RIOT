/*
 * Copyright (C) Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdio.h>

#include "bitfield.h"
#include "net/ipv6/addr.h"

#include "net/gnrc/ipv6/whitelist.h"

extern ipv6_addr_t gnrc_ipv6_whitelist[GNRC_IPV6_WHITELIST_SIZE];
extern BITFIELD(gnrc_ipv6_whitelist_set, GNRC_IPV6_WHITELIST_SIZE);

void gnrc_ipv6_whitelist_print(void)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    for (int i = 0; i < GNRC_IPV6_WHITELIST_SIZE; i++) {
        if (bf_isset(gnrc_ipv6_whitelist_set, i)) {
            puts(ipv6_addr_to_str(addr_str, &gnrc_ipv6_whitelist[i], sizeof(addr_str)));
        }
    }
}

/** @} */
