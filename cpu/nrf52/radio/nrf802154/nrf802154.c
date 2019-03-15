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
 * @author      Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <string.h>
#include <errno.h>

#include "cpu.h"
#include "luid.h"
#include "mutex.h"

#include "net/ieee802154.h"
#include "periph/timer.h"
#include "net/netdev/ieee802154.h"
#include "nrf802154.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief Default number of retransmissions
 */
#define NRF802154_DEFAULT_RETRANS    (3U)

/**
 * @brief Maximum number of retransmissions as per IEEE802.15.4
 */
#define NRF802154_MAX_RETRANS        (7U)

/* Internal device option flags */
#define NRF802154_OPT_AUTOACK        (0x0080)      /**< Auto ACK active */
#define NRF802154_OPT_ACK_REQUEST    (0x0100)      /**< Transmit unicast with
                                                    *   Ack request set */

static const netdev_driver_t nrf802154_netdev_driver;

netdev_ieee802154_t nrf802154_dev = {
    {
        .driver = &nrf802154_netdev_driver,
        .event_callback = NULL,
        .context = NULL,
    },
#ifdef MODULE_GNRC
#ifdef MODULE_GNRC_SIXLOWPAN
    .proto = GNRC_NETTYPE_SIXLOWPAN,
#else
    .proto = GNRC_NETTYPE_UNDEF,
#endif
#endif
    .pan = IEEE802154_DEFAULT_PANID,
    .short_addr = { 0, 0 },
    .long_addr = { 0, 0, 0, 0, 0, 0, 0, 0 },
    .chan = IEEE802154_DEFAULT_CHANNEL,
    .flags = 0
};

static uint8_t rxbuf[IEEE802154_FRAME_LEN_MAX + 3]; /* len PHR + PSDU + LQI */
static uint8_t txbuf[IEEE802154_FRAME_LEN_MAX + 3]; /* len PHR + PSDU + LQI */
static uint8_t aackbuf[4];

static uint8_t _last_seq_no;
/* Max number of retransmission attempts configured */
static uint8_t _retrans_max;
/* Retransmission counter, the retrans_max + 1 indicates all retransmissions
 * failed */
static uint8_t _retransmissions;
/* Number of retransmissions required for the last transmission */
static uint8_t _retrans_used;

#define ED_RSSISCALE        (4U)
#define ED_RSSIOFFS         (92U)

#define RX_COMPLETE         (0x1)
#define TX_COMPLETE         (0x2)
#define LIFS                (40U)
#define SIFS                (12U)
#define TACK                (54U) /* Ack timeout in symbols */
#define SIFS_MAXPKTSIZE     (18U)
#define TIMER_FREQ          (62500UL)
static nrf802154_state_t _state;
static volatile uint8_t _event_flags;
static mutex_t _txlock;
static uint16_t _setting_flags;

static inline bool _setting_isset(uint16_t flag)
{
    return _setting_flags & flag;
}

static inline void _setting_enable(uint16_t flag)
{
    _setting_flags |= flag;
}

static inline void _setting_disable(uint16_t flag)
{
    _setting_flags &= ~flag;
}

static inline bool _ack_expected(void)
{
    return (_setting_isset(NRF802154_OPT_ACK_REQUEST) &&
            txbuf[1] & IEEE802154_FCF_ACK_REQ);
}

static inline bool _ack_xmit_required(void)
{
    return (_setting_isset(NRF802154_OPT_AUTOACK) &&
            rxbuf[1] & IEEE802154_FCF_ACK_REQ);
}

static inline void _set_and_start_timer(unsigned timeout)
{
    timer_set(NRF802154_TIMER, 0, timeout);
    timer_start(NRF802154_TIMER);
}

static inline unsigned _get_tx_ifs(void)
{
    return (txbuf[0] > SIFS_MAXPKTSIZE) ? LIFS : SIFS;
}

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
 * @brief   Set the radio in RXIDLE
 */
