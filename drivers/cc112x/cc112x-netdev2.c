/**
 * @ingroup     drivers_cc112x
 * @{
 * @file
 * @brief       Implementation of netdev2 interface for cc112x
 *
 * @author      Kubaszek Mateusz <mathir.km.riot@gmail.com>
 * @}
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cc112x.h"
#include "cc112x-netdev2.h"
#include "cc112x-internal.h"
#include "cc112x-interface.h"

#include "net/eui64.h"

#include "periph/cpuid.h"
#include "periph/gpio.h"
#include "net/netdev2.h"
#include "net/gnrc/nettype.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

static int _send(netdev2_t *dev, const struct iovec *vector, int count)
{
    DEBUG("%s:%u\n", __func__, __LINE__);

    netdev2_cc112x_t *netdev2_cc112x = (netdev2_cc112x_t*)dev;
    cc112x_pkt_t *cc112x_pkt = vector[0].iov_base;

    return cc112x_send(&netdev2_cc112x->cc112x, cc112x_pkt);
}

static int _recv(netdev2_t *dev, char* buf, int len)
{
    DEBUG("%s:%u\n", __func__, __LINE__);

    cc112x_t *cc112x = &((netdev2_cc112x_t*)dev)->cc112x;

    cc112x_pkt_t *cc112x_pkt = &cc112x->pkt_buf.packet;
    if(cc112x_pkt->length > len) {
        return -ENOSPC;
    }

    memcpy(buf, (void*)cc112x_pkt, cc112x_pkt->length);
    return cc112x_pkt->length;
}

static inline int _get_iid(netdev2_t *netdev, eui64_t *value, size_t max_len)
{
    if(max_len < sizeof(eui64_t)) {
        return -EOVERFLOW;
    }

    uint8_t *eui64 = (uint8_t*)value;
#ifdef CPUID_ID_LEN
    int n = (CPUID_ID_LEN < sizeof(eui64_t)) ? CPUID_ID_LEN : sizeof(eui64_t);

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
    cc112x_t *cc112x = &((netdev2_cc112x_t*)dev)->cc112x;

    switch(opt) {
    case NETOPT_DEVICE_TYPE:
        assert(value_len == 2);
        *((uint16_t *)value) = NETDEV2_TYPE_CC112X;
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
        *((uint16_t *)value) = (uint16_t)cc112x->radio_channel;
        return 2;
    case NETOPT_ADDRESS:
        assert(value_len > 0);
        *((uint8_t *)value) = cc112x->radio_address;
        return 1;
    case NETOPT_MAX_PACKET_SIZE:
        assert(value_len > 0);
        *((uint8_t *)value) = CC112X_PACKET_LENGTH;
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
    cc112x_t *cc112x = &((netdev2_cc112x_t*)dev)->cc112x;

    switch(opt) {
    case NETOPT_CHANNEL: {
        uint8_t *arg = (uint8_t*)value;
        uint8_t channel = arg[value_len - 1];
        if((channel < CC112X_MIN_CHANNR) || (channel > CC112X_MAX_CHANNR)) {
            return -EINVAL;
        }
        if(cc112x_set_channel(cc112x, channel) == -1) {
            return -EINVAL;
        }
        return 1;
    }
    case NETOPT_ADDRESS:
        if(value_len < 1) {
            return -EINVAL;
        }
        if(!cc112x_set_address(cc112x, *(uint8_t*)value)) {
            return -EINVAL;
        }
        return 1;
    default:
        return -ENOTSUP;
    }

    return 0;
}

static void _netdev2_cc112x_isr(void *arg)
{
    netdev2_t *netdev2 = (netdev2_t*)arg;
    netdev2->event_callback(netdev2, NETDEV2_EVENT_ISR, netdev2->isr_arg);
}

static void _netdev2_cc112x_rx_callback(void *arg)
{
    DEBUG("%s:%u\n", __func__, __LINE__);
    netdev2_t *netdev2 = (netdev2_t*)arg;
    cc112x_t *cc112x = &((netdev2_cc112x_t*)arg)->cc112x;
    gpio_irq_disable(cc112x->params.gpio2);
    netdev2->event_callback(netdev2, NETDEV2_EVENT_RX_COMPLETE,
            netdev2->isr_arg);
}

static void _isr(netdev2_t *dev)
{
    cc112x_t *cc112x = &((netdev2_cc112x_t*)dev)->cc112x;
    cc112x_isr_handler(cc112x, _netdev2_cc112x_rx_callback, (void*)dev);
}

static int _init(netdev2_t *dev)
{
    DEBUG("%s:%u\n", __func__, __LINE__);

    cc112x_t *cc112x = &((netdev2_cc112x_t*)dev)->cc112x;

    gpio_init_int(cc112x->params.gpio2, GPIO_PULLDOWN, GPIO_BOTH,
            &_netdev2_cc112x_isr, (void*)dev);

    gpio_irq_disable(cc112x->params.gpio2);

    /* Switch to RX mode */
    cc112x_rd_set_mode(cc112x, RADIO_MODE_ON);

    return 0;
}

const netdev2_driver_t netdev2_cc112x_driver = {.send = _send, .recv = _recv,
        .init = _init, .get = _get, .set = _set, .isr = _isr};

int netdev2_cc112x_setup(netdev2_cc112x_t *netdev2_cc112x,
        const cc112x_params_t *params)
{
    DEBUG("netdev2_cc112x_setup()\n");
    netdev2_cc112x->netdev.driver = &netdev2_cc112x_driver;

    return cc112x_setup(&netdev2_cc112x->cc112x, params);
}
