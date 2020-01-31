/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc13x2
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx IEEE 802.15.4 netdev driver
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "cc13x2_rf_prop_internal.h"
#include "cc13x2_rf_netdev.h"

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "iolist.h"
#include "irq.h"

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

#include "driverlib/setup.h"
#include "driverlib/vims.h"
#include "driverlib/rf_mailbox.h"

static void _irq_handler(void *arg)
{
    netdev_t *dev = (netdev_t *)arg;

    if (dev->event_callback) {
        dev->event_callback(dev, NETDEV_EVENT_ISR);
    }
}

static int _init(netdev_t *netdev)
{
    SetupTrimDevice();

    VIMSModeSet(VIMS_BASE, VIMS_MODE_ENABLED);
    VIMSConfigure(VIMS_BASE, true, true);

    /* enable & turn on hardware */
    cc13x2_rf_enable();

    /* setup some things */
    cc13x2_rf_t *dev = (cc13x2_rf_t *)netdev;
    cc13x2_rf_get_ieee_eui64(dev->netdev.long_addr);
    cc13x2_rf_irq_set_handler(_irq_handler, dev);

    /* set short addr */
    memcpy(dev->netdev.short_addr, dev->netdev.long_addr + 6, 2);

    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    dev->netdev.short_addr[1] &= 0x7F;

    /* reset upper layer */
    netdev_ieee802154_reset(&dev->netdev);

    /* enter RX */
    cc13x2_rf_rx_start();

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    //DEBUG("%s:%s:%u\n", __FILE__, __func__, __LINE__);
    (void)netdev;
    return cc13x2_rf_send(iolist);
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    //DEBUG("%s:%s:%u\n", __FILE__, __func__, __LINE__);
    cc13x2_rf_t *dev = (cc13x2_rf_t *)netdev;
    (void)dev;
    return cc13x2_rf_recv(buf, len, info);
}

static int _set_state(cc13x2_rf_t *dev, netopt_state_t state)
{
    (void)dev;
    switch (state) {
#if 0
        case NETOPT_STATE_STANDBY:
            break;
        case NETOPT_STATE_SLEEP:
            break;
        case NETOPT_STATE_IDLE:
            break;
        case NETOPT_STATE_TX:
            break;
        case NETOPT_STATE_RESET:
            break;
#endif
        default:
            return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

netopt_state_t _get_state(cc13x2_rf_t *dev)
{
    (void)dev;

    switch (_cc13x2_rf_state) {
        case cc13x2_stateDisabled:
            return NETOPT_STATE_OFF;
        case cc13x2_stateSleep:
            return NETOPT_STATE_IDLE;
        case cc13x2_stateReceive:
            return NETOPT_STATE_RX;
        case cc13x2_stateTransmit:
            return NETOPT_STATE_TX;
        default:
            return NETOPT_STATE_IDLE;
    }
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    cc13x2_rf_t *dev = (cc13x2_rf_t *) netdev;

    if (netdev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_AUTOACK:
            return -ENOTSUP;
        case NETOPT_STATE:
            assert(max_len >= sizeof(netopt_state_t));
            *((netopt_state_t *)val) = _get_state(dev);
            return sizeof(netopt_state_t);
        case NETOPT_RX_END_IRQ:
            *((netopt_enable_t *)val) = cc13x2_rf_irq_is_enabled(IRQ_RX_OK);
            return sizeof(netopt_enable_t);
        case NETOPT_TX_END_IRQ:
            *((netopt_enable_t *)val) = cc13x2_rf_irq_is_enabled(IRQ_TX_DONE);
            return sizeof(netopt_enable_t);
        case NETOPT_RETRANS:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *)val) = IEEE802154_MAC_MAX_FRAMES_RETRIES;
            return sizeof(uint8_t);
        case NETOPT_CSMA_RETRIES:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *)val) = IEEE802154_MAC_MAX_CSMA_BACKOFFS;
            return sizeof(uint8_t);
        case NETOPT_PROMISCUOUSMODE:
            return -ENOTSUP;

        case NETOPT_CSMA:
            return -ENOTSUP;

        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            *((uint16_t *)val) = cc13x2_rf_get_txpower();
            return sizeof(uint16_t);

