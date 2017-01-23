/*
 * Copyright (C) 2016 Hochschule für Angewandte Wissenschaften Hamburg
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
 * @author      Tobias Fredersdorf <Tobias.Fredersdorf@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev2.h"
#include "net/netdev2_ieee802154.h"

#include "mrf24j40.h"
#include "mrf24j40_netdev.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_registers.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define _MAX_MHR_OVERHEAD   (25)

static int _send(netdev2_t *netdev, const struct iovec *vector, int count);
static int _recv(netdev2_t *netdev, char *buf, int len);
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
        dev->event_callback(dev, NETDEV2_EVENT_ISR, NULL);
    }
}

static int _init(netdev2_t *netdev)
{
    mrf24j40_t *dev = (mrf24j40_t *)netdev;

    /* initialise GPIOs */
    gpio_init(dev->cs_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_set(dev->cs_pin);
    gpio_init(dev->sleep_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_clear(dev->sleep_pin);
    gpio_init(dev->reset_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_set(dev->reset_pin);
    gpio_init_int(dev->int_pin, GPIO_NOPULL, GPIO_RISING, _irq_handler, dev);

    /* make sure device is not sleeping, so we can query part number */
    mrf24j40_assert_awake(dev);
    /* reset device to default values and put it into RX state */
    mrf24j40_reset(dev);

    return 0;
}

static int _send(netdev2_t *netdev, const struct iovec *vector, int count)
{
    mrf24j40_t *dev = (mrf24j40_t *)netdev;
    const struct iovec *ptr = vector;
    size_t len = 0;

    mrf24j40_tx_prepare(dev);

    /* load packet data into FIFO */
    for (int i = 0; i < count; i++, ptr++) {
        /* current packet data + FCS too long */
        if ((len + ptr->iov_len + 2) > MRF24J40_MAX_PKT_LENGTH) {
            printf("[mrf24j40] error: packet too large (%u byte) to be send\n",
                   (unsigned)len + 2);
            return -EOVERFLOW;
        }
        len = mrf24j40_tx_load(dev, ptr->iov_base, ptr->iov_len, len);
    }
/
    return (int)len;
}

static int _recv(netdev2_t *netdev, char *buf, int len)
{
    mrf24j40_t *dev = (mrf24j40_t *)netdev;
    uint8_t phr;
    size_t pkt_len;

    /* frame buffer protection will be unlocked as soon as mrf24j40_fb_stop()
     * is called*/
    mrf24j40_fb_start(dev); 
    /* get the size of the received packet */
    mrf24j40_fb_read(dev, &phr, 1);

    /* Include FCS, LQI and RSSI in packet length */
    pkt_len = phr + 2;
    /* just return length when buf == NULL */
    if (buf == NULL) {
        mrf24j40_fb_stop(dev);
        return pkt_len;
    }
    /* not enough space in buf */
    if (pkt_len > len) {
        mrf24j40_fb_stop(dev);
        return -ENOBUFS;
    }
}

netopt_state_t _get_state(mrf24j40_t *dev)
{
    switch (mrf24j40_get_status(dev)) {
        case MRF24J40_RFSTATE_SLEEP:
            return NETOPT_STATE_SLEEP;
        case MRF24J40_RFSTATE_RX:
            return NETOPT_STATE_RX;
        case MRF24J40_RFSTATE_TX:
            return NETOPT_STATE_TX;
        case MRF24J40_RFSTATE_CALVCO:
            return NETOPT_STATE_CALVCO;
        case MRF24J40_RFSTATE_CALFIL:
            return NETOPT_STATE_CALFIL;
        case MRF24J40_RFSTATE_RESET:
            return NETOPT_STATE_RESET;
        case MRF24J40_RFSTATE_RTSEL1:
            return NETOPT_STATE_RTSEL1;
        case MRF24J40_RFSTATE_RTSEL2:
            return NETOPT_STATE_RTSEL2;
    }
}

static int _set_state(mrf24j40_t *dev, netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_SLEEP:
            mrf24j40_set_state(dev, MRF24J40_RFSTATE_SLEEP);
            break;
        case NETOPT_STATE_RX:
            mrf24j40_set_state(dev, MRF24J40_RFSTATE_RX);
            break;
        case NETOPT_STATE_TX:
            mrf24j40_set_state(dev, MRF24J40_RFSTATE_TX);
            break;
        case NETOPT_STATE_RESET:
            mrf24j40_reset(dev);
            break;
        default:
            return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

static int _get(netdev2_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    mrf24j40_t *dev = (mrf24j40_t *) netdev;

    if (netdev == NULL) {
        return -ENODEV;
    }

    /* getting these options doesn't require the transceiver to be responsive */
    switch (opt) {
        

        case NETOPT_MAX_PACKET_SIZE:
            if (max_len < sizeof(int16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)val) = MRF24J40_MAX_PKT_LENGTH - _MAX_MHR_OVERHEAD;
            return sizeof(uint16_t);

        case NETOPT_STATE:
            if (max_len < sizeof(netopt_state_t)) {
                return -EOVERFLOW;
            }
            *((netopt_state_t *)val) = _get_state(dev);
            return sizeof(netopt_state_t);

        case NETOPT_PRELOADING:
            if (dev->netdev.flags & MRF24J40_OPT_PRELOADING) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_PROMISCUOUSMODE:
            if (dev->netdev.flags & MRF24J40_OPT_PROMISCUOUS) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_RX_START_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & MRF24J40_OPT_TELL_RX_START);
            return sizeof(netopt_enable_t);

        case NETOPT_RX_END_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & MRF24J40_OPT_TELL_RX_END);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_START_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & MRF24J40_OPT_TELL_TX_START);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_END_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & MRF24J40_OPT_TELL_TX_END);
            return sizeof(netopt_enable_t);

        case NETOPT_CSMA:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & MRF24J40_OPT_CSMA);
            return sizeof(netopt_enable_t);

        default:
            /* Can still be handled in second switch */
            break;
    }

    int res;

    if (((res = netdev2_ieee802154_get((netdev2_ieee802154_t *)netdev, opt, val,
                                       max_len)) >= 0) || (res != -ENOTSUP)) {
        return res;
    }

    uint8_t old_state = mrf24j40_get_status(dev);
    res = 0;

    /* temporarily wake up if sleeping */
    if (old_state == MRF24J40_STATE_SLEEP) {
        mrf24j40_assert_awake(dev);
    }

    /* these options require the transceiver to be not sleeping*/
    switch (opt) {
        case NETOPT_TX_POWER:
            if (max_len < sizeof(int16_t)) {
                res = -EOVERFLOW;
            }
            else {
                *((uint16_t *)val) = mrf24j40_get_txpower(dev);
                res = sizeof(uint16_t);
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

        default:
            res = -ENOTSUP;
    }

    /* go back to sleep if were sleeping */
    if (old_state == MRF24J40_STATE_SLEEP) {
        mrf24j40_set_state(dev, MRF24J40_STATE_SLEEP);
    }

    return res;
}

