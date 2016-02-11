/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev2.h"

#include "net/netdev2/ieee802154.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int _get_iid(netdev2_ieee802154_t *dev, eui64_t *value, size_t max_len)
{
    uint8_t *addr;
    uint16_t addr_len;

    if (max_len < sizeof(eui64_t)) {
        return -EOVERFLOW;
    }

    if (dev->flags & NETDEV2_IEEE802154_SRC_MODE_LONG) {
        addr_len = IEEE802154_LONG_ADDRESS_LEN;
        addr = dev->long_addr;
    }
    else {
        addr_len = IEEE802154_SHORT_ADDRESS_LEN;
        addr = dev->short_addr;
    }
    ieee802154_get_iid(value, addr, addr_len);

    return sizeof(eui64_t);
}

int netdev2_ieee802154_get(netdev2_ieee802154_t *dev, netopt_t opt, void *value,
                           size_t max_len)
{
    int res = -ENOTSUP;

    switch (opt) {
        case NETOPT_ADDRESS:
            if (max_len < sizeof(dev->short_addr)) {
                res = -EOVERFLOW;
                break;
            }
            memcpy(value, dev->short_addr, sizeof(dev->short_addr));
            res = sizeof(dev->short_addr);
            break;
        case NETOPT_ADDRESS_LONG:
            if (max_len < sizeof(dev->long_addr)) {
                res = -EOVERFLOW;
                break;
            }
            memcpy(value, dev->long_addr, sizeof(dev->long_addr));
            res = sizeof(dev->long_addr);
            break;
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            if (max_len < sizeof(uint16_t)) {
                res = -EOVERFLOW;
                break;
            }
            if (dev->flags & NETDEV2_IEEE802154_SRC_MODE_LONG) {
                *((uint16_t *)value) = IEEE802154_LONG_ADDRESS_LEN;
            }
            else {
                *((uint16_t *)value) = IEEE802154_SHORT_ADDRESS_LEN;
            }
            res = sizeof(uint16_t);
            break;
        case NETOPT_NID:
            if (max_len < sizeof(dev->pan)) {
                res = -EOVERFLOW;
                break;
            }
            *((uint16_t *)value) = dev->pan;
            res = sizeof(dev->pan);
            break;
        case NETOPT_CHANNEL:
            if (max_len < sizeof(uint16_t)) {
                res = -EOVERFLOW;
                break;
            }
            *((uint16_t *)value) = (uint16_t)dev->chan;
            res = sizeof(dev->chan);
            break;
        case NETOPT_AUTOACK:
            if (max_len < sizeof(netopt_enable_t)) {
                res = -EOVERFLOW;
                break;
            }
            if (dev->flags & NETDEV2_IEEE802154_ACK_REQ) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            res = sizeof(netopt_enable_t);
            break;
        case NETOPT_RAWMODE:
            if (max_len < sizeof(netopt_enable_t)) {
                res = -EOVERFLOW;
                break;
            }
            if (dev->flags & NETDEV2_IEEE802154_RAW) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            res = sizeof(netopt_enable_t);
            break;
#ifdef MODULE_GNRC
        case NETOPT_PROTO:
            if (max_len < sizeof(gnrc_nettype_t)) {
                res = -EOVERFLOW;
                break;
            }
            *((gnrc_nettype_t *)value) = dev->proto;
            res = sizeof(gnrc_nettype_t);
            break;
#endif
        case NETOPT_DEVICE_TYPE:
            if (max_len < sizeof(uint16_t)) {
                res = -EOVERFLOW;
                break;
            }
            *((uint16_t *)value) = NETDEV2_TYPE_IEEE802154;
            res = sizeof(uint16_t);
            break;
        case NETOPT_IPV6_IID:
            res = _get_iid(dev, value, max_len);
            break;
#ifdef MODULE_NETSTATS_L2
        case NETOPT_STATS:
            if (max_len < sizeof(uintptr_t)) {
                res = -EOVERFLOW;
                break;
            }
            *((netstats_t**)value) = &dev->netdev.stats;
            res = sizeof(uintptr_t);
            break;
#endif
        default:
            break;
    }
    return res;
}

int netdev2_ieee802154_set(netdev2_ieee802154_t *dev, netopt_t opt, void *value,
                           size_t len)
{
    int res = -ENOTSUP;

    switch (opt) {
        case NETOPT_ADDRESS:
            if (len > sizeof(dev->short_addr)) {
                res = -EOVERFLOW;
                break;
            }
            memset(dev->short_addr, 0, sizeof(dev->short_addr));
            memcpy(dev->short_addr, value, len);
            res = sizeof(dev->short_addr);
            break;
        case NETOPT_ADDRESS_LONG:
            if (len > sizeof(dev->long_addr)) {
                res = -EOVERFLOW;
                break;
            }
            memset(dev->long_addr, 0, sizeof(dev->long_addr));
            memcpy(dev->long_addr, value, len);
            res = sizeof(dev->long_addr);
            break;
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            if (len > sizeof(uint16_t)) {
                res = -EOVERFLOW;
                break;
            }
            switch ((*(uint16_t *)value)) {
                case IEEE802154_SHORT_ADDRESS_LEN:
                    dev->flags &= ~NETDEV2_IEEE802154_SRC_MODE_LONG;
                    break;
                case IEEE802154_LONG_ADDRESS_LEN:
                    dev->flags |= NETDEV2_IEEE802154_SRC_MODE_LONG;
                    break;
                default:
                    res = -EAFNOSUPPORT;
                    break;
            }
            res = sizeof(uint16_t);
            break;
        case NETOPT_NID:
            if (len > sizeof(dev->pan)) {
                res = -EOVERFLOW;
                break;
            }
            dev->pan = *((uint16_t *)value);
            res = sizeof(dev->pan);
            break;
        /* channel can be very device specific */
        case NETOPT_AUTOACK:
            if ((*(bool *)value)) {
                dev->flags |= NETDEV2_IEEE802154_ACK_REQ;
            }
            else {
                dev->flags &= ~NETDEV2_IEEE802154_ACK_REQ;
            }
            res = sizeof(uint16_t);
            break;
        case NETOPT_RAWMODE:
            if ((*(bool *)value)) {
                dev->flags |= NETDEV2_IEEE802154_RAW;
            }
            else {
                dev->flags &= ~NETDEV2_IEEE802154_RAW;
            }
            res = sizeof(uint16_t);
            break;
#ifdef MODULE_GNRC
        case NETOPT_PROTO:
            if (len > sizeof(gnrc_nettype_t)) {
                res = -EOVERFLOW;
                break;
            }
            dev->proto = *((gnrc_nettype_t *)value);
            res = sizeof(gnrc_nettype_t);
            break;
#endif
        default:
            break;
    }
    return res;
}

/** @} */
