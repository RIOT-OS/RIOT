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

#include <string.h>
#include "bitfield.h"

#include "net/gnrc/ipv6/whitelist.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

ipv6_addr_t gnrc_ipv6_whitelist[GNRC_IPV6_WHITELIST_SIZE];
BITFIELD(gnrc_ipv6_whitelist_set, GNRC_IPV6_WHITELIST_SIZE);

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

int gnrc_ipv6_whitelist_add(const ipv6_addr_t *addr)
{
    for (int i = 0; i < GNRC_IPV6_WHITELIST_SIZE; i++) {
        if (!bf_isset(gnrc_ipv6_whitelist_set, i)) {
            bf_set(gnrc_ipv6_whitelist_set, i);
            memcpy(&gnrc_ipv6_whitelist[i], addr, sizeof(*addr));
            DEBUG("IPv6 whitelist: whitelisted %s\n",
                  ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)));
            return 0;
        }
    }
    return -1;
}

void gnrc_ipv6_whitelist_del(const ipv6_addr_t *addr)
{
    for (int i = 0; i < GNRC_IPV6_WHITELIST_SIZE; i++) {
        if (ipv6_addr_equal(addr, &gnrc_ipv6_whitelist[i])) {
            bf_unset(gnrc_ipv6_whitelist_set, i);
            DEBUG("IPv6 whitelist: unwhitelisted %s\n",
                  ipv6_addr_to_str(addr_str, addr, sizeof(addr_str)));
        }
    }
}

bool gnrc_ipv6_whitelisted(const ipv6_addr_t *addr)
{
    for (int i = 0; i < GNRC_IPV6_WHITELIST_SIZE; i++) {
        if (bf_isset(gnrc_ipv6_whitelist_set, i) &&
            ipv6_addr_equal(addr, &gnrc_ipv6_whitelist[i])) {
            return true;
        }
    }
    return false;
}

/** @} */
