/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "container.h"
#include "mutex.h"
#include "ztimer.h"

#include "mac_bh.h"
#include "mac_fsm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _process_event(ieee802154_mac_t *mac, uint8_t ev);

static void _process_event(ieee802154_mac_t *mac, uint8_t ev)
{
    switch ((ieee802154_mac_ev_t)ev) {
    case IEEE802154_MAC_EV_RADIO_TX_DONE:
        DEBUG("IEEE802154 MAC: processing event IEEE802154_MAC_EV_RADIO_TX_DONE\n");
        ieee802154_submac_tx_done_cb(&mac->submac);
        if (mac->scan_active && mac->scan_timer_pending && (mac->scan_req != NULL)) {
            uint32_t duration_us = mac->scan_req->duration * mac->sym_us;
            mac->scan_timer_pending = false;
            ztimer_set(ZTIMER_USEC, &mac->scan_timer, duration_us);
        }
        break;
    case IEEE802154_MAC_EV_RADIO_RX_DONE:
        DEBUG("IEEE802154 MAC: processing event IEEE802154_MAC_EV_RADIO_RX_DONE\n");
        ieee802154_submac_rx_done_cb(&mac->submac);
        break;

    case IEEE802154_MAC_EV_RADIO_CRC_ERR:
        DEBUG("IEEE802154 MAC: processing event IEEE802154_MAC_EV_RADIO_CRC_ERR\n");
        ieee802154_submac_crc_error_cb(&mac->submac);
        break;

    default:
        break;
    }
}

void ieee802154_mac_handle_radio(ieee802154_dev_t *dev, ieee802154_trx_ev_t st)
{
    ieee802154_submac_t *submac = container_of(dev, ieee802154_submac_t, dev);
    ieee802154_mac_t *mac = container_of(submac, ieee802154_mac_t, submac);

    ieee802154_mac_ev_t ev;

    switch (st) {
    case IEEE802154_RADIO_CONFIRM_TX_DONE:
        ev = IEEE802154_MAC_EV_RADIO_TX_DONE;
        break;
    case IEEE802154_RADIO_INDICATION_RX_DONE:
        ev = IEEE802154_MAC_EV_RADIO_RX_DONE;
        break;
    case IEEE802154_RADIO_INDICATION_CRC_ERROR:
        ev = IEEE802154_MAC_EV_RADIO_CRC_ERR;
        break;
    default:
        return;
    }

    mutex_lock(&mac->submac_lock);
    _process_event(mac, ev);
    mutex_unlock(&mac->submac_lock);
}

void ieee802154_mac_scan_timer_process(ieee802154_mac_t *mac)
{
    if (!mac) {
        return;
    }

    (void)ieee802154_mac_fsm_request(mac, IEEE802154_MAC_FSM_EV_SCAN_TIMER, NULL);
}
