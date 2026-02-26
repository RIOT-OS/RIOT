/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
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

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "log.h"
#include "slipdev.h"
#include "slipdev_internal.h"
#include "slipdev_params.h"
#include "net/eui_provider.h"

/* XXX: BE CAREFUL ABOUT USING OUTPUT WITH MODULE_SLIPDEV_STDIO IN SENDING
 * FUNCTIONALITY! MIGHT CAUSE DEADLOCK!!!1!! */
#define ENABLE_DEBUG 0
#include "debug.h"

#include "isrpipe.h"
#include "mutex.h"

#include "stdio_uart.h"

/* forward declarations, implementation is in net/coap */
void slipdev_setup_net(slipdev_t *dev, uint8_t index);
void slipdev_setup_coap(slipdev_t *dev);

#if (IS_USED(MODULE_SLIPDEV_STDIO) || IS_USED(MODULE_SLIPDEV_CONFIG))
/* For synchronization with stdio/config threads */
mutex_t slipdev_mutex = MUTEX_INIT;
#endif

static inline void _slipdev_stdio_add_to_frame(slipdev_t *dev, uint8_t byte)
{
    if (!IS_USED(MODULE_SLIPDEV_STDIO) ||
        dev->config.uart != slipdev_params[0].uart) {
        return;
    }
    isrpipe_write_one(&stdin_isrpipe, byte);
}

static inline bool _slipdev_config_start_frame(slipdev_t *dev)
{
#ifdef MODULE_SLIPDEV_CONFIG
    /* try to create new configuration / CoAP frame */
    return crb_start_chunk(&dev->rb_config);
#else
    (void)dev;
    DEBUG("Ignoring new configuration frame as support is not enabled\n");
    return 1;
#endif
}

static inline void _slipdev_config_end_frame(slipdev_t *dev)
{
#ifdef MODULE_SLIPDEV_CONFIG
    crb_end_chunk(&dev->rb_config, true);
    thread_flags_set(thread_get(dev->coap_server_pid), 1);
#else
    (void)dev;
#endif
}

static inline bool _slipdev_config_add_to_frame(slipdev_t *dev, uint8_t byte)
{
#ifdef MODULE_SLIPDEV_CONFIG
    /* discard frame if byte can't be added */
    if (!crb_add_byte(&dev->rb_config, byte)) {
        DEBUG("slipmux: coap rx buffer full, drop frame\n");
        crb_end_chunk(&dev->rb_config, false);
        return 0;
    }
#else
    (void)dev;
    (void)byte;
#endif
    return 1;
}

static inline bool _slipdev_net_start_frame(slipdev_t *dev, uint8_t byte)
{
    /* try to create new ip frame */
    if (!crb_start_chunk(&dev->rb)) {
        DEBUG("slipmux: can't start new net frame, drop frame\n");
        return 0;
    }
    if (!crb_add_byte(&dev->rb, byte)) {
        DEBUG("slipmux: net rx buffer full, drop frame\n");
        crb_end_chunk(&dev->rb, false);
        return 0;
    }
    return 1;
}

static inline void _slipdev_net_end_frame(slipdev_t *dev)
{
    crb_end_chunk(&dev->rb, true);
    netdev_trigger_event_isr(&dev->netdev);
}

static inline bool _slipdev_net_add_to_frame(slipdev_t *dev, uint8_t byte)
{
    /* discard frame if byte can't be added */
    if (!crb_add_byte(&dev->rb, byte)) {
        DEBUG("slipmux: net rx buffer full, drop frame\n");
        crb_end_chunk(&dev->rb, false);
        return 0;
    }
    return 1;
}

