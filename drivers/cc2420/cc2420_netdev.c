/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc2420
 * @{
 *
 * @file
 * @brief       Netdev adaption for the cc2420 driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev2.h"
#include "net/netdev2/ieee802154.h"
#include "xtimer.h"

#include "cc2420.h"
#include "cc2420_netdev.h"
#include "cc2420_internal.h"
#include "cc2420_registers.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"


static int _send(netdev2_t *netdev, const struct iovec *vector, unsigned count);
static int _recv(netdev2_t *netdev, void *buf, size_t len, void *info);
static int _init(netdev2_t *netdev);
static void _isr(netdev2_t *netdev);
static int _get(netdev2_t *netdev, netopt_t opt, void *val, size_t max_len);
static int _set(netdev2_t *netdev, netopt_t opt, void *val, size_t len);

const netdev2_driver_t cc2420_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

static void _irq_handler(void *arg)
{
    netdev2_t *dev = (netdev2_t *)arg;

    if(dev->event_callback) {
        dev->event_callback(dev, NETDEV2_EVENT_ISR);
    }
}

static inline uint16_t to_u16(void *buf)
{
    return *((uint16_t *)buf);
}

static inline int16_t to_i16(void *buf)
{
    return *((int16_t *)buf);
}

static inline bool to_bool(void *buf)
{
    return *((bool *)buf);
}

static inline int w_u16(void *buf, uint16_t val)
{
    memcpy(buf, &val, sizeof(uint16_t));
    return sizeof(uint16_t);
}

static inline int w_i16(void *buf, int16_t val)
{
    memcpy(buf, &val, sizeof(int16_t));
    return sizeof(int16_t);
}

static inline int opt_state(void *buf, bool cond)
{
    *((netopt_enable_t *)buf) = !!(cond);
    return sizeof(netopt_enable_t);
}

static int _init(netdev2_t *netdev)
{
    cc2420_t *dev = (cc2420_t *)netdev;

    uint16_t reg;

    /* initialize power and reset pins -> put the device into reset state */
    gpio_init(dev->params.pin_reset, GPIO_OUT);
    gpio_set(dev->params.pin_reset);
    gpio_init(dev->params.pin_vrefen, GPIO_OUT);
    gpio_clear(dev->params.pin_vrefen);

    /* initialize the input lines */
    gpio_init(dev->params.pin_cca, GPIO_IN);
    gpio_init(dev->params.pin_sfd, GPIO_IN);
    gpio_init(dev->params.pin_fifo, GPIO_IN);
    gpio_init_int(dev->params.pin_fifop, GPIO_IN, GPIO_RISING, _irq_handler, dev);

    /* initialize the chip select line and the SPI bus */
    spi_init_cs(dev->params.spi, dev->params.pin_cs);

    /* power on and toggle reset */
    gpio_set(dev->params.pin_vrefen);
    gpio_clear(dev->params.pin_reset);
    xtimer_usleep(CC2420_RESET_DELAY);
    gpio_set(dev->params.pin_reset);

    /* test the connection to the device by reading MANFIDL register */
    reg = cc2420_reg_read(dev, CC2420_REG_MANFIDL);
    if (reg != CC2420_MANFIDL_VAL) {
        DEBUG("cc2420: init: unable to communicate with device\n");
        return -1;
    }

    /* turn on the oscillator and wait for it to be stable */
    cc2420_en_xosc(dev);
    if (!(cc2420_status(dev) & CC2420_STATUS_XOSC_STABLE)) {
        DEBUG("cc2420: init: oscillator did not stabilize\n");
        return -1;
    }

#ifdef MODULE_NETSTATS_L2
    memset(&netdev->stats, 0, sizeof(netstats_t));
#endif

    return cc2420_init((cc2420_t *)dev);
}

static void _isr(netdev2_t *netdev)
{
    netdev->event_callback(netdev, NETDEV2_EVENT_RX_COMPLETE);
}

static int _send(netdev2_t *netdev, const struct iovec *vector, unsigned count)
{
    cc2420_t *dev = (cc2420_t *)netdev;
    return (int)cc2420_send(dev, vector, count);
}

static int _recv(netdev2_t *netdev, void *buf, size_t len, void *info)
{
    cc2420_t *dev = (cc2420_t *)netdev;
    return (int)cc2420_rx(dev, buf, len, info);
}

static int _get(netdev2_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    if (netdev == NULL) {
        return -ENODEV;
    }

    cc2420_t *dev = (cc2420_t *)netdev;

    int ext = netdev2_ieee802154_get(&dev->netdev, opt, val, max_len);
    if (ext > 0) {
        return ext;
    }

    switch (opt) {

        case NETOPT_ADDRESS:
            assert(max_len >= sizeof(uint16_t));
            cc2420_get_addr_short(dev, val);
            return sizeof(uint16_t);

        case NETOPT_ADDRESS_LONG:
            assert(max_len >= 8);
            cc2420_get_addr_long(dev, val);
            return 8;

        case NETOPT_NID:
            assert(max_len >= sizeof(uint16_t));
            return w_u16(val, cc2420_get_pan(dev));

        case NETOPT_CHANNEL:
            assert(max_len >= sizeof(uint16_t));
            return w_u16(val, cc2420_get_chan(dev));

        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            return w_i16(val, cc2420_get_txpower(dev));

        case NETOPT_STATE:
            assert(max_len >= sizeof(netopt_state_t));
            *((netopt_state_t *)val) = cc2420_get_state(dev);
            return sizeof(netopt_state_t);

        case NETOPT_IS_CHANNEL_CLR:
            return opt_state(val, cc2420_cca(dev));

        case NETOPT_AUTOACK:
            return opt_state(val, (dev->options & CC2420_OPT_AUTOACK));

        case NETOPT_CSMA:
            return opt_state(val, (dev->options & CC2420_OPT_CSMA));

        case NETOPT_PRELOADING:
            return opt_state(val, (dev->options & CC2420_OPT_PRELOADING));

        case NETOPT_PROMISCUOUSMODE:
            return opt_state(val, (dev->options & CC2420_OPT_PROMISCUOUS));

        case NETOPT_RX_START_IRQ:
            return opt_state(val, (dev->options & CC2420_OPT_TELL_RX_START));

        case NETOPT_RX_END_IRQ:
            return opt_state(val, (dev->options & CC2420_OPT_TELL_TX_END));

        case NETOPT_TX_START_IRQ:
            return opt_state(val, (dev->options & CC2420_OPT_TELL_RX_START));

        case NETOPT_TX_END_IRQ:
            return opt_state(val, (dev->options & CC2420_OPT_TELL_RX_END));

        default:
            return -ENOTSUP;
    }
}

static int _set(netdev2_t *netdev, netopt_t opt, void *val, size_t val_len)
{
    if (netdev == NULL) {
        return -ENODEV;
    }

    cc2420_t *dev = (cc2420_t *)netdev;

    int ext = netdev2_ieee802154_set(&dev->netdev, opt, val, val_len);

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(val_len == 2);
            cc2420_set_addr_short(dev, (uint8_t *)val);
            return 2;

        case NETOPT_ADDRESS_LONG:
            assert(val_len == 8);
            cc2420_set_addr_long(dev, (uint8_t *)val);
            return 8;

        case NETOPT_NID:
            assert(val_len == sizeof(uint16_t));
            cc2420_set_pan(dev, to_u16(val));
            return sizeof(uint16_t);

        case NETOPT_CHANNEL:
            assert(val_len == sizeof(uint16_t));
            return cc2420_set_chan(dev, to_u16(val));

        case NETOPT_TX_POWER:
            assert(val_len == sizeof(int16_t));
            cc2420_set_txpower(dev, to_i16(val));
            return sizeof(int16_t);

        case NETOPT_STATE:
            assert(val_len == sizeof(netopt_state_t));
            return cc2420_set_state(dev, *((netopt_state_t *)val));

        case NETOPT_AUTOACK:
            return cc2420_set_option(dev, CC2420_OPT_AUTOACK, to_bool(val));

        case NETOPT_CSMA:
            return cc2420_set_option(dev, CC2420_OPT_CSMA, to_bool(val));

        case NETOPT_PRELOADING:
            return cc2420_set_option(dev, CC2420_OPT_PRELOADING, to_bool(val));

        case NETOPT_PROMISCUOUSMODE:
            return cc2420_set_option(dev, CC2420_OPT_PROMISCUOUS, to_bool(val));

        case NETOPT_RX_START_IRQ:
            return cc2420_set_option(dev, CC2420_OPT_TELL_RX_START, to_bool(val));

        case NETOPT_RX_END_IRQ:
            return cc2420_set_option(dev, CC2420_OPT_TELL_RX_END, to_bool(val));

        case NETOPT_TX_START_IRQ:
            return cc2420_set_option(dev, CC2420_OPT_TELL_TX_START, to_bool(val));

        case NETOPT_TX_END_IRQ:
            return cc2420_set_option(dev, CC2420_OPT_TELL_TX_END, to_bool(val));

        default:
            return ext;
    }

    return 0;
}
