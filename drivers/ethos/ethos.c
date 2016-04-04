/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_ethos
 * @{
 *
 * @file
 * @brief       Implementation of a simple ethernet-over-serial driver
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "random.h"
#include "ethos.h"
#include "periph/uart.h"
#include "tsrb.h"
#include "irq.h"

#include "net/netdev2.h"
#include "net/netdev2/eth.h"
#include "net/eui64.h"
#include "net/ethernet.h"

#ifdef USE_ETHOS_FOR_STDIO
#include "uart_stdio.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _get_mac_addr(netdev2_t *dev, uint8_t* buf);
static void ethos_isr(void *arg, uint8_t c);
static const netdev2_driver_t netdev2_driver_ethos;

static const uint8_t _esc_esc[] = {ETHOS_ESC_CHAR, (ETHOS_ESC_CHAR ^ 0x20)};
static const uint8_t _esc_delim[] = {ETHOS_ESC_CHAR, (ETHOS_FRAME_DELIMITER ^ 0x20)};


void ethos_setup(ethos_t *dev, const ethos_params_t *params)
{
    dev->netdev.driver = &netdev2_driver_ethos;
    dev->uart = params->uart;
    dev->state = WAIT_FRAMESTART;
    dev->framesize = 0;
    dev->frametype = 0;
    dev->last_framesize = 0;

    tsrb_init(&dev->inbuf, (char*)params->buf, params->bufsize);
    mutex_init(&dev->out_mutex);

    uint32_t a = random_uint32();
    memcpy(dev->mac_addr, (char*)&a, 4);
    a = random_uint32();
    memcpy(dev->mac_addr+4, (char*)&a, 2);

    dev->mac_addr[0] &= (0x2);      /* unset globally unique bit */
    dev->mac_addr[0] &= ~(0x1);     /* set unicast bit*/

    uart_init(params->uart, params->baudrate, ethos_isr, (void*)dev);

    uint8_t frame_delim = ETHOS_FRAME_DELIMITER;
    uart_write(dev->uart, &frame_delim, 1);
    ethos_send_frame(dev, dev->mac_addr, 6, ETHOS_FRAME_TYPE_HELLO);
}

static void _reset_state(ethos_t *dev)
{
    dev->state = WAIT_FRAMESTART;
    dev->frametype = 0;
    dev->framesize = 0;
}

static void _handle_char(ethos_t *dev, char c)
{
    switch (dev->frametype) {
        case ETHOS_FRAME_TYPE_DATA:
        case ETHOS_FRAME_TYPE_HELLO:
        case ETHOS_FRAME_TYPE_HELLO_REPLY:
             if (tsrb_add_one(&dev->inbuf, c) == 0) {
                dev->framesize++;
            } else {
                //puts("lost frame");
                dev->inbuf.reads = 0;
                dev->inbuf.writes = 0;
                _reset_state(dev);
            }
            break;
#ifdef USE_ETHOS_FOR_STDIO
        case ETHOS_FRAME_TYPE_TEXT:
            dev->framesize++;
            uart_stdio_rx_cb(NULL, c);
#endif
    }
}

static void _end_of_frame(ethos_t *dev)
{
    switch(dev->frametype) {
        case ETHOS_FRAME_TYPE_DATA:
            if (dev->framesize) {
                dev->last_framesize = dev->framesize;
                dev->netdev.event_callback((netdev2_t*) dev, NETDEV2_EVENT_ISR, dev->netdev.isr_arg);
            }
            break;
        case ETHOS_FRAME_TYPE_HELLO:
            ethos_send_frame(dev, dev->mac_addr, 6, ETHOS_FRAME_TYPE_HELLO_REPLY);
            /* fall through */
        case ETHOS_FRAME_TYPE_HELLO_REPLY:
            if (dev->framesize == 6) {
                tsrb_get(&dev->inbuf, (char*)dev->remote_mac_addr, 6);
            }
            break;
    }

    _reset_state(dev);
}

static void ethos_isr(void *arg, uint8_t c)
{
    ethos_t *dev = (ethos_t *) arg;

    switch (dev->state) {
        case WAIT_FRAMESTART:
            if (c == ETHOS_FRAME_DELIMITER) {
                _reset_state(dev);
                dev->state = IN_FRAME;
            }
            break;
        case IN_FRAME:
            if (c == ETHOS_ESC_CHAR) {
                dev->state = IN_ESCAPE;
            }
            else if (c == ETHOS_FRAME_DELIMITER) {
                if (dev->framesize) {
                    _end_of_frame(dev);
                }
            }
            else {
                _handle_char(dev, c);
            }
            break;
        case IN_ESCAPE:
            switch (c) {
                case (ETHOS_FRAME_DELIMITER ^ 0x20):
                    _handle_char(dev, ETHOS_FRAME_DELIMITER);
                    break;
                case (ETHOS_ESC_CHAR ^ 0x20):
                    _handle_char(dev, ETHOS_ESC_CHAR);
                    break;
                case (ETHOS_FRAME_TYPE_TEXT ^ 0x20):
                    dev->frametype = ETHOS_FRAME_TYPE_TEXT;
                    break;
                case (ETHOS_FRAME_TYPE_HELLO ^ 0x20):
                    dev->frametype = ETHOS_FRAME_TYPE_HELLO;
                    break;
                case (ETHOS_FRAME_TYPE_HELLO_REPLY ^ 0x20):
                    dev->frametype = ETHOS_FRAME_TYPE_HELLO_REPLY;
                    break;
            }
            dev->state = IN_FRAME;
            break;
    }
}

static void _isr(netdev2_t *netdev)
{
    ethos_t *dev = (ethos_t *) netdev;
    dev->netdev.event_callback((netdev2_t*) dev, NETDEV2_EVENT_RX_COMPLETE, NULL);
}

static int _init(netdev2_t *encdev)
{
    ethos_t *dev = (ethos_t *) encdev;
    (void)dev;
    return 0;
}

static size_t iovec_count_total(const struct iovec *vector, int count)
{
    size_t result = 0;
    while(count--) {
        result += vector->iov_len;
        vector++;
    }
    return result;
}

static void _write_escaped(uart_t uart, uint8_t c)
{
    uint8_t *out;
    int n;

    switch(c) {
        case ETHOS_FRAME_DELIMITER:
            out = (uint8_t*)_esc_delim;
            n = 2;
            break;
        case ETHOS_ESC_CHAR:
            out = (uint8_t*)_esc_esc;
            n = 2;
            break;
        default:
            out = &c;
            n = 1;
    }

    uart_write(uart, out, n);
}

void ethos_send_frame(ethos_t *dev, const uint8_t *data, size_t len, unsigned frame_type)
{
    uint8_t frame_delim = ETHOS_FRAME_DELIMITER;

    if (!irq_is_in()) {
        mutex_lock(&dev->out_mutex);
    }
    else {
        /* Send frame delimiter. This cancels the current frame,
         * but enables in-ISR writes.  */
        uart_write(dev->uart, &frame_delim, 1);
    }

    /* send frame delimiter */
    uart_write(dev->uart, &frame_delim, 1);

    /* set frame type */
    if (frame_type) {
        uint8_t out[2] = { ETHOS_ESC_CHAR, (frame_type ^ 0x20) };
        uart_write(dev->uart, out, 2);
    }

    /* send frame content */
    while(len--) {
        _write_escaped(dev->uart, *(uint8_t*)data++);
    }

    /* end of frame */
    uart_write(dev->uart, &frame_delim, 1);

    if (!irq_is_in()) {
        mutex_unlock(&dev->out_mutex);
    }
}

static int _send(netdev2_t *netdev, const struct iovec *vector, int count)
{
    ethos_t * dev = (ethos_t *) netdev;
    (void)dev;

    /* count total packet length */
    size_t pktlen = iovec_count_total(vector, count);

    /* lock line in order to prevent multiple writes */
    mutex_lock(&dev->out_mutex);

    /* send start-frame-delimiter */
    uint8_t frame_delim = ETHOS_FRAME_DELIMITER;
    uart_write(dev->uart, &frame_delim, 1);

    /* send iovec */
    while(count--) {
        size_t n = vector->iov_len;
        uint8_t *ptr = vector->iov_base;
        while(n--) {
            _write_escaped(dev->uart, *ptr++);
        }
        vector++;
    }

    uart_write(dev->uart, &frame_delim, 1);

    mutex_unlock(&dev->out_mutex);

    return pktlen;
}

static void _get_mac_addr(netdev2_t *encdev, uint8_t* buf)
{
    ethos_t * dev = (ethos_t *) encdev;
    memcpy(buf, dev->mac_addr, 6);
}

static int _recv(netdev2_t *netdev, char* buf, int len, void* info)
{
    (void) info;
    ethos_t * dev = (ethos_t *) netdev;

    if (buf) {
        if (len < (int)dev->last_framesize) {
            DEBUG("ethos _recv(): receive buffer too small.");
            return -1;
        }

        len = (int)dev->last_framesize;
        dev->last_framesize = 0;

        if ((tsrb_get(&dev->inbuf, buf, len) != len)) {
            DEBUG("ethos _recv(): inbuf doesn't contain enough bytes.");
            return -1;
        }

        return len;
    }
    else {
        return dev->last_framesize;
    }
}

static int _get(netdev2_t *dev, netopt_t opt, void *value, size_t max_len)
{
    int res = 0;

    switch (opt) {
        case NETOPT_ADDRESS:
            if (max_len < ETHERNET_ADDR_LEN) {
                res = -EINVAL;
            }
            else {
                _get_mac_addr(dev, (uint8_t*)value);
                res = ETHERNET_ADDR_LEN;
            }
            break;
        default:
            res = netdev2_eth_get(dev, opt, value, max_len);
            break;
    }

    return res;
}

/* netdev2 interface */
static const netdev2_driver_t netdev2_driver_ethos = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = netdev2_eth_set
};
