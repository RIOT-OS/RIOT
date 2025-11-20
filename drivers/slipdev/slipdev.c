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
#include "net/eui_provider.h"

/* XXX: BE CAREFUL ABOUT USING OUTPUT WITH MODULE_SLIPDEV_STDIO IN SENDING
 * FUNCTIONALITY! MIGHT CAUSE DEADLOCK!!!1!! */
#define ENABLE_DEBUG 0
#include "debug.h"

#include "isrpipe.h"
#include "mutex.h"
#if IS_USED(MODULE_SLIPDEV_CONFIG)
#include "checksum/crc16_ccitt.h"
#include "net/nanocoap.h"
#endif
#include "stdio_uart.h"

#if (IS_USED(MODULE_SLIPDEV_STDIO) || IS_USED(MODULE_SLIPDEV_CONFIG))
/* For synchronization with stdio/config threads */
mutex_t slipdev_mutex = MUTEX_INIT;
#endif

#if IS_USED(MODULE_SLIPDEV_CONFIG)
/* The special init is the result of normal fcs init combined with slipmux config start (0xa9) */
#define SPECIAL_INIT_FCS (0x374cU)
#define COAP_STACKSIZE (1024)

static char coap_stack[COAP_STACKSIZE];
#endif /* IS_USED(MODULE_SLIPDEV_CONFIG) */

static int _check_state(slipdev_t *dev);

static inline void slipdev_lock(void)
{
    if (IS_USED(MODULE_SLIPDEV_STDIO) || IS_USED(MODULE_SLIPDEV_CONFIG)) {
        mutex_lock(&slipdev_mutex);
    }
}

static inline void slipdev_unlock(void)
{
    if (IS_USED(MODULE_SLIPDEV_STDIO) || IS_USED(MODULE_SLIPDEV_CONFIG)) {
        mutex_unlock(&slipdev_mutex);
    }
}

