/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

/**
 * @addtogroup drivers_netdev
 * @{
 */

/**
 * @file
 * @brief  Definitions for @ref netdev ethernet driver for host system's
 *         TAP interfaces
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "net/netdev.h"
#include "net/eui48.h"
#include "net/ethernet/hdr.h"

#include "net/if.h"

/* MARK: - Low-level ethernet driver for native tap interfaces */
/**
 * @name Low-level ethernet driver for native tap interfaces
 * @{
 */
/**
 * @brief tap interface state
 */
typedef struct netdev_tap {
    netdev_t netdev;                    /**< netdev internal member */
    char tap_name[IFNAMSIZ];            /**< host dev file name */
    int tap_fd;                         /**< host file descriptor for the TAP */
    uint8_t addr[ETHERNET_ADDR_LEN];    /**< The MAC address of the TAP */
    bool promiscuous;                   /**< Flag for promiscuous mode */
    bool wired;                         /**< Flag for wired mode */
} netdev_tap_t;

/**
 * @brief tap interface initialization parameters
 */
typedef struct {
    char **tap_name;                    /**< Name of the host system's tap
                                             interface to bind to. */
    bool wired;                         /**< Interface should behave like a
                                             wired interface. */
} netdev_tap_params_t;

/**
 * @brief Setup netdev_tap_t structure.
 *
 * @param dev       the preallocated netdev_tap device handle to setup
 * @param params    initialization parameters
 * @param index     Index of @p params in a global parameter struct array.
 *                  If initialized manually, pass a unique identifier instead.
 */
void netdev_tap_setup(netdev_tap_t *dev, const netdev_tap_params_t *params, int index);

/**
 * @brief   Get the EUI48 of the host side tap interface
 *
 * @param dev   tap device to query
 * @param out   struct to write the result to
 */
void netdev_tap_get_host_eui48(netdev_tap_t *dev, eui48_t *out);
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
