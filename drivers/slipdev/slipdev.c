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
#include "net/nanocoap.h"
#endif
#include "stdio_uart.h"


#if (IS_USED(MODULE_SLIPDEV_STDIO) || IS_USED(MODULE_SLIPDEV_CONFIG))
/* For synchronization with stdio/config threads */
mutex_t slipdev_mutex = MUTEX_INIT;
#endif

#if IS_USED(MODULE_SLIPDEV_CONFIG)
#define COAP_STACKSIZE (1024)

/* Magic values for the fcs */
/* The special init is the result of normal fcs init combined with slipmux config start (0xa9) */
#define SPECIAL_INIT_FCS (0x374cU)
#define INIT_FCS (0xffffU)
#define GOOD_FCS (0xf0b8U)

static const uint16_t FCS_LOOKUP[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf, 0x8c48, 0x9dc1, 0xaf5a, 0xbed3,
    0xca6c, 0xdbe5, 0xe97e, 0xf8f7, 0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876, 0x2102, 0x308b, 0x0210, 0x1399,
    0x6726, 0x76af, 0x4434, 0x55bd, 0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c, 0xbdcb, 0xac42, 0x9ed9, 0x8f50,
    0xfbef, 0xea66, 0xd8fd, 0xc974, 0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3, 0x5285, 0x430c, 0x7197, 0x601e,
    0x14a1, 0x0528, 0x37b3, 0x263a, 0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9, 0xef4e, 0xfec7, 0xcc5c, 0xddd5,
    0xa96a, 0xb8e3, 0x8a78, 0x9bf1, 0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70, 0x8408, 0x9581, 0xa71a, 0xb693,
    0xc22c, 0xd3a5, 0xe13e, 0xf0b7, 0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036, 0x18c1, 0x0948, 0x3bd3, 0x2a5a,
    0x5ee5, 0x4f6c, 0x7df7, 0x6c7e, 0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd, 0xb58b, 0xa402, 0x9699, 0x8710,
    0xf3af, 0xe226, 0xd0bd, 0xc134, 0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3, 0x4a44, 0x5bcd, 0x6956, 0x78df,
    0x0c60, 0x1de9, 0x2f72, 0x3efb, 0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a, 0xe70e, 0xf687, 0xc41c, 0xd595,
    0xa12a, 0xb0a3, 0x8238, 0x93b1, 0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330, 0x7bc7, 0x6a4e, 0x58d5, 0x495c,
    0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

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
            isrpipe_write_one(&stdin_isrpipe, byte);
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
        isrpipe_write_one(&stdin_isrpipe, byte);
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
        if (IS_USED(MODULE_SLIPDEV_STDIO) &&
            (byte == SLIPDEV_STDIO_START) &&
            (dev->config.uart == STDIO_UART_DEV)) {
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
static uint16_t _fcs_part(const uint8_t * data, size_t len) {
    uint16_t fcs = SPECIAL_INIT_FCS;
    for (size_t i = 0; i < len; ++i)
    {
        fcs = (fcs >> 8) ^ FCS_LOOKUP[(fcs ^ data[i]) & 0xff];
    }
    return fcs;
}

static int check_fcs(const uint8_t * data, size_t len) {
    uint16_t trialfcs = _fcs_part(data, len);
    return trialfcs == GOOD_FCS;
}

static uint16_t fcs(const uint8_t * data, size_t len) {
    uint16_t trialfcs = _fcs_part(data, len);
    uint16_t result = trialfcs ^ INIT_FCS;
    return result;
}

static void *_coap_server_thread(void *arg)
{
    static uint8_t buf[512];
    slipdev_t *dev = arg;
    while (1) {
        thread_flags_wait_any(1);
        size_t len;
        while (crb_get_chunk_size(&dev->rb_config, &len)) {
            crb_consume_chunk(&dev->rb_config, buf, len);

            if (!check_fcs(buf, len)) {
                break;
            }

            // cut off the FCS checksum at the end
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

            uint16_t fcs_sum = fcs(buf, res);

            slipdev_lock();
            slipdev_write_byte(dev->config.uart, SLIPDEV_CONFIG_START);
            slipdev_write_bytes(dev->config.uart, buf, res);
            slipdev_write_byte(dev->config.uart, fcs_sum);
            slipdev_write_byte(dev->config.uart, fcs_sum >> 8);
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
