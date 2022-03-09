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

#include <string.h>
#include "bitfield.h"

#include "net/gnrc/ipv6/blacklist.h"

#define ENABLE_DEBUG 0
#include "debug.h"

ipv6_addr_t gnrc_ipv6_blacklist[CONFIG_GNRC_IPV6_BLACKLIST_SIZE];
BITFIELD(gnrc_ipv6_blacklist_set, CONFIG_GNRC_IPV6_BLACKLIST_SIZE);

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

int gnrc_ipv6_blacklist_add(const ipv6_addr_t *addr)
{
    for (int i = 0; i < CONFIG_GNRC_IPV6_BLACKLIST_SIZE; i++) {
        if (!bf_isset(gnrc_ipv6_blacklist_set, i)) {
            bf_set(gnrc_ipv6_blacklist_set, i);
            memcpy(&gnrc_ipv6_blacklist[i], addr, sizeof(*addr));
            DEBUG("IPv6 blacklist: blacklisted %s\n",
                  ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)));
            return 0;
        }
    }
    return -1;
}

void gnrc_ipv6_blacklist_del(const ipv6_addr_t *addr)
{
    for (int i = 0; i < CONFIG_GNRC_IPV6_BLACKLIST_SIZE; i++) {
        if (ipv6_addr_equal(addr, &gnrc_ipv6_blacklist[i])) {
            bf_unset(gnrc_ipv6_blacklist_set, i);
            DEBUG("IPv6 blacklist: unblacklisted %s\n",
                  ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)));
        }
    }
}

bool gnrc_ipv6_blacklisted(const ipv6_addr_t *addr)
{
    for (int i = 0; i < CONFIG_GNRC_IPV6_BLACKLIST_SIZE; i++) {
        if (bf_isset(gnrc_ipv6_blacklist_set, i) &&
            ipv6_addr_equal(addr, &gnrc_ipv6_blacklist[i])) {
            return true;
        }
    }
    return false;
}

/** @} */
