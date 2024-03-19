/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "net/utils.h"
#if defined(MODULE_DNS)
#include "net/af.h"
#include "net/dns.h"
#endif

/* get the next netif, returns true if there are more */
static bool _netif_get(netif_t **current_netif)
{
    netif_t *netif = *current_netif;
    netif = netif_iter(netif);

    *current_netif = netif;
    return netif_iter(netif);
}

int netutils_get_ipv4(ipv4_addr_t *addr, const char *hostname)
{
    if (hostname == NULL) {
        return -EINVAL;
    }

    for (size_t i = 0; i < strlen(hostname); i++) {
        bool is_not_ipv4 = (hostname[i] < '0' || hostname[i] > '9') && hostname[i] != '.';

#if defined(MODULE_SOCK_DNS) || defined(MODULE_SOCK_DNS_MOCK)
        /* once we see an invalid character for an IPv4 address try to
         * resolve the hostname by DNS */
        if (is_not_ipv4) {
            int res = dns_query(hostname, addr, AF_INET);
            if (res < 0) {
                return res;
            }
            return 0;
        }
#else
        if (is_not_ipv4) {
            return -EINVAL;
        }
#endif
    }

    size_t len = strlen(hostname);
    if (ipv4_addr_from_buf(addr, hostname, len) == NULL) {
        return -EINVAL;
    }

    return 0;
}

int netutils_get_ipv6(ipv6_addr_t *addr, netif_t **netif, const char *hostname)
{
    *netif = NULL;

    if (hostname == NULL) {
        return -EINVAL;
    }

#if defined(MODULE_SOCK_DNS) || defined(MODULE_SOCK_DNS_MOCK)
    /* hostname is not an IPv6 address */
    if (strchr(hostname, ':') == NULL) {
        int res = dns_query(hostname, addr, AF_INET6);
        if (res < 0) {
            return res;
        }
        return 0;
    }
#endif

    /* search for interface ID */
    size_t len = strlen(hostname);
    char *iface = strchr(hostname, '%');
    if (iface) {
        *netif = netif_get_by_name(iface + 1);
        len -= strlen(iface);

        if (*netif == NULL) {
            return -EINVAL;
        }
    }
    /* preliminary select the first interface */
    else if (_netif_get(netif)) {
        /* don't take it if there is more than one interface */
        *netif = NULL;
    }

    if (ipv6_addr_from_buf(addr, hostname, len) == NULL) {
        return -EINVAL;
    }

    return 0;
}
/** @} */
