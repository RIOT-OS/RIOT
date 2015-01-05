/**
 * nativenet.h implementation
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup native_cpu
 * @ingroup net
 * @{
 * @file
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#include <stdio.h>
#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#include <arpa/inet.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "transceiver.h"
#include "msg.h"

#include "native_internal.h"
#include "tap.h"
#include "nativenet.h"
#include "nativenet_internal.h"
#include "cpu.h"
#include "thread.h"

#define _NATIVENET_NETDEV_RCV_EVENT (362811672) /* magic number ;-) */

static _native_callback_t _nativenet_callbacks[255];

struct rx_buffer_s _nativenet_rx_buffer[RX_BUF_SIZE];
static volatile uint8_t rx_buffer_next;

static kernel_pid_t _native_net_tpid = KERNEL_PID_UNDEF;

_nativenet_netdev_more_t _nativenet_default_dev_more;

/************************************************************************/
/* nativenet.h **********************************************************/
/************************************************************************/

int _nativenet_init(netdev_t *dev)
{
    if (dev == NULL) {
        return -ENODEV;
    }

#ifdef MODULE_NETDEV_BASE

    if (dev->driver != &nativenet_driver) {
        return -ENODEV;
    }

#endif

    dev->type = NETDEV_TYPE_BASE;
    dev->event_handler = thread_getpid();
    dev->more = (void *)(&_nativenet_default_dev_more);

    _NATIVENET_DEV_MORE(dev)->_channel = 0;
    _NATIVENET_DEV_MORE(dev)->_pan_id = 0;
    _NATIVENET_DEV_MORE(dev)->_is_monitoring = 0;
    _NATIVENET_DEV_MORE(dev)->_rx_pos = -1;
    memset(_NATIVENET_DEV_MORE(dev)->_callbacks, 0,
           sizeof((_NATIVENET_DEV_MORE(dev)->_callbacks)));

    return 0;
}

void nativenet_init(kernel_pid_t transceiver_pid)
{
    DEBUG("nativenet_init(transceiver_pid=%" PRIkernel_pid ")\n", transceiver_pid);
    rx_buffer_next = 0;
    _nativenet_init((netdev_t *)(&nativenet_default_dev));
    _native_net_tpid = transceiver_pid;
}

void nativenet_powerdown(void)
{
    return;
}

void nativenet_set_monitor(uint8_t mode)
{
    DEBUG("nativenet_set_monitor(mode=%d)\n", mode);
    _nativenet_default_dev_more._is_monitoring = mode;
}

int16_t nativenet_set_channel(uint8_t channel)
{
    _nativenet_default_dev_more._channel = channel;
    return _nativenet_default_dev_more._channel;
}

int16_t nativenet_get_channel(void)
{
    return _nativenet_default_dev_more._channel;
}

uint16_t nativenet_set_pan(uint16_t pan)
{
    _nativenet_default_dev_more._pan_id = pan;
    return _nativenet_default_dev_more._pan_id;
}

uint16_t nativenet_get_pan(void)
{
    return _nativenet_default_dev_more._pan_id;
}

radio_address_t nativenet_set_address(radio_address_t address)
{
    DEBUG("nativenet_set_address(address=%d)\n", address);
    _nativenet_default_dev_more._radio_addr = address;
    return _nativenet_default_dev_more._radio_addr;
}

radio_address_t nativenet_get_address(void)
{
    DEBUG("nativenet_get_address -> address = %d\n",
          _nativenet_default_dev_more._radio_addr);
    return _nativenet_default_dev_more._radio_addr;
}

uint64_t nativenet_get_address_long(void)
{
    DEBUG("nativenet_get_address_long -> address = %" PRIx64 "\n",
          _nativenet_default_dev_more._long_addr);
    return _nativenet_default_dev_more._long_addr;
}

uint64_t nativenet_set_address_long(uint64_t address)
{
    DEBUG("nativenet_set_address_long(address=%" PRIx64 ")\n", address);
    warnx("nativenet_set_address_long: this does not actually change the interfaces address");
    _nativenet_default_dev_more._long_addr = address;
    return _nativenet_default_dev_more._long_addr;
}

int8_t nativenet_send(radio_packet_t *packet)
{
    packet->src = _nativenet_default_dev_more._radio_addr;
    DEBUG("nativenet_send:  Sending packet of length %" PRIu16 " from %" PRIu16 " to %" PRIu16 "\n",
          packet->length, packet->src, packet->dst);

    return send_buf(packet);
}

void nativenet_switch_to_rx(void)
{
    return;
}

/************************************************************************/
/* nativenet_internal.h *************************************************/
/************************************************************************/

