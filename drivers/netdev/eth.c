/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "net/eui48.h"
#include "net/eui64.h"
#include "net/ethernet.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int netdev_eth_get(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    int res = 0;

#ifndef MODULE_L2FILTER
    (void)dev;
#endif
    (void)max_len;  /* only used in assert() */
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
        case NETOPT_MAX_PDU_SIZE:
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
