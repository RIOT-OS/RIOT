/*
 * Copyright (C) 2019 Freie Universität Berlin
 *               2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_nrf52_802154
 * @{
 *
 * @file
 * @brief       Implementation of the radio driver for nRF52 radios
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 * @}
 */

#include <assert.h>
#include <string.h>
#include <errno.h>

#include "cpu.h"
#include "mutex.h"
#include "nrf_clock.h"

#include "net/ieee802154.h"
#include "periph/timer.h"
#include "net/netdev/ieee802154.h"
#include "nrf802154.h"

#define ENABLE_DEBUG        0
#include "debug.h"

static const netdev_driver_t nrf802154_netdev_driver;

static uint8_t rxbuf[IEEE802154_FRAME_LEN_MAX + 3]; /* len PHR + PSDU + LQI */
static uint8_t txbuf[IEEE802154_FRAME_LEN_MAX + 3]; /* len PHR + PSDU + LQI */
static netdev_ieee802154_t *nrf802154_dev;

#define ED_RSSISCALE        (4U)
#define ED_RSSIOFFS         (-92)

#define RX_COMPLETE         (0x1)
#define TX_COMPLETE         (0x2)
#define LIFS                (40U)
#define SIFS                (12U)
#define SIFS_MAXPKTSIZE     (18U)
#define TIMER_FREQ          (62500UL)
static volatile uint8_t _state;
static mutex_t _txlock;

/**
 * @brief   Set radio into DISABLED state
 */
static void _disable(void)
{
    /* set device into DISABLED state */
    if (NRF_RADIO->STATE != RADIO_STATE_STATE_Disabled) {
        NRF_RADIO->EVENTS_DISABLED = 0;
        NRF_RADIO->TASKS_DISABLE = 1;
        while (!(NRF_RADIO->EVENTS_DISABLED)) {};
        DEBUG("[nrf802154] Device state: DISABLED\n");
    }
}

/**
 * @brief   Set radio into RXIDLE state
 */
static void _enable_rx(void)
{
    DEBUG("[nrf802154] Set device state to RXIDLE\n");
    /* set device into RXIDLE state */
    if (NRF_RADIO->STATE != RADIO_STATE_STATE_RxIdle) {
        _disable();
    }
    NRF_RADIO->PACKETPTR = (uint32_t)rxbuf;
    NRF_RADIO->EVENTS_RXREADY = 0;
    NRF_RADIO->TASKS_RXEN = 1;
    while (!(NRF_RADIO->EVENTS_RXREADY)) {};
    DEBUG("[nrf802154] Device state: RXIDLE\n");
}

/**
 * @brief   Set radio into TXIDLE state
 */
static void _enable_tx(void)
{
    DEBUG("[nrf802154] Set device state to TXIDLE\n");
    /* set device into TXIDLE state */
    if (NRF_RADIO->STATE != RADIO_STATE_STATE_TxIdle) {
        _disable();
    }
    NRF_RADIO->PACKETPTR = (uint32_t)txbuf;
    NRF_RADIO->EVENTS_TXREADY = 0;
    NRF_RADIO->TASKS_TXEN = 1;
    while (!(NRF_RADIO->EVENTS_TXREADY)) {};
    DEBUG("[nrf802154] Device state: TXIDLE\n");
}

/**
 * @brief   Convert from dBm to the internal representation, when the
 *          radio operates as a IEEE802.15.4 transceiver.
 */
static inline uint8_t _dbm_to_ieee802154_hwval(int8_t dbm)
{
    return ((dbm - ED_RSSIOFFS) / ED_RSSISCALE);
}

/**
 * @brief   Convert from the internal representation to dBm, when the
 *          radio operates as a IEEE802.15.4 transceiver.
 */
static inline int8_t _hwval_to_ieee802154_dbm(uint8_t hwval)
{
    return (ED_RSSISCALE * hwval) + ED_RSSIOFFS;
}

/**
 * @brief   Get CCA threshold value in internal represetion
 */
static int _get_cca_thresh(void)
{
    return (NRF_RADIO->CCACTRL & RADIO_CCACTRL_CCAEDTHRES_Msk) >>
            RADIO_CCACTRL_CCAEDTHRES_Pos;
}

/**
 * @brief   Set CCA threshold value in internal represetion
 */
static void _set_cca_thresh(uint8_t thresh)
{
    NRF_RADIO->CCACTRL &= ~RADIO_CCACTRL_CCAEDTHRES_Msk;
    NRF_RADIO->CCACTRL |= thresh << RADIO_CCACTRL_CCAEDTHRES_Pos;
}

