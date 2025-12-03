/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
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

/* XXX: BE CAREFUL ABOUT USING OUTPUT WITH MODULE_SLIPMUX_STDIO IN SENDING
 * FUNCTIONALITY! MIGHT CAUSE DEADLOCK!!!1!! */
#define ENABLE_DEBUG 0
#include "debug.h"

#include "auto_init_utils.h"
#include "chunked_ringbuffer.h"
#include "isrpipe.h"
#include "mutex.h"
#include "periph/uart.h"

#include "slipmux_internal.h"
#include "slipmux_params.h"

slipmux_t slipmux_devs[SLIPMUX_DEV_NUM];

void slipmux_rx_cb(void *arg, uint8_t byte)
{
    slipmux_t *dev = arg;

    switch (dev->state) {
    case SLIPMUX_STATE_STANDBY:
        /* fall through */
    case SLIPMUX_STATE_SLEEP:
        /* do nothing if we are supposed to sleep */
        /* and we should usually not be able to hit this case anyways */
        assert(0);
        break;
    case SLIPMUX_STATE_STDIN:
        switch (byte) {
        case SLIPMUX_ESC:
            dev->state = SLIPMUX_STATE_STDIN_ESC;
            break;
        case SLIPMUX_END:
            dev->state = SLIPMUX_STATE_NONE;
            break;
        default:
#if IS_USED(MODULE_SLIPMUX_STDIO)
            if (dev->config.uart == STDIO_UART_DEV) {
                isrpipe_write_one(&stdin_isrpipe, byte);
            }
#endif
            break;
        }
        return;
    case SLIPMUX_STATE_STDIN_ESC:
        switch (byte) {
        case SLIPMUX_END_ESC:
            byte = SLIPMUX_END;
            break;
        case SLIPMUX_ESC_ESC:
            byte = SLIPMUX_ESC;
            break;
        }
        dev->state = SLIPMUX_STATE_STDIN;
#if IS_USED(MODULE_SLIPMUX_STDIO)
        if (dev->config.uart == STDIO_UART_DEV) {
            isrpipe_write_one(&stdin_isrpipe, byte);
        }
#endif
        return;
    case SLIPMUX_STATE_COAP:
        switch (byte) {
        case SLIPMUX_ESC:
            dev->state = SLIPMUX_STATE_COAP_ESC;
            break;
        case SLIPMUX_END:
            dev->state = SLIPMUX_STATE_NONE;
#if IS_USED(MODULE_SLIPMUX_COAP)
            crb_end_chunk(&dev->coap_rb, true);
            slipmux_coap_notify(dev);
#endif
            break;
        default:
#if IS_USED(MODULE_SLIPMUX_COAP)
            /* discard frame if byte can't be added */
            if (!crb_add_byte(&dev->coap_rb, byte)) {
                DEBUG("slipmux: coap rx buffer full, drop frame\n");
                crb_end_chunk(&dev->coap_rb, false);
                dev->state = SLIPMUX_STATE_UNKNOWN;
                return;
            }
#endif
            break;
        }
        return;
    case SLIPMUX_STATE_COAP_ESC:
        switch (byte) {
        case SLIPMUX_END_ESC:
            byte = SLIPMUX_END;
            break;
        case SLIPMUX_ESC_ESC:
            byte = SLIPMUX_ESC;
            break;
        }
#if IS_USED(MODULE_SLIPMUX_COAP)
        /* discard frame if byte can't be added */
        if (!crb_add_byte(&dev->coap_rb, byte)) {
            DEBUG("slipmux: coap rx buffer full, drop frame\n");
            crb_end_chunk(&dev->coap_rb, false);
            dev->state = SLIPMUX_STATE_UNKNOWN;
            return;
        }
#endif
        dev->state = SLIPMUX_STATE_COAP;
        return;
    case SLIPMUX_STATE_NET:
        switch (byte) {
        case SLIPMUX_ESC:
            dev->state = SLIPMUX_STATE_NET_ESC;
            return;
        case SLIPMUX_END:
#if IS_USED(MODULE_SLIPMUX_NET)
            crb_end_chunk(&dev->net_rb, true);
            slipmux_net_notify(dev);
#endif
            dev->state = SLIPMUX_STATE_NONE;
            return;
        }
#if IS_USED(MODULE_SLIPMUX_NET)
        /* discard frame if byte can't be added */
        if (!crb_add_byte(&dev->net_rb, byte)) {
            DEBUG("slipmux: net rx buffer full, drop frame\n");
            crb_end_chunk(&dev->net_rb, false);
            dev->state = SLIPMUX_STATE_UNKNOWN;
        }
#endif
        return;
    /* escaped byte received */
    case SLIPMUX_STATE_NET_ESC:
        switch (byte) {
        case SLIPMUX_END_ESC:
            byte = SLIPMUX_END;
            break;
        case SLIPMUX_ESC_ESC:
            byte = SLIPMUX_ESC;
            break;
        }
#if IS_USED(MODULE_SLIPMUX_NET)
        /* discard frame if byte can't be added */
        if (!crb_add_byte(&dev->net_rb, byte)) {
            DEBUG("slipmux: net rx buffer full, drop frame\n");
            crb_end_chunk(&dev->net_rb, false);
            dev->state = SLIPMUX_STATE_UNKNOWN;
            return;
        }
#endif
        dev->state = SLIPMUX_STATE_NET;
        return;
    case SLIPMUX_STATE_UNKNOWN:
        if (byte == SLIPMUX_END) {
            dev->state = SLIPMUX_STATE_NONE;
        }
        return;
    case SLIPMUX_STATE_NONE:
        /* is diagnostic frame? */
        if (byte == SLIPMUX_START_STDIO) {
            dev->state = SLIPMUX_STATE_STDIN;
            return;
        }

        if (byte == SLIPMUX_START_COAP) {
#if IS_USED(MODULE_SLIPMUX_COAP)
            /* try to create new configuration / CoAP frame */
            if (!crb_start_chunk(&dev->coap_rb)) {
                dev->state = SLIPMUX_STATE_UNKNOWN;
                return;
            }
#endif
            dev->state = SLIPMUX_STATE_COAP;
            return;
        }

        if (SLIPMUX_START_NET(byte)) {
#if IS_USED(MODULE_SLIPMUX_NET)
            /* try to create new ip frame */
            if (!crb_start_chunk(&dev->net_rb)) {
                DEBUG("slipmux: can't start new net frame, drop frame\n");
                dev->state = SLIPMUX_STATE_UNKNOWN;
                return;
            }
            if (!crb_add_byte(&dev->net_rb, byte)) {
                DEBUG("slipmux: net rx buffer full, drop frame\n");
                crb_end_chunk(&dev->net_rb, false);
                dev->state = SLIPMUX_STATE_UNKNOWN;
                return;
            }
#endif
            dev->state = SLIPMUX_STATE_NET;
            return;
        }

        /* ignore empty frame */
        if (byte == SLIPMUX_END) {
            return;
        }

        dev->state = SLIPMUX_STATE_UNKNOWN;
        DEBUG("slipmux: Unknown start byte %b ignored\n", byte);
    }
}

