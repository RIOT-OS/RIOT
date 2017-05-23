/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netdev
 * @brief
 * @{
 *
 * @file
 * @brief   netdev gnrc IEEE 802.15.4 glue code interface
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETDEV_IEEE802154_H
#define NET_GNRC_NETDEV_IEEE802154_H

#include "net/netdev/ieee802154.h"
#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize gnrc handler for netdev IEEE 802.15.4 device
 *
 * @param[in] gnrc_netdev  gnrc_netdev struct to initialize
 * @param[in] dev           netdev device to handle
 *
 * @return 1    on success
 * @return <=0  on error
 */
int gnrc_netdev_ieee802154_init(gnrc_netdev_t *gnrc_netdev,
                                 netdev_ieee802154_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETDEV_IEEE802154_H */
/** @} */
