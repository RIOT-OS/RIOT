/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author  Bennet Hattesen <bennet.hattesen@haw-hamburg.de>
 */

#include <assert.h>

#include "chunked_ringbuffer.h"
#include "slipmux_internal.h"
#include "slipmux.h"

#include "net/eui_provider.h"
#include "net/gnrc/netif/raw.h"
#include "net/gnrc.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define SLIPDEV_STACKSIZE       (THREAD_STACKSIZE_DEFAULT - 128)
#ifndef SLIPDEV_PRIO
#  define SLIPDEV_PRIO            (GNRC_NETIF_PRIO)
#endif
/** @} */

#if IS_USED(MODULE_GNRC)
static char _slipdev_stacks[SLIPMUX_DEV_NUM][SLIPDEV_STACKSIZE];
static gnrc_netif_t _netif[SLIPMUX_DEV_NUM];
#endif

static void _poweron(slipmux_t *dev)
{
    if ((dev->state != SLIPMUX_STATE_STANDBY) &&
            (dev->state != SLIPMUX_STATE_SLEEP)) {
        return;
    }

    dev->state = SLIPMUX_STATE_NONE;
    uart_init(dev->config.uart, dev->config.baudrate, slipmux_rx_cb, dev);
}

static inline void _poweroff(slipmux_t *dev, uint8_t state)
{
    uart_poweroff(dev->config.uart);
    dev->state = state;
}

static int _init(netdev_t *netdev)
{
    /* signal link UP */
    netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static int _check_state(slipmux_t *dev)
{
    /* power states not supported when multiplexing stdio / configuration */
    if (IS_USED(MODULE_SLIPMUX_STDIO) || IS_USED(MODULE_SLIPMUX_COAP)) {
        return 0;
    }

    /* discard data when interface is in SLEEP mode */
    if (dev->state == SLIPMUX_STATE_SLEEP) {
        return -ENETDOWN;
    }

    /* sending data wakes the interface from STANDBY */
    if (dev->state == SLIPMUX_STATE_STANDBY) {
        _poweron(dev);
    }

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    slipmux_t *dev = container_of(netdev, slipmux_t, netdev);
    int bytes = _check_state(dev);
    if (bytes) {
        return bytes;
    }

    slipmux_lock(dev);
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        uint8_t *data = iol->iol_base;
        slipmux_write_bytes(dev->config.uart, data, iol->iol_len);
        bytes += iol->iol_len;
    }
    slipmux_write_byte(dev->config.uart, SLIPMUX_END);
    slipmux_unlock(dev);
    return bytes;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    slipmux_t *dev = container_of(netdev, slipmux_t, netdev);
    size_t res = 0;

    (void)info;
    if (buf == NULL) {
        if (len > 0) {
            /* remove data */
            crb_consume_chunk(&dev->net_rb, NULL, len);
        } else {
            /* the user was warned not to use a buffer size > `INT_MAX` ;-) */
            crb_get_chunk_size(&dev->net_rb, &res);
        }
    }
    else {
        crb_consume_chunk(&dev->net_rb, buf, len);
        res = len;
    }
    return res;
}

static void _isr(netdev_t *netdev)
{
    slipmux_t *dev = container_of(netdev, slipmux_t, netdev);

    size_t len;
    while (crb_get_chunk_size(&dev->net_rb, &len)) {
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
}

#if !(IS_USED(MODULE_SLIPMUX_STDIO) ||  IS_USED(MODULE_SLIPMUX_COAP))
static int _set_state(slipmux_t *dev, netopt_state_t state)
{
    switch (state) {
    case NETOPT_STATE_STANDBY:
        _poweroff(dev, SLIPMUX_STATE_STANDBY);
        break;
    case NETOPT_STATE_SLEEP:
        _poweroff(dev, SLIPMUX_STATE_SLEEP);
        break;
    case NETOPT_STATE_IDLE:
        _poweron(dev);
        break;
    default:
        return -ENOTSUP;
    }

    return sizeof(netopt_state_t);
}

static int _set(netdev_t *netdev, netopt_t opt, const void *value, size_t max_len)
{
    (void)max_len;

    slipmux_t *dev = container_of(netdev, slipmux_t, netdev);
    switch (opt) {
    case NETOPT_STATE:
        assert(max_len <= sizeof(netopt_state_t));
        return _set_state(dev, *((const netopt_state_t *)value));
    default:
        return -ENOTSUP;
    }
}
#endif /* !(MODULE_SLIPMUX_STDIO || MODULE_SLIPMUX_COAP) */

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    (void)netdev;
    (void)value;
    (void)max_len;
    switch (opt) {
        case NETOPT_IS_WIRED:
            return 1;
        case NETOPT_DEVICE_TYPE:
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)value) = NETDEV_TYPE_SLIP;
            return sizeof(uint16_t);
#if IS_USED(MODULE_SLIPMUX_NET_L2ADDR)
        case NETOPT_ADDRESS_LONG:
            assert(max_len == sizeof(eui64_t));
            netdev_eui64_get(netdev, value);
            return sizeof(eui64_t);
#endif
        default:
            return -ENOTSUP;
    }
}

static int _confirm_send(netdev_t *netdev, void *info)
{
    (void)netdev;
    (void)info;
    return -EOPNOTSUPP;
}

static const netdev_driver_t slip_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .confirm_send = _confirm_send,
#if !(IS_USED(MODULE_SLIPMUX_STDIO) || IS_USED(MODULE_SLIPMUX_COAP))
    .set = _set,
#else
    .set = netdev_set_notsup,
#endif
};

void slipmux_net_notify(slipmux_t *dev)
{
    netdev_trigger_event_isr(&dev->netdev);
}

void slipmux_net_init(slipmux_t * dev, unsigned index)
{
    dev->netdev.driver = &slip_driver;
    crb_init(&dev->net_rb, dev->net_rx, sizeof(dev->net_rx));
    netdev_register(&dev->netdev, NETDEV_SLIPDEV, index);

#if IS_USED(MODULE_GNRC)
    gnrc_netif_raw_create(&_netif[index], _slipdev_stacks[index], SLIPDEV_STACKSIZE,
                              SLIPDEV_PRIO, "slipdev",
                              &dev->netdev);
#endif
}

/** @} */
