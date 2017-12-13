/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <string.h>

#include "log.h"
#include "slipdev.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define SLIP_END               (0xc0U)
#define SLIP_ESC               (0xdbU)
#define SLIP_END_ESC           (0xdcU)
#define SLIP_ESC_ESC           (0xddU)

static int _send(netdev_t *dev, const struct iovec *vector, unsigned count);
static int _recv(netdev_t *dev, void *buf, size_t len, void *info);
static int _init(netdev_t *dev);
static void _isr(netdev_t *dev);
static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len);
static int _set(netdev_t *dev, netopt_t opt, const void *value,
                size_t value_len);

static const netdev_driver_t slip_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

void slipdev_setup(slipdev_t *dev, const slipdev_params_t *params)
{
    /* set device descriptor fields */
    memcpy(&dev->config, params, sizeof(dev->config));
    dev->inbytes = 0U;
    dev->inesc = 0U;
    dev->netdev.driver = &slip_driver;
}

static inline void _add_byte_to_inbuf(slipdev_t *dev, uint8_t byte)
{
    if (ringbuffer_add_one(&dev->inbuf, byte) < 0) {
        dev->inbytes++;
    }
}

static void _slip_rx_cb(void *arg, uint8_t data)
{
    slipdev_t *dev = arg;

    if ((data == SLIP_END) && (dev->netdev.event_callback != NULL)) {
        int idx = cib_put(&dev->pktfifo_idx);
        if (idx >= 0) {
            dev->netdev.event_callback((netdev_t *)dev, NETDEV_EVENT_ISR);
            dev->pktfifo[idx] = dev->inbytes;
        }
        else {
            /* can't handover packet => dropping it */
            ringbuffer_remove(&dev->inbuf, dev->inbytes);
        }
        dev->inbytes = 0;
    }
    else if (dev->inesc) {
        dev->inesc = 0U;
        uint8_t actual = (data == SLIP_END_ESC) ? SLIP_END :
                         ((data == SLIP_ESC_ESC) ? SLIP_ESC : 0);

        switch (data) {
            case SLIP_END_ESC:
            case SLIP_ESC_ESC:
                _add_byte_to_inbuf(dev, actual);
                break;
            default:
                break;
        }
    }
    else if (data == SLIP_ESC) {
        dev->inesc = 1U;
    }
    else {
        _add_byte_to_inbuf(dev, data);
    }
}

static int _init(netdev_t *netdev)
{
    slipdev_t *dev = (slipdev_t *)netdev;

    DEBUG("slipdev: initializing device %p on UART %i with baudrate %" PRIu32 "\n",
          (void *)dev, dev->config.uart, dev->config.baudrate);
    /* initialize buffers */
    ringbuffer_init(&dev->inbuf, dev->rxmem, sizeof(dev->rxmem));
    cib_init(&dev->pktfifo_idx, SLIPDEV_PKTFIFO_SIZE);
    if (uart_init(dev->config.uart, dev->config.baudrate, _slip_rx_cb,
                  dev) != UART_OK) {
        LOG_ERROR("slipdev: error initializing UART %i with baudrate %" PRIu32 "\n",
                  dev->config.uart, dev->config.baudrate);
        return -ENODEV;
    }
    return 0;
}

static inline void _write_byte(slipdev_t *dev, uint8_t byte)
{
    uart_write(dev->config.uart, &byte, 1);
}

static int _send(netdev_t *netdev, const struct iovec *vector, unsigned count)
{
    slipdev_t *dev = (slipdev_t *)netdev;
    int bytes = 0;

    DEBUG("slipdev: sending vector of length %u\n", count);
    for (unsigned i = 0; i < count; i++) {
        uint8_t *data = vector[i].iov_base;

        for (unsigned j = 0; j < vector[i].iov_len; j++, data++) {
            switch(*data) {
                case SLIP_END:
                    /* escaping END byte*/
                    _write_byte(dev, SLIP_ESC);
                    _write_byte(dev, SLIP_END_ESC);
                    break;
                case SLIP_ESC:
                    /* escaping ESC byte*/
                    _write_byte(dev, SLIP_ESC);
                    _write_byte(dev, SLIP_ESC_ESC);
                    break;
                default:
                    _write_byte(dev, *data);
            }
            bytes++;
        }
    }
    _write_byte(dev, SLIP_END);
    return bytes;
}

static void wait(void) {
    /* waits a few instructions */
    static volatile int i = 0;
    (void)i;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    slipdev_t *dev = (slipdev_t *)netdev;
    int res, idx = cib_peek(&dev->pktfifo_idx);

    (void)info;
    if (idx < 0) {
        return -EFAULT;
    }
    if (buf == NULL) {
        if (len > 0) {
            /* drop packet */
            cib_get(&dev->pktfifo_idx);
            /* and remove data */
            res = ringbuffer_remove(&dev->inbuf, len);
        }
        else {
            res = dev->pktfifo[idx];
        }
    }
    else if (len < dev->pktfifo[idx]) {
        res = -ENOBUFS;
    }
    else {
        size_t bytes = dev->pktfifo[cib_get(&dev->pktfifo_idx)];
        wait(); /* wait a very little while to prevent race-condition */
        bytes = ringbuffer_get(&dev->inbuf, buf, bytes);
        res = bytes;
    }
    return res;
}

static void _isr(netdev_t *netdev)
{
    DEBUG("slipdev: handling ISR event\n");
    if (netdev->event_callback != NULL) {
        DEBUG("slipdev: event handler set, issuing RX_COMPLETE event\n");
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
}

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
        default:
            return -ENOTSUP;
    }
}

static int _set(netdev_t *netdev, netopt_t opt, const void *value,
                size_t value_len)
{
    (void)netdev;
    (void)opt;
    (void)value;
    (void)value_len;
    return -ENOTSUP;
}

/** @} */
