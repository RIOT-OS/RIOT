/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 RF driver
 */

#include "cc26x2_cc13x2_rf_netdev.h"
#include "cc26x2_cc13x2_rf_internal.h"

/* Reference pointer for the IRQ handler */
static netdev_t *_dev;

void _irq_handler(void)
{
    netdev_trigger_event_isr(_dev);
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    (void)netdev;
    (void)opt;
    (void)value;
    (void)max_len;

    return -ENOTSUP;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *value, size_t value_len)
{
    (void)netdev;
    (void)opt;
    (void)value;
    (void)value_len;

    return -ENOTSUP;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    (void)netdev;
    (void)iolist;

    return -EAGAIN;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void)netdev;
    (void)buf;
    (void)len;
    (void)info;

    return -EAGAIN;
}

static void _isr(netdev_t *netdev)
{
    netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
}

static int _init(netdev_t *netdev)
{
    (void)netdev;
    return 0;
}

const netdev_driver_t cc26x2_cc13x2_rf_driver = {
    .get  = _get,
    .set  = _set,
    .send = _send,
    .recv = _recv,
    .isr  = _isr,
    .init = _init,
};

void cc26x2_cc13x2_rf_setup(cc26x2_cc13x2_rf_t* dev)
{
    dev->rf_handle = cc26x2_cc13x2_rf_open();

    netdev_t *netdev = (netdev_t *)dev;
    netdev->driver = &cc26x2_cc13x2_rf_driver;
}