int _nativenet_register_cb(int event, _native_callback_t func)
{
    if (event > NNEV_MAXEV) {
        DEBUG("_nativenet_register_cb: event > NNEV_MAXEV\n");
        return -1;
    }

    _nativenet_callbacks[event] = func;
    return 0;
}

int _nativenet_unregister_cb(int event)
{
    if (event > NNEV_MAXEV) {
        DEBUG("_nativenet_unregister_cb: event > NNEV_MAXEV\n");
        return -1;
    }

    _nativenet_callbacks[event] = NULL;
    return 0;
}

void do_cb(int event)
{
    if (event > NNEV_MAXEV) {
        DEBUG("do_cb: event > NNEV_MAXEV\n");
        return;
    }

    if (_nativenet_callbacks[event] != NULL) {
        _nativenet_callbacks[event]();
    }
}

void _nativenet_handle_packet(radio_packet_t *packet)
{
    radio_address_t dst_addr = packet->dst;
    int notified = 0;

    /* TODO: find way to demultiplex reception from several taps and map them
     *       to devices. */
    netdev_t *dev = &nativenet_default_dev;

    /* address filter / monitor mode */
    if (_nativenet_default_dev_more._is_monitoring == 1) {
        DEBUG("_nativenet_handle_packet: monitoring, not filtering address \n");
    }
    else {
        /* own addr check */
        if (dst_addr == _nativenet_default_dev_more._radio_addr) {
            DEBUG("_nativenet_handle_packet: accept packet, addressed to us\n");
        }
        else if (dst_addr == 0) {
            DEBUG("_nativenet_handle_packet: accept packet, broadcast\n");
        }
        else {
            DEBUG("_nativenet_handle_packet: discard packet addressed to someone else\n");
            return;
        }
    }

    /* copy packet to rx buffer */
    DEBUG("\n\t\trx_buffer_next: %i\n\n", rx_buffer_next);
    memcpy(&_nativenet_rx_buffer[rx_buffer_next].data, packet->data, packet->length);
    memcpy(&_nativenet_rx_buffer[rx_buffer_next].packet, packet, sizeof(radio_packet_t));
    _nativenet_rx_buffer[rx_buffer_next].packet.data = (uint8_t *)
            &_nativenet_rx_buffer[rx_buffer_next].data;

    /* notify transceiver thread if any */
    if (_native_net_tpid != KERNEL_PID_UNDEF) {
        DEBUG("_nativenet_handle_packet: notifying transceiver thread!\n");
        msg_t m;
        m.type = (uint16_t) RCV_PKT_NATIVE;
        m.content.value = rx_buffer_next;
        msg_send_int(&m, _native_net_tpid);
        notified = 1;
    }

    if (dev->event_handler != KERNEL_PID_UNDEF) {
        DEBUG("_nativenet_handle_packet: notifying netdev event handler (pid = %d)!\n",
              dev->event_handler);
        msg_t m;
        m.type = NETDEV_MSG_EVENT_TYPE;
        m.content.value = _NATIVENET_NETDEV_RCV_EVENT;
        _NATIVENET_DEV_MORE(dev)->_rx_pos = rx_buffer_next;

        msg_send_int(&m, dev->event_handler);

        notified = 1;
    }
    else {
        DEBUG("_nativenet_handle_packet: no event handler registered\n");
    }

    if (!notified) {
        DEBUG("_nativenet_handle_packet: no one to notify =(\n");
    }

    /* shift to next buffer element */
    if (++rx_buffer_next == RX_BUF_SIZE) {
        rx_buffer_next = 0;
    }
}

/***************************************************************
 * netdev_base wrapper
 ***************************************************************/

#ifdef MODULE_NETDEV_BASE
int _nativenet_send_data(netdev_t *dev, void *dest, size_t dest_len,
                         netdev_hlist_t *upper_layer_hdrs, void *data,
                         size_t data_len)
{
    netdev_hlist_t *ptr = upper_layer_hdrs;
    uint8_t tx_buffer[data_len + netdev_get_hlist_len(upper_layer_hdrs)];
    size_t tx_ptr = 0;
    radio_packet_t pkt = {0, 0, 0, 0, 0, {0, 0}, sizeof(tx_buffer), tx_buffer};

    if (dev->type != NETDEV_TYPE_BASE) {
        return -ENODEV;
    }

    if (dest_len > sizeof(uint16_t)) {
        return -EAFNOSUPPORT;
    }

    if (sizeof(tx_buffer) > NATIVE_MAX_DATA_LENGTH) {
        return -EMSGSIZE;
    }

    if (upper_layer_hdrs) {
        do {
            memcpy(&(tx_buffer[tx_ptr]), ptr->header, ptr->header_len);
            tx_ptr += ptr->header_len;
            netdev_hlist_advance(&ptr);
        } while (ptr != upper_layer_hdrs);
    }

    memcpy(&(tx_buffer[tx_ptr]), data, data_len);

    if (dest_len == sizeof(uint16_t)) {
        pkt.dst = *((uint16_t *)dest);
    }
    else {
        pkt.dst = (uint16_t)(*((uint8_t *)dest));
    }

    return nativenet_send(&pkt);
}

