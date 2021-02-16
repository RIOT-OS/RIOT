/*
 * Copyright (C) 2016-21 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include <string.h>

#include "mutex.h"

#include "net/netdev_test/ieee802154.h"

static int _get_device_type(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    assert(max_len == sizeof(uint16_t));
    *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
    return sizeof(uint16_t);
}

static int _get_max_packet_size(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    assert(max_len == sizeof(uint16_t));
    *((uint16_t *)value) = 102U;
    return sizeof(uint16_t);
}

static int _get_src_len(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    assert(max_len == sizeof(uint16_t));
    *((uint16_t *)value) = IEEE802154_LONG_ADDRESS_LEN;
    return sizeof(uint16_t);
}

static int _get_address_long(netdev_t *dev, void *value, size_t max_len)
{
    netdev_test_ieee802154_t *netdev = (netdev_test_ieee802154_t *)dev;
    assert(max_len >= sizeof(netdev->eui64));
    memcpy(value, &netdev->eui64, sizeof(netdev->eui64));
    return sizeof(netdev->eui64);
}

static int _get_proto(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
#if IS_USED(MODULE_GNRC_NETTYPE_SIXLOWPAN)
    assert(max_len == sizeof(gnrc_nettype_t));
    *((gnrc_nettype_t *)value) = GNRC_NETTYPE_SIXLOWPAN;
    return sizeof(gnrc_nettype_t);
#else
    (void)value;
    (void)max_len;
    return -ENOTSUP;
#endif
}

void netdev_test_ieee802154_setup(netdev_test_ieee802154_t *netdev, void *state,
                                  const eui64_t *eui64)
{
    netdev_test_setup(&netdev->netdev, state);
    mutex_lock(&netdev->netdev.mutex);
    if (eui64 != NULL) {
        memcpy(&netdev->eui64, eui64, sizeof(netdev->eui64));
    }
    else {
        netdev_eui64_get((netdev_t *)netdev, &netdev->eui64);
    }
    mutex_unlock(&netdev->netdev.mutex);
    netdev_test_set_get_cb(&netdev->netdev, NETOPT_DEVICE_TYPE,
                           _get_device_type);
    netdev_test_set_get_cb(&netdev->netdev, NETOPT_MAX_PACKET_SIZE,
                           _get_max_packet_size);
    netdev_test_set_get_cb(&netdev->netdev, NETOPT_SRC_LEN, _get_src_len);
    netdev_test_set_get_cb(&netdev->netdev, NETOPT_ADDRESS_LONG,
                           _get_address_long);
    if (IS_USED(MODULE_GNRC_NETTYPE_SIXLOWPAN)) {
        netdev_test_set_get_cb(&netdev->netdev, NETOPT_PROTO, _get_proto);
    }
}

/** @} */
