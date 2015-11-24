/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_netdev2_eth
 * @{
 *
 * @file
 * @brief       Common code for netdev2 ethernet drivers
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "net/netdev2.h"
#include "net/eui64.h"
#include "net/ethernet.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int _get_iid(netdev2_t *netdev, eui64_t *value, size_t max_len)
{
    if (max_len < sizeof(eui64_t)) {
        return -EOVERFLOW;
    }

    uint8_t addr[ETHERNET_ADDR_LEN];
    netdev->driver->get(netdev, NETOPT_ADDRESS, addr, ETHERNET_ADDR_LEN);
    ethernet_get_iid(value, addr);

    return sizeof(eui64_t);
}

int netdev2_eth_get(netdev2_t *dev, netopt_t opt, void *value, size_t max_len)
{
    int res = 0;

    switch (opt) {
        case NETOPT_DEVICE_TYPE:
            {
                uint16_t *tgt = (uint16_t *)value;
                *tgt = NETDEV2_TYPE_ETHERNET;
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
        default:
            {
                res = -ENOTSUP;
                break;
            }
    }

    return res;
}

int netdev2_eth_set(netdev2_t *dev, netopt_t opt, void *value, size_t value_len)
{
    (void)dev;
    (void)value;
    (void)value_len;

    int res = 0;

    switch (opt) {
        default:
            return -ENOTSUP;
    }

    return res;
}