static void _enable_listen(void)
{
    DEBUG("[nrf802154] Set device state to RXIDLE\n");
    NRF_RADIO->PACKETPTR = (uint32_t)rxbuf;
    NRF_RADIO->EVENTS_RXREADY = 0;
    /* set device into RXIDLE state */
    if (NRF_RADIO->STATE != RADIO_STATE_STATE_RxIdle) {
        _disable();
    }
    else {
        NRF_RADIO->TASKS_RXEN = 1;
        return;
    }
    NRF_RADIO->TASKS_RXEN = 1;
    while (!(NRF_RADIO->EVENTS_RXREADY)) {};
    DEBUG("[nrf802154] Device state: RXIDLE\n");
}

/**
 * @brief   Set radio into RXIDLE state and start listening for new frames
 */
static void _enable_rx(void)
{
    _state = NRF802154_STATE_RX;
    _enable_listen();
}

static void _load_tx(void)
{
    NRF_RADIO->PACKETPTR = (uint32_t)txbuf;
    NRF_RADIO->EVENTS_TXREADY = 0;
}

static void _isr_tx_complete(void)
{
    _set_and_start_timer(_get_tx_ifs());
    _event_flags |= TX_COMPLETE;
    _retrans_used = _retransmissions;
    nrf802154_dev.netdev.event_callback(&nrf802154_dev.netdev, NETDEV_EVENT_ISR);
    _enable_rx();
}

/**
 * @brief   Set radio into TXIDLE state
 */
static void _enable_tx(void)
{
    _state = NRF802154_STATE_TX;
    DEBUG("[nrf802154] Set device state to TXIDLE\n");
    /* set device into TXIDLE state */
    if (NRF_RADIO->STATE != RADIO_STATE_STATE_TxIdle) {
        _disable();
    }
    _load_tx();
    NRF_RADIO->TASKS_TXEN = 1;
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
    _event_flags &= ~RX_COMPLETE;
    NRF_RADIO->TASKS_START = 1;
 }

static void _send_ack(void)
{
    aackbuf[0] = 5;                       /* Length including 2 byte fcs */
    aackbuf[1] = IEEE802154_FCF_TYPE_ACK; /* Ack type */
    aackbuf[2] = 0;                       /* Other bits zeroed */
    aackbuf[3] = _last_seq_no;            /* Sequence number */
    /* We should be in the Rx state, transitioning to disabled should be 0 us */
    if (NRF_RADIO->STATE != RADIO_STATE_STATE_TxIdle) {
        _disable();
    }
    NRF_RADIO->PACKETPTR = (uint32_t)aackbuf;
    NRF_RADIO->EVENTS_TXREADY = 0;
    NRF_RADIO->TASKS_TXEN = 1;
}

static bool _ack_frame_filter(void)
{
    size_t psdu_len = rxbuf[0];
    return ((NRF_RADIO->CRCSTATUS == 1) &&
            (psdu_len == (IEEE802154_FCF_LEN + 1)) &&
            ((rxbuf[1] & IEEE802154_FCF_TYPE_MASK) == IEEE802154_FCF_TYPE_ACK) &&
            txbuf[3] == ieee802154_get_seq(&rxbuf[1]));
}

static void _set_chan(uint16_t chan)
{
    assert((chan >= IEEE802154_CHANNEL_MIN) && (chan <= IEEE802154_CHANNEL_MAX));
    /* Channel map between 2400 MHZ ... 2500 MHz
     * -> Frequency = 2400 + FREQUENCY (MHz) */
    NRF_RADIO->FREQUENCY = (chan - 10) * 5;
    nrf802154_dev.chan = chan;
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
    if (txpower > 8) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Pos8dBm;
    }
    if (txpower > 1) {
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
    timer_stop(NRF802154_TIMER);
    switch (_state) {
        case NRF802154_STATE_AACK:
            /* Transmit ack */
            _send_ack();
            break;
        case NRF802154_STATE_ACKWAIT:
            /* Timeout waiting for ACK, TACK is more than SIFS and LIFS, no
             * need to wait addionally */
            if (_retransmissions >= _retrans_max) {
                _isr_tx_complete();
            }
            else {
                _enable_tx();
            }
            _retransmissions++;
            break;
        default:
            mutex_unlock(&_txlock);
            break;
    }
}

