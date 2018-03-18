/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_netdev_eth
 * @{
 *
 * @file
 * @brief       Common code for netdev ethernet drivers
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include "net/netdev.h"
#include "net/netdev/eth.h"
#include "net/netdev/layer.h"
#include "net/eui64.h"
#include "net/ethernet.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len);
static int _set(netdev_t *dev, netopt_t opt, const void *value, size_t value_len);

const netdev_driver_t eth_layer = {
    .send = netdev_send_pass,
    .recv = netdev_recv_pass,
    .init = netdev_init_pass,
    .isr = netdev_isr_pass,
    .get = _get,
    .set = _set,
};

netdev_t *netdev_eth_add(netdev_t *head, netdev_eth_t *dev_eth) {
    dev_eth->netdev.driver = &eth_layer;
    head->event_callback = netdev_event_cb_pass;
    dev_eth->hwdev = head;
    return netdev_add_layer(head, (netdev_t *)dev_eth);
}

static int _get_iid(netdev_eth_t *dev_eth, eui64_t *value, size_t max_len)
{
    DEBUG("netdev_eth: IID called\n");
    if (max_len < sizeof(eui64_t)) {
        return -EOVERFLOW;
    }

    uint8_t addr[ETHERNET_ADDR_LEN];
    dev_eth->netdev.lower->driver->get(dev_eth->netdev.lower,
                                       NETOPT_ADDRESS,
                                       addr, ETHERNET_ADDR_LEN);
    ethernet_get_iid(value, addr);
    return sizeof(eui64_t);
}

int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    DEBUG("netdev_layer_eth: Get called: %d\n", opt);
    netdev_eth_t *dev_eth = (netdev_eth_t *)dev;
    int res = -ENOTSUP;

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
                res = _get_iid(dev_eth, value, max_len);
                break;
            }
#ifdef MODULE_NETSTATS_L2
        case NETOPT_STATS:
            {
                assert(max_len >= sizeof(uintptr_t));
                *((netstats_t**)value) = &dev_eth->hwdev->stats;
                res = sizeof(uintptr_t);
                break;
            }
#endif
#ifdef MODULE_L2FILTER
        case NETOPT_L2FILTER:
            {
                assert(max_len >= sizeof(l2filter_t **));
                *((l2filter_t **)value) = dev_eth->hwdev->filter;
                res = sizeof(l2filter_t **);
                break;
            }
#endif
        default:
            break;
    }
    if (res == -ENOTSUP) {
        DEBUG("netdev_layer_eth: Passing get to device\n");
        res = dev_eth->netdev.lower->driver->get(dev_eth->netdev.lower,
                                                 opt, value, max_len);
    }
    DEBUG("netdev_layer_eth: Get returned %d\n", res);
    return res;
}

int _set(netdev_t *dev, netopt_t opt, const void *value, size_t value_len)
{
    DEBUG("netdev_eth: Set called\n");
    netdev_eth_t *dev_eth = (netdev_eth_t *)dev;
    int res = -ENOTSUP;

    switch (opt) {

#ifdef MODULE_L2FILTER
        case NETOPT_L2FILTER:
            res = l2filter_add(dev_eth->hwdev->filter,
                               value, value_len);
            break;
        case NETOPT_L2FILTER_RM:
            res = l2filter_rm(dev_eth->hwdev->filter,
                              value, value_len);
            break;
#endif
        default:
            break;
    }
    if (res == -ENOTSUP) {
        res = dev_eth->netdev.lower->driver->set(dev_eth->netdev.lower,
                                                 opt, value, value_len);
    }

    return res;
}
