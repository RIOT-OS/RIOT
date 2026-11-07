/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "container.h"
#include "mutex.h"
#include "ztimer.h"

#include "mac_pib.h"
#include "mac_fsm.h"
#include "mac_tx.h"
#include "mac_queue.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _tx_finish(ieee802154_mac_t *mac, ieee802154_mac_indirect_q_t *indirect_q, int slot,
                       int status, ieee802154_tx_info_t *info);

static void _tx_finish(ieee802154_mac_t *mac,
                       ieee802154_mac_indirect_q_t *indirect_q,
                       int slot,
                       int status,
                       ieee802154_tx_info_t *info)
{
    if (ieee802154_mac_tx_empty(&indirect_q->q[slot])) {
        mac->indirect_q.busy = false;
        return;
    }

    ieee802154_mac_tx_desc_t *d = ieee802154_mac_tx_peek(&indirect_q->q[slot]);
    if (d == NULL) {
        mac->indirect_q.busy = false;
        return;
    }

    bool request_rx = false;
    bool frame_pending = info && info->frame_pending;

    if (!mac->is_coordinator && frame_pending && !mac->scan_active) {
        mac->poll_rx_active = true;
        mac->poll_rx_deadline = ieee802154_indirect_get_deadline(mac);
        request_rx = true;
    }

    if (mac->scan_active || mac->assoc_pending) {
        request_rx = true;
    }

    if (d->indirect &&
        ((status == TX_STATUS_MEDIUM_BUSY) || (status == TX_STATUS_NO_ACK))) {
        d->tx_state = IEEE802154_TX_STATE_QUEUED;
        mac->indirect_q.busy = false;

        if (mac->is_coordinator) {
            ieee802154_pib_value_t rx_on;
            ieee802154_mac_mlme_get(mac, IEEE802154_PIB_RX_ON_WHEN_IDLE, &rx_on);
            if (rx_on.v.b) {
                request_rx = true;
            }
        }

        if (request_rx && mac->cbs.rx_request) {
            mac->cbs.rx_request(mac);
        }

        return;
    }

    uint8_t confirm_handle = d->handle;
    uint8_t confirm_type = d->type;
    int confirm_status = status;

    d->tx_state = IEEE802154_TX_STATE_DONE;

    d->in_use = false;
    ieee802154_mac_tx_pop(&indirect_q->q[slot]);
    ieee802154_mac_handle_indirectq_auto_free(mac, indirect_q, slot);
    mac->indirect_q.busy = false;

    ieee802154_pib_value_t rx_on;
    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_RX_ON_WHEN_IDLE, &rx_on);
    if (rx_on.v.b) {
        request_rx = true;
    }

    if (request_rx && mac->cbs.rx_request) {
        mac->cbs.rx_request(mac);
    }

    /* Call upper layer last, after queue state and RX state are repaired. */
    if ((confirm_type == IEEE802154_FCF_TYPE_DATA) && mac->cbs.data_confirm) {
        mac->cbs.data_confirm(mac->cbs.mac, confirm_handle, confirm_status);
    }
}

typedef struct {
    uint8_t handle;
    int status;
} mac_expired_confirm_t;

void ieee802154_mac_tick(ieee802154_mac_t *mac)
{
    mac_expired_confirm_t expired[IEEE802154_MAC_TX_INDIRECTQ_SIZE];
    unsigned expired_count = 0;
    bool stop_poll_rx = false;

    mutex_lock(&mac->indirect_q.lock);

    mac->indirect_q.tick++;
    uint32_t now_tick = mac->indirect_q.tick;

    if (mac->poll_rx_active
        && ieee802154_mac_frame_is_expired(mac->indirect_q.tick,
                                        mac->poll_rx_deadline)
        && !mac->is_coordinator
        && !mac->scan_active)
    {
        mac->poll_rx_active = false;
        stop_poll_rx = true;
    }

    for (unsigned i = 0; i < IEEE802154_MAC_TX_INDIRECTQ_SIZE; i++) {
        ieee802154_mac_txq_t *txq = &mac->indirect_q.q[i];

        if (ieee802154_mac_tx_empty(txq) || (txq->deadline_tick == NULL)) {
            continue;
        }

        if (!ieee802154_mac_frame_is_expired(mac->indirect_q.tick,
                                             *txq->deadline_tick)) {
            continue;
        }

        /* Do not expire the frame currently being transmitted. */
        if (mac->indirect_q.busy &&
            mac->indirect_q.current_slot == i) {
            continue;
        }

        ieee802154_mac_tx_desc_t *d = ieee802154_mac_tx_peek(txq);
        if (d == NULL) {
            continue;
        }

        if ((d->type == IEEE802154_FCF_TYPE_DATA)
            && expired_count < IEEE802154_MAC_TX_INDIRECTQ_SIZE)
        {
            expired[expired_count].handle = d->handle;
            expired[expired_count].status = -ETIMEDOUT;
            expired_count++;
        }

        d->in_use = false;
        ieee802154_mac_tx_pop(txq);
        ieee802154_mac_handle_indirectq_auto_free(mac, &mac->indirect_q, i);
    }

    ztimer_set(ZTIMER_MSEC, &mac->tick,
               (uint32_t)IEEE802154_MAC_TICK_INTERVAL_MS);

    mutex_unlock(&mac->indirect_q.lock);

    mutex_lock(&mac->submac_lock);
    if (mac->assoc_pending &&
        ieee802154_mac_frame_is_expired(now_tick, mac->assoc_deadline_tick)) {
        (void)ieee802154_mac_fsm_process_ev_ctx(mac, IEEE802154_MAC_FSM_EV_ASSOC_TIMEOUT,
                                               NULL);
    }
    mutex_unlock(&mac->submac_lock);

    if (stop_poll_rx) {
        mutex_lock(&mac->submac_lock);
        (void)ieee802154_set_idle(&mac->submac);
        mutex_unlock(&mac->submac_lock);
    }

    for (unsigned i = 0; i < expired_count; i++) {
        if (mac->cbs.data_confirm) {
            mac->cbs.data_confirm(mac->cbs.mac,
                                  expired[i].handle,
                                  expired[i].status);
        }
    }
}

void ieee802154_mac_tx_finish_current(ieee802154_mac_t *mac, int status, ieee802154_tx_info_t *info)
{
    _tx_finish(mac, &mac->indirect_q, mac->indirect_q.current_slot, status, info);
}

void ieee802154_mac_ack_timeout_fired(ieee802154_mac_t *mac)
{
    mutex_lock(&mac->submac_lock);
    ieee802154_submac_ack_timeout_fired(&mac->submac);
    mutex_unlock(&mac->submac_lock);
}

void ieee802154_submac_ack_timer_set(ieee802154_submac_t *submac)
{
    ieee802154_mac_t *mac = container_of(submac, ieee802154_mac_t, submac);

    ztimer_set(ZTIMER_USEC, &mac->ack_timer, (uint32_t)submac->ack_timeout_us);
}

void ieee802154_submac_ack_timer_cancel(ieee802154_submac_t *submac)
{
    ieee802154_mac_t *mac = container_of(submac, ieee802154_mac_t, submac);

    ztimer_remove(ZTIMER_USEC, &mac->ack_timer);
}