static int _init(netdev_t *dev)
{
    (void)dev;

    int result = timer_init(NRF802154_TIMER, TIMER_FREQ, _timer_cb, NULL);
    assert(result >= 0);
    timer_stop(NRF802154_TIMER);
    (void)result;
    timer_stop(NRF802154_TIMER);

    /* initialize local variables */
    mutex_init(&_txlock);

    /* reset buffer */
    rxbuf[0] = 0;
    txbuf[0] = 0;
    _event_flags = 0;
    _setting_flags = 0;
    _retrans_max = NRF802154_DEFAULT_RETRANS;

    static const netopt_enable_t enable = NETOPT_ENABLE;
    /* Use the setter here to ensure setting propagates to netdev_ieee802154 */
    nrf802154_dev.netdev.driver->set(&nrf802154_dev.netdev, NETOPT_AUTOACK,
            &enable, sizeof(enable));
    nrf802154_dev.netdev.driver->set(&nrf802154_dev.netdev, NETOPT_ACK_REQ,
            &enable, sizeof(enable));

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
    luid_get(nrf802154_dev.long_addr, IEEE802154_LONG_ADDRESS_LEN);
    memcpy(nrf802154_dev.short_addr, &nrf802154_dev.long_addr[6],
           IEEE802154_SHORT_ADDRESS_LEN);

    /* set default channel */
    _set_chan(nrf802154_dev.chan);

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

    _retransmissions = 0;

    /* copy packet data into the transmit buffer */
    unsigned int len = 0;
    for (; iolist; iolist = iolist->iol_next) {
        if ((IEEE802154_FCS_LEN + len + iolist->iol_len) > (IEEE802154_FRAME_LEN_MAX)) {
            DEBUG("[nrf802154] send: unable to do so, packet is too large!\n");
            mutex_unlock(&_txlock);
            return -EOVERFLOW;
        }
        memcpy(&txbuf[len + 1], iolist->iol_base, iolist->iol_len);
        len += iolist->iol_len;
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
    if (!(_event_flags & RX_COMPLETE)) {
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
            /* Calculate RSSI by substracting the offset from the datasheet.
             * Intentionally using a different calculation than the one from
             * figure 122 of the v1.1 product specification. This appears to
             * match real world performance better */
            radio_info->rssi = (int16_t)hwlqi - ED_RSSIOFFS;
        }
    }

    _reset_rx();

    return (int)pktlen;
}

