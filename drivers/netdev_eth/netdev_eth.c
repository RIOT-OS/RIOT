/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_netdev_eth
 * @{
 *
 * @file
 * @brief       Common code for netdev ethernet drivers
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "net/netdev.h"
#include "net/eui64.h"
#include "net/ethernet.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int _get_iid(netdev_t *netdev, eui64_t *value, size_t max_len)
{
    if (max_len < sizeof(eui64_t)) {
        return -EOVERFLOW;
    }

    uint8_t addr[ETHERNET_ADDR_LEN];
    netdev->driver->get(netdev, NETOPT_ADDRESS, addr, ETHERNET_ADDR_LEN);
    ethernet_get_iid(value, addr);

    return sizeof(eui64_t);
}

int netdev_eth_get(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    int res = 0;

    switch (opt) {
        case NETOPT_DEVICE_TYPE:
            {
                uint16_t *tgt = (uint16_t *)value;
                *tgt = NETDEV_TYPE_ETHERNET;
                res = 2;
                break;
            }
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            {
                assert(max_len == 2);
                uint16_t *tgt = (uint16_t*)value;
                *tgt=6;
                res = sizeof(uint16_t);
                break;
            }
        case NETOPT_MAX_PACKET_SIZE:
            {
                assert(max_len >= 2);
                uint16_t *val = (uint16_t*) value;
                *val = ETHERNET_DATA_LEN;
                res = sizeof(uint16_t);
                break;
            }
        case NETOPT_IS_WIRED:
            {
                res = 1;
                break;
            }
        case NETOPT_IPV6_IID:
            {
                return _get_iid(dev, value, max_len);
            }
#ifdef MODULE_NETSTATS_L2
        case NETOPT_STATS:
            {
                assert(max_len >= sizeof(uintptr_t));
                *((netstats_t**)value) = &dev->stats;
                res = sizeof(uintptr_t);
                break;
            }
#endif
#ifdef MODULE_L2FILTER
        case NETOPT_L2FILTER:
            {
                assert(max_len >= sizeof(l2filter_t **));
                *((l2filter_t **)value) = dev->filter;
                res = sizeof(l2filter_t **);
                break;
            }
#endif
        default:
            {
                res = -ENOTSUP;
                break;
            }
    }

    return res;
}

int netdev_eth_set(netdev_t *dev, netopt_t opt, const void *value, size_t value_len)
{
#ifndef MODULE_L2FILTER
    (void)dev;
#endif
    (void)value;
    (void)value_len;

    int res = 0;

    switch (opt) {

#ifdef MODULE_L2FILTER
        case NETOPT_L2FILTER:
            res = l2filter_add(dev->filter, value, value_len);
            break;
        case NETOPT_L2FILTER_RM:
            res = l2filter_rm(dev->filter, value, value_len);
            break;
#endif
        default:
            res = -ENOTSUP;
            break;
    }

    return res;
}
