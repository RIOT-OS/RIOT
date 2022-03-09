/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "net/dhcpv6/client.h"
#if IS_USED(MODULE_SOCK_DNS)
#include "net/sock/dns.h"
#endif

#include "_dhcpv6.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static char addr_str[IPV6_ADDR_MAX_STR_LEN];

void dhcpv6_client_dns_rns_conf(const dhcpv6_opt_dns_rns_t *opt, uint16_t netif)
{
    if (byteorder_ntohs(opt->len) < sizeof(ipv6_addr_t)) {
        DEBUG("dhcpv6_client_dns: no DNS recursive name server provided.\n");
        return;
    }
#if IS_USED(MODULE_SOCK_DNS) && IS_ACTIVE(SOCK_HAS_IPV6)
    DEBUG("Overriding sock_dns_server with %s\n",
          ipv6_addr_to_str(addr_str, opt->dns_rns, sizeof(addr_str)));
    sock_dns_server.port = SOCK_DNS_PORT;
    sock_dns_server.family = AF_INET6;
    sock_dns_server.netif = netif;
    memcpy(sock_dns_server.addr.ipv6, opt->dns_rns,
           sizeof(sock_dns_server.addr.ipv6));
    return;
#endif
}

/** @} */