void _slip_rx_cb(void *arg, uint8_t byte)
{
    slipdev_t *dev = arg;

    switch (dev->state) {
    case SLIPDEV_STATE_STANDBY:
    /* fall through */
    case SLIPDEV_STATE_SLEEP:
        /* do nothing if we are supposed to sleep */
        /* and we should usually not be able to hit this case anyways */
        assert(0);
        break;
    case SLIPDEV_STATE_STDIN:
        switch (byte) {
        case SLIPDEV_ESC:
            dev->state = SLIPDEV_STATE_STDIN_ESC;
            break;
        case SLIPDEV_END:
            dev->state = SLIPDEV_STATE_NONE;
            break;
        default:
            _slipdev_stdio_add_to_frame(dev, byte);
            break;
        }
        return;
    case SLIPDEV_STATE_STDIN_ESC:
        switch (byte) {
        case SLIPDEV_END_ESC:
            byte = SLIPDEV_END;
            break;
        case SLIPDEV_ESC_ESC:
            byte = SLIPDEV_ESC;
            break;
        }
        dev->state = SLIPDEV_STATE_STDIN;
        _slipdev_stdio_add_to_frame(dev, byte);
        return;
    case SLIPDEV_STATE_CONFIG:
        switch (byte) {
        case SLIPDEV_ESC:
            dev->state = SLIPDEV_STATE_CONFIG_ESC;
            break;
        case SLIPDEV_END:
            dev->state = SLIPDEV_STATE_NONE;
            _slipdev_config_end_frame(dev);
            break;
        default:
            if (!_slipdev_config_add_to_frame(dev, byte)) {
                dev->state = SLIPDEV_STATE_UNKNOWN;
            }
            break;
        }
        return;
    case SLIPDEV_STATE_CONFIG_ESC:
        switch (byte) {
        case SLIPDEV_END_ESC:
            byte = SLIPDEV_END;
            break;
        case SLIPDEV_ESC_ESC:
            byte = SLIPDEV_ESC;
            break;
        }
        if (_slipdev_config_add_to_frame(dev, byte)) {
            dev->state = SLIPDEV_STATE_CONFIG;
        }
        else {
            dev->state = SLIPDEV_STATE_UNKNOWN;
        }
        return;
    case SLIPDEV_STATE_NET:
        switch (byte) {
        case SLIPDEV_ESC:
            dev->state = SLIPDEV_STATE_NET_ESC;
            break;
        case SLIPDEV_END:
            _slipdev_net_end_frame(dev);
            dev->state = SLIPDEV_STATE_NONE;
            break;
        default:
            if (!_slipdev_net_add_to_frame(dev, byte)) {
                dev->state = SLIPDEV_STATE_UNKNOWN;
            }
            break;
        }
        return;
    /* escaped byte received */
    case SLIPDEV_STATE_NET_ESC:
        switch (byte) {
        case SLIPDEV_END_ESC:
            byte = SLIPDEV_END;
            break;
        case SLIPDEV_ESC_ESC:
            byte = SLIPDEV_ESC;
            break;
        }
        if (_slipdev_net_add_to_frame(dev, byte)) {
            dev->state = SLIPDEV_STATE_NET;
        }
        else {
            dev->state = SLIPDEV_STATE_UNKNOWN;
        }
        return;
    case SLIPDEV_STATE_UNKNOWN:
        if (byte == SLIPDEV_END) {
            dev->state = SLIPDEV_STATE_NONE;
        }
        return;
    case SLIPDEV_STATE_NONE:
        /* is diagnostic frame? */
        if (byte == SLIPDEV_START_STDIO) {
            dev->state = SLIPDEV_STATE_STDIN;
            return;
        }

        if (byte == SLIPDEV_START_COAP) {
            if (_slipdev_config_start_frame(dev)) {
                dev->state = SLIPDEV_STATE_CONFIG;
            }
            else {
                dev->state = SLIPDEV_STATE_UNKNOWN;
            }
            return;
        }

        if (SLIPDEV_START_NET(byte)) {
            if (_slipdev_net_start_frame(dev, byte)) {
                dev->state = SLIPDEV_STATE_NET;
            }
            else {
                dev->state = SLIPDEV_STATE_UNKNOWN;
            }
            return;
        }

        /* ignore empty frame */
        if (byte == SLIPDEV_END) {
            return;
        }

        dev->state = SLIPDEV_STATE_UNKNOWN;
        DEBUG("slipmux: Unknown start byte %02x ignored\n", byte);
    }
}

void slipdev_write_bytes(uart_t uart, const uint8_t *data, size_t len)
{
    for (unsigned j = 0; j < len; j++, data++) {
        switch (*data) {
        case SLIPDEV_END:
            /* escaping END byte*/
            slipdev_write_byte(uart, SLIPDEV_ESC);
            slipdev_write_byte(uart, SLIPDEV_END_ESC);
            break;
        case SLIPDEV_ESC:
            /* escaping ESC byte*/
            slipdev_write_byte(uart, SLIPDEV_ESC);
            slipdev_write_byte(uart, SLIPDEV_ESC_ESC);
            break;
        default:
            slipdev_write_byte(uart, *data);
        }
    }
}

void slipdev_setup(slipdev_t *dev, const slipdev_params_t *params, uint8_t index)
{
    /* set device descriptor fields */
    dev->config = *params;
    dev->state = 0;

    /* we only support one coap server at the moment */
    if ((index == 0) && IS_USED(MODULE_SLIPDEV_CONFIG)) {
        slipdev_setup_coap(dev);
    }

    if (IS_USED(MODULE_SLIPDEV_NET)) {
        slipdev_setup_net(dev, index);
    }
    else {
        if (uart_init(dev->config.uart, dev->config.baudrate, _slip_rx_cb,
                      dev) != UART_OK) {
            LOG_ERROR("slipdev: error initializing UART %i with baudrate %" PRIu32 "\n",
                      dev->config.uart, dev->config.baudrate);
        }
    }
}

/** @} */
