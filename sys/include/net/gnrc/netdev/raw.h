/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief   netdev gnrc raw (i.e. pure L3) glue code interface
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETDEV_RAW_H
#define NET_GNRC_NETDEV_RAW_H

#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize gnrc handler for netdev raw L3 devices
 *
 * @param[in] gnrc_netdev   gnrc_netdev struct to initialize
 * @param[in] dev           network device to handle
 *
 * @return 1 on success
 * @return <=0 on error
 */
int gnrc_netdev_raw_init(gnrc_netdev_t *gnrc_netdev, netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETDEV_RAW_H */
/** @} */
