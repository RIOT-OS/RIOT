/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_netdev
 * @{
 *
 * @file
 * @author  José I. Alamos <jose.alamos@haw-hamburg.de>
 */

#include "net/netdev.h"

void netdev_event_thread_handler(event_t *event) {
    netdev_t *dev = container_of(event, netdev_t, irq_handler);
    dev->driver->isr(dev);
}
