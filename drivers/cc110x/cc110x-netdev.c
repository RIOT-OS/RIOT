/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 * @file        cc110x_netdev.c
 * @brief       Functionality for netdev base interface
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @}
 */
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cc110x.h"
#include "cc110x-internal.h"

#include "periph/gpio.h"
#include "netdev/base.h"
#include "kernel_types.h"
#include "cpu-conf.h"

#ifdef MODULE_NETDEV_BASE
kernel_pid_t cc110x_netdev_event_handler = KERNEL_PID_UNDEF;
static netdev_rcv_data_cb_t cc110x_recv_cb = NULL;

int _cc110x_send_data(netdev_t *dev, void *dest, size_t dest_len,
        netdev_hlist_t *upper_layer_hdrs, void *data, size_t data_len)
{
    netdev_hlist_t *ptr = upper_layer_hdrs;
    uint8_t tx_buffer[data_len + netdev_get_hlist_len(upper_layer_hdrs)];
    size_t tx_ptr = 0;
    cc110x_packet_t cc110x_pkt;

    if (dev != &cc110x_dev) {
        return -ENODEV;
    }
    if (dest_len > sizeof(uint8_t)) {
        return -EAFNOSUPPORT;
    }
    if ((sizeof(tx_buffer) + CC1100_HEADER_LENGTH + 1) > PACKET_LENGTH) {
        return -EMSGSIZE;
    }

    /* append possible upper layer headers */
    if (upper_layer_hdrs) {
        do {
            memcpy(&(tx_buffer[tx_ptr]), ptr->header, ptr->header_len);
            tx_ptr += ptr->header_len;
            netdev_hlist_advance(&ptr);
        } while (ptr != upper_layer_hdrs);
    }

    /* append data */
    memcpy(&(tx_buffer[tx_ptr]), data, data_len);

    cc110x_pkt.length = sizeof(tx_buffer) + CC1100_HEADER_LENGTH;
    cc110x_pkt.address = *((uint8_t *)dest);
    cc110x_pkt.flags = 0;
    memcpy(cc110x_pkt.data, &tx_buffer[0], sizeof(tx_buffer));

    return cc110x_send(&cc110x_pkt);
}

int _cc110x_add_rcv_data_cb(netdev_t *dev, netdev_rcv_data_cb_t cb)
{
    if (dev != &cc110x_dev) {
        return -ENODEV;
    }
    else if (cc110x_recv_cb != NULL) {
        return -ENOBUFS;
    }

    cc110x_recv_cb = cb;

    return 0;
}

int _cc110x_rem_rcv_data_cb(netdev_t *dev, netdev_rcv_data_cb_t cb)
{
    if (dev != &cc110x_dev) {
        return -ENODEV;
    }

    if (cc110x_recv_cb == cb) {
        cc110x_recv_cb = NULL;
    }

    return 0;
}

int _cc110x_get_option(netdev_t *dev, netdev_opt_t opt, void *value, size_t *value_len)
{
    if (dev != &cc110x_dev) {
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
            *((uint8_t *)value) = cc110x_get_channel();
            break;
        case NETDEV_OPT_ADDRESS:
            if (*value_len < sizeof(radio_address_t)) {
                return -EOVERFLOW;
            }
            if (*value_len > sizeof(uint8_t)) {
                *value_len = sizeof(uint8_t);
            }
            *((uint8_t *)value) = (uint8_t) cc110x_get_address();
            break;
        case NETDEV_OPT_PROTO:
            if (*value_len < sizeof(netdev_proto_t)) {
                return -EOVERFLOW;
            }
            if (*value_len > sizeof(netdev_proto_t)) {
                *value_len = sizeof(netdev_proto_t);
            }
            *((netdev_proto_t *)value) = NETDEV_PROTO_CC110X;
            break;
        case NETDEV_OPT_MAX_PACKET_SIZE:
            if (*value_len == 0) {
                return -EOVERFLOW;
            }
            if (*value_len > sizeof(uint8_t)) {
                *value_len = sizeof(uint8_t);
            }
            *((uint8_t *)value) = PACKET_LENGTH;
            break;
        default:
            return -ENOTSUP;
    }

    return 0;
}