static int _set(netdev2_t *netdev, netopt_t opt, void *val, size_t len)
{
    mrf24j40_t *dev = (mrf24j40_t *) netdev;
    uint8_t old_state = mrf24j40_get_status(dev);
    int res = 0;

    if (dev == NULL) {
        return -ENODEV;
    }

    /* temporarily wake up if sleeping */
    if (old_state == MRF24J40_STATE_SLEEP) {
        mrf24j40_assert_awake(dev);
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
                if (chan < MRF24J40_MIN_CHANNEL ||
                    chan > MRF24J40_MAX_CHANNEL) {
                    res = -ENOTSUP;
                    break;
                }
                mrf24j40_set_chan(dev, chan);
                /* don't set res to set netdev2_ieee802154_t::chan */
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
            else if (!(dev->netdev.flags & MRF24J40_OPT_CSMA)) {
                /* If CSMA is disabled, don't allow setting retries */
                res = -ENOTSUP;
            }
            else {
                mrf24j40_set_csma_max_retries(dev, *((uint8_t *)val));
                res = sizeof(uint8_t);
            }
            break;


        default:
            res = -ENOTSUP;
    }

    /* go back to sleep if were sleeping and state hasn't been changed */
    if ((old_state == MRF24J40_STATE_SLEEP) &&
        (opt != NETOPT_STATE)) {
        mrf24j40_set_state(dev, MRF24J40_STATE_SLEEP);
    }

    if (res == -ENOTSUP) {
        res = netdev2_ieee802154_set((netdev2_ieee802154_t *)netdev, opt,
                                     val, len);
    }

    return res;
}

static void _isr(netdev2_t *netdev)
{
    mrf24j40_t *dev = (mrf24j40_t *) netdev;
    uint8_t irq_mask;
    uint8_t state;

    /* If transceiver is sleeping register access is impossible and frames are
     * lost anyway, so return immediately.
     */
    state = mrf24j40_get_status(dev);
    if (state == MRF24J40_STATE_SLEEP) {
        return;
    }

    /* read (consume) device status */
    irq_mask = mrf24j40_reg_read_short(dev, MRF24J40_REG_INTSTAT);

    if (irq_mask & MRF24J40_IRQ_STATUS_MASK__TRX_END) {
        if (state == MRF24J40_RFSTATE_RX){
            if (!(dev->netdev.flags & MRF24J40_OPT_TELL_RX_END)) {
                return;
            }
            netdev->event_callback(netdev, NETDEV2_EVENT_RX_COMPLETE, NULL);
        }
        else if (state == MRF24J40_RFSTATE_TX) {
            mrf24j40_set_state(dev, dev->idle_state);
            if (netdev->event_callback && (dev->netdev.flags & MRF24J40_OPT_TELL_TX_END)) {
                netdev->event_callback(netdev, NETDEV2_EVENT_TX_COMPLETE, NULL);
            }
        }
    }
}