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

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "errno.h"
#include "kernel_defines.h"

#include "net/lora.h"
#include "net/gnrc/lorawan.h"
#include "net/gnrc/pktbuf.h"
#include "net/lorawan/hdr.h"
#include "net/loramac.h"
#include "net/gnrc/lorawan/region.h"
#include "timex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define GNRC_LORAWAN_DL_RX2_DR_MASK       (0x0F)    /**< DL Settings DR Offset mask */
#define GNRC_LORAWAN_DL_RX2_DR_POS        (0)       /**< DL Settings DR Offset pos */
#define GNRC_LORAWAN_DL_DR_OFFSET_MASK    (0x70)    /**< DL Settings RX2 DR mask */
#define GNRC_LORAWAN_DL_DR_OFFSET_POS     (4)       /**< DL Settings RX2 DR pos */

static inline void gnrc_lorawan_mlme_reset(gnrc_lorawan_t *mac)
{
    mac->mlme.activation = MLME_ACTIVATION_NONE;
    mac->mlme.pending_mlme_opts = 0;
    mac->rx_delay = (CONFIG_LORAMAC_DEFAULT_RX1_DELAY / MS_PER_SEC);
    mac->mlme.nid = CONFIG_LORAMAC_DEFAULT_NETID;
}

static inline void gnrc_lorawan_mlme_backoff_init(gnrc_lorawan_t *mac)
{
    mac->mlme.backoff_state = 0;

    gnrc_lorawan_mlme_backoff_expire_cb(mac);
}

static inline void gnrc_lorawan_mcps_reset(gnrc_lorawan_t *mac)
{
    mac->mcps.ack_requested = false;
    mac->mcps.waiting_for_ack = false;
    mac->mcps.fcnt = 0;
    mac->mcps.fcnt_down = 0;
}

void gnrc_lorawan_set_rx2_dr(gnrc_lorawan_t *mac, uint8_t rx2_dr)
{
    mac->dl_settings &= ~GNRC_LORAWAN_DL_RX2_DR_MASK;
    mac->dl_settings |= (rx2_dr << GNRC_LORAWAN_DL_RX2_DR_POS) &
                        GNRC_LORAWAN_DL_RX2_DR_MASK;
}

static void _sleep_radio(gnrc_lorawan_t *mac)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);
    netopt_state_t state = NETOPT_STATE_SLEEP;

    dev->driver->set(dev, NETOPT_STATE, &state, sizeof(state));
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
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);
    uint8_t cr = LORA_CR_4_5;

    dev->driver->set(dev, NETOPT_CODING_RATE, &cr, sizeof(cr));

    uint8_t syncword = LORAMAC_DEFAULT_PUBLIC_NETWORK ? LORA_SYNCWORD_PUBLIC
                                                      : LORA_SYNCWORD_PRIVATE;

    dev->driver->set(dev, NETOPT_SYNCWORD, &syncword, sizeof(syncword));

    /* Continuous reception */
    uint32_t rx_timeout = 0;

    dev->driver->set(dev, NETOPT_RX_TIMEOUT, &rx_timeout, sizeof(rx_timeout));

    gnrc_lorawan_set_rx2_dr(mac, CONFIG_LORAMAC_DEFAULT_RX2_DR);

    mac->toa = 0;
    gnrc_lorawan_mcps_reset(mac);
    gnrc_lorawan_mlme_reset(mac);
    gnrc_lorawan_channels_init(mac);
}

static void _config_radio(gnrc_lorawan_t *mac, uint32_t channel_freq,
                          uint8_t dr, int rx)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);

    if (channel_freq != 0) {
        dev->driver->set(dev, NETOPT_CHANNEL_FREQUENCY, &channel_freq,
                         sizeof(channel_freq));
    }

    netopt_enable_t iq_invert = rx;

    dev->driver->set(dev, NETOPT_IQ_INVERT, &iq_invert, sizeof(iq_invert));

    gnrc_lorawan_set_dr(mac, dr);

    if (rx) {
        /* Switch to single listen mode */
        const netopt_enable_t single = true;
        dev->driver->set(dev, NETOPT_SINGLE_RECEIVE, &single, sizeof(single));
        const uint8_t timeout = CONFIG_GNRC_LORAWAN_MIN_SYMBOLS_TIMEOUT;
        dev->driver->set(dev, NETOPT_RX_SYMBOL_TIMEOUT, &timeout,
                         sizeof(timeout));
    }
}