int _nativenet_add_rcv_data_cb(netdev_t *dev, netdev_rcv_data_cb_t cb)
{
    int i = 0;

    if (dev->type != NETDEV_TYPE_BASE) {
        return -ENODEV;
    }

    for (i = 0; i < NATIVENET_DEV_CB_MAX; i++) {
        if (_NATIVENET_DEV_MORE(dev)->_callbacks[i] == NULL ||
            _NATIVENET_DEV_MORE(dev)->_callbacks[i] == cb) {
            break;
        }
    }

    if (i >= NATIVENET_DEV_CB_MAX) {
        return -ENOBUFS;
    }

    _NATIVENET_DEV_MORE(dev)->_callbacks[i] = cb;

    return 0;
}

int _nativenet_rem_rcv_data_cb(netdev_t *dev, netdev_rcv_data_cb_t cb)
{
    int i = 0;

    if (dev->type != NETDEV_TYPE_BASE) {
        return -ENODEV;
    }

    for (i = 0; i < NATIVENET_DEV_CB_MAX; i++) {
        if (_NATIVENET_DEV_MORE(dev)->_callbacks[i] == cb) {
            _NATIVENET_DEV_MORE(dev)->_callbacks[i] = NULL;
        }
    }

    return 0;
}

int _nativenet_get_option(netdev_t *dev, netdev_opt_t opt, void *value,
                          size_t *value_len)
{
    if (dev->type != NETDEV_TYPE_BASE) {
        return -ENODEV;
    }

    switch (opt) {
        case NETDEV_OPT_CHANNEL:
            if (*value_len == 0) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(uint8_t)) {
                *value_len = sizeof(uint8_t);
            }

            *((uint8_t *)value) = _NATIVENET_DEV_MORE(dev)->_channel;
            break;

        case NETDEV_OPT_ADDRESS:
            if (*value_len < sizeof(radio_address_t)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(radio_address_t)) {
                *value_len = sizeof(radio_address_t);
            }

            *((radio_address_t *)value) = _NATIVENET_DEV_MORE(dev)->_radio_addr;
            break;

        case NETDEV_OPT_NID:
            if (*value_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(uint16_t)) {
                *value_len = sizeof(uint16_t);
            }

            *((uint16_t *)value) = _NATIVENET_DEV_MORE(dev)->_pan_id;
            break;

        case NETDEV_OPT_ADDRESS_LONG:
            if (*value_len < sizeof(uint64_t)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(uint64_t)) {
                *value_len = sizeof(uint64_t);
            }

            *((uint64_t *)value) = _NATIVENET_DEV_MORE(dev)->_long_addr;
            break;

        case NETDEV_OPT_MAX_PACKET_SIZE:
            if (*value_len < sizeof(NATIVE_MAX_DATA_LENGTH)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(NATIVE_MAX_DATA_LENGTH)) {
                *value_len = sizeof(NATIVE_MAX_DATA_LENGTH);
            }

            *((netdev_proto_t *)value) = NATIVE_MAX_DATA_LENGTH;
            break;

        case NETDEV_OPT_PROTO:
            if (*value_len < sizeof(netdev_proto_t)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(netdev_proto_t)) {
                *value_len = sizeof(netdev_proto_t);
            }

            *((netdev_proto_t *)value) = NETDEV_PROTO_RADIO;
            break;

        default:
            return -ENOTSUP;
    }

    return 0;
}

