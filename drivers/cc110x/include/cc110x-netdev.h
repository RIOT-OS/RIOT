/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Variables for the cc110x netdev interface
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef CC110X_NETDEV_H
#define CC110X_NETDEV_H

#include "periph/gpio.h"
#include "periph/spi.h"
#include "net/netdev.h"
#include "cc110x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Implementation of netdev_driver_t for CC110X device
 */
extern const netdev_driver_t netdev_cc110x_driver;

/**
 * @brief cc110x netdev struct
 */
typedef struct netdev_cc110x {
    netdev_t netdev;        /**< writing obious */
    cc110x_t cc110x;        /**< documentation here */
} netdev_cc110x_t;

/**
 * @brief   Received packet status information for cc110x radios
 */
typedef struct netdev_radio_rx_info netdev_cc110x_rx_info_t;

/**
 * @brief netdev <-> cc110x glue code initialization function
 *
 * @param[out]      netdev_cc110x  ptr to netdev_cc110x struct ti initialize
 * @param[in]       params          cc110x IO parameter struct to use
 *
 * @return          0               on success
 * @return          -1              on error
 */
int netdev_cc110x_setup(netdev_cc110x_t *netdev_cc110x, const cc110x_params_t *params);

#ifdef __cplusplus
}
#endif

#endif /* CC110X_NETDEV_H */
/** @} */
