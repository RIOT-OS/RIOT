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

static void _slip_rx_cb(void *arg, uint8_t byte)
{
    slipdev_t *dev = arg;

    tsrb_add_one(&dev->inbuf, byte);
    if ((byte == SLIP_END) && (dev->netdev.event_callback != NULL)) {
        dev->netdev.event_callback((netdev_t *)dev, NETDEV_EVENT_ISR);
    }
}

static int _init(netdev_t *netdev)
{
    slipdev_t *dev = (slipdev_t *)netdev;

    DEBUG("slipdev: initializing device %p on UART %i with baudrate %" PRIu32 "\n",
          (void *)dev, dev->config.uart, dev->config.baudrate);
    /* initialize buffers */
    tsrb_init(&dev->inbuf, dev->rxmem, sizeof(dev->rxmem));
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

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    slipdev_t *dev = (slipdev_t *)netdev;
    int bytes = 0;

    DEBUG("slipdev: sending iolist\n");
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        uint8_t *data = iol->iol_base;

        for (unsigned j = 0; j < iol->iol_len; j++, data++) {
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

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    slipdev_t *dev = (slipdev_t *)netdev;
    int res = 0;

    (void)info;
    if (buf == NULL) {
        if (len > 0) {
            /* remove data */
            for (; len > 0; len--) {
                int byte = tsrb_get_one(&dev->inbuf);
                if ((byte == (int)SLIP_END) || (byte < 0)) {
                    /* end early if end of packet or ringbuffer is reached;
                     * len might be larger than the actual packet */
                    break;
                }
            }
        } else {
            /* the user was warned not to use a buffer size > `INT_MAX` ;-) */
            res = (int)tsrb_avail(&dev->inbuf);
        }
    }
    else {
        int byte;
        uint8_t *ptr = buf;

        do {
            if ((byte = tsrb_get_one(&dev->inbuf)) < 0) {
                /* something went wrong, return error */
                return -EIO;
            }
            switch (byte) {
                case SLIP_END:
                    break;
                case SLIP_ESC:
                    dev->inesc = 1;
                    break;
                case SLIP_END_ESC:
                    if (dev->inesc) {
                        *(ptr++) = SLIP_END;
                        res++;
                        dev->inesc = 0;
                        break;
                    }
                    /* Intentionally falls through */
                    /* to default when !dev->inesc */
                case SLIP_ESC_ESC:
                    if (dev->inesc) {
                        *(ptr++) = SLIP_ESC;
                        res++;
                        dev->inesc = 0;
                        break;
                    }
                    /* Intentionally falls through */
                    /* to default when !dev->inesc */
                default:
                    *(ptr++) = (uint8_t)byte;
                    res++;
                    break;
            }
            if ((unsigned)res > len) {
                while (byte != SLIP_END) {
                    /* clear out unreceived packet */
                    byte = tsrb_get_one(&dev->inbuf);
                }
                return -ENOBUFS;
            }
        } while (byte != SLIP_END);
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
    dev->inesc = 0U;
    dev->netdev.driver = &slip_driver;
}

/** @} */
