/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   net_gnrc
 * @{
 *
 * @file
 * @brief     netdev2 gnrc ethernet glue code interface
 *
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef GNRC_NETDEV2_ETH_H
#define GNRC_NETDEV2_ETH_H

#include "net/gnrc/netdev2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize gnrc handler thread for netdev2 ethernet device
 *
 * @param[in] gnrc_netdev2  gnrc_netdev2 struct to initialize
 * @param[in] dev           netdev2 device to handle
 *
 * @return 1    on success
 * @return <=0  on error
 */
int gnrc_netdev2_eth_init(gnrc_netdev2_t *gnrc_netdev2, netdev2_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETDEV2_ETH_H */
/** @} */
