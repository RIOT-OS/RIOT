/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_nrf52
 * @{
 *
 * @file
 * @brief       Implementation of the radio driver for nRF52 radios
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>
#include <errno.h>

#include "cpu.h"
#include "luid.h"
#include "mutex.h"

#include "net/ieee802154.h"
#include "net/netdev/ieee802154.h"
#include "nrf802154.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"


/**
 * @brief   Possible internal device states
 */
typedef enum {
    STATE_OFF,                  /**< device is powered off */
    STATE_IDLE,                 /**< device is in idle mode */
    STATE_RX,                   /**< device is in receive mode */
} idle_state_t;


static const netdev_driver_t nrf802154_netdev_driver;

netdev_ieee802154_t nrf802154_dev = {
    {
        .driver = &nrf802154_netdev_driver,
        .event_callback = NULL,
        .context = NULL,
    #ifdef MODULE_NETSTATS_L2
        .stats = { 0 }
    #endif
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

static uint8_t rxbuf[IEEE802154_FRAME_LEN_MAX + 2]; /* length PHR + PSDU + LQI */
static uint8_t txbuf[IEEE802154_FRAME_LEN_MAX + 2]; /* length PHR + PSDU + LQI */

static mutex_t txlock;

static volatile idle_state_t idle_state = STATE_RX;

/**
 * @brief   Set radio into idle (DISABLED) state
 */
static void go_idle(void)
{
    /* set device into basic disabled state */
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->TASKS_DISABLE = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0) {}
}

static void reset_state(void)
{
    go_idle();

    if ((idle_state == STATE_RX) && (rxbuf[0] == 0)) {
        NRF_RADIO->PACKETPTR = (uint32_t)rxbuf;
        NRF_RADIO->TASKS_RXEN = 1;
    }
    else if (idle_state == STATE_OFF) {
        NRF_RADIO->POWER = 0;
    }
}

static void set_chan(uint16_t chan)
{
    assert((chan >= IEEE802154_CHANNEL_MIN) && (chan <= IEEE802154_CHANNEL_MAX));
    NRF_RADIO->FREQUENCY = (chan - 10) * 5;
    nrf802154_dev.chan = chan;
}

static int16_t get_txpower(void)
{
    int8_t p = (int8_t)NRF_RADIO->TXPOWER;
    if (p < 0) {
        return (int16_t)(0xff00 | p);
    }
    return (int16_t)p;
}

static void set_txpower(int16_t power)
{
    if (power > 9) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Pos9dBm;
    }
    else if (power > -2 && power < 2) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_0dBm;
    }
    else if (power < 10 && power > 1) {
        NRF_RADIO->TXPOWER = (uint8_t)power;
    }
    else if (power > -6) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg4dBm;
    }
    else if (power > -10) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg8dBm;
    }
    else if (power > -14) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg12dBm;
    }
    else if (power > -18) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg16dBm;
    }
    else if (power > -30) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg20dBm;
    }
    else {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg40dBm;
    }
}

static int init(netdev_t *dev)
{
    assert(dev);

    /* initialize local variables */
    mutex_init(&txlock);

    /* reset buffers */
    rxbuf[0] = 0;

    /* power on peripheral */
    NRF_RADIO->POWER = 1;
    /* make sure the radio is disabled/stopped */
    go_idle();

    /* we configure it to run in IEEE802.15.4 mode */
    NRF_RADIO->MODE = RADIO_MODE_MODE_Ieee802154_250Kbit;
    /* and set some fitting configuration */
    NRF_RADIO->PCNF0 = ((8 << RADIO_PCNF0_LFLEN_Pos) |
                        (RADIO_PCNF0_PLEN_32bitZero << RADIO_PCNF0_PLEN_Pos) |
                        (RADIO_PCNF0_CRCINC_Include << RADIO_PCNF0_CRCINC_Pos));
    NRF_RADIO->PCNF1 = (IEEE802154_FRAME_LEN_MAX);
    /* set start frame delimiter */
    NRF_RADIO->SFD = IEEE802154_SFD;
    /* set MHR filters */
    NRF_RADIO->MHRMATCHCONF = 0;
    NRF_RADIO->MHRMATCHMAS = 0xff0007ff;
    /* configure CRC conform to IEEE802154 */
    NRF_RADIO->CRCCNF = 0x202;
    NRF_RADIO->CRCPOLY = 0x011021;
    NRF_RADIO->CRCINIT = 0;

    /* assign default addresses */
    luid_get(nrf802154_dev.short_addr, IEEE802154_SHORT_ADDRESS_LEN);
    luid_get(nrf802154_dev.long_addr, IEEE802154_LONG_ADDRESS_LEN);

    /* set default channel */
    set_chan(nrf802154_dev.chan);

    /* configure some shortcuts */
    NRF_RADIO->SHORTS = (RADIO_SHORTS_TXREADY_START_Msk |
                         RADIO_SHORTS_RXREADY_START_Msk);// |
                         // RADIO_SHORTS_END_DISABLE_Msk);

    /* enable interrupts */
    NVIC_EnableIRQ(RADIO_IRQn);
    NRF_RADIO->INTENSET = (RADIO_INTENSET_END_Msk);

    /* switch to RX mode */
    reset_state();

    return 0;
}

