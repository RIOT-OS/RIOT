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
 * @brief       Header for auto-init function for the low-level ble driver
 *
 * @author      James Hollister <jhollisterjr@gmail.com>
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef DEV_BLE_AUTOINIT_H
#define DEV_BLE_AUTOINIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief enum for available ethernet devices
 */
enum {
#ifdef MODULE_NG_BLE
    DEV_BLE_NRF,
#endif
    /* must be last: */
    NUM_DEV_BLE
};

/**
 * @brief Array of const pointers to available ethernet devices
 */
extern dev_ble_t *const dev_ble_devices[];

/**
 * @brief Automatically sets up available dev_ble ble devices
 *
 * ... by calling the respective *_setup() functions if available.
 */
void dev_ble_autoinit(void);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* DEV_BLE_AUTOINIT_H */
