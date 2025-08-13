/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     test_utils_netdev_eth_minimal
 *
 * @{
 * @file
 * @brief       Internal definitions for the netdev_eth_minimal module
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include "net/netdev.h"
#include "event.h"
#include "init_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Registry entry to keep track of registered Ethernet network devices
 *
 * Each registered device has an associated event to serve its ISRs.
 */
typedef struct {
    event_t event;      /**< event to serve ISR */
    netdev_t *dev;      /**< pointer to the device */
} device_reg_entry_t;

/**
 * @brief   Registry of Ethernet devices to serve ISRs.
 */
extern device_reg_entry_t _devices[NETDEV_ETH_MINIMAL_NUMOF];

#ifdef __cplusplus
}
#endif

/** @} */
