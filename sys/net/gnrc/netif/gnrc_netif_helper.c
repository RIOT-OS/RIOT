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
 */

#include "net/gnrc/netif.h"
#ifdef MODULE_SOCK_DNS
#include "net/sock/dns.h"
#endif

/* get the next netif, returns true if there are more */
static bool _netif_get(gnrc_netif_t **current_netif)
{
    gnrc_netif_t *netif = *current_netif;
    netif = gnrc_netif_iter(netif);

    *current_netif = netif;
    return !gnrc_netif_highlander() && gnrc_netif_iter(netif);
}

int gnrc_netif_parse_hostname(const char *hostname, ipv6_addr_t *addr,
                              gnrc_netif_t **netif)
{
    *netif = NULL;

#ifdef MODULE_SOCK_DNS
    /* hostname is not an IPv6 address */
    if (strchr(hostname, ':') == NULL) {
        int res = sock_dns_query(hostname, addr, AF_INET6);
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
        *netif = gnrc_netif_get_by_pid(atoi(iface + 1));
        len -= strlen(iface);
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
