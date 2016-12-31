/*
 * Copyright (C) 2017 Neo Nenaco <neo@nenaco.de>
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Netdev adaption for the MRF24J40 drivers
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Neo Nenaco <neo@nenaco.de>
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

#include "mrf24j40.h"
#include "mrf24j40_netdev.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_registers.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define _MAX_MHR_OVERHEAD   (25)

static int _send(netdev2_t *netdev, const struct iovec *vector, unsigned count);
static int _recv(netdev2_t *netdev, void *buf, size_t len, void *info);
static int _init(netdev2_t *netdev);
static void _isr(netdev2_t *netdev);
static int _get(netdev2_t *netdev, netopt_t opt, void *val, size_t max_len);
static int _set(netdev2_t *netdev, netopt_t opt, void *val, size_t len);

const netdev2_driver_t mrf24j40_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

static void _irq_handler(void *arg)
{
    netdev2_t *dev = (netdev2_t *) arg;

    if (dev->event_callback) {
        dev->event_callback(dev, NETDEV2_EVENT_ISR);
    }
    ((mrf24j40_t *)arg)->irq_flag = 1;
}

static int _init(netdev2_t *netdev)
{
    mrf24j40_t *dev = (mrf24j40_t *)netdev;

    /* initialise GPIOs */
    spi_init_cs(dev->params.spi, dev->params.cs_pin);
    gpio_init(dev->params.reset_pin, GPIO_OUT);
    gpio_set(dev->params.reset_pin);
    gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_RISING, _irq_handler, dev);

#ifdef MODULE_NETSTATS_L2
    memset(&netdev->stats, 0, sizeof(netstats_t));
#endif
    /* reset device to default values and put it into RX state */
    mrf24j40_reset(dev);
    return 0;
}

static int _send(netdev2_t *netdev, const struct iovec *vector, unsigned count)
{
    mrf24j40_t *dev = (mrf24j40_t *)netdev;
    const struct iovec *ptr = vector;
    size_t len = 0;

    mrf24j40_tx_prepare(dev);

    /* load packet data into FIFO */
    for (int i = 0; i < count; i++, ptr++) {
        /* current packet data + FCS too long */
        if ((len + ptr->iov_len + 2) > IEEE802154_FRAME_LEN_MAX) {
            DEBUG("[mrf24j40] error: packet too large (%u byte) to be send\n",
                  (unsigned)len + 2);
            return -EOVERFLOW;
        }

#ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_bytes += len;
#endif
        len = mrf24j40_tx_load(dev, ptr->iov_base, ptr->iov_len, len);
        if (i == 0) {
            dev->header_len = len;
        }

    }

    /* send data out directly if pre-loading is disabled */
    if (!(dev->netdev.flags & MRF24J40_OPT_PRELOADING)) {
        mrf24j40_tx_exec(dev);
    }
    /* return the number of bytes that were actually send out */
    return (int)len;

}

static int _recv(netdev2_t *netdev, void *buf, size_t len, void *info)
{
    mrf24j40_t *dev = (mrf24j40_t *)netdev;
    uint8_t phr;
    size_t pkt_len;

    /* Disable receiving while reading the RX fifo (datasheet sec. 3.11.4) */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG1, MRF24J40_BBREG1_RXDECINV );

    /* get the size of the received packet */
    phr = mrf24j40_reg_read_long(dev, MRF24J40_RX_FIFO);

    pkt_len = (phr & 0x7f) - 2;

    /* just return length when buf == NULL */
    if (buf == NULL) {
        return pkt_len;
    }
#ifdef MODULE_NETSTATS_L2
    netdev->stats.rx_count++;
    netdev->stats.rx_bytes += pkt_len;
