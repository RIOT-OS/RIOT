/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     netdev2
 * @brief       Low-level ethernet driver for native tap interfaces
 * @{
 *
 * @file
 * @brief       Definitions for @ref netdev2 ethernet driver for host system's
 *              TAP interfaces
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef NETDEV2_RAW802154_H
#define NETDEV2_RAW802154_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "net/netdev2.h"
#include "net/netdev2/ieee802154.h"

#ifdef __MACH__
#include "net/if_var.h"
#else
#include "net/if.h"
#endif

/**
 * @brief tap interface state
 */
typedef struct netdev2_raw802154 {
    netdev2_ieee802154_t netdev;
    char ifname[IFNAMSIZ + 1];
    int fd;
    int rand_addr;
    uint8_t page;
} netdev2_raw802154_t;

/**
 * @brief global device struct. driver only supports one tap device as of now.
 */
extern netdev2_raw802154_t netdev2_raw802154;

/**
 * @brief Setup netdev2_tap_t structure.
 *
 * @param dev       the preallocated netdev2_tap device handle to setup
 */
void netdev2_raw802154_setup(netdev2_raw802154_t *dev, const char *ifname, int rand_addr);

/**
 * @brief Cleanup tap resources
 *
 * @param dev  the netdev2_tap device handle to cleanup
 */
void netdev2_raw802154_cleanup(netdev2_raw802154_t *dev);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* NETDEV2_RAW802154_H */
