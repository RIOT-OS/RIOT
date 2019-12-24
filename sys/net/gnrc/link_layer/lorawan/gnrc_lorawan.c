/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 * @}
 */
#include <stdio.h>
#include <string.h>
#include "net/lora.h"
#include "net/gnrc/lorawan.h"
#include "errno.h"
#include "net/gnrc/pktbuf.h"

#include "net/lorawan/hdr.h"
#include "net/loramac.h"
#include "net/gnrc/lorawan/region.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* This factor is used for converting "real" seconds into microcontroller
 * microseconds. This is done in order to correct timer drift.
 */
#define _DRIFT_FACTOR (int) (US_PER_SEC * 100 / (100 + (CONFIG_GNRC_LORAWAN_TIMER_DRIFT / 10.0)))

#define GNRC_LORAWAN_DL_RX2_DR_MASK       (0x0F)  /**< DL Settings DR Offset mask */
#define GNRC_LORAWAN_DL_RX2_DR_POS        (0)     /**< DL Settings DR Offset pos */
#define GNRC_LORAWAN_DL_DR_OFFSET_MASK    (0x70)  /**< DL Settings RX2 DR mask */
#define GNRC_LORAWAN_DL_DR_OFFSET_POS     (4)     /**< DL Settings RX2 DR pos */

static inline void gnrc_lorawan_mlme_reset(gnrc_lorawan_t *mac)
{
    mac->mlme.activation = MLME_ACTIVATION_NONE;
    mac->mlme.pending_mlme_opts = 0;
    mac->rx_delay = (LORAMAC_DEFAULT_RX1_DELAY/MS_PER_SEC);
    mac->mlme.nid = LORAMAC_DEFAULT_NETID;
}

static inline void gnrc_lorawan_mlme_backoff_init(gnrc_lorawan_t *mac)
{
    mac->mlme.backoff_msg.type = MSG_TYPE_MLME_BACKOFF_EXPIRE;
    mac->mlme.backoff_state = 0;

    gnrc_lorawan_mlme_backoff_expire(mac);
}

static inline void gnrc_lorawan_mcps_reset(gnrc_lorawan_t *mac)
{
    mac->mcps.ack_requested = false;
    mac->mcps.waiting_for_ack = false;
    mac->mcps.fcnt = 0;
    mac->mcps.fcnt_down = 0;
}

static inline void _set_rx2_dr(gnrc_lorawan_t *mac, uint8_t rx2_dr)
{
    mac->dl_settings &= ~GNRC_LORAWAN_DL_RX2_DR_MASK;
    mac->dl_settings |= (rx2_dr << GNRC_LORAWAN_DL_RX2_DR_POS) &
        GNRC_LORAWAN_DL_RX2_DR_MASK;
}

static void _sleep_radio(gnrc_lorawan_t *mac)
{
    netopt_state_t state = NETOPT_STATE_SLEEP;

    netdev_set_pass((netdev_t *) mac, NETOPT_STATE, &state, sizeof(state));
}

void gnrc_lorawan_init(gnrc_lorawan_t *mac, uint8_t *nwkskey, uint8_t *appskey)
{
    mac->nwkskey = nwkskey;
    mac->appskey = appskey;
    mac->busy = false;
    gnrc_lorawan_mlme_backoff_init(mac);
    gnrc_lorawan_reset(mac);
}

void gnrc_lorawan_reset(gnrc_lorawan_t *mac)
{
    uint8_t cr = LORA_CR_4_5;

    netdev_set_pass(&mac->netdev, NETOPT_CODING_RATE, &cr, sizeof(cr));

    uint8_t syncword = LORAMAC_DEFAULT_PUBLIC_NETWORK ? LORA_SYNCWORD_PUBLIC
                                                      : LORA_SYNCWORD_PRIVATE;
    netdev_set_pass(&mac->netdev, NETOPT_SYNCWORD, &syncword, sizeof(syncword));

    /* Continuous reception */
    uint32_t rx_timeout = 0;
    netdev_set_pass(&mac->netdev, NETOPT_RX_TIMEOUT, &rx_timeout, sizeof(rx_timeout));

    _set_rx2_dr(mac, LORAMAC_DEFAULT_RX2_DR);

    mac->toa = 0;
    gnrc_lorawan_mcps_reset(mac);
    gnrc_lorawan_mlme_reset(mac);
    gnrc_lorawan_channels_init(mac);
}

