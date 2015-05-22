/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  netdev
 * @{
 *
 * @file
 * @brief       Provides wrappers of @ref netdev/base.h functions to
 *              netdev/802154.h functions.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "netdev/802154.h"

static inline netdev_802154_driver_t *_get_driver(netdev_t *dev)
{
    return (netdev_802154_driver_t *)dev->driver;
}

/* define to implement yourself and omit compilation of this function */
#ifndef NETDEV_802154_SEND_DATA_OVERLOAD
static size_t _get_src_len(netdev_t *dev)
{
    size_t src_len, src_len_len = sizeof(size_t);

    if (_get_driver(dev)->get_option(dev, NETDEV_OPT_SRC_LEN, &src_len, &src_len_len) < 0) {
        return 2;
    }

    return src_len;
}

int netdev_802154_send_data(netdev_t *dev, void *dest, size_t dest_len,
                            netdev_hlist_t *upper_layer_hdrs, void *data,
                            size_t data_len)
{
    int use_long_addr;

    if (dev == NULL || dev->type != NETDEV_TYPE_802154) {
        return -ENODEV;
    }

    use_long_addr = _get_src_len(dev) == 8;

    if (dest_len != 8 && dest_len != 4) { /* 8 for EUI-64, 4 for short address + PAN ID*/
        return -EAFNOSUPPORT;
    }

    switch (_get_driver(dev)->send(dev, NETDEV_802154_PKT_KIND_DATA,
                                   (netdev_802154_node_addr_t *)dest,
                                   use_long_addr, 0, upper_layer_hdrs,
                                   data, data_len)) {
        case NETDEV_802154_TX_STATUS_OK:
            return (int)(data_len + netdev_get_hlist_len(upper_layer_hdrs));

        case NETDEV_802154_TX_STATUS_MEDIUM_BUSY:
        case NETDEV_802154_TX_STATUS_COLLISION:
            return -EBUSY;

        case NETDEV_802154_TX_STATUS_INVALID_PARAM:
            return -EINVAL;

        case NETDEV_802154_TX_STATUS_PACKET_TOO_LONG:
            return -EMSGSIZE;

        case NETDEV_802154_TX_STATUS_ERROR:
        default:
            return -EIO;
            /* NETDEV_802154_TX_STATUS_UNDERFLOW can not happen since
             * netdev_802154_driver_t::send always calls
             * netdev_802154_driver_t::load_tx */
            /* NETDEV_802154_TX_STATUS_NOACK can not happen since
             * wants_ack == 0 */
    }
}
#endif /* NETDEV_802154_SEND_DATA_OVERLOAD */

/* define to implement yourself and omit compilation of this function */
#ifndef NETDEV_802154_SEND_OVERLOAD
netdev_802154_tx_status_t netdev_802154_send(netdev_t *dev,
        netdev_802154_pkt_kind_t kind,
        netdev_802154_node_addr_t *dest,
        int use_long_addr,
        int wants_ack,
        netdev_hlist_t *upper_layer_hdrs,
        void *buf,
        unsigned int len)
{
    netdev_802154_tx_status_t status;

    if (dev == NULL || dev->type != NETDEV_TYPE_802154) {
        return NETDEV_802154_TX_STATUS_NO_DEV;
    }

    status = _get_driver(dev)->load_tx(dev, kind, dest, use_long_addr, wants_ack,
                                       upper_layer_hdrs, buf, len);

    if (status != NETDEV_802154_TX_STATUS_OK) {
        return status;
    }

    return _get_driver(dev)->transmit(dev);
}
#endif /* NETDEV_802154_SEND_OVERLOAD */

/**
 * @}
 */
