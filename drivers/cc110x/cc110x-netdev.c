/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 * @file
 * @brief       Implementation of netdev interface for cc110x
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cc110x.h"
#include "cc110x-netdev.h"
#include "cc110x-internal.h"
#include "cc110x-interface.h"
#include "net/eui64.h"

#include "periph/gpio.h"
#include "net/netdev.h"
#include "net/gnrc/nettype.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static int _send(netdev_t *dev, const struct iovec *vector, unsigned count)
{
    DEBUG("%s:%u\n", __func__, __LINE__);

    netdev_cc110x_t *netdev_cc110x = (netdev_cc110x_t*) dev;
    cc110x_pkt_t *cc110x_pkt = vector[0].iov_base;

    return cc110x_send(&netdev_cc110x->cc110x, cc110x_pkt);
}

static int _recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    DEBUG("%s:%u\n", __func__, __LINE__);

    cc110x_t *cc110x = &((netdev_cc110x_t*) dev)->cc110x;

    cc110x_pkt_t *cc110x_pkt = &cc110x->pkt_buf.packet;
    if (cc110x_pkt->length > len) {
        return -ENOSPC;
    }

    memcpy(buf, (void*)cc110x_pkt, cc110x_pkt->length);
    if (info != NULL) {
        netdev_cc110x_rx_info_t *cc110x_info = info;

        cc110x_info->rssi = cc110x->pkt_buf.rssi;
        cc110x_info->lqi = cc110x->pkt_buf.lqi;
    }
    return cc110x_pkt->length;
}

static inline int _get_iid(netdev_t *netdev, eui64_t *value, size_t max_len)
{
    cc110x_t *cc110x = &((netdev_cc110x_t*) netdev)->cc110x;
    uint8_t *eui64 = (uint8_t*) value;

    if (max_len < sizeof(eui64_t)) {
        return -EOVERFLOW;
    }

    /* make address compatible to https://tools.ietf.org/html/rfc6282#section-3.2.2*/
    memset(eui64, 0, sizeof(eui64_t));
    eui64[3] = 0xff;
    eui64[4] = 0xfe;
    eui64[7] = cc110x->radio_address;

    return sizeof(eui64_t);
}

static int _get(netdev_t *dev, netopt_t opt, void *value, size_t value_len)
{
    cc110x_t *cc110x = &((netdev_cc110x_t*) dev)->cc110x;

    switch (opt) {
        case NETOPT_DEVICE_TYPE:
            assert(value_len == 2);
            *((uint16_t *) value) = NETDEV_TYPE_CC110X;
            return 2;
#ifdef MODULE_GNRC_NETIF
        case NETOPT_PROTO:
            assert(value_len == sizeof(gnrc_nettype_t));
            *((gnrc_nettype_t *)value) = cc110x->proto;
            return sizeof(gnrc_nettype_t);
#endif
        case NETOPT_CHANNEL:
            assert(value_len > 1);
            *((uint16_t *)value) = (uint16_t)cc110x->radio_channel;
            return 2;
        case NETOPT_ADDRESS:
            assert(value_len > 0);
            *((uint8_t *)value) = cc110x->radio_address;
            return 1;
        case NETOPT_MAX_PACKET_SIZE:
            assert(value_len > 0);
            *((uint8_t *)value) = CC110X_PACKET_LENGTH;
            return 1;
        case NETOPT_IPV6_IID:
            return _get_iid(dev, value, value_len);
        default:
            break;
    }

    return -ENOTSUP;
}

static int _set(netdev_t *dev, netopt_t opt, void *value, size_t value_len)
{
    cc110x_t *cc110x = &((netdev_cc110x_t*) dev)->cc110x;

    switch (opt) {
        case NETOPT_CHANNEL:
            {
                uint8_t *arg = (uint8_t*)value;
                uint8_t channel = arg[value_len-1];
                if ((channel < CC110X_MIN_CHANNR) || (channel > CC110X_MAX_CHANNR)) {
                    return -EINVAL;
                }
                if (cc110x_set_channel(cc110x, channel) == -1) {
                    return -EINVAL;
                }
                return 1;
            }
        case NETOPT_ADDRESS:
            if (value_len < 1) {
                return -EINVAL;
            }
            if (!cc110x_set_address(cc110x, *(uint8_t*)value)) {
                return -EINVAL;
            }
            return 1;
#ifdef MODULE_GNRC_NETIF
        case NETOPT_PROTO:
            if (value_len != sizeof(gnrc_nettype_t)) {
                return -EINVAL;
            }
            else {
                cc110x->proto = (gnrc_nettype_t) value;
                return sizeof(gnrc_nettype_t);
            }
            break;
#endif
        default:
            return -ENOTSUP;
    }

    return 0;
}

static void _netdev_cc110x_isr(void *arg)
{
    netdev_t *netdev = (netdev_t*) arg;
    netdev->event_callback(netdev, NETDEV_EVENT_ISR);
}

static void _netdev_cc110x_rx_callback(void *arg)
{
    netdev_t *netdev = (netdev_t*) arg;
    cc110x_t *cc110x = &((netdev_cc110x_t*) arg)->cc110x;
    gpio_irq_disable(cc110x->params.gdo2);
    netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
}

static void _isr(netdev_t *dev)
{
    cc110x_t *cc110x = &((netdev_cc110x_t*) dev)->cc110x;
    cc110x_isr_handler(cc110x, _netdev_cc110x_rx_callback, (void*)dev);
}

static int _init(netdev_t *dev)
{
    DEBUG("%s:%u\n", __func__, __LINE__);

    cc110x_t *cc110x = &((netdev_cc110x_t*) dev)->cc110x;

    gpio_init_int(cc110x->params.gdo2, GPIO_IN, GPIO_BOTH,
            &_netdev_cc110x_isr, (void*)dev);

    gpio_set(cc110x->params.gdo2);
    gpio_irq_disable(cc110x->params.gdo2);

    /* Switch to RX mode */
    cc110x_rd_set_mode(cc110x, RADIO_MODE_ON);

    return 0;
}

const netdev_driver_t netdev_cc110x_driver = {
    .send=_send,
    .recv=_recv,
    .init=_init,
    .get=_get,
    .set=_set,
    .isr=_isr
};

int netdev_cc110x_setup(netdev_cc110x_t *netdev_cc110x, const cc110x_params_t *params)
{
    DEBUG("netdev_cc110x_setup()\n");
    netdev_cc110x->netdev.driver = &netdev_cc110x_driver;

    /* set default protocol */
#ifdef MODULE_GNRC_NETIF
# ifdef MODULE_GNRC_SIXLOWPAN
    netdev_cc110x->cc110x.proto = GNRC_NETTYPE_SIXLOWPAN;
# else
    netdev_cc110x->cc110x.proto = GNRC_NETTYPE_UNDEF;
# endif
#endif

    return cc110x_setup(&netdev_cc110x->cc110x, params);
}