static void _config_radio(gnrc_lorawan_t *mac, uint32_t channel_freq, uint8_t dr, int rx)
{
    if (channel_freq != 0) {
        netdev_set_pass(&mac->netdev, NETOPT_CHANNEL_FREQUENCY, &channel_freq, sizeof(channel_freq));
    }

    netopt_enable_t iq_invert = rx;
    netdev_set_pass(&mac->netdev, NETOPT_IQ_INVERT, &iq_invert, sizeof(iq_invert));

    gnrc_lorawan_set_dr(mac, dr);

    if (rx) {
        /* Switch to single listen mode */
        const netopt_enable_t single = true;
        netdev_set_pass(&mac->netdev, NETOPT_SINGLE_RECEIVE, &single, sizeof(single));
        const uint16_t timeout = CONFIG_GNRC_LORAWAN_MIN_SYMBOLS_TIMEOUT;
        netdev_set_pass(&mac->netdev, NETOPT_RX_SYMBOL_TIMEOUT, &timeout, sizeof(timeout));
    }
}

static void _configure_rx_window(gnrc_lorawan_t *mac, uint32_t channel_freq, uint8_t dr)
{
    _config_radio(mac, channel_freq, dr, true);
}

void gnrc_lorawan_open_rx_window(gnrc_lorawan_t *mac)
{
    mac->msg.type = MSG_TYPE_TIMEOUT;
    /* Switch to RX state */
    if (mac->state == LORAWAN_STATE_RX_1) {
        xtimer_set_msg(&mac->rx, _DRIFT_FACTOR, &mac->msg, thread_getpid());
    }
    uint8_t state = NETOPT_STATE_RX;
    netdev_set_pass(&mac->netdev, NETOPT_STATE, &state, sizeof(state));
}

void gnrc_lorawan_event_tx_complete(gnrc_lorawan_t *mac)
{
    mac->msg.type = MSG_TYPE_TIMEOUT;
    mac->state = LORAWAN_STATE_RX_1;

    int rx_1;
    /* if the MAC is not activated, then this is a Join Request */
    rx_1 = mac->mlme.activation == MLME_ACTIVATION_NONE ?
           LORAMAC_DEFAULT_JOIN_DELAY1 : mac->rx_delay;

    xtimer_set_msg(&mac->rx, rx_1 * _DRIFT_FACTOR, &mac->msg, thread_getpid());

    uint8_t dr_offset = (mac->dl_settings & GNRC_LORAWAN_DL_DR_OFFSET_MASK) >>
        GNRC_LORAWAN_DL_DR_OFFSET_POS;
    _configure_rx_window(mac, 0, gnrc_lorawan_rx1_get_dr_offset(mac->last_dr, dr_offset));

    _sleep_radio(mac);
}

void gnrc_lorawan_event_timeout(gnrc_lorawan_t *mac)
{
    (void) mac;
    switch (mac->state) {
        case LORAWAN_STATE_RX_1:
            _configure_rx_window(mac, LORAMAC_DEFAULT_RX2_FREQ, mac->dl_settings & GNRC_LORAWAN_DL_RX2_DR_MASK);
            mac->state = LORAWAN_STATE_RX_2;
            break;
        case LORAWAN_STATE_RX_2:
            gnrc_lorawan_mlme_no_rx(mac);
            gnrc_lorawan_mcps_event(mac, MCPS_EVENT_NO_RX, 0);
            mac->state = LORAWAN_STATE_IDLE;
            gnrc_lorawan_mac_release(mac);
            break;
        default:
            assert(false);
            break;
    }
    _sleep_radio(mac);
}

/* This function uses a precomputed table to calculate time on air without
 * using floating point arithmetics */
static uint32_t lora_time_on_air(size_t payload_size, uint8_t dr, uint8_t cr)
{
    assert(dr <= LORAMAC_DR_6);
    uint8_t _K[6][4] = {    { 0, 1, 5, 5 },
                            { 0, 1, 4, 5 },
                            { 1, 5, 5, 5 },
                            { 1, 4, 5, 4 },
                            { 1, 3, 4, 4 },
                            { 1, 2, 4, 3 } };

    uint32_t t_sym = 1 << (15 - dr);
    uint32_t t_preamble = (t_sym << 3) + (t_sym << 2) + (t_sym >> 2);

    int index = (dr < LORAMAC_DR_6) ? dr : LORAMAC_DR_5;
    uint8_t n0 = _K[index][0];
    int nb_symbols;

    uint8_t offset = _K[index][1];
    if (payload_size < offset) {
        nb_symbols = 8 + n0 * cr;
    }
    else {
        uint8_t c1 = _K[index][2];
        uint8_t c2 = _K[index][3];
        uint8_t pos = (payload_size - offset) % (c1 + c2);
        uint8_t cycle = (payload_size - offset) / (c1 + c2);
        nb_symbols = 8 + (n0 + 2 * cycle + 1 + (pos > (c1 - 1))) * cr;
    }

    uint32_t t_payload = t_sym * nb_symbols;
    return t_preamble + t_payload;
}

