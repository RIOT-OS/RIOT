/*
 * Copyright (C) 2015 James Hollister
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        drivers_nrf51822_ble NRF BLE Radio Driver
 * @ingroup         drivers
 * @brief           BLE driver for the NRF51822 radio
 *
 *
 * @{
 *
 * @file
 * @brief           Interface definition for the RF51822 ble driver
 *
 * @author          James Hollister <jhollisterjr@gmail.com>
 */

#ifndef NRF_BLE_H_
#define NRF_BLE_H_

#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev driver interface
 */
extern const gnrc_netdev_driver_t ble_driver;

/**
 * @brief   Initialize the NRF51822 ble radio
 *
 * The initialization uses static configuration values.
 *
 * @param dev[out]      pointer to the netdev device descriptor
 *
 * @return              0 on success
 * @return              -ENODEV if @p dev is invalid
 */
int nrf51_ble_init(gnrc_netdev_t *dev);


#ifdef __cplusplus
}
#endif

#endif /* NRF_BLE_H_ */
/** @} */
