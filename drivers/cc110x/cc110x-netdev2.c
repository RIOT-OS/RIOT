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
 * @brief       Implementation of netdev2 interface for cc110x
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
#include "cc110x-netdev2.h"
#include "cc110x-internal.h"
#include "cc110x-interface.h"
#include "net/eui64.h"

#include "periph/cpuid.h"
#include "periph/gpio.h"
#include "net/netdev2.h"
#include "net/gnrc/nettype.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static int _send(netdev2_t *dev, const struct iovec *vector, int count)
{
    DEBUG("%s:%u\n", __func__, __LINE__);

    netdev2_cc110x_t *netdev2_cc110x = (netdev2_cc110x_t*) dev;
    cc110x_pkt_t *cc110x_pkt = vector[0].iov_base;

    return cc110x_send(&netdev2_cc110x->cc110x, cc110x_pkt);
}

static int _recv(netdev2_t *dev, char* buf, int len)
{
    DEBUG("%s:%u\n", __func__, __LINE__);

    cc110x_t *cc110x = &((netdev2_cc110x_t*) dev)->cc110x;

    cc110x_pkt_t *cc110x_pkt = &cc110x->pkt_buf.packet;
    if (cc110x_pkt->length > len) {
        return -ENOSPC;
    }

    memcpy(buf, (void*)cc110x_pkt, cc110x_pkt->length);
    return cc110x_pkt->length;
}

static inline int _get_iid(netdev2_t *netdev, eui64_t *value, size_t max_len)
{
    if (max_len < sizeof(eui64_t)) {
        return -EOVERFLOW;
    }

    uint8_t *eui64 = (uint8_t*) value;
#ifdef CPUID_ID_LEN
    int n = (CPUID_ID_LEN < sizeof(eui64_t))
        ? CPUID_ID_LEN
        : sizeof(eui64_t);

    char cpuid[CPUID_ID_LEN];
    cpuid_get(cpuid);

    memcpy(eui64 + 8 - n, cpuid, n);

#else
    for (int i = 0; i < 8; i++) {
        eui64[i] = i;
    }
#endif

    /* make sure we mark the address as non-multicast and not globally unique */
    eui64[0] &= ~(0x01);
    eui64[0] |= 0x02;

    return sizeof(eui64_t);
}

static int _get(netdev2_t *dev, netopt_t opt, void *value, size_t value_len)
{
    cc110x_t *cc110x = &((netdev2_cc110x_t*) dev)->cc110x;

    switch (opt) {
        case NETOPT_DEVICE_TYPE:
            assert(value_len == 2);
            *((uint16_t *) value) = NETDEV2_TYPE_CC110X;
            return 2;
        case NETOPT_PROTO:
            assert(value_len == sizeof(gnrc_nettype_t));
#ifdef MODULE_GNRC_SIXLOWPAN
            *((gnrc_nettype_t*)value) = GNRC_NETTYPE_SIXLOWPAN;
#else
            *((gnrc_nettype_t*)value) = GNRC_NETTYPE_UNDEF;
#endif
            return sizeof(gnrc_nettype_t);
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

static int _set(netdev2_t *dev, netopt_t opt, void *value, size_t value_len)
{
    cc110x_t *cc110x = &((netdev2_cc110x_t*) dev)->cc110x;

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
        default:
            return -ENOTSUP;
    }

    return 0;
}

static void _netdev2_cc110x_isr(void *arg)
{
    netdev2_t *netdev2 = (netdev2_t*) arg;
    netdev2->event_callback(netdev2, NETDEV2_EVENT_ISR, netdev2->isr_arg);
}

static void _netdev2_cc110x_rx_callback(void *arg)
{
    netdev2_t *netdev2 = (netdev2_t*) arg;
    cc110x_t *cc110x = &((netdev2_cc110x_t*) arg)->cc110x;
    gpio_irq_disable(cc110x->params.gdo2);
    netdev2->event_callback(netdev2, NETDEV2_EVENT_RX_COMPLETE, netdev2->isr_arg);
}

static void _isr(netdev2_t *dev)
{
    cc110x_t *cc110x = &((netdev2_cc110x_t*) dev)->cc110x;
    cc110x_isr_handler(cc110x, _netdev2_cc110x_rx_callback, (void*)dev);
}

static int _init(netdev2_t *dev)
{
    DEBUG("%s:%u\n", __func__, __LINE__);

    cc110x_t *cc110x = &((netdev2_cc110x_t*) dev)->cc110x;

    gpio_init_int(cc110x->params.gdo2, GPIO_NOPULL, GPIO_BOTH,
            &_netdev2_cc110x_isr, (void*)dev);

    gpio_set(cc110x->params.gdo2);
    gpio_irq_disable(cc110x->params.gdo2);

    /* Switch to RX mode */
    cc110x_rd_set_mode(cc110x, RADIO_MODE_ON);

    return 0;
}

const netdev2_driver_t netdev2_cc110x_driver = {
    .send=_send,
    .recv=_recv,
    .init=_init,
    .get=_get,
    .set=_set,
    .isr=_isr
};

int netdev2_cc110x_setup(netdev2_cc110x_t *netdev2_cc110x, const cc110x_params_t *params)
{
    DEBUG("netdev2_cc110x_setup()\n");
    netdev2_cc110x->netdev.driver = &netdev2_cc110x_driver;

    return cc110x_setup(&netdev2_cc110x->cc110x, params);
}