static void _configure_rx_window(gnrc_lorawan_t *mac, uint32_t channel_freq,
                                 uint8_t dr)
{
    _config_radio(mac, channel_freq, dr, true);
}

void gnrc_lorawan_open_rx_window(gnrc_lorawan_t *mac)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);

    /* Switch to RX state */
    if (mac->state == LORAWAN_STATE_RX_1) {
        gnrc_lorawan_set_timer(mac, US_PER_SEC);
    }
    netopt_state_t state = NETOPT_STATE_RX;

    dev->driver->set(dev, NETOPT_STATE, &state, sizeof(state));
}

void gnrc_lorawan_timeout_cb(gnrc_lorawan_t *mac)
{
    switch(mac->state) {
        case LORAWAN_STATE_RX_1:
        case LORAWAN_STATE_RX_2:
            gnrc_lorawan_open_rx_window(mac);
            break;
        case LORAWAN_STATE_JOIN:
            gnrc_lorawan_trigger_join(mac);
            break;
        default:
            gnrc_lorawan_event_ack_timeout(mac);
            break;
    }
}

void gnrc_lorawan_radio_tx_done_cb(gnrc_lorawan_t *mac)
{
    mac->state = LORAWAN_STATE_RX_1;

    int rx_1;

    /* if the MAC is not activated, then this is a Join Request */
    rx_1 = mac->mlme.activation == MLME_ACTIVATION_NONE ?
           CONFIG_LORAMAC_DEFAULT_JOIN_DELAY1 : mac->rx_delay;

    gnrc_lorawan_set_timer(mac, rx_1 * US_PER_SEC);

    uint8_t dr_offset = (mac->dl_settings & GNRC_LORAWAN_DL_DR_OFFSET_MASK) >>
                        GNRC_LORAWAN_DL_DR_OFFSET_POS;

    _configure_rx_window(mac, 0,
                         gnrc_lorawan_rx1_get_dr_offset(mac->last_dr,
                                                        dr_offset));

    _sleep_radio(mac);
}

void gnrc_lorawan_radio_rx_timeout_cb(gnrc_lorawan_t *mac)
{
    (void)mac;
    switch (mac->state) {
        case LORAWAN_STATE_RX_1:
            DEBUG("gnrc_lorawan: RX1 timeout.\n");
            _configure_rx_window(mac, CONFIG_LORAMAC_DEFAULT_RX2_FREQ,
                                 mac->dl_settings &
                                 GNRC_LORAWAN_DL_RX2_DR_MASK);
            mac->state = LORAWAN_STATE_RX_2;
            break;
        case LORAWAN_STATE_RX_2:
            DEBUG("gnrc_lorawan: RX2 timeout.\n");
            gnrc_lorawan_event_no_rx(mac);
            mac->state = LORAWAN_STATE_IDLE;
            break;
        default:
            assert(false);
            break;
    }
    _sleep_radio(mac);
}

void gnrc_lorawan_send_pkt(gnrc_lorawan_t *mac, iolist_t *psdu, uint8_t dr)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);

    mac->state = LORAWAN_STATE_TX;

    uint32_t chan = gnrc_lorawan_pick_channel(mac);

    DEBUG("gnrc_lorawan: Channel: %" PRIu32 "Hz \n", chan);

    _config_radio(mac, chan, dr, false);

    uint8_t cr;

    dev->driver->get(dev, NETOPT_CODING_RATE, &cr, sizeof(cr));

    mac->toa = lora_time_on_air(iolist_size(psdu), dr, cr);

    if (dev->driver->send(dev, psdu) == -ENOTSUP) {
        DEBUG("gnrc_lorawan: Cannot send: radio is still transmitting");
    }

}

void gnrc_lorawan_radio_rx_done_cb(gnrc_lorawan_t *mac, uint8_t *psdu,
                                   size_t size)
{
    assert(psdu);
    _sleep_radio(mac);
    mac->state = LORAWAN_STATE_IDLE;
    gnrc_lorawan_remove_timer(mac);

    uint8_t mtype = (*psdu & MTYPE_MASK) >> 5;

    switch (mtype) {
        case MTYPE_JOIN_ACCEPT:
            gnrc_lorawan_mlme_process_join(mac, psdu, size);
            break;
        case MTYPE_CNF_DOWNLINK:
        case MTYPE_UNCNF_DOWNLINK:
            gnrc_lorawan_mcps_process_downlink(mac, psdu, size);
            break;
        default:
            break;
    }
}