int _cc110x_set_option(netdev_t *dev, netdev_opt_t opt, void *value, size_t value_len)
{
    if (dev != &cc110x_dev) {
        return -ENODEV;
    }

    switch (opt) {
        case NETDEV_OPT_CHANNEL:
            if (value_len != sizeof(uint8_t)) {
                return -EOVERFLOW;
            }
            if (cc110x_set_channel(*((uint8_t *)value)) == -1) {
                return -EINVAL;
            }
            break;
        case NETDEV_OPT_ADDRESS:
            /* leaves room for optimization */
            if (value_len > sizeof(radio_address_t)) {
                return -EOVERFLOW;
            }
            radio_address_t temp_address;
            if (value_len == sizeof(uint8_t)) {
                temp_address = ((radio_address_t)(*((uint8_t *)value)));
            }
            else {
                temp_address = *((radio_address_t *)value);
            }
            if (!cc110x_set_address(temp_address)) {
                return -EINVAL;
            }
            break;
        default:
            return -ENOTSUP;
    }

    return 0;
}

int _cc110x_get_state(netdev_t *dev, netdev_state_t *state)
{
    if (dev != &cc110x_dev) {
        return -ENODEV;
    }

    switch(radio_state) {
        case RADIO_IDLE:
            *state = NETDEV_STATE_POWER_IDLE;
            break;
        case RADIO_SEND_BURST:
            *state = NETDEV_STATE_TX_BURST;
            break;
        case RADIO_RX:
            *state = NETDEV_STATE_RX_MODE;
            break;
        case RADIO_UNKNOWN:
        case RADIO_PWD:
        default:
            *state = NETDEV_STATE_POWER_OFF;
            break;
    }

    return 0;
}

int _cc110x_set_state(netdev_t *dev, netdev_state_t state)
{
    if (dev != &cc110x_dev) {
        return -ENODEV;
    }

    switch (state) {
        case NETDEV_STATE_POWER_OFF:
            gpio_irq_disable(CC110X_GDO2);
            cc110x_switch_to_pwd();
            break;
        case NETDEV_STATE_RX_MODE:
            gpio_irq_enable(CC110X_GDO2);
            cc110x_setup_rx_mode();
            break;
        default:
            return -ENOTSUP;
    }

    return 0;
}

void _cc110x_event(netdev_t *dev, uint32_t event_type)
{
    (void)dev;

    switch (event_type) {
        case CC110X_NETDEV_EVENT_RX:
            /* Call recv cb if registered */
            if (cc110x_recv_cb != NULL) {
                cc110x_packet_t p = cc110x_rx_buffer[rx_buffer_to_read].packet;
                cc110x_recv_cb(&cc110x_dev, &p.phy_src, sizeof(uint8_t), &p.address,
                        sizeof(uint8_t), p.data, p.length - CC1100_HEADER_LENGTH);
            }

            /* Increment counter to next packet in array*/
            if (++rx_buffer_to_read == RX_BUF_SIZE) {
                rx_buffer_to_read = 0;
            }
            break;
        default:
            break;
    }
}

void _cc110x_set_event_handler(netdev_t *dev, kernel_pid_t event_handler)
{
    (void)dev;
    cc110x_netdev_event_handler = event_handler;
}

const netdev_driver_t cc110x_net_driver = {
    cc110x_initialize,
    _cc110x_send_data,
    _cc110x_add_rcv_data_cb,
    _cc110x_rem_rcv_data_cb,
    _cc110x_get_option,
    _cc110x_set_option,
    _cc110x_get_state,
    _cc110x_set_state,
    _cc110x_event,
    _cc110x_set_event_handler,
};

netdev_t cc110x_dev = {NETDEV_TYPE_BASE, &cc110x_net_driver, 0};
#else
netdev_t cc110x_dev = {NETDEV_TYPE_BASE, 0, 0};
#endif /* MODULE_NETDEV_BASE */