void slipmux_write_bytes(uart_t uart, const uint8_t *data, size_t len)
{
    for (unsigned j = 0; j < len; j++, data++) {
        switch (*data) {
            case SLIPMUX_END:
                /* escaping END byte*/
                slipmux_write_byte(uart, SLIPMUX_ESC);
                slipmux_write_byte(uart, SLIPMUX_END_ESC);
                break;
            case SLIPMUX_ESC:
                /* escaping ESC byte*/
                slipmux_write_byte(uart, SLIPMUX_ESC);
                slipmux_write_byte(uart, SLIPMUX_ESC_ESC);
                break;
            default:
                slipmux_write_byte(uart, *data);
        }
    }
}

void slipmux_init(void)
{
    for (unsigned i = 0; i < SLIPMUX_DEV_NUM; i++) {
        slipmux_t *dev = &slipmux_devs[i];
        dev->config = slipmux_params[i];

        if (uart_init(dev->config.uart, dev->config.baudrate, slipmux_rx_cb,
                      dev) != UART_OK) {
            DEBUG("slipmux: error initializing UART %i with baudrate %" PRIu32 "\n",
                      dev->config.uart, dev->config.baudrate);
            return;
        }
        DEBUG("slipmux: initialized device %p on UART %i with baudrate %" PRIu32 "\n",
            (void *)dev, dev->config.uart, dev->config.baudrate);

#if IS_USED(MODULE_SLIPMUX_COAP)
        slipmux_coap_init(dev, i);
#endif
#if IS_USED(MODULE_SLIPMUX_NET)
        slipmux_net_init(dev, i);
#endif
    }
}

AUTO_INIT(slipmux_init, 0);
/** @} */
