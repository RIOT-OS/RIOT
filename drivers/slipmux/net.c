/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author  Bennet Hattesen <bennet.hattesen@haw-hamburg.de>
 */

#include "log.h"
#include "slipdev.h"
#include "slipdev_internal.h"
#include "slipdev_params.h"
#include "net/eui_provider.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void _slip_rx_cb(void *arg, uint8_t byte);

static inline void _slipdev_lock(void)
{
    if (IS_USED(MODULE_SLIPDEV_STDIO) || IS_USED(MODULE_SLIPDEV_CONFIG)) {
        mutex_lock(&slipdev_mutex);
    }
}

static inline void _slipdev_unlock(void)
{
    if (IS_USED(MODULE_SLIPDEV_STDIO) || IS_USED(MODULE_SLIPDEV_CONFIG)) {
        mutex_unlock(&slipdev_mutex);
    }
}

static void _poweron(slipdev_t *dev)
{
    if ((dev->state != SLIPDEV_STATE_STANDBY) &&
        (dev->state != SLIPDEV_STATE_SLEEP)) {
        return;
    }

    dev->state = 0;
    uart_init(dev->config.uart, dev->config.baudrate, _slip_rx_cb, dev);
}

static inline void _poweroff(slipdev_t *dev, uint8_t state)
{
    uart_poweroff(dev->config.uart);
    dev->state = state;
}

static int _init(netdev_t *netdev)
{
    slipdev_t *dev = (slipdev_t *)netdev;

    DEBUG("slipdev: initializing device %p on UART %i with baudrate %" PRIu32 "\n",
          (void *)dev, dev->config.uart, dev->config.baudrate);
    /* initialize buffers */
    crb_init(&dev->rb, dev->rxmem, sizeof(dev->rxmem));
    if (uart_init(dev->config.uart, dev->config.baudrate, _slip_rx_cb,
                  dev) != UART_OK) {
        LOG_ERROR("slipdev: error initializing UART %i with baudrate %" PRIu32 "\n",
                  dev->config.uart, dev->config.baudrate);
        return -ENODEV;
    }

    /* signal link UP */
    netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static int _check_state(slipdev_t *dev)
{
    /* power states not supported when multiplexing stdio / configuration */
    if (IS_USED(MODULE_SLIPDEV_STDIO) || IS_USED(MODULE_SLIPDEV_CONFIG)) {
        return 0;
    }

    /* discard data when interface is in SLEEP mode */
    if (dev->state == SLIPDEV_STATE_SLEEP) {
        return -ENETDOWN;
    }

    /* sending data wakes the interface from STANDBY */
    if (dev->state == SLIPDEV_STATE_STANDBY) {
        _poweron(dev);
    }

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    slipdev_t *dev = (slipdev_t *)netdev;
    int bytes = _check_state(dev);

    if (bytes) {
        return bytes;
    }

    DEBUG("slipdev: sending iolist\n");
    _slipdev_lock();
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        uint8_t *data = iol->iol_base;
        slipdev_write_bytes(dev->config.uart, data, iol->iol_len);
        bytes += iol->iol_len;
    }
    slipdev_write_byte(dev->config.uart, SLIPDEV_END);
    _slipdev_unlock();

    return bytes;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    slipdev_t *dev = (slipdev_t *)netdev;
    size_t res = 0;

    (void)info;
    if (buf == NULL) {
        if (len > 0) {
            /* remove data */
            crb_consume_chunk(&dev->rb, NULL, len);
        }
        else {
            /* the user was warned not to use a buffer size > `INT_MAX` ;-) */
            crb_get_chunk_size(&dev->rb, &res);
        }
    }
    else {
        crb_consume_chunk(&dev->rb, buf, len);
        res = len;
    }
    return res;
}

static void _isr(netdev_t *netdev)
{
    slipdev_t *dev = (slipdev_t *)netdev;

    DEBUG("slipdev: handling ISR event\n");

    size_t len;
    while (crb_get_chunk_size(&dev->rb, &len)) {
        DEBUG("slipdev: event handler set, issuing RX_COMPLETE event\n");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
}

#if !(IS_USED(MODULE_SLIPDEV_STDIO) ||  IS_USED(MODULE_SLIPDEV_CONFIG))
static int _set_state(slipdev_t *dev, netopt_state_t state)
{
    if (IS_USED(MODULE_SLIPDEV_STDIO)) {
        return -ENOTSUP;
    }

    switch (state) {
    case NETOPT_STATE_STANDBY:
        _poweroff(dev, SLIPDEV_STATE_STANDBY);
        break;
    case NETOPT_STATE_SLEEP:
        _poweroff(dev, SLIPDEV_STATE_SLEEP);
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

    slipdev_t *dev = (slipdev_t *)netdev;
    switch (opt) {
    case NETOPT_STATE:
        assert(max_len <= sizeof(netopt_state_t));
        return _set_state(dev, *((const netopt_state_t *)value));
    default:
        return -ENOTSUP;
    }
}
#endif /* !(MODULE_SLIPDEV_STDIO || MODULE_SLIPDEV_CONFIG) */

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
#if IS_USED(MODULE_SLIPDEV_L2ADDR)
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
#if (IS_USED(MODULE_SLIPDEV_STDIO) || IS_USED(MODULE_SLIPDEV_CONFIG))
    .set = netdev_set_notsup,
#else
    .set = _set,
#endif
};

void slipdev_setup_net(slipdev_t *dev, uint8_t index) {
    dev->netdev.driver = &slip_driver;

    netdev_register(&dev->netdev, NETDEV_SLIPDEV, index);
}

/** @} */