static int _type_pun_up(void *value_out, size_t desired_len,
                        void *value_in, size_t given_len)
{
    if (given_len > desired_len) {
        return -EOVERFLOW;
    }

    /* XXX this is ugly, but bear with me */
    switch (given_len) {
        case 8:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (*((uint64_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        case 4:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (uint64_t)(*((uint32_t *)value_in));
                    return 0;

                case 4:
                    *((uint32_t *)value_out) = (*((uint32_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        case 2:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (uint64_t)(*((uint16_t *)value_in));
                    return 0;

                case 4:
                    *((uint32_t *)value_out) = (uint32_t)(*((uint16_t *)value_in));
                    return 0;

                case 2:
                    *((uint16_t *)value_out) = (*((uint16_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        case 1:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (uint64_t)(*((uint8_t *)value_in));
                    return 0;

                case 4:
                    *((uint32_t *)value_out) = (uint32_t)(*((uint8_t *)value_in));
                    return 0;

                case 2:
                    *((uint16_t *)value_out) = (uint16_t)(*((uint8_t *)value_in));
                    return 0;

                case 1:
                    *((uint8_t *)value_out) = (*((uint8_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        default:
            return -EINVAL;
    }
}

int _nativenet_set_option(netdev_t *dev, netdev_opt_t opt, void *value,
                          size_t value_len)
{
    uint8_t set_value[sizeof(uint64_t)];
    int res = 0;

    if (dev->type != NETDEV_TYPE_BASE) {
        return -ENODEV;
    }

    switch (opt) {
        case NETDEV_OPT_CHANNEL:
            if ((res = _type_pun_up(set_value, sizeof(uint8_t),
                                    value, value_len)) == 0) {
                _NATIVENET_DEV_MORE(dev)->_channel = *((uint8_t *)set_value);
            }

            break;

        case NETDEV_OPT_ADDRESS:
            if ((res = _type_pun_up(set_value, sizeof(radio_address_t),
                                    value, value_len)) == 0) {
                _NATIVENET_DEV_MORE(dev)->_radio_addr = *((radio_address_t *)set_value);
            }

            break;

        case NETDEV_OPT_NID:
            if ((res = _type_pun_up(set_value, sizeof(uint16_t),
                                    value, value_len)) == 0) {
                _NATIVENET_DEV_MORE(dev)->_pan_id = *((uint16_t *)set_value);
            }

            break;

        case NETDEV_OPT_PROTO:
            /* TODO: wouldn't this be awesome */
            return -ENOTSUP;

        default:
            return -ENOTSUP;
    }

    return res;
}

int _nativenet_get_state(netdev_t *dev, netdev_state_t *state)
{
    if (dev->type != NETDEV_TYPE_BASE) {
        return -ENODEV;
    }

    if (_NATIVENET_DEV_MORE(dev)->_is_monitoring) {
        *state = NETDEV_STATE_PROMISCUOUS_MODE;
    }
    else {
        *state = NETDEV_STATE_RX_MODE;
    }

    return 0;
}

int _nativenet_set_state(netdev_t *dev, netdev_state_t state)
{
    if (state != NETDEV_STATE_PROMISCUOUS_MODE && _NATIVENET_DEV_MORE(dev)->_is_monitoring) {
        _NATIVENET_DEV_MORE(dev)->_is_monitoring = 0;
    }

    switch (state) {
        case NETDEV_STATE_RX_MODE:
            nativenet_switch_to_rx();
            break;

        case NETDEV_STATE_PROMISCUOUS_MODE:
            _NATIVENET_DEV_MORE(dev)->_is_monitoring = 1;
            break;

        default:
            return -ENOTSUP;
    }

    return 0;
}

void _nativenet_event(netdev_t *dev, uint32_t event_type)
{
    switch (event_type) {
        case _NATIVENET_NETDEV_RCV_EVENT:
            if (_NATIVENET_DEV_MORE(dev)->_rx_pos < 0) {
                return;
            }

            for (int i = 0; i < NATIVENET_DEV_CB_MAX; i++) {
                int rx_pos = _NATIVENET_DEV_MORE(dev)->_rx_pos;
                if (_NATIVENET_DEV_MORE(dev)->_callbacks[i]) {
                    _NATIVENET_DEV_MORE(dev)->_callbacks[i]((netdev_t *)dev,
                            &(_nativenet_rx_buffer[rx_pos].packet.src),
                            sizeof(uint16_t),
                            &(_nativenet_rx_buffer[rx_pos].packet.dst),
                            sizeof(uint16_t),
                            _nativenet_rx_buffer[rx_pos].data,
                            (size_t)_nativenet_rx_buffer[rx_pos].packet.length);
                }
            }

            _NATIVENET_DEV_MORE(dev)->_rx_pos = -1;

            break;

        default:
            break;
    }
}

const netdev_driver_t nativenet_driver = {
    _nativenet_init,
    _nativenet_send_data,
    _nativenet_add_rcv_data_cb,
    _nativenet_rem_rcv_data_cb,
    _nativenet_get_option,
    _nativenet_set_option,
    _nativenet_get_state,
    _nativenet_set_state,
    _nativenet_event,
};

netdev_t nativenet_default_dev = { NETDEV_TYPE_UNKNOWN, &nativenet_driver, 0, NULL };
#else
netdev_t nativenet_default_dev = { NETDEV_TYPE_UNKNOWN, NULL, 0, NULL };
#endif /* MODULE_NETDEV_BASE */

/** @} */