static int send(netdev_t *dev, const struct iovec *vector, unsigned count)
{
    (void)dev;

    DEBUG("Send a packet\n");

    assert(vector && count);

    /* we need to grab the TX mutex, so we don't end up with two concurrent
     * transfers */
    mutex_lock(&txlock);

    /* wait for any ongoing transmission to finish and go into idle state */
    while ((NRF_RADIO->STATE == RADIO_STATE_STATE_TxIdle) ||
          (NRF_RADIO->STATE == RADIO_STATE_STATE_RxIdle)) {}
    go_idle();

    /* copy packet data into the transmit buffer */
    int pos = 1;
    for (unsigned i = 0; i < count; i++) {
        if ((pos + vector[i].iov_len) > (IEEE802154_FRAME_LEN_MAX + 1)) {
            DEBUG("[nrf802154] send: unable to do so, packet is too large!\n");
            return -EOVERFLOW;
        }
        memcpy(&txbuf[pos], vector[i].iov_base, vector[i].iov_len);
        pos += vector[i].iov_len;
    }

    txbuf[0] = pos + 1;
    /* set packet pointer to TX buffer */
    NRF_RADIO->PACKETPTR = (uint32_t)(txbuf);

    /* trigger the actual transmission */
    DEBUG("[nrf802154] send: putting %i byte into the ether\n", pos);
    NRF_RADIO->TASKS_TXEN = 1;
    // while (NRF_RADIO->STATE == RADIO_STATE_STATE_TxIdle) {}
    NRF_RADIO->TASKS_START = 1;

    /* @todo: how about CCA, retransmits, and ACKs? */

    return pos;
}

static int recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    (void)dev;
    (void)info;

    assert(idle_state != STATE_OFF);

    int pktlen = (int)rxbuf[0] - 2;

    /* check if packet data is readable */
    if (pktlen == 0) {
        DEBUG("recv: no packet data available\n");
        return 0;
    }

    if (buf == NULL) {
        if (len > 0) {
            /* drop packet */
            DEBUG("recv: dropping packet of length %i\n", pktlen);
            rxbuf[0] = 0;
            reset_state();
        }
        else if (len == 0) {
          /* return packet length */
          DEBUG("recv: return packet length: %i\n", pktlen);
          return pktlen;
        }
    }
    else {
        DEBUG("recv: reading packet of length %i\n", pktlen);
        pktlen = (len < pktlen) ? len : pktlen;
        memcpy(buf, &rxbuf[1], pktlen);
        rxbuf[0] = 0;
        reset_state();
    }

    return pktlen;
}

static void isr(netdev_t *dev)
{
    if (nrf802154_dev.netdev.event_callback) {
        nrf802154_dev.netdev.event_callback(dev, NETDEV_EVENT_RX_COMPLETE);
    }
}

static int get(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    assert(dev);

    DEBUG("get: %s\n", netopt2str(opt));

    switch (opt) {
        case NETOPT_CHANNEL:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t *)value) = nrf802154_dev.chan;
            return sizeof(uint16_t);
        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            *((int16_t *)value) = get_txpower();
            return sizeof(int16_t);

        default:
            return netdev_ieee802154_get((netdev_ieee802154_t *)dev,
                                          opt, value, max_len);
    }
}

static int set(netdev_t *dev, netopt_t opt, void *value, size_t value_len)
{
    assert(dev);

    DEBUG("set: %s\n", netopt2str(opt));

    switch (opt) {
        case NETOPT_CHANNEL:
            assert(value_len == sizeof(uint16_t));
            set_chan(*((uint16_t *)value));
            return sizeof(uint16_t);
        case NETOPT_TX_POWER:
            assert(value_len == sizeof(int16_t));
            set_txpower(*((int16_t *)value));
            return sizeof(int16_t);

        default:
            return netdev_ieee802154_set((netdev_ieee802154_t *)dev,
                                         opt, value, value_len);
    }

}

void isr_radio(void)
{
    DEBUG("radio: ISR\n");

    if (NRF_RADIO->EVENTS_END == 1) {
        NRF_RADIO->EVENTS_END = 0;
        /* did we just send or receive something? */
        if (NRF_RADIO->STATE == RADIO_STATE_STATE_RxIdle) {
            /* only process packet if event callback is set and CRC is valid */
            if ((nrf802154_dev.netdev.event_callback) &&
                (NRF_RADIO->CRCSTATUS == 1) &&
                netdev_ieee802154_dst_filter(&nrf802154_dev, &rxbuf[1])) {
                DEBUG("isr-rx: packet valid -> processing\n");
                /* TODO: send ACK */
                nrf802154_dev.netdev.event_callback(&nrf802154_dev.netdev, NETDEV_EVENT_ISR);
            }
            else {
                DEBUG("isr-rx: dropping incoming packet\n");
                /* reset RX buffer and listen for new packets */
                rxbuf[0] = 0;
                NRF_RADIO->TASKS_START = 1;
            }
        }
        else if (NRF_RADIO->STATE == RADIO_STATE_STATE_TxIdle) {
            mutex_unlock(&txlock);
            reset_state();
        }
    }

    cortexm_isr_end();
}

/**
 * @brief   Export of the netdev interface
 */
static const netdev_driver_t nrf802154_netdev_driver = {
    .send = send,
    .recv = recv,
    .init = init,
    .isr  = isr,
    .get  = get,
    .set  = set
};
