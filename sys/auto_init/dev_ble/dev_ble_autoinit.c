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
 * @brief       Auto-init function for the low-level ble driver
 *
 * @author      James Hollister <jhollisterjr@gmail.com>
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "net/dev_ble.h"
#include "dev_ble_autoinit.h"

#ifdef MODULE_NG_BLE
#include "dev_ble_nrf.h"
#endif

#define ENABLE_DEBUG (1)
#include "debug.h"

dev_ble_t * const dev_ble_devices[] = {
    [DEV_BLE_NRF] = (dev_ble_t*)&dev_ble_nrf
};

void dev_ble_autoinit(void)
{
    DEBUG("dev_ble_autoinit: dev_ble_autoinit()\n");
    return;
}