/**
 * @brief   Check whether the channel is clear or not
 * @note    So far only CCA with Energy Detection is supported (CCA_MODE=1).
 */
static bool _channel_is_clear(void)
{
    NRF_RADIO->CCACTRL |= RADIO_CCACTRL_CCAMODE_EdMode;
    NRF_RADIO->EVENTS_CCAIDLE = 0;
    NRF_RADIO->EVENTS_CCABUSY = 0;
    NRF_RADIO->TASKS_CCASTART = 1;

    for(;;) {
        if(NRF_RADIO->EVENTS_CCAIDLE) {
            return true;
        }
        if(NRF_RADIO->EVENTS_CCABUSY) {
            return false;
        }
    }
}

/**
 * @brief   Reset the RXIDLE state
 */
 static void _reset_rx(void)
 {
    if (NRF_RADIO->STATE != RADIO_STATE_STATE_RxIdle) {
        return;
    }

    /* reset RX state and listen for new packets */
    _state &= ~RX_COMPLETE;
    NRF_RADIO->TASKS_START = 1;
 }

static void _set_chan(uint16_t chan)
{
    assert((chan >= IEEE802154_CHANNEL_MIN) && (chan <= IEEE802154_CHANNEL_MAX));
    /* Channel map between 2400 MHZ ... 2500 MHz
     * -> Frequency = 2400 + FREQUENCY (MHz) */
    NRF_RADIO->FREQUENCY = (chan - 10) * 5;
    nrf802154_dev->chan = chan;
}

static int16_t _get_txpower(void)
{
    int8_t txpower = (int8_t)NRF_RADIO->TXPOWER;
    if (txpower < 0) {
        return (int16_t)(0xff00 | txpower);
    }
    return (int16_t)txpower;
}

static void _set_txpower(int16_t txpower)
{
    if (txpower > (int)RADIO_TXPOWER_TXPOWER_Max) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Max;
    }
    else if (txpower > 1) {
        NRF_RADIO->TXPOWER = (uint32_t)txpower;
    }
    else if (txpower > -1) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_0dBm;
    }
    else if (txpower > -5) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg4dBm;
    }
    else if (txpower > -9) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg8dBm;
    }
    else if (txpower > -13) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg12dBm;
    }
    else if (txpower > -17) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg16dBm;
    }
    else if (txpower > -21) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg20dBm;
    }
    else {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg40dBm;
    }
}

static void _timer_cb(void *arg, int chan)
{
    (void)arg;
    (void)chan;
    mutex_unlock(&_txlock);
    timer_stop(NRF802154_TIMER);
}

static int _init(netdev_t *dev)
{
    (void)dev;

    int result = timer_init(NRF802154_TIMER, TIMER_FREQ, _timer_cb, NULL);
    assert(result >= 0);
    (void)result;
    timer_stop(NRF802154_TIMER);

    /* initialize local variables */
    mutex_init(&_txlock);

    /* reset buffer */
    rxbuf[0] = 0;
    txbuf[0] = 0;
    _state = 0;

    /* the radio need the external HF clock source to be enabled */
    /* @todo    add proper handling to release the clock whenever the radio is
     *          idle */
    clock_hfxo_request();

    /* power on peripheral */
    NRF_RADIO->POWER = 1;

    /* make sure the radio is disabled/stopped */
    _disable();

    /* we configure it to run in IEEE802.15.4 mode */
    NRF_RADIO->MODE = RADIO_MODE_MODE_Ieee802154_250Kbit;
    /* and set some fitting configuration */
    NRF_RADIO->PCNF0 = ((8 << RADIO_PCNF0_LFLEN_Pos) |
                        (RADIO_PCNF0_PLEN_32bitZero << RADIO_PCNF0_PLEN_Pos) |
                        (RADIO_PCNF0_CRCINC_Include << RADIO_PCNF0_CRCINC_Pos));
    NRF_RADIO->PCNF1 = IEEE802154_FRAME_LEN_MAX;
    /* set start frame delimiter */
    NRF_RADIO->SFD = IEEE802154_SFD;
    /* set MHR filters */
    NRF_RADIO->MHRMATCHCONF = 0;              /* Search Pattern Configuration */
    NRF_RADIO->MHRMATCHMAS = 0xff0007ff;      /* Pattern mask */
    /* configure CRC conform to IEEE802154 */
    NRF_RADIO->CRCCNF = ((RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos) |
                         (RADIO_CRCCNF_SKIPADDR_Ieee802154 << RADIO_CRCCNF_SKIPADDR_Pos));
    NRF_RADIO->CRCPOLY = 0x011021;
    NRF_RADIO->CRCINIT = 0;

    /* Disable the hardware IFS handling  */
    NRF_RADIO->MODECNF0 |= RADIO_MODECNF0_RU_Msk;

    /* assign default addresses */
    netdev_ieee802154_setup(nrf802154_dev);

    nrf802154_dev->chan = CONFIG_IEEE802154_DEFAULT_CHANNEL;
    /* set default channel */
    _set_chan(nrf802154_dev->chan);

    /* set default CCA threshold */
    _set_cca_thresh(CONFIG_NRF802154_CCA_THRESH_DEFAULT);

    /* configure some shortcuts */
    NRF_RADIO->SHORTS = RADIO_SHORTS_RXREADY_START_Msk | RADIO_SHORTS_TXREADY_START_Msk;

    /* enable interrupts */
    NVIC_EnableIRQ(RADIO_IRQn);
    NRF_RADIO->INTENSET = RADIO_INTENSET_END_Msk;

    /* switch to RX mode */
    _enable_rx();

    return 0;
}

