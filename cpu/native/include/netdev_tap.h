/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     drivers_netdev
 * @brief       Low-level ethernet driver for native tap interfaces
 * @{
 *
 * @file
 * @brief       Definitions for @ref netdev ethernet driver for host system's
 *              TAP interfaces
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef NETDEV_TAP_H
#define NETDEV_TAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "net/netdev.h"

#include "net/ethernet/hdr.h"

#ifdef __MACH__
#include "net/if_var.h"
#else
#include "net/if.h"
#endif

/**
 * @brief tap interface state
 */
typedef struct netdev_tap {
    netdev_t netdev;                    /**< netdev internal member */
    char tap_name[IFNAMSIZ];            /**< host dev file name */
    int tap_fd;                         /**< host file descriptor for the TAP */
    uint8_t addr[ETHERNET_ADDR_LEN];    /**< The MAC address of the TAP */
    uint8_t promiscuous;                 /**< Flag for promiscuous mode */
} netdev_tap_t;

/**
 * @brief tap interface initialization parameters
 */
typedef struct {
    char **tap_name;                    /**< Name of the host system's tap
                                             interface to bind to. */
} netdev_tap_params_t;

/**
 * @brief Setup netdev_tap_t structure.
 *
 * @param dev       the preallocated netdev_tap device handle to setup
 * @param params    initialization parameters
 */
void netdev_tap_setup(netdev_tap_t *dev, const netdev_tap_params_t *params);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* NETDEV_TAP_H */
