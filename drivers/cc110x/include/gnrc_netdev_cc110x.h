/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   driver_cc110x
 * @{
 *
 * @file
 * @brief     cc110x gnrc glue code interface
 *
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "net/gnrc/netdev.h"
#include "cc110x-netdev.h"

#ifndef GNRC_NETDEV_CC110X_H
#define GNRC_NETDEV_CC110X_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize cc110x gnrc glue code
 *
 * @param[in] gnrc_netdev  gnrc_netdev state structure to initialize
 * @param[in] dev           cc110x device structure to setup
 *
 * @return 1    on sucess
 * @return <=0  on error
 */
int gnrc_netdev_cc110x_init(gnrc_netdev_t *gnrc_netdev, netdev_cc110x_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETDEV_CC110X_H */
/** @} */