void gnrc_lorawan_send_pkt(gnrc_lorawan_t *mac, gnrc_pktsnip_t *pkt, uint8_t dr)
{
    mac->state = LORAWAN_STATE_TX;

    iolist_t iolist = {
        .iol_base = pkt->data,
        .iol_len = pkt->size,
        .iol_next = (iolist_t *) pkt->next
    };

    uint32_t chan = gnrc_lorawan_pick_channel(mac);
    _config_radio(mac, chan, dr, false);

    mac->last_dr = dr;

    uint8_t cr;
    netdev_get_pass(&mac->netdev, NETOPT_CODING_RATE, &cr, sizeof(cr));

    mac->toa = lora_time_on_air(gnrc_pkt_len(pkt), dr, cr + 4);

    if (netdev_send_pass(&mac->netdev, &iolist) == -ENOTSUP) {
        DEBUG("gnrc_lorawan: Cannot send: radio is still transmitting");
    }

}

void gnrc_lorawan_process_pkt(gnrc_lorawan_t *mac, gnrc_pktsnip_t *pkt)
{
    mac->state = LORAWAN_STATE_IDLE;
    xtimer_remove(&mac->rx);

    uint8_t *p = pkt->data;

    uint8_t mtype = (*p & MTYPE_MASK) >> 5;
    switch (mtype) {
        case MTYPE_JOIN_ACCEPT:
            gnrc_lorawan_mlme_process_join(mac, pkt);
            break;
        case MTYPE_CNF_DOWNLINK:
        case MTYPE_UNCNF_DOWNLINK:
            gnrc_lorawan_mcps_process_downlink(mac, pkt);
            break;
        default:
            gnrc_pktbuf_release(pkt);
            break;
    }

    gnrc_lorawan_mac_release(mac);
}

int gnrc_lorawan_netdev_get(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
    int res = 0;
    gnrc_lorawan_t *mac = (gnrc_lorawan_t *) dev;
    uint32_t tmp;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= sizeof(mac->dev_addr));
            tmp = byteorder_swapl(mac->dev_addr.u32);
            memcpy(value, &tmp, sizeof(mac->dev_addr));
            res = sizeof(mac->dev_addr);
            break;
        default:
            res = netdev_get_pass(dev, opt, value, max_len);
            break;
    }
    return res;
}

int gnrc_lorawan_netdev_set(netdev_t *dev, netopt_t opt, const void *value, size_t len)
{
    gnrc_lorawan_t *mac = (gnrc_lorawan_t *) dev;
    uint32_t tmp;

    if (mac->busy) {
        return -EBUSY;
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(len == sizeof(uint32_t));
            tmp = byteorder_swapl(*((uint32_t *) value));
            memcpy(&mac->dev_addr, &tmp, sizeof(uint32_t));
            break;
        case NETOPT_LORAWAN_RX2_DR:
            assert(len == sizeof(uint8_t));
            _set_rx2_dr(mac, *((uint8_t *) value));
            break;
        default:
            netdev_set_pass(dev, opt, value, len);
            break;
    }
    return 0;
}

const netdev_driver_t gnrc_lorawan_driver = {
    .init = netdev_init_pass,
    .send = netdev_send_pass,
    .recv = netdev_recv_pass,
    .get = gnrc_lorawan_netdev_get,
    .set = gnrc_lorawan_netdev_set,
    .isr = netdev_isr_pass,
};

void gnrc_lorawan_setup(gnrc_lorawan_t *mac, netdev_t *lower)
{
    mac->netdev.driver = &gnrc_lorawan_driver;
    mac->netdev.lower = lower;
    lower->context = mac;
}

void gnrc_lorawan_recv(gnrc_lorawan_t *mac)
{
    int bytes_expected = netdev_recv_pass((netdev_t *) mac, NULL, 0, 0);
    int nread;
    struct netdev_radio_rx_info rx_info;
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, NULL, bytes_expected, GNRC_NETTYPE_UNDEF);
    if (pkt == NULL) {
        DEBUG("_recv_ieee802154: cannot allocate pktsnip.\n");
        /* Discard packet on netdev device */
        netdev_recv_pass((netdev_t *) mac, NULL, bytes_expected, NULL);
        return;
    }
    nread = netdev_recv_pass((netdev_t *) mac, pkt->data, bytes_expected, &rx_info);
    _sleep_radio(mac);
    if (nread <= 0) {
        gnrc_pktbuf_release(pkt);
        return;
    }

    gnrc_lorawan_process_pkt(mac, pkt);
}
