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
 * @brief       Low-level bluetooth low-energy driver for the NRF51822 radio interface
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

#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#include "net/dev_ble.h"
#include "dev_ble_nrf.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

/* support one tap interface for now */
dev_ble_nrf_t dev_ble_nrf;

/* dev_ble interface */
static int _init(dev_ble_t *bledev);
static int _send(dev_ble_t *bledev, char* buf, int n);
static int _recv(dev_ble_t *bledev, char* buf, int n);

static int _recv(dev_ble_t *bledev, char *buf, int len) {
    dev_ble_nrf_t *dev = (dev_ble_nrf_t*)bledev;
    DEBUG("dev_ble_nrf: _recv (dev=0x%08x)\n", (unsigned) dev);
    return 0;
}

static int _send(dev_ble_t *bledev, char* buf, int n) {
    dev_ble_nrf_t *dev = (dev_ble_nrf_t*)bledev;
    DEBUG("dev_ble_nrf: _send (dev=0x%08x)\n", (unsigned) dev);
    return 0;
}

static inline void _isr(dev_ble_t *bledev) {
    DEBUG("dev_ble_nrf: _isr(dev=0x%08x)\n", (unsigned) bledev);
    dev_ble_rx_handler(bledev);
}

static int _init(dev_ble_t *bledev)
{
    DEBUG("_init\n");
    dev_ble_nrf_t *dev = (dev_ble_nrf_t*)bledev;
    DEBUG("dev_ble_nrf: _init (dev=0x%08x)\n", (unsigned) dev);

    /* check device parametrs */
    if (dev == NULL) {
        return -ENODEV;
    }
    return 1;
}

static ble_driver_t ble_driver_nrf = {
    .init = _init,
    .send = _send,
    .recv = _recv,
    .isr = _isr,
};

void dev_ble_nrf_setup(dev_ble_nrf_t *dev) {
    dev->bledev.driver = &ble_driver_nrf;
}