static int _send(netdev_t *dev,  const iolist_t *iolist)
{
    (void)dev;

    DEBUG("[nrf802154] Send a packet\n");

    assert(iolist);

    mutex_lock(&_txlock);

    /* copy packet data into the transmit buffer */
    unsigned int len = 0;
    for (; iolist; iolist = iolist->iol_next) {
        if ((IEEE802154_FCS_LEN + len + iolist->iol_len) > (IEEE802154_FRAME_LEN_MAX)) {
            DEBUG("[nrf802154] send: unable to do so, packet is too large!\n");
            mutex_unlock(&_txlock);
            return -EOVERFLOW;
        }
        /* Check if there is data to copy, prevents undefined behaviour with
         * memcpy when iolist->iol_base == NULL */
        if (iolist->iol_len) {
            memcpy(&txbuf[len + 1], iolist->iol_base, iolist->iol_len);
            len += iolist->iol_len;
        }
    }

    /* specify the length of the package. */
    txbuf[0] = len + IEEE802154_FCS_LEN;

    /* trigger the actual transmission */
    _enable_tx();
    DEBUG("[nrf802154] send: putting %i byte into the ether\n", len);

    /* set interframe spacing based on packet size */
    unsigned int ifs = (len + IEEE802154_FCS_LEN > SIFS_MAXPKTSIZE) ? LIFS
                                                                    : SIFS;
    timer_set(NRF802154_TIMER, 0, ifs);

    return len;
}

static int _recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    (void)dev;
    (void)info;

    size_t pktlen = (size_t)rxbuf[0] - IEEE802154_FCS_LEN;

    /* check if packet data is readable */
    if (!(_state & RX_COMPLETE)) {
        DEBUG("[nrf802154] recv: no packet data available\n");
        return 0;
    }

    if (buf == NULL) {
        if (len > 0) {
            /* drop packet */
            DEBUG("[nrf802154] recv: dropping packet of length %i\n", pktlen);
        }
        else {
          /* return packet length */
          DEBUG("[nrf802154] recv: return packet length: %i\n", pktlen);
          return pktlen;
        }
    }
    else if (len < pktlen) {
        DEBUG("[nrf802154] recv: buffer is to small\n");
        return -ENOBUFS;
    }
    else {
        DEBUG("[nrf802154] recv: reading packet of length %i\n", pktlen);
        memcpy(buf, &rxbuf[1], pktlen);
        if (info != NULL) {
            netdev_ieee802154_rx_info_t *radio_info = info;
            /* Hardware link quality indicator */
            uint8_t hwlqi = rxbuf[pktlen + 1];
            /* Convert to 802.15.4 LQI (page 319 of product spec v1.1) */
            radio_info->lqi = (uint8_t)(hwlqi > UINT8_MAX/ED_RSSISCALE
                                       ? UINT8_MAX
                                       : hwlqi * ED_RSSISCALE);
            /* Calculate RSSI by subtracting the offset from the datasheet.
             * Intentionally using a different calculation than the one from
             * figure 122 of the v1.1 product specification. This appears to
             * match real world performance better */
            radio_info->rssi = (int16_t)hwlqi + ED_RSSIOFFS;
        }
    }

    _reset_rx();

    return (int)pktlen;
}

static void _isr(netdev_t *dev)
{
    if (!nrf802154_dev->netdev.event_callback) {
        return;
    }
    if (_state & RX_COMPLETE) {
        nrf802154_dev->netdev.event_callback(dev, NETDEV_EVENT_RX_COMPLETE);
    }
    if (_state & TX_COMPLETE) {
        nrf802154_dev->netdev.event_callback(dev, NETDEV_EVENT_TX_COMPLETE);
        _state &= ~TX_COMPLETE;
    }
}