#if 0
        case NETOPT_CHANNEL_PAGE:
            assert(max_len >= sizeof(uint16_t));
            ((uint8_t *)val)[1] = 0;
            ((uint8_t *)val)[0] = cc13x2_rf_get_page(dev);
            return sizeof(uint16_t);


        case NETOPT_PRELOADING:
            if (dev->flags & AT86RF2XX_OPT_PRELOADING) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_RX_START_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->flags & AT86RF2XX_OPT_TELL_RX_START);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_START_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->flags & AT86RF2XX_OPT_TELL_TX_START);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_RETRIES_NEEDED:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *)val) = dev->tx_retries;
            return sizeof(uint8_t);

        case NETOPT_CCA_THRESHOLD:
            assert(max_len >= sizeof(int8_t));
            *((int8_t *)val) = cc13x2_rf_get_cca_threshold(dev);
            res = sizeof(int8_t);
            break;

        case NETOPT_IS_CHANNEL_CLR:
            assert(max_len >= sizeof(netopt_enable_t));
            *((netopt_enable_t *)val) = cc13x2_rf_cca(dev);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_LAST_ED_LEVEL:
            assert(max_len >= sizeof(int8_t));
            *((int8_t *)val) = cc13x2_rf_get_ed_level(dev);
            res = sizeof(int8_t);
            break;

#endif
        default:
            break;
    }

    return netdev_ieee802154_get((netdev_ieee802154_t *)netdev, opt, val, max_len);
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    cc13x2_rf_t *dev = (cc13x2_rf_t *) netdev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            res = -ENOTSUP;
            break;
        case NETOPT_ADDRESS_LONG:
            res = -ENOTSUP;
            break;
        case NETOPT_NID:
            res = -ENOTSUP;
            break;
        case NETOPT_CHANNEL:
            assert(len == sizeof(uint16_t));
            uint8_t chan = (((const uint16_t *)val)[0]) & UINT8_MAX;
            cc13x2_rf_set_chan(chan);
            /* don't set res to set netdev_ieee802154_t::chan */
            break;

        case NETOPT_CHANNEL_PAGE:
            assert(len == sizeof(uint16_t));
            uint8_t page = (((const uint16_t *)val)[0]) & UINT8_MAX;
            if (page != 0) {
                res = -EINVAL;
            }
            else {
                res = sizeof(uint16_t);
            }
            break;

        case NETOPT_PROMISCUOUSMODE:
            res = -ENOTSUP;
            break;

        case NETOPT_CSMA:
            if (((const bool *)val)[0] == NETOPT_ENABLE) {
                res = sizeof(netopt_enable_t);
            }
            else {
                res = -ENOTSUP;
            }
            break;

        case NETOPT_TX_POWER:
            assert(len <= sizeof(int16_t));
            cc13x2_rf_set_txpower(*((const int16_t *)val));
            res = sizeof(uint16_t);
            break;

        case NETOPT_STATE:
            res = -ENOTSUP;
            break;

        case NETOPT_AUTOACK:
            res = -ENOTSUP;
            break;

        case NETOPT_ACK_PENDING:
            res = -ENOTSUP;
            break;

        case NETOPT_RETRANS:
            res = -ENOTSUP;
            break;

        case NETOPT_PRELOADING:
            res = -ENOTSUP;
            break;

        case NETOPT_RX_START_IRQ:
            res = -ENOTSUP;
            break;

        case NETOPT_RX_END_IRQ:
            if (((const bool *)val)[0]) {
                cc13x2_rf_irq_enable(IRQ_RX_OK);
            }
            else {
                cc13x2_rf_irq_disable(IRQ_RX_OK);
            }
            return sizeof(netopt_enable_t);

        case NETOPT_TX_START_IRQ:
            res = -ENOTSUP;
            break;

        case NETOPT_TX_END_IRQ:
            if (((const bool *)val)[0]) {
                cc13x2_rf_irq_enable(IRQ_TX_DONE);
            }
            else {
                cc13x2_rf_irq_disable(IRQ_TX_DONE);
            }
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA_RETRIES:
            res = -ENOTSUP;
            break;

        case NETOPT_CCA_THRESHOLD:
            res = -ENOTSUP;
            break;

        default:
            break;
    }

    if (res == -ENOTSUP) {
        res = netdev_ieee802154_set((netdev_ieee802154_t *)netdev, opt, val, len);
    }

    return res;
}

static void _isr(netdev_t *netdev)
{
    cc13x2_rf_t *dev = (cc13x2_rf_t *) netdev;
    (void)dev;

    unsigned state = irq_disable();
    unsigned flags = cc13x2_rf_get_flags();
    irq_restore(state);

    if (flags & 1) {
        while(cc13x2_rf_recv_avail()) {
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        }
    }
    if (flags & 2) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
}

const netdev_driver_t cc13x2_rf_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

void cc13x2_rf_setup(cc13x2_rf_t *dev)
{
    memset(dev, 0, sizeof(*dev));

    cc13x2_rf_init();

    dev->netdev.netdev.driver = &cc13x2_rf_driver;
    dev->netdev.pan = cc13x2_rf_get_pan();
    dev->netdev.chan = cc13x2_rf_get_chan();
}
