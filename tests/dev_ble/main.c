/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests 
 * @{
 *
 * @file
 * @brief       Test application for dev_ble low-level bluetooth low-energy drivers
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
#include <string.h>

#include "thread.h"
#include "board.h"
#include "vtimer.h"
#include "periph/spi.h"
#include "periph/gpio.h"

#include "net/dev_ble.h"
#include "dev_ble_autoinit.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

kernel_pid_t handler_pid = KERNEL_PID_UNDEF;

char _rxbuf[2000];

void dev_ble_isr(dev_ble_t *dev) {
    (void)dev;
    thread_wakeup(handler_pid);
}

void dev_ble_rx_handler(dev_ble_t *dev) {
    DEBUG("dev_ble_rx_handler dev=0x%08x\n", (unsigned) dev);

    int n = dev->driver->recv(dev, _rxbuf, sizeof(_rxbuf));

    DEBUG("handle_incoming: received %i bytes\n", n);
}

void dev_ble_linkstate_handler(dev_ble_t *dev, int newstate)
{
    DEBUG("dev_ble: dev=0x%08x link %s\n", (unsigned)dev, newstate ? "UP" : "DOWN");
}

int main(void)
{
    dev_ble_t *const dev = dev_ble_devices[0];

    DEBUG("dev_ble: main\n");
    handler_pid = thread_getpid();

    DEBUG("dev_ble: dev_ble_init\n");
    dev_ble_init(dev);

    while(1) {
        DEBUG("dev_ble: dev->driver->isr\n");
        dev->driver->isr(dev);
        thread_sleep();
        DEBUG("main: woke up\n");
    }

    return 0;
}