static void _slip_rx_cb(void *arg, uint8_t byte)
{
    slipdev_t *dev = arg;

    switch (dev->state) {
    case SLIPDEV_STATE_STDIN:
        switch (byte) {
        case SLIPDEV_ESC:
            dev->state = SLIPDEV_STATE_STDIN_ESC;
            break;
        case SLIPDEV_END:
            dev->state = SLIPDEV_STATE_NONE;
            break;
        default:
#if IS_USED(MODULE_SLIPDEV_STDIO)
            if (dev->config.uart == STDIO_UART_DEV) {
                isrpipe_write_one(&stdin_isrpipe, byte);
            }
#endif
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
#if IS_USED(MODULE_SLIPDEV_STDIO)
        if (dev->config.uart == STDIO_UART_DEV) {
            isrpipe_write_one(&stdin_isrpipe, byte);
        }
#endif
        return;
    case SLIPDEV_STATE_CONFIG:
        switch (byte) {
        case SLIPDEV_ESC:
            dev->state = SLIPDEV_STATE_CONFIG_ESC;
            break;
        case SLIPDEV_END:
            dev->state = SLIPDEV_STATE_NONE;
#if IS_USED(MODULE_SLIPDEV_CONFIG)
            crb_end_chunk(&dev->rb_config, true);
            thread_flags_set(thread_get(dev->coap_server_pid), 1);
#endif
            break;
        default:
#if IS_USED(MODULE_SLIPDEV_CONFIG)
            /* discard frame if byte can't be added */
            if (!crb_add_byte(&dev->rb_config, byte)) {
                DEBUG("slipdev: rx buffer full, drop frame\n");
                crb_end_chunk(&dev->rb_config, false);
                dev->state = SLIPDEV_STATE_NONE;
                return;
            }
#endif
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
#if IS_USED(MODULE_SLIPDEV_CONFIG)
        /* discard frame if byte can't be added */
        if (!crb_add_byte(&dev->rb_config, byte)) {
            DEBUG("slipdev: rx buffer full, drop frame\n");
            crb_end_chunk(&dev->rb_config, false);
            dev->state = SLIPDEV_STATE_NONE;
            return;
        }
#endif
        dev->state = SLIPDEV_STATE_CONFIG;
        return;
    case SLIPDEV_STATE_NONE:
        /* is diagnostic frame? */
        if (byte == SLIPDEV_STDIO_START) {
            dev->state = SLIPDEV_STATE_STDIN;
            return;
        }

        if (byte == SLIPDEV_CONFIG_START) {
#if IS_USED(MODULE_SLIPDEV_CONFIG)
            /* try to create new frame */
            if (!crb_start_chunk(&dev->rb_config)) {
                return;
            }
#endif
            dev->state = SLIPDEV_STATE_CONFIG;
            return;
        }

        /* ignore empty frame */
        if (byte == SLIPDEV_END) {
            return;
        }

        /* try to create new ip frame */
        if (!crb_start_chunk(&dev->rb)) {
            return;
        }
        dev->state = SLIPDEV_STATE_NET;
        /* fall-through */
    case SLIPDEV_STATE_NET:
        switch (byte) {
        case SLIPDEV_ESC:
            dev->state = SLIPDEV_STATE_NET_ESC;
            return;
        case SLIPDEV_END:
            crb_end_chunk(&dev->rb, true);
            netdev_trigger_event_isr(&dev->netdev);
            dev->state = SLIPDEV_STATE_NONE;
            return;
        }
        break;
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
        dev->state = SLIPDEV_STATE_NET;
        break;
    }

    assert(dev->state == SLIPDEV_STATE_NET);

    /* discard frame if byte can't be added */
    if (!crb_add_byte(&dev->rb, byte)) {
        DEBUG("slipdev: rx buffer full, drop frame\n");
        crb_end_chunk(&dev->rb, false);
        dev->state = SLIPDEV_STATE_NONE;
        return;
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
    slipdev_lock();
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        uint8_t *data = iol->iol_base;
        slipdev_write_bytes(dev->config.uart, data, iol->iol_len);
        bytes += iol->iol_len;
    }
    slipdev_write_byte(dev->config.uart, SLIPDEV_END);
    slipdev_unlock();

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
        } else {
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

#if IS_USED(MODULE_SLIPDEV_CONFIG)
static void *_coap_server_thread(void *arg)
{
    static uint8_t buf[512];
    slipdev_t *dev = arg;
    while (1) {
        thread_flags_wait_any(1);
        size_t len;
        while (crb_get_chunk_size(&dev->rb_config, &len)) {
            crb_consume_chunk(&dev->rb_config, buf, len);

            /* Is the crc correct via residue(=0xF0B8) test */
            if (crc16_ccitt_fcs_update(SPECIAL_INIT_FCS, buf, len) != 0xF0B8) {
                break;
            }

            /* cut off the FCS checksum at the end */
            size_t pktlen = len - 2;

            coap_pkt_t pkt;
            sock_udp_ep_t remote;
            coap_request_ctx_t ctx = {
                .remote = &remote,
            };
            if (coap_parse(&pkt, buf, pktlen) < 0) {
                break;
            }
            unsigned int res = 0;
            if ((res = coap_handle_req(&pkt, buf, sizeof(buf), &ctx)) <= 0) {
                break;
            }

            uint16_t fcs_sum = crc16_ccitt_fcs_finish(SPECIAL_INIT_FCS, buf, res);

            slipdev_lock();
            slipdev_write_byte(dev->config.uart, SLIPDEV_CONFIG_START);
            slipdev_write_bytes(dev->config.uart, buf, res);
            slipdev_write_bytes(dev->config.uart, (uint8_t *) &fcs_sum, 2);
            slipdev_write_byte(dev->config.uart, SLIPDEV_END);
            slipdev_unlock();
        }
    }

    return NULL;
}
#endif /* MODULE_SLIPDEV_CONFIG */

void slipdev_setup(slipdev_t *dev, const slipdev_params_t *params, uint8_t index)
{
#if IS_USED(MODULE_SLIPDEV_CONFIG)
    crb_init(&dev->rb_config, dev->rxmem_config, sizeof(dev->rxmem_config));

    dev->coap_server_pid = thread_create(coap_stack, sizeof(coap_stack), THREAD_PRIORITY_MAIN - 1,
                                     THREAD_CREATE_STACKTEST, _coap_server_thread,
                                     (void *)dev, "Slipmux CoAP server");
#endif
    /* set device descriptor fields */
    dev->config = *params;
    dev->state = 0;
    dev->netdev.driver = &slip_driver;

    netdev_register(&dev->netdev, NETDEV_SLIPDEV, index);
}

/** @} */
