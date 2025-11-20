/*
 * SPDX-FileCopyrightText: 2020 Dylan Laduranty
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup     cpu_sam0_common_eth sam0 Ethernet peripheral
 * @ingroup      cpu_sam0_common
 * @{
 *
 * @file
 * @brief       Netdev interface for the SAM0 Ethernet GMAC peripheral
 *
 * @author      Dylan Laduranty <dylanladuranty@gmail.com>
 */

#include <stdbool.h>

#include "net/ethernet.h"
#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t sam0_eth_driver;

/**
 * @brief   Device descriptor for SAM0-ETH devices
 */
typedef struct {
    netdev_t* netdev;                    /**< netdev parent struct */
} sam0_eth_netdev_t;

/**
 * @brief Setup SAM0 Ethernet peripheral
 *
 * @param[in] dev   Pointer to the netdev struct
 *
 */
void sam0_eth_setup(netdev_t* dev);

#ifdef __cplusplus
}
#endif

/** @} */
