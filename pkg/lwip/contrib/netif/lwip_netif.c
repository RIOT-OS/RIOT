/*
 * Copyright (C) 2018 Freie Universität Berlin
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

#include <stdint.h>

#include "fmt.h"
#include "net/eui64.h"
#include "net/netdev.h"

#include "lwip/netif.h"
#include "lwip/opt.h"
#include "netif/lowpan6_opts.h"

#include "netif_types.h"

netif_t netif_iter(netif_t last)
{
    if (last == NETIF_INVALID) {
        return netif_list;
    }
    else {
        return last->next;
    }
}

int netif_get_name(netif_t netif, char *name)
{
    size_t offset = sizeof(netif->name);

    /* construct name expected by lwIP's `netif_find()` */
    memcpy(name, netif->name, offset);
    offset += fmt_u16_dec(&name[offset], netif->num);
    name[offset] = '\0';
    return offset;
}

netif_t netif_get_by_name(const char *name)
{
    return netif_find(name);
}

int netif_get_opt(netif_t netif, netopt_t opt, uint16_t context,
                  void *value, size_t max_len)
{
    netdev_t *dev = netif->state;

    (void)context;
    switch (opt) {
#if LWIP_IPV6
        case NETOPT_IPV6_IID:
            return sizeof(eui64_t);
        case NETOPT_IPV6_ADDR:
            return sizeof(ip6_addr_t);
        case NETOPT_IPV6_ADDR_REMOVE:
            return sizeof(ip6_addr_t);
        case NETOPT_IPV6_ADDR_FLAGS:
            return sizeof(uint8_t);
        case NETOPT_IPV6_FORWARDING: {
            netopt_enable_t *enabled = (netopt_enable_t *)value;

            assert(max_len == sizeof(netopt_enable_t));
            *enabled = IP_FORWARD;
            return sizeof(netopt_enable_t);
        }
        case NETOPT_IPV6_SND_RTR_ADV:
            return sizeof(netopt_enable_t);
#endif
#if LWIP_6LOWPAN
        case NETOPT_IPV6_6LO_IPHC: {
            netopt_enable_t *enabled = (netopt_enable_t *)value;

            assert(max_len == sizeof(netopt_enable_t));
            *enabled = LWIP_6LOWPAN_IPHC;
            return sizeof(netopt_enable_t);
        }
#endif
        default:
            break;
    }
    return dev->driver->get(dev, opt, value, max_len);
}

static void _update_hwaddr_from_dev(netif_t netif, netdev_t *dev)
{
    int res;
    netopt_t opt = NETOPT_ADDRESS;
    uint16_t tmp;

    res = dev->driver->get(dev, NETOPT_SRC_LEN, &tmp, sizeof(tmp));
    assert(res == sizeof(tmp));
    netif->hwaddr_len = (uint8_t)tmp;
    if (tmp == sizeof(eui64_t)) {
        opt = NETOPT_ADDRESS_LONG;
    }
    dev->driver->get(dev, opt, netif->hwaddr, sizeof(netif->hwaddr));
}

int netif_set_opt(netif_t netif, netopt_t opt, uint16_t context,
                  void *value, size_t value_len)
{
    netdev_t *dev = netif->state;
    int res;

    (void)context;
    switch (opt) {
#if LWIP_IPV6
        case NETOPT_IPV6_ADDR:
            return sizeof(ip6_addr_t);
        case NETOPT_IPV6_ADDR_REMOVE:
            return sizeof(ip6_addr_t);
        case NETOPT_IPV6_ADDR_FLAGS:
            return sizeof(uint8_t);
#endif
        default:
            break;
    }
    res = dev->driver->set(dev, opt, value, value_len);
    if (res > 0) {
        switch (opt) {
            case NETOPT_ADDRESS:
            case NETOPT_ADDRESS_LONG:
            case NETOPT_ADDR_LEN:
            case NETOPT_SRC_LEN:
                _update_hwaddr_from_dev(netif, dev);
                break;
            default:
                break;
        }
    }
    return res;
}

/** @} */