static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    assert(dev);

#ifdef MODULE_NETOPT
    DEBUG("[nrf802154] get: %s\n", netopt2str(opt));
#else
    DEBUG("[nrf802154] get: %d\n", opt);
#endif

    switch (opt) {
        case NETOPT_CHANNEL:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t *)value) = nrf802154_dev->chan;
            return sizeof(uint16_t);
        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            *((int16_t *)value) = _get_txpower();
            return sizeof(int16_t);
        case NETOPT_IS_CHANNEL_CLR:
            assert(max_len >= sizeof(netopt_enable_t));
            *((netopt_enable_t*)value) = _channel_is_clear();
            return sizeof(netopt_enable_t);
        case NETOPT_CCA_THRESHOLD:
            assert(max_len >= sizeof(int8_t));
            *((int8_t*)value) = _hwval_to_ieee802154_dbm(_get_cca_thresh());
            return sizeof(netopt_enable_t);
        default:
            return netdev_ieee802154_get((netdev_ieee802154_t *)dev,
                                          opt, value, max_len);
    }
}

static int _set(netdev_t *dev, netopt_t opt,
                const void *value, size_t value_len)
{
    assert(dev);

#ifdef MODULE_NETOPT
    DEBUG("[nrf802154] set: %s\n", netopt2str(opt));
#else
    DEBUG("[nrf802154] set: %d\n", opt);
#endif

    int8_t tmp;
    switch (opt) {
        case NETOPT_CHANNEL:
            assert(value_len == sizeof(uint16_t));
            _set_chan(*((uint16_t *)value));
            return sizeof(uint16_t);
        case NETOPT_TX_POWER:
            assert(value_len == sizeof(int16_t));
            _set_txpower(*((int16_t *)value));
            return sizeof(int16_t);
        case NETOPT_CCA_THRESHOLD:
            assert(value_len == sizeof(int8_t));
            tmp = *((int8_t*) value);
            /* ED offset cannot be less than the min RSSI offset */
            if ((tmp - ED_RSSIOFFS) < 0) {
                return -EINVAL;
            }
            _set_cca_thresh(_dbm_to_ieee802154_hwval(tmp));
            return sizeof(int8_t);
        default:
            return netdev_ieee802154_set((netdev_ieee802154_t *)dev,
                                          opt, value, value_len);
    }
}

void isr_radio(void)
{
    /* Clear flag */
    if (NRF_RADIO->EVENTS_END) {
        NRF_RADIO->EVENTS_END = 0;

        /* did we just send or receive something? */
        uint8_t state = (uint8_t)NRF_RADIO->STATE;
        switch(state) {
            case RADIO_STATE_STATE_RxIdle:
                /* only process packet if event callback is set and CRC is valid */
                if ((nrf802154_dev->netdev.event_callback) &&
                    (NRF_RADIO->CRCSTATUS == 1) &&
                    (netdev_ieee802154_dst_filter(nrf802154_dev,
                                                  &rxbuf[1]) == 0)) {
                    _state |= RX_COMPLETE;
                }
                else {
                    _reset_rx();
                }
                break;
            case RADIO_STATE_STATE_Tx:
            case RADIO_STATE_STATE_TxIdle:
            case RADIO_STATE_STATE_TxDisable:
                timer_start(NRF802154_TIMER);
                DEBUG("[nrf802154] TX state: %x\n", (uint8_t)NRF_RADIO->STATE);
                _state |= TX_COMPLETE;
                _enable_rx();
                break;
            default:
                DEBUG("[nrf802154] Unhandled state: %x\n", (uint8_t)NRF_RADIO->STATE);
        }
        if (_state) {
            netdev_trigger_event_isr(&nrf802154_dev->netdev);
        }
    }
    else {
        DEBUG("[nrf802154] Unknown interrupt triggered\n");
    }

    cortexm_isr_end();
}

void nrf802154_setup(nrf802154_t *dev)
{
    netdev_t *netdev = (netdev_t*) dev;
    netdev_ieee802154_t *netdev_ieee802154 = (netdev_ieee802154_t*) dev;
    nrf802154_dev = netdev_ieee802154;

    netdev->driver = &nrf802154_netdev_driver;
    netdev_register(netdev, NETDEV_NRF802154, 0);
    netdev_ieee802154_reset(netdev_ieee802154);
}

/**
 * @brief   Export of the netdev interface
 */
static const netdev_driver_t nrf802154_netdev_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr  = _isr,
    .get  = _get,
    .set  = _set
};