#endif
    /* not enough space in buf */
    if (pkt_len > len) {
        DEBUG("[mrf24j40] No space in receive buffers\n");
        mrf24j40_reg_write_short(dev, MRF24J40_REG_RXFLUSH, MRF24J40_RXFLUSH_RXFLUSH);
        /* Turn on reception of packets off the air */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG1, 0x00);
        return -ENOBUFS;
    }
    /* copy payload */
    mrf24j40_rx_fifo_read(dev, 1, (uint8_t *)buf, pkt_len);

    if (info != NULL) {
        netdev2_ieee802154_rx_info_t *radio_info = info;
        /* Read LQI and RSSI values from the RX fifo */
        mrf24j40_rx_fifo_read(dev, phr + 1, &(radio_info->lqi), 1);
        mrf24j40_rx_fifo_read(dev, phr + 2, &(radio_info->rssi), 1);
    }

    /* Turn on reception of packets off the air */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG1, 0x00);
    return pkt_len;
}

static netopt_state_t _get_state(mrf24j40_t *dev)
{
    if (!(dev->pending & MRF24J40_TASK_TX_DONE)) {
        return NETOPT_STATE_TX;
    }
    if (dev->pending & MRF24J40_TASK_RX_READY) {
        return NETOPT_STATE_RX;
    }
    switch (dev->state) {
        case MRF24J40_PSEUDO_STATE_SLEEP:
            return NETOPT_STATE_SLEEP;
    }
    return NETOPT_STATE_IDLE;
}

static int _get(netdev2_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    mrf24j40_t *dev = (mrf24j40_t *) netdev;

    if (netdev == NULL) {
        return -ENODEV;
    }

    int res;
    switch (opt) {
        case NETOPT_CHANNEL_PAGE:
            if (max_len < sizeof(uint16_t)) {
                res = -EOVERFLOW;
            }
            else {
                ((uint8_t *)val)[1] = 0;
                ((uint8_t *)val)[0] = 0;
                res = sizeof(uint16_t);
            }
            break;

        case NETOPT_MAX_PACKET_SIZE:
            if (max_len < sizeof(int16_t)) {
                res = -EOVERFLOW;
            }
            else {
                *((uint16_t *)val) = IEEE802154_FRAME_LEN_MAX - _MAX_MHR_OVERHEAD;
                res = sizeof(uint16_t);
            }
            break;

        case NETOPT_STATE:
            if (max_len < sizeof(netopt_state_t)) {
                res = -EOVERFLOW;
            }
            else {
                *((netopt_state_t *)val) = _get_state(dev);
                res = sizeof(netopt_state_t);
            }
            break;

        case NETOPT_PRELOADING:
            if (dev->netdev.flags & MRF24J40_OPT_PRELOADING) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_PROMISCUOUSMODE:
            if (dev->netdev.flags & MRF24J40_OPT_PROMISCUOUS) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RX_START_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & MRF24J40_OPT_TELL_RX_START);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RX_END_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & MRF24J40_OPT_TELL_RX_END);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_START_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & MRF24J40_OPT_TELL_TX_START);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_END_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & MRF24J40_OPT_TELL_TX_END);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & MRF24J40_OPT_CSMA);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_POWER:
            if (max_len < sizeof(int16_t)) {
                res = -EOVERFLOW;
            }
            else {
                *((uint16_t *)val) = mrf24j40_get_txpower(dev);
                res = sizeof(uint16_t);
            }
            break;

        case NETOPT_RETRANS:
            if (max_len < sizeof(uint8_t)) {
                res = -EOVERFLOW;
            }
            else {
                *((uint8_t *)val) = MRF24J40_MAX_FRAME_RETRIES;
                res = sizeof(uint8_t);
            }
            break;

        case NETOPT_IS_CHANNEL_CLR:
            if (mrf24j40_cca(dev)) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA_RETRIES:
            if (max_len < sizeof(uint8_t)) {
                res = -EOVERFLOW;
            }
            else {
                *((uint8_t *)val) = mrf24j40_get_csma_max_retries(dev);
                res = sizeof(uint8_t);
            }
            break;

        case NETOPT_CCA_THRESHOLD:
            if (max_len < sizeof(int8_t)) {
                res = -EOVERFLOW;
            }
            else {
                *((int8_t *)val) = mrf24j40_get_cca_threshold(dev);
                res = sizeof(int8_t);
            }
            break;

        default:
            /* try netdev2 settings */
            res = netdev2_ieee802154_get((netdev2_ieee802154_t *)netdev, opt,
                                         val, max_len);
    }
    return res;
}

