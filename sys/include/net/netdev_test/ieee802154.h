/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_netdev_test_ieee802154 netdev dummy IEEE 802.15.4 test driver
 * @ingroup     sys_netdev_test
 * @brief   Provides a mock for netdev IEEE 802.15.4 devices
 *
 * Apart from setting a defined set of get callbacks on
 * @ref netdev_test_ieee802154_setup() to fulfill the network stack's
 * constraints, this mock is exactly usable as a @ref sys_netdev_test would be.
 * The initialized get callbacks are the ones for the following options:
 * - @ref NETOPT_DEVICE_TYPE (returns `value == NETDEV_TYPE_IEEE802154`)
 * - @ref NETOPT_MAX_PACKET_SIZE (returns `value == 102U`)
 * - @ref NETOPT_SRC_LEN (returns `value == IEEE802154_LONG_ADDRESS_LEN`)
 * - @ref NETOPT_ADDRESS_LONG
 *   (returns `value == ` netdev_test_ieee802154_t::eui64)
 *
 * Lastly the get callback for `NETOPT_PROTO` is initialized when module
 * `gnrc_nettype_sixlowpan` is compiled in to return `value ==
 * GNRC_NETTYPE_SIXLOWPAN` but will not be supported otherwise
 *
 * @{
 *
 * @file
 * @brief   IEEE 802.15.4 netdev mock definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_NETDEV_TEST_IEEE802154_H
#define NET_NETDEV_TEST_IEEE802154_H

#include "net/eui64.h"

#include "net/netdev_test.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device descriptor for @ref sys_netdev_test_ieee802154 device
 * @extends netdev_test_t
 */
typedef struct {
    netdev_test_t netdev;   /**< superclass */
    eui64_t eui64;          /**< current long address of the device */
} netdev_test_ieee802154_t;

/**
 * @brief   Setup a given @ref sys_netdev_test_ieee802154 device
 *
 * @pre Address range @p eui64 points to must be at least of length
 *      `sizeof(eui64_t)`
 *
 * @param[in] dev       A @ref sys_netdev_test_ieee802154 device to initialize
 * @param[in] state     External state for the device
 * @param[in] eui64     Initial EUI-64 for the device. May be NULL. Must be at
 *                      least of length `sizeof(eui64_t)`.
 *
 *                      This sets netdev_test_ieee802154_t:eui64. If NULL,
 *                      netdev_test_ieee802154_t::eui64 will be
 *                      automatically generated using @ref net_eui_provider.
 */
void netdev_test_ieee802154_setup(netdev_test_ieee802154_t *dev, void *state,
                                  const eui64_t *eui64);

#ifdef __cplusplus
}
#endif

#endif /* NET_NETDEV_TEST_IEEE802154_H */
/** @} */
