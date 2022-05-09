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
 * Implements @ref net_netif for @ref net_lwip
 *
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author  Erik Ekman <eekman@google.com>
 */

#include "fmt.h"
#include "lwip/netif/compat.h"
#include "net/netif.h"

int netif_get_name(netif_t *iface, char *name)
{
    lwip_netif_t *lwip_netif = (lwip_netif_t*) iface;
    struct netif *netif = &lwip_netif->lwip_netif;

    int res = 2;
    name[0] = netif->name[0];
    name[1] = netif->name[1];
    res += fmt_u16_dec(&name[res], netif->num);
    name[res] = '\0';
    return res;
}

int netif_get_opt(netif_t *iface, netopt_t opt, uint16_t context,
                  void *value, size_t max_len)
{
    (void)context;
    lwip_netif_t *lwip_netif = (lwip_netif_t*) iface;
    struct netif *netif = &lwip_netif->lwip_netif;
    struct netdev *dev = netif->state;
    int res = -ENOTSUP;
    switch (opt) {
#ifdef MODULE_LWIP_IPV6
        case NETOPT_IPV6_ADDR: {
                assert(max_len >= sizeof(ipv6_addr_t));
                ipv6_addr_t *tgt = value;

                res = 0;
                for (unsigned i = 0;
                     ((res + sizeof(ipv6_addr_t)) <= max_len) &&
                     (i < LWIP_IPV6_NUM_ADDRESSES);
                     i++) {
                    if (netif_ip6_addr_state(netif, i) != IP6_ADDR_INVALID) {
                        memcpy(tgt, &(netif_ip6_addr(netif, i)->addr), sizeof(ipv6_addr_t));
                        res += sizeof(ipv6_addr_t);
                        tgt++;
                    }
                }
            }
            break;
#endif
        default:
            break;
    }
    if (res == -ENOTSUP) {
        // Ask underlying netdev
        res = dev->driver->get(dev, opt, value, max_len);
    }
    return res;
}

int netif_set_opt(netif_t *iface, netopt_t opt, uint16_t context,
                  void *value, size_t value_len)
{
    (void)iface;
    (void)opt;
    (void)context;
    (void)value;
    (void)value_len;

    return -ENOTSUP;
}

/** @} */
