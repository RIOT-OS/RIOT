/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author Moritz Holzer <moritz.holzer@haw-hamburg.de>
 */

#include <string.h>

#include "container.h"
#include "ztimer.h"
#include "mutex.h"

#include "mac_internal.h"
#include "mac_pib.h"
#include "mac_tx.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _ack_timer_cb(void *arg);
static void _submac_rx_done(ieee802154_submac_t *submac);
static void _submac_tx_done(ieee802154_submac_t *submac, int status, ieee802154_tx_info_t *info);
static void _init_tx_q(ieee802154_mac_t *mac);

void ieee802154_init_mac_internal(ieee802154_mac_t *mac);

/* ACK timer callback */
static void _ack_timer_cb(void *arg)
{
    ieee802154_mac_t *mac = (ieee802154_mac_t *)arg;

    mac->cbs.ack_timeout(mac);
}

/* ===== SubMAC callbacks ===== */
static void _submac_rx_done(ieee802154_submac_t *submac)
{
    ieee802154_mac_t *mac = container_of(submac, ieee802154_mac_t, submac);

    int len = ieee802154_get_frame_length(&mac->submac);
    mac->cbs.allocate_request(mac, (len > 0) ? (size_t)len : 0);
}

static void _submac_tx_done(ieee802154_submac_t *submac, int status, ieee802154_tx_info_t *info)
{
    (void)info;
    ieee802154_mac_t *mac = container_of(submac, ieee802154_mac_t, submac);
    ieee802154_mac_tx_finish_current(mac, status, info);
}

static const ieee802154_submac_cb_t _submac_cbs = {
    .rx_done = _submac_rx_done,
    .tx_done = _submac_tx_done,
};

static void _init_tx_q(ieee802154_mac_t *mac)
{
    /* 1 means free with this the count of 1 is == IEEE802154_MAC_TX_INDIRECTQ_SIZE */
    mac->indirect_q.free_mask = (1U << IEEE802154_MAC_TX_INDIRECTQ_SIZE) - 1;
    mutex_init(&mac->indirect_q.lock);
    memset(&mac->indirect_q.q, 0, sizeof(mac->indirect_q.q));
    memset(&mac->assoc_table, 0, sizeof(mac->assoc_table));
}

void ieee802154_init_mac_internal(ieee802154_mac_t *mac)
{
    DEBUG("IEEE802154 MAC: init\n");
    memset(mac->cmd_buf, 0, IEEE802154_FRAME_LEN_MAX);
    mac->cmd.iol_base = mac->cmd_buf;
    mac->cmd.iol_len = IEEE802154_FRAME_LEN_MAX;
    mac->cmd.iol_next = NULL;
    mac->state = IEEE802154_MAC_STATE_IDLE;
    mac->state_history = IEEE802154_MAC_STATE_IDLE;
    /* radio HAL must be initialized externally before MAC init */
    ieee802154_pib_value_t short_addr_value;
    ieee802154_pib_value_t ext_addr_value;
    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_SHORT_ADDR, &short_addr_value);
    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_EXTENDED_ADDRESS, &ext_addr_value);
    uint16_t sym_us;
    ieee802154_phy_mode_t phy_mode = ieee802154_get_phy_mode(&mac->submac);
    switch (phy_mode) {
    case IEEE802154_PHY_OQPSK:
        sym_us = IEEE802154_SYMBOL_TIME_US;
        break;
    case IEEE802154_PHY_MR_FSK:
        sym_us = IEEE802154_MR_FSK_SYMBOL_TIME_US;
        break;
    case IEEE802154_PHY_MR_OFDM:
        sym_us = IEEE802154_MR_OFDM_SYMBOL_TIME_US;
        break;
    default:
        /* TODO: check for correct symbol times
                 MR-OQPSK / ASK / BPSK etc */
        sym_us = IEEE802154_SYMBOL_TIME_US; /* fallback rn */
        break;
    }
    mac->sym_us = sym_us;

    _init_tx_q(mac);
    mutex_init(&mac->submac_lock);
    mutex_lock(&mac->submac_lock);
    DEBUG("IEEE802154 MAC: init submac\n");
    ieee802154_submac_init(&mac->submac, &short_addr_value.v.short_addr,
                           &ext_addr_value.v.ext_addr);
    mac->submac.cb = &_submac_cbs;
    mac->submac.dev.cb = mac->cbs.radio_cb_request;
    mac->poll_rx_active = false;
    mac->poll_rx_deadline = 0;
    mac->scan_idx = 0;
    mac->scan_timer_pending = false;
    mac->assoc_pending = false;
    mac->assoc_deadline_tick = 0;
    mac->tick.callback = mac->cbs.tick_request;
    mac->tick.arg = mac;
    mac->ack_timer.callback = _ack_timer_cb;
    mac->ack_timer.arg = mac;
    mac->scan_timer.callback = mac->cbs.scan_timer_request;
    mac->scan_timer.arg = mac;
    ztimer_set(ZTIMER_MSEC, &mac->tick, (uint32_t)IEEE802154_MAC_TICK_INTERVAL_MS);
    mutex_unlock(&mac->submac_lock);
}

/** @} */
