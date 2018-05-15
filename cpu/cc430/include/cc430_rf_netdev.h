/*
 * Copyright (C) 2018 Baptiste Cartier
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     cpu_cc430
 * @{
 *
 * @file
 * @brief       Netdev interface to CC430 radio driver, based on the cc110x Netdev interface
 *
 * @author      Baptiste Cartier
 */

#ifndef CC430_RF_NETDEV_H
#define CC430_RF_NETDEV_H

#include "net/netdev.h"
#include "cc430_rf_internal.h"
#include "cc430_rf_getset.h"
#include <errno.h>

#include "net/gnrc.h"


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
    netdev_t netdev;
    cc110x_t cc110x;
} netdev_cc110x_t;

/**
 * @brief   Received packet status information for cc110x radios
 */
typedef struct netdev_radio_rx_info netdev_cc110x_rx_info_t;

void _irq_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* CC430_RF_NETDEV_H */
/** @} */
