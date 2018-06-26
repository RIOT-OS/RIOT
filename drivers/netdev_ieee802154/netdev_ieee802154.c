/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_netdev_ieee802154
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Koen Zandberg <koen@bergzand.net>
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev.h"

#include "net/netdev/ieee802154.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int _get_iid(netdev_ieee802154_t *dev, eui64_t *value, size_t max_len)
{
    (void)max_len;

    uint8_t *addr;
    uint16_t addr_len;

    assert(max_len >= sizeof(eui64_t));

    if (dev->flags & NETDEV_IEEE802154_SRC_MODE_LONG) {
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

int netdev_ieee802154_send(netdev_t *dev, const iolist_t *list)
{
    netdev_ieee802154_t *netdev = (netdev_ieee802154_t *)dev;
    netdev_ieee802154_data_hdr_t *data = list->iol_base;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
    uint8_t flags = (uint8_t)(netdev->flags & NETDEV_IEEE802154_SEND_MASK);
    int res = 0;
    le_uint16_t dev_pan = byteorder_btols(byteorder_htons(netdev->pan));
    flags |= IEEE802154_FCF_TYPE_DATA;
    /* fill MAC header, seq should be set by device */
    if ((res = ieee802154_set_frame_hdr(mhr, data->src, data->src_len,
                                        data->dst, data->dst_len,
                                        dev_pan, dev_pan,
                                        flags, netdev->seq++)) == 0) {
        DEBUG("netdev_ieee802154: Error preperaring frame\n");
        return -EINVAL;
    }

#ifdef MODULE_NETSTATS_L2
    if (data->dst_len == 2 && data->dst[0] == 0xff && data->dst[1] == 0xff) {
        dev->stats.tx_mcast_count++;
    }
    else {
        dev->stats.tx_unicast_count++;
    }
#endif
    /* prepare iolist for netdev / mac layer */
    iolist_t iolist = {
        .iol_next = list->iol_next,
        .iol_base = mhr,
        .iol_len = res
    };
    return dev->driver->send(dev, &iolist);
}

int netdev_ieee802154_recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    /* Leave space for the generic data header */
    netdev_ieee802154_t *netdev = (netdev_ieee802154_t *)dev;
    /* If buf is NULL, pdu_start should also be NULL */
    uint8_t *pdu_start = (buf) ? buf + sizeof(netdev_ieee802154_data_hdr_t) : buf;
    netdev_ieee802154_data_hdr_t *l2data_hdr = buf;
    int res = dev->driver->recv(dev, pdu_start, len, info);
    if (res < 0) {
        return res;
    }
    if (!(netdev->flags & NETDEV_IEEE802154_RAW)) {
        if (buf) {
            int dst_len, src_len;
            le_uint16_t _pan_tmp;
            size_t mhr_len = ieee802154_get_frame_hdr_len(pdu_start);
            dst_len = ieee802154_get_dst(pdu_start, l2data_hdr->dst, &_pan_tmp);
            src_len = ieee802154_get_src(pdu_start, l2data_hdr->src, &_pan_tmp);
            if ((dst_len < 0) || (src_len < 0)) {
                DEBUG("netdev_ieee802154: unable to get addresses\n");
                return -EINVAL;
            }
            l2data_hdr->src_len = src_len;
            l2data_hdr->dst_len = dst_len;
            /* Remove the ieee802154 frame header */
            memmove(pdu_start, pdu_start+mhr_len, res - mhr_len);
            res -= mhr_len;
        }
        res += sizeof(netdev_ieee802154_data_hdr_t);
    }
    return res;
}

int netdev_ieee802154_get(netdev_ieee802154_t *dev, netopt_t opt, void *value,
                           size_t max_len)
{
    int res = -ENOTSUP;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= sizeof(dev->short_addr));
            memcpy(value, dev->short_addr, sizeof(dev->short_addr));
            res = sizeof(dev->short_addr);
            break;
        case NETOPT_ADDRESS_LONG:
            assert(max_len >= sizeof(dev->long_addr));
            memcpy(value, dev->long_addr, sizeof(dev->long_addr));
            res = sizeof(dev->long_addr);
            break;
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            assert(max_len == sizeof(uint16_t));
            if (dev->flags & NETDEV_IEEE802154_SRC_MODE_LONG) {
                *((uint16_t *)value) = IEEE802154_LONG_ADDRESS_LEN;
            }
            else {
                *((uint16_t *)value) = IEEE802154_SHORT_ADDRESS_LEN;
            }
            res = sizeof(uint16_t);
            break;
        case NETOPT_NID:
            assert(max_len == sizeof(dev->pan));
            *((uint16_t *)value) = dev->pan;
            res = sizeof(dev->pan);
            break;
        case NETOPT_CHANNEL:
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)value) = (uint16_t)dev->chan;
            res = sizeof(dev->chan);
            break;
        case NETOPT_ACK_REQ:
            assert(max_len == sizeof(netopt_enable_t));
            if (dev->flags & NETDEV_IEEE802154_ACK_REQ) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            res = sizeof(netopt_enable_t);
            break;
        case NETOPT_RAWMODE:
            assert(max_len == sizeof(netopt_enable_t));
            if (dev->flags & NETDEV_IEEE802154_RAW) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            res = sizeof(netopt_enable_t);
            break;
