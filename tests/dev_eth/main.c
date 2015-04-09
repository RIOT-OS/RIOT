/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Ell-i open source co-operative
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for dev_eth low level ethernet drivers
 *
 * This test application will bounce back every received l2 ethernet
 * frame by exchanging target and destination MAC addresses.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
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

#include "net/dev_eth.h"

#define ENABLE_DEBUG 1
#include "debug.h"

kernel_pid_t handler_pid = KERNEL_PID_UNDEF;

char _rxbuf[2000];

/* reverse src/dst addresses in raw ethernet packet */
void turn_packet(char *pkt) {
    uint8_t mac[6];
    /* save old dst addr */
    memcpy(mac, pkt, 6);

    /* use sender MAC address as new destination */
    memcpy(pkt, pkt+6, 6);

    /* set our MAC address as sender */
    memcpy(pkt+6, mac, 6);
}

#ifdef MODULE_NG_NATIVENET
#include "dev_eth_tap.h"
extern dev_eth_tap_t dev_eth_tap;
dev_eth_t *dev = (dev_eth_t*)&dev_eth_tap;
#endif

void dev_eth_isr(dev_eth_t *dev) {
    (void)dev;
    thread_wakeup(handler_pid);
}

void dev_eth_rx_handler(dev_eth_t *dev) {
    DEBUG("dev_eth_rx_handler dev=0x%08x\n", (unsigned) dev);

    int n = dev->driver->recv(dev, _rxbuf, sizeof(_rxbuf));

    DEBUG("handle_incoming: received %i bytes\n", n);

    if (n>0) {
        turn_packet(_rxbuf);
        dev->driver->send(dev, _rxbuf, n);
    }
}

int main(void)
{
    handler_pid = thread_getpid();

    dev_eth_init(dev);

    while(1) {
        dev->driver->isr(dev);
        thread_sleep();
        DEBUG("main: woke up\n");
    }

    return 0;
}