static int _set_state(mrf24j40_t *dev, netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_SLEEP:
            mrf24j40_set_state(dev, MRF24J40_PSEUDO_STATE_SLEEP);
            break;
        case NETOPT_STATE_IDLE:
            mrf24j40_set_state(dev, MRF24J40_PSEUDO_STATE_IDLE);
            break;
        case NETOPT_STATE_TX:
            if (dev->netdev.flags & MRF24J40_OPT_PRELOADING) {
                mrf24j40_tx_exec(dev);
            }
            break;
        case NETOPT_STATE_RESET:
            mrf24j40_reset(dev);
            break;
        default:
            return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

static int _set(netdev2_t *netdev, netopt_t opt, void *val, size_t len)
{
    mrf24j40_t *dev = (mrf24j40_t *) netdev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            if (len > sizeof(uint16_t)) {
                res = -EOVERFLOW;
            }
            else {
                mrf24j40_set_addr_short(dev, *((uint16_t *)val));
                /* don't set res to set netdev2_ieee802154_t::short_addr */
            }
            break;

        case NETOPT_ADDRESS_LONG:
            if (len > sizeof(uint64_t)) {
                res = -EOVERFLOW;
            }
            else {
                mrf24j40_set_addr_long(dev, *((uint64_t *)val));
                /* don't set res to set netdev2_ieee802154_t::long_addr */
            }
            break;

        case NETOPT_NID:
            if (len > sizeof(uint16_t)) {
                res = -EOVERFLOW;
            }
            else {
                mrf24j40_set_pan(dev, *((uint16_t *)val));
                /* don't set res to set netdev2_ieee802154_t::pan */
            }
            break;

        case NETOPT_CHANNEL:
            if (len != sizeof(uint16_t)) {
                res = -EINVAL;
            }
            else {
                uint8_t chan = ((uint8_t *)val)[0];
                if (chan < IEEE802154_CHANNEL_MIN ||
                    chan > IEEE802154_CHANNEL_MAX ||
                    dev->netdev.chan == chan) {
                    res = -EINVAL;
                    break;
                }
                mrf24j40_set_chan(dev, chan);
                /* don't set res to set netdev2_ieee802154_t::chan */
            }
            break;

        case NETOPT_CHANNEL_PAGE:
            if (len != sizeof(uint16_t)) {
                res = -EINVAL;
            }
            else {
                uint8_t page = ((uint8_t *)val)[0];

                /* mrf24j40 only supports page 0, no need to configure anything in the driver. */
                if (page != 0) {
                    res = -EINVAL;
                }
                else {
                    res = sizeof(uint16_t);
                }
            }
            break;

        case NETOPT_TX_POWER:
            if (len > sizeof(int16_t)) {
                res = -EOVERFLOW;
            }
            else {
                mrf24j40_set_txpower(dev, *((int16_t *)val));
                res = sizeof(uint16_t);
            }
            break;

        case NETOPT_STATE:
            if (len > sizeof(netopt_state_t)) {
                res = -EOVERFLOW;
            }
            else {
                res = _set_state(dev, *((netopt_state_t *)val));
            }
            break;

        case NETOPT_AUTOACK:
            mrf24j40_set_option(dev, NETDEV2_IEEE802154_ACK_REQ,
                                ((bool *)val)[0]);
            /* don't set res to set netdev2_ieee802154_t::flags */
            break;

        case NETOPT_PRELOADING:
            mrf24j40_set_option(dev, MRF24J40_OPT_PRELOADING,
                                ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_PROMISCUOUSMODE:
            mrf24j40_set_option(dev, MRF24J40_OPT_PROMISCUOUS,
                                ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RX_START_IRQ:
            mrf24j40_set_option(dev, MRF24J40_OPT_TELL_RX_START,
                                ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RX_END_IRQ:
            mrf24j40_set_option(dev, MRF24J40_OPT_TELL_RX_END,
                                ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_START_IRQ:
            mrf24j40_set_option(dev, MRF24J40_OPT_TELL_TX_START,
                                ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_END_IRQ:
            mrf24j40_set_option(dev, MRF24J40_OPT_TELL_TX_END,
                                ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA:
            mrf24j40_set_option(dev, MRF24J40_OPT_CSMA,
                                ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA_RETRIES:
            if ((len > sizeof(uint8_t)) ||
                (*((uint8_t *)val) > 5)) {
                res = -EOVERFLOW;
            }
            else if (dev->netdev.flags & MRF24J40_OPT_CSMA) {
                /* only set if CSMA is enabled */
                mrf24j40_set_csma_max_retries(dev, *((uint8_t *)val));
                res = sizeof(uint8_t);
            }
            break;

        case NETOPT_CCA_THRESHOLD:
            if (len > sizeof(int8_t)) {
                res = -EOVERFLOW;
            }
            else {
                mrf24j40_set_cca_threshold(dev, *((int8_t *)val));
                res = sizeof(int8_t);
            }
            break;

        default:
            break;
    }
    /* try netdev2 building flags */
    if (res == -ENOTSUP) {
        res = netdev2_ieee802154_set((netdev2_ieee802154_t *)netdev, opt,
                                     val, len);
    }
    return res;
}

static void _isr(netdev2_t *netdev)
{
    mrf24j40_t *dev = (mrf24j40_t *) netdev;
    /* update pending bits */
    mrf24j40_update_tasks(dev);
    DEBUG("[mrf24j40] INTERRUPT (pending: %x),\n", dev->pending);
    /* Transmit interrupt occured */
    if (dev->pending & MRF24J40_TASK_TX_READY) {
        dev->pending &= ~(MRF24J40_TASK_TX_READY);
        DEBUG("[mrf24j40] EVT - TX_END\n");
#ifdef MODULE_NETSTATS_L2
        if (netdev->event_callback && (dev->netdev.flags & MRF24J40_OPT_TELL_TX_END)) {
            uint8_t txstat = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXSTAT);
            /* transmision failed */
            if (txstat & MRF24J40_TXSTAT_TXNSTAT) {
                /* TX_NOACK - CCAFAIL */
                if (txstat & MRF24J40_TXSTAT_CCAFAIL) {
                    netdev->event_callback(netdev, NETDEV2_EVENT_TX_MEDIUM_BUSY);
                    DEBUG("[mrf24j40] TX_CHANNEL_ACCESS_FAILURE\n");
                }
                /* check max retries */
                else if (txstat & MRF24J40_TXSTAT_MAX_FRAME_RETRIES) {
                    netdev->event_callback(netdev, NETDEV2_EVENT_TX_NOACK);
                    DEBUG("[mrf24j40] TX NO_ACK\n");
                }
            }
            else {
                netdev->event_callback(netdev, NETDEV2_EVENT_TX_COMPLETE);
            }
        }
#endif
    }
    /* Receive interrupt occured */
    if (dev->pending & MRF24J40_TASK_RX_READY) {
        DEBUG("[mrf24j40] EVT - RX_END\n");
        if ((dev->netdev.flags & MRF24J40_OPT_TELL_RX_END)) {
            netdev->event_callback(netdev, NETDEV2_EVENT_RX_COMPLETE);
        }
        dev->pending &= ~(MRF24J40_TASK_RX_READY);
    }
    DEBUG("[mrf24j40] END IRQ\n");
}