static void _isr(netdev_t *dev)
{
    if (!nrf802154_dev.netdev.event_callback) {
        return;
    }
    if (_event_flags & RX_COMPLETE) {
        nrf802154_dev.netdev.event_callback(dev, NETDEV_EVENT_RX_COMPLETE);
    }
    if (_event_flags & TX_COMPLETE) {
        if (_retrans_used > _retrans_max) {
            nrf802154_dev.netdev.event_callback(dev, NETDEV_EVENT_TX_NOACK);
        }
        else {
            nrf802154_dev.netdev.event_callback(dev, NETDEV_EVENT_TX_COMPLETE);
        }
        _event_flags &= ~TX_COMPLETE;
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
            *((uint16_t *)value) = nrf802154_dev.chan;
            return sizeof(uint16_t);
        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            *((int16_t *)value) = _get_txpower();
            return sizeof(int16_t);
        case NETOPT_AUTOACK:
            assert(max_len >= sizeof(netopt_enable_t));
            *(netopt_enable_t*)value = _setting_isset(NRF802154_OPT_AUTOACK);
            return sizeof(netopt_enable_t);
        case NETOPT_RETRANS:
            assert(max_len >= sizeof(uint8_t));
            *(uint8_t*)value = _retrans_max;
            return sizeof(uint8_t);
        case NETOPT_ACK_REQ:
            assert(max_len >= sizeof(netopt_enable_t));
            *(netopt_enable_t*)value = _setting_isset(NRF802154_OPT_ACK_REQUEST);
            return sizeof(netopt_enable_t);
        case NETOPT_TX_RETRIES_NEEDED:
            assert(max_len >= sizeof(uint8_t));
            *(uint8_t*)value = _retrans_used > _retrans_max ? _retrans_max
                                                            : _retrans_used;
            return sizeof(uint8_t);

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

    switch (opt) {
        case NETOPT_CHANNEL:
            assert(value_len == sizeof(uint16_t));
            _set_chan(*((uint16_t *)value));
            return sizeof(uint16_t);
        case NETOPT_TX_POWER:
            assert(value_len == sizeof(int16_t));
            _set_txpower(*((int16_t *)value));
            return sizeof(int16_t);
        case NETOPT_AUTOACK:
            assert(value_len == sizeof(netopt_enable_t));
            *(netopt_enable_t*)value ? _setting_enable(NRF802154_OPT_AUTOACK)
                                     : _setting_disable(NRF802154_OPT_AUTOACK);
            return sizeof(netopt_enable_t);
        case NETOPT_RETRANS:
            assert(value_len == sizeof(uint8_t));
            if (*(uint8_t*)value > NRF802154_MAX_RETRANS) {
                return -EINVAL;
            }
            else {
                _retrans_max = *(uint8_t*)value;
            }
            return sizeof(uint8_t);
        case NETOPT_ACK_REQ:
            assert(value_len == sizeof(netopt_enable_t));
            *(netopt_enable_t*)value ? _setting_enable(NRF802154_OPT_ACK_REQUEST)
                                     : _setting_disable(NRF802154_OPT_ACK_REQUEST);
            /* Intentionally falls through */
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
                if (_state == NRF802154_STATE_RX) {
                    if ((nrf802154_dev.netdev.event_callback) &&
                        (NRF_RADIO->CRCSTATUS == 1) &&
                        (netdev_ieee802154_dst_filter(&nrf802154_dev,
                                                      &rxbuf[1]) == 0)) {
                        _event_flags |= RX_COMPLETE;
                        if (_ack_xmit_required()) {
                            _last_seq_no = ieee802154_get_seq(&rxbuf[1]);
                            _state = NRF802154_STATE_AACK;
                            _set_and_start_timer(SIFS);
                        }
                        nrf802154_dev.netdev.event_callback(&nrf802154_dev.netdev, NETDEV_EVENT_ISR);
                    }
                    else {
                        _reset_rx();
                    }
                }
                else if (_state == NRF802154_STATE_ACKWAIT) {
                    /* Check if this is the expected ACK frame */
                    if (_ack_frame_filter()) {
                        timer_stop(NRF802154_TIMER);
                        _state = NRF802154_STATE_TX;
                        _isr_tx_complete();
                    }
                    _reset_rx();
                }
                break;
            case RADIO_STATE_STATE_Tx:
            case RADIO_STATE_STATE_TxIdle:
            case RADIO_STATE_STATE_TxDisable:
                if (_state == NRF802154_STATE_TX) {
                    if (_ack_expected()) {
                        _state = NRF802154_STATE_ACKWAIT;
                        _set_and_start_timer(TACK);
                        _enable_listen();
                    }
                    else {
                        _isr_tx_complete();
                    }
                }
                else if (_state == NRF802154_STATE_AACK) {
                    _enable_rx();
                }
                break;
            default:
                DEBUG("[nrf802154] Unhandled state: %x\n", (uint8_t)NRF_RADIO->STATE);
        }
    }
    else {
        DEBUG("[nrf802154] Unknown interrupt triggered\n");
    }

    cortexm_isr_end();
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
