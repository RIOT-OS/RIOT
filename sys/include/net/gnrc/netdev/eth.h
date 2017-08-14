/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netdev
 * @{
 *
 * @file
 * @brief     netdev gnrc ethernet glue code interface
 *
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef NET_GNRC_NETDEV_ETH_H
#define NET_GNRC_NETDEV_ETH_H

#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize gnrc handler thread for netdev ethernet device
 *
 * @param[in] gnrc_netdev  gnrc_netdev struct to initialize
 * @param[in] dev           netdev device to handle
 *
 * @return 1    on success
 * @return <=0  on error
 */
int gnrc_netdev_eth_init(gnrc_netdev_t *gnrc_netdev, netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETDEV_ETH_H */
/** @} */
