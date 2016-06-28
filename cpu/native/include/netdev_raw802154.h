/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     netdev
 * @brief       Low-level ethernet driver for native tap interfaces
 * @{
 *
 * @file
 * @brief       Definitions for @ref netdev ethernet driver for host system's
 *              TAP interfaces
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef NETDEV_RAW802154_H
#define NETDEV_RAW802154_H

#ifdef __cplusplus
extern "C" {
#endif

#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

#ifdef __MACH__
#include "net/if_var.h"
#else
#include "net/if.h"
#endif

/**
 * @brief tap interface state
 */
typedef struct netdev_raw802154 {
    netdev_ieee802154_t netdev;
    char ifname[IFNAMSIZ + 1];
    int fd;
    int rand_addr;
    uint8_t page;
} netdev_raw802154_t;

/**
 * @brief global device struct. driver only supports one tap device as of now.
 */
extern netdev_raw802154_t netdev_raw802154;

/**
 * @brief Setup netdev_tap_t structure.
 *
 * @param dev       the preallocated netdev_tap device handle to setup
 */
void netdev_raw802154_setup(netdev_raw802154_t *dev, const char *ifname, int rand_addr);

/**
 * @brief Cleanup tap resources
 *
 * @param dev  the netdev_tap device handle to cleanup
 */
void netdev_raw802154_cleanup(netdev_raw802154_t *dev);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* NETDEV_RAW802154_H */
