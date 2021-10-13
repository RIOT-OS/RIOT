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
 * @author Martin Landsmann <martin.landsmann@haw-hamburg.de>
 */

#include <stdio.h>

#include "bitfield.h"
#include "net/ipv6/addr.h"

#include "net/gnrc/ipv6/blacklist.h"

extern ipv6_addr_t gnrc_ipv6_blacklist[CONFIG_GNRC_IPV6_BLACKLIST_SIZE];
extern BITFIELD(gnrc_ipv6_blacklist_set, CONFIG_GNRC_IPV6_BLACKLIST_SIZE);

void gnrc_ipv6_blacklist_print(void)
{
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    for (int i = 0; i < CONFIG_GNRC_IPV6_BLACKLIST_SIZE; i++) {
        if (bf_isset(gnrc_ipv6_blacklist_set, i)) {
            puts(ipv6_addr_to_str(addr_str, &gnrc_ipv6_blacklist[i], sizeof(addr_str)));
        }
    }
}

/** @} */
