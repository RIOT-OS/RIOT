/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief   Ethernet adaption for @ref net_gnrc_netif
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF_ETHERNET_H
#define NET_GNRC_NETIF_ETHERNET_H

#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Creates an Ethernet network interface
 *
 * @param[in] stack     The stack for the network interface's thread.
 * @param[in] stacksize Size of @p stack.
 * @param[in] priority  Priority for the network interface's thread.
 * @param[in] name      Name for the network interface. May be NULL.
 * @param[in] dev       Device for the interface.
 *
 * @see @ref gnrc_netif_create()
 *
 * @attention   Fails and crashes (assertion error with @ref DEVELHELP or
 *              segmentation fault without) if `GNRC_NETIF_NUMOF` is lower than
 *              the number of calls to this function.
 *
 * @return  The network interface on success.
 */
gnrc_netif_t *gnrc_netif_ethernet_create(char *stack, int stacksize, char priority,
                                         char *name, netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_ETHERNET_H */
/** @} */
