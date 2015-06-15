/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     dev_ble_nrf Low-Level Driver Inteface
 * @{
 *
 * @file
 * @brief       Low-level ble driver for the NRF51822 radio interface
 *
 * @author      James Hollister <jhollisterjr@gmail.com>
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef DEV_BLE_NRF_H
#define DEV_BLE_NRF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "net/dev_ble.h"

/**
 * @brief tap interface state
 */
typedef struct dev_ble_nrf {
    dev_ble_t bledev;                   /**< dev_ble internal member */
    int ble_fd;                         /**< host file descriptor for the TAP */
} dev_ble_nrf_t;

/**
 * @brief global device struct. driver only supports one tap device as of now.
 */
extern dev_ble_nrf_t dev_ble_nrf;

/**
 * @brief Setup dev_ble_nrf_t structure
 *
 * @param dev  the preallocated dev_ble_nrf device handle to setup
 */
void dev_ble_nrf_setup(dev_ble_nrf_t *dev);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* DEV_BLE_NRF_H */
