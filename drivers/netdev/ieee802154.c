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
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev.h"
#include "random.h"

#include "net/netdev/ieee802154.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void netdev_ieee802154_reset(netdev_ieee802154_t *dev)
{
    /* Only the least significant byte of the random value is used */
    dev->seq = random_uint32();
    dev->flags = 0;

    /* set default protocol */
#ifdef MODULE_GNRC_SIXLOWPAN
    dev->proto = GNRC_NETTYPE_SIXLOWPAN;
#elif MODULE_GNRC
    dev->proto = GNRC_NETTYPE_UNDEF;
#endif

    /* Initialize PAN ID and call netdev::set to propagate it */
    dev->pan = CONFIG_IEEE802154_DEFAULT_PANID;
    dev->netdev.driver->set(&dev->netdev, NETOPT_NID, &dev->pan, sizeof(dev->pan));

#if IS_USED(MODULE_IEEE802154_SECURITY)
    ieee802154_sec_init(&dev->sec_ctx);
    const netopt_enable_t e = NETOPT_ENABLE;
    netdev_ieee802154_set(dev, NETOPT_ENCRYPTION, &e, sizeof(e));
#endif
}

static inline uint16_t _get_ieee802154_pdu(netdev_ieee802154_t *dev)
{
#if defined(MODULE_NETDEV_IEEE802154_MR_OQPSK) || \
    defined(MODULE_NETDEV_IEEE802154_MR_OFDM)  || \
    defined(MODULE_NETDEV_IEEE802154_MR_FSK)
    uint8_t type = IEEE802154_PHY_DISABLED;
    dev->netdev.driver->get(&dev->netdev, NETOPT_IEEE802154_PHY, &type, sizeof(type));
#else
    (void) dev;
#endif

#ifdef MODULE_NETDEV_IEEE802154_MR_OQPSK
    if (type == IEEE802154_PHY_MR_OQPSK) {
        return IEEE802154G_FRAME_LEN_MAX;
    }
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_OFDM
    if (type == IEEE802154_PHY_MR_OFDM) {
        return IEEE802154G_FRAME_LEN_MAX;
    }
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_FSK
    if (type == IEEE802154_PHY_MR_FSK) {
        return IEEE802154G_FRAME_LEN_MAX;
    }
#endif

    return IEEE802154_FRAME_LEN_MAX;
}

int netdev_ieee802154_get(netdev_ieee802154_t *dev, netopt_t opt, void *value,
                           size_t max_len)
{
    int res = -ENOTSUP;

    (void)max_len;  /* only used in assert() */
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
#if IS_USED(MODULE_IEEE802154_SECURITY)
        case NETOPT_ENCRYPTION:
            assert(max_len == sizeof(netopt_enable_t));
            if (dev->flags & NETDEV_IEEE802154_SECURITY_EN) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            res = sizeof(netopt_enable_t);
            break;
#endif /* IS_USED(MODULE_IEEE802154_SECURITY) */
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
#ifdef MODULE_L2FILTER
        case NETOPT_L2FILTER:
            assert(max_len >= sizeof(l2filter_t **));
            *((l2filter_t **)value) = dev->netdev.filter;
            res = sizeof(l2filter_t **);
            break;
#endif
        case NETOPT_MAX_PDU_SIZE:
            assert(max_len >= sizeof(int16_t));

            *((uint16_t *)value) = (_get_ieee802154_pdu(dev)
                                    - IEEE802154_MAX_HDR_LEN)
#if IS_USED(MODULE_IEEE802154_SECURITY)
                                    -IEEE802154_SEC_MAX_AUX_HDR_LEN
#endif /* IS_USED(MODULE_IEEE802154_SECURITY) */
                                    - IEEE802154_FCS_LEN;
            res = sizeof(uint16_t);
            break;
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
#if IS_USED(MODULE_IEEE802154_SECURITY)
        case NETOPT_ENCRYPTION:
            assert(len == sizeof(netopt_enable_t));
            if ((*(bool *)value)) {
                dev->flags |= NETDEV_IEEE802154_SECURITY_EN;
            }
            else {
                dev->flags &= ~NETDEV_IEEE802154_SECURITY_EN;
            }
            res = sizeof(netopt_enable_t);
            break;
        case NETOPT_ENCRYPTION_KEY:
            assert(len >= IEEE802154_SEC_KEY_LENGTH);
            if (memcmp(dev->sec_ctx.cipher.context.context, value, len)) {
                /* If the key changes, the frame conter can be reset to 0*/
                dev->sec_ctx.frame_counter = 0;
            }
            memcpy(dev->sec_ctx.cipher.context.context, value,
                   IEEE802154_SEC_KEY_LENGTH);
            res = IEEE802154_SEC_KEY_LENGTH;
            break;
#endif /* IS_USED(MODULE_IEEE802154_SECURITY) */
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

int netdev_ieee802154_dst_filter(netdev_ieee802154_t *dev, const uint8_t *mhr)
{
    uint8_t dst_addr[IEEE802154_LONG_ADDRESS_LEN];
    le_uint16_t dst_pan;
    uint8_t pan_bcast[] = IEEE802154_PANID_BCAST;

    int addr_len = ieee802154_get_dst(mhr, dst_addr, &dst_pan);

    /* filter PAN ID */
    if ((memcmp(pan_bcast, dst_pan.u8, 2) != 0) &&
        (memcmp(&dev->pan, dst_pan.u8, 2) != 0)) {
        return 1;
    }

    /* check destination address */
    if (((addr_len == IEEE802154_SHORT_ADDRESS_LEN) &&
          (memcmp(dev->short_addr, dst_addr, addr_len) == 0 ||
           memcmp(ieee802154_addr_bcast, dst_addr, addr_len) == 0)) ||
        ((addr_len == IEEE802154_LONG_ADDRESS_LEN) &&
          (memcmp(dev->long_addr, dst_addr, addr_len) == 0))) {
        return 0;
    }

    return 1;
}
/** @} */
