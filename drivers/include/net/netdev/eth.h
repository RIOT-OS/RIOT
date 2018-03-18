/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     drivers_netdev_api
 * @{
 *
 * @file
 * @brief       Definitions for netdev common ethernet code
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef NET_NETDEV_ETH_H
#define NET_NETDEV_ETH_H

#include <stdint.h>

#include "net/netdev.h"
#include "net/netopt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Layer descriptor for ethernet netdev layer
 */
typedef struct {
    netdev_t netdev;    /**< Netdev layer parent struct. */
    uint8_t type;       /**< Device type, used to determine multicast detection. */
    netdev_t *hwdev;    /**< Pointer to the hardware driver struct. */
} netdev_eth_t;

/**
 * @brief   Add a ethernet netdev layer to the top of the netdev stack
 *
 * @param[in] head    Top netdev device of the stack.
 * @param[in] layer   New ethernet layer to push to the top of the stack.
 *
 * @return  The new top of the netdev stack.
 */
netdev_t *netdev_eth_add(netdev_t *head, netdev_eth_t *layer);

#ifdef __cplusplus
}
#endif

#endif /* NET_NETDEV_ETH_H */
/** @} */