#ifdef MODULE_GNRC
        case NETOPT_PROTO:
            assert(max_len == sizeof(gnrc_nettype_t));
            *((gnrc_nettype_t *)value) = dev->proto;
            res = sizeof(gnrc_nettype_t);
            break;
#endif
        case NETOPT_DEVICE_TYPE:
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
            res = sizeof(uint16_t);
            break;
        case NETOPT_IPV6_IID:
            res = _get_iid(dev, value, max_len);
            break;
#ifdef MODULE_NETSTATS_L2
        case NETOPT_STATS:
            assert(max_len == sizeof(uintptr_t));
            *((netstats_t **)value) = &dev->netdev.stats;
            res = sizeof(uintptr_t);
            break;
#endif
#ifdef MODULE_L2FILTER
        case NETOPT_L2FILTER:
            assert(max_len >= sizeof(l2filter_t **));
            *((l2filter_t **)value) = dev->netdev.filter;
            res = sizeof(l2filter_t **);
            break;
#endif
        default:
            break;
    }
    return res;
}

int netdev_ieee802154_set(netdev_ieee802154_t *dev, netopt_t opt, const void *value,
                           size_t len)
{
    int res = -ENOTSUP;

    switch (opt) {
        case NETOPT_CHANNEL:
        {
            assert(len == sizeof(uint16_t));
            uint16_t chan = *((uint16_t *)value);
            /* real validity needs to be checked by device, since sub-GHz and
             * 2.4 GHz band radios have different legal values. Here we only
             * check that it fits in an 8-bit variabl*/
            assert(chan <= UINT8_MAX);
            dev->chan = chan;
            res = sizeof(uint16_t);
            break;
        }
        case NETOPT_ADDRESS:
            assert(len <= sizeof(dev->short_addr));
            memset(dev->short_addr, 0, sizeof(dev->short_addr));
            memcpy(dev->short_addr, value, len);
            res = sizeof(dev->short_addr);
            break;
        case NETOPT_ADDRESS_LONG:
            assert(len <= sizeof(dev->long_addr));
            memset(dev->long_addr, 0, sizeof(dev->long_addr));
            memcpy(dev->long_addr, value, len);
            res = sizeof(dev->long_addr);
            break;
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            assert(len == sizeof(uint16_t));
            res = sizeof(uint16_t);
            switch ((*(uint16_t *)value)) {
                case IEEE802154_SHORT_ADDRESS_LEN:
                    dev->flags &= ~NETDEV_IEEE802154_SRC_MODE_LONG;
                    break;
                case IEEE802154_LONG_ADDRESS_LEN:
                    dev->flags |= NETDEV_IEEE802154_SRC_MODE_LONG;
                    break;
                default:
                    res = -EAFNOSUPPORT;
                    break;
            }
            break;
        case NETOPT_NID:
            assert(len == sizeof(dev->pan));
            dev->pan = *((uint16_t *)value);
            res = sizeof(dev->pan);
            break;
        case NETOPT_ACK_REQ:
            if ((*(bool *)value)) {
                dev->flags |= NETDEV_IEEE802154_ACK_REQ;
            }
            else {
                dev->flags &= ~NETDEV_IEEE802154_ACK_REQ;
            }
            res = sizeof(uint16_t);
            break;
        case NETOPT_RAWMODE:
            if ((*(bool *)value)) {
                dev->flags |= NETDEV_IEEE802154_RAW;
            }
            else {
                dev->flags &= ~NETDEV_IEEE802154_RAW;
            }
            res = sizeof(uint16_t);
            break;
#ifdef MODULE_GNRC
        case NETOPT_PROTO:
            assert(len == sizeof(gnrc_nettype_t));
            dev->proto = *((gnrc_nettype_t *)value);
            res = sizeof(gnrc_nettype_t);
            break;
#endif
#ifdef MODULE_L2FILTER
        case NETOPT_L2FILTER:
            res = l2filter_add(dev->netdev.filter, value, len);
            break;
        case NETOPT_L2FILTER_RM:
            res = l2filter_rm(dev->netdev.filter, value, len);
            break;
#endif
        default:
            break;
    }
    return res;
}

/** @} */
