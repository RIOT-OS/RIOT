/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <string.h>

#include "mutex.h"
#include "byteorder.h"

#include "mac_fsm.h"
#include "mac_pib.h"
#include "mac_queue.h"
#include "mac_tx.h"
#include "net/ieee802154/mac.h"
#include "net/ieee802154.h"
#include "net/eui_provider.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _mac_tx_request(ieee802154_mac_t *mac, ieee802154_addr_mode_t dst_mode,
                           const void *dst_addr);
static int _mac_enqueue_and_tx(ieee802154_mac_t *mac, const ieee802154_mac_fsm_ctx_t *ctx,
                               ieee802154_addr_mode_t src_mode, uint8_t frame_type,
                               iolist_t *msdu, const uint8_t *msdu_handle,
                               bool ack_req, bool indirect);
static int _mac_data_request(ieee802154_mac_t *mac, const ieee802154_mac_fsm_ctx_t *ctx);
static int _mac_assoc_request(ieee802154_mac_t *mac, const ieee802154_mac_fsm_ctx_t *ctx);
static int _mac_poll_request(ieee802154_mac_t *mac, const ieee802154_mac_fsm_ctx_t *ctx);
static int _mac_assoc_response(ieee802154_mac_t *mac, const ieee802154_mac_fsm_ctx_t *ctx);
static int _mac_enqueue_beacon(ieee802154_mac_t *mac);
static ieee802154_mac_state_t _mac_fsm_state_idle(ieee802154_mac_t *mac,
                                                  ieee802154_mac_fsm_ev_t ev,
                                                  const ieee802154_mac_fsm_ctx_t *ctx);
static ieee802154_mac_state_t _mac_fsm_state_scan_active(ieee802154_mac_t *mac,
                                                         ieee802154_mac_fsm_ev_t ev,
                                                         const ieee802154_mac_fsm_ctx_t *ctx);
static ieee802154_mac_state_t _mac_fsm_state_coordinator(ieee802154_mac_t *mac,
                                                         ieee802154_mac_fsm_ev_t ev,
                                                         const ieee802154_mac_fsm_ctx_t *ctx);
static ieee802154_mac_state_t _mac_fsm_state_device(ieee802154_mac_t *mac,
                                                    ieee802154_mac_fsm_ev_t ev,
                                                    const ieee802154_mac_fsm_ctx_t *ctx);
static ieee802154_mac_state_t _mac_fsm_state_associating(ieee802154_mac_t *mac,
                                                         ieee802154_mac_fsm_ev_t ev,
                                                         const ieee802154_mac_fsm_ctx_t *ctx);
static ieee802154_mac_state_t _mac_fsm_state_sleep(ieee802154_mac_t *mac,
                                                   ieee802154_mac_fsm_ev_t ev,
                                                   const ieee802154_mac_fsm_ctx_t *ctx);
static int _mac_fsm_process_ev(ieee802154_mac_t *mac, ieee802154_mac_fsm_ev_t ev,
                               const ieee802154_mac_fsm_ctx_t *ctx);

static const char *const _mac_fsm_ev_str[] = {
    [IEEE802154_MAC_FSM_EV_SCAN_START] = "SCAN_START",
    [IEEE802154_MAC_FSM_EV_SCAN_TIMER] = "SCAN_TIMER",
    [IEEE802154_MAC_FSM_EV_SCAN_DONE] = "SCAN_DONE",
    [IEEE802154_MAC_FSM_EV_ASSOC_REQ_RX] = "ASSOC_REQ_RX",
    [IEEE802154_MAC_FSM_EV_ASSOC_RES_RX] = "ASSOC_RES_RX",
    [IEEE802154_MAC_FSM_EV_DISASSOC_RX] = "DISASSOC_RX",
    [IEEE802154_MAC_FSM_EV_COORD_START] = "COORD_START",
    [IEEE802154_MAC_FSM_EV_MLME_ASSOC_REQ] = "MLME_ASSOC_REQ",
    [IEEE802154_MAC_FSM_EV_MLME_POLL] = "MLME_POLL",
    [IEEE802154_MAC_FSM_EV_MLME_ASSOC_RES] = "MLME_ASSOC_RES",
    [IEEE802154_MAC_FSM_EV_ASSOC_TIMEOUT] = "ASSOC_TIMEOUT",
    [IEEE802154_MAC_FSM_EV_TX_REQUEST] = "TX_REQUEST",
    [IEEE802154_MAC_FSM_EV_SLEEP] = "SLEEP",
    [IEEE802154_MAC_FSM_EV_WAKE] = "WAKE",
    [IEEE802154_MAC_FSM_EV_RX_BEACON] = "RX_BEACON",
    [IEEE802154_MAC_FSM_EV_RX_DATA] = "RX_DATA",
    [IEEE802154_MAC_FSM_EV_RX_CMD_DATA_REQ] = "RX_CMD_DATA_REQ",
    [IEEE802154_MAC_FSM_EV_RX_CMD_BEACON_REQ] = "RX_CMD_BEACON_REQ",
    [IEEE802154_MAC_FSM_EV_RX_CMD_ASSOC_REQ] = "RX_CMD_ASSOC_REQ",
    [IEEE802154_MAC_FSM_EV_RX_CMD_ASSOC_RES] = "RX_CMD_ASSOC_RES",
    [IEEE802154_MAC_FSM_EV_RX_CMD_DISASSOC] = "RX_CMD_DISASSOC",
    [IEEE802154_MAC_FSM_EV_MCPS_DATA_REQ] = "MCPS_DATA_REQ",
};

static const char *const _mac_fsm_state_str[] = {
    [IEEE802154_MAC_STATE_IDLE] = "IDLE",
    [IEEE802154_MAC_STATE_SCAN_ACTIVE] = "SCAN_ACTIVE",
    [IEEE802154_MAC_STATE_ASSOCIATING] = "ASSOCIATING",
    [IEEE802154_MAC_STATE_COORDINATOR] = "COORDINATOR",
    [IEEE802154_MAC_STATE_DEVICE] = "DEVICE",
    [IEEE802154_MAC_STATE_SLEEP] = "SLEEP",
    [IEEE802154_MAC_STATE_INVALID] = "INVALID",
};

static int _mac_enqueue_and_tx(ieee802154_mac_t *mac, const ieee802154_mac_fsm_ctx_t *ctx,
                               ieee802154_addr_mode_t src_mode, uint8_t frame_type,
                               iolist_t *msdu, const uint8_t *msdu_handle,
                               bool ack_req, bool indirect)
{
    if (!ctx || !ctx->result || !ctx->data_dst_addr) {
        return -EINVAL;
    }

    int res = ieee802154_mac_map_push(mac, frame_type,
                                      src_mode, ctx->dst_mode,
                                      &ctx->dst_panid, ctx->data_dst_addr,
                                      msdu, msdu_handle,
                                      ack_req, indirect);
    if (res < 0) {
        *ctx->result = res;
        return res;
    }
    if (!indirect) {
        res = _mac_tx_request(mac, ctx->dst_mode, ctx->data_dst_addr);
        *ctx->result = res;
        return res;
    }
    *ctx->result = 0;
    return 0;
}

static int _mac_data_request(ieee802154_mac_t *mac, const ieee802154_mac_fsm_ctx_t *ctx)
{
    int res = _mac_enqueue_and_tx(mac, ctx, ctx->src_mode, IEEE802154_FCF_TYPE_DATA,
                                  ctx->msdu, &ctx->msdu_handle, ctx->ack_req, ctx->indirect);

    /* Ensure coordinator stays in RX to receive polls after queuing */
    if ((res >= 0) && ctx->indirect && mac->is_coordinator) {
        ieee802154_pib_value_t rx_on;

        ieee802154_mac_mlme_get(mac, IEEE802154_PIB_RX_ON_WHEN_IDLE, &rx_on);

        if (rx_on.v.b && mac->cbs.rx_request) {
            mac->cbs.rx_request(mac);
        }
    }
    return res;
}

static int _mac_assoc_request(ieee802154_mac_t *mac, const ieee802154_mac_fsm_ctx_t *ctx)
{
    if (!ctx || !ctx->result) {
        return -EINVAL;
    }

    uint8_t *buf = (uint8_t *)mac->cmd.iol_base;
    buf[0] = IEEE802154_CMD_ASSOCIATION_REQ;
    buf[1] = ctx->capability.u8;
    mac->cmd.iol_len = 2;
    mac->cmd.iol_next = NULL;
    uint8_t handle = 0xFFU;

    ieee802154_pib_value_t short_addr;
    ieee802154_addr_mode_t src_mode = IEEE802154_ADDR_MODE_EXTENDED;
    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_SHORT_ADDR, &short_addr);
    if (short_addr.v.short_addr.u16 != 0xFFFFU) {
        src_mode = IEEE802154_ADDR_MODE_SHORT;
    }

    int res = _mac_enqueue_and_tx(mac, ctx, src_mode, IEEE802154_FCF_TYPE_MACCMD,
                                  &mac->cmd, &handle, true, false);
    if (res < 0) {
        return res;
    }

    ieee802154_pib_value_t wait;
    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_RESPONSE_WAIT_TIME, &wait);
    uint32_t duration_us = (uint32_t)wait.v.u8 * 60U * mac->sym_us;
    uint32_t duration_ms = (duration_us + 999U) / 1000U;
    uint16_t ticks = (uint16_t)((duration_ms + IEEE802154_MAC_TICK_INTERVAL_MS - 1U) /
                                IEEE802154_MAC_TICK_INTERVAL_MS);
    if (ticks == 0) {
        ticks = 1;
    }
    mac->assoc_pending = true;
    mac->assoc_deadline_tick = mac->indirect_q.tick + ticks;
    return res;
}

static int _mac_poll_request(ieee802154_mac_t *mac, const ieee802154_mac_fsm_ctx_t *ctx)
{
    uint8_t *buf = (uint8_t *)mac->cmd.iol_base;
    buf[0] = IEEE802154_CMD_DATA_REQ;
    mac->cmd.iol_len = 1;
    mac->cmd.iol_next = NULL;
    uint8_t handle = 0xFFU;
    ieee802154_addr_mode_t src_mode = ctx->dst_mode;
    if (ctx->src_mode != IEEE802154_ADDR_MODE_NONE) {
        src_mode = ctx->src_mode;
    }

    return _mac_enqueue_and_tx(mac, ctx, src_mode, IEEE802154_FCF_TYPE_MACCMD,
                               &mac->cmd, &handle, true, false);
}

static int _mac_assoc_response(ieee802154_mac_t *mac, const ieee802154_mac_fsm_ctx_t *ctx)
{
    if (!ctx || !ctx->result || !ctx->data_dst_addr) {
        return -EINVAL;
    }

    uint8_t *buf = (uint8_t *)mac->cmd.iol_base;
    buf[0] = IEEE802154_CMD_ASSOCIATION_RES;
    /* In the MAC payload it has to be little endian */
    buf[1] = (uint8_t)(ctx->assoc_short_addr);
    buf[2] = (uint8_t)(ctx->assoc_short_addr >> 8);
    buf[3] = ctx->assoc_status;
    mac->cmd.iol_len = 4;
    mac->cmd.iol_next = NULL;
    uint8_t handle = 0xFFU;

    ieee802154_pib_value_t short_addr;
    ieee802154_addr_mode_t src_mode = IEEE802154_ADDR_MODE_EXTENDED;
    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_SHORT_ADDR, &short_addr);
    if (short_addr.v.short_addr.u16 != 0xFFFFU) {
        src_mode = IEEE802154_ADDR_MODE_SHORT;
    }

    if ((ctx->assoc_status == IEEE802154_ASSOC_STATUS_SUCCESS) &&
        (ctx->dst_mode == IEEE802154_ADDR_MODE_EXTENDED)) {
        ieee802154_mac_assoc_update(mac,
                                    (const ieee802154_ext_addr_t *)ctx->data_dst_addr,
                                    byteorder_htons(ctx->assoc_short_addr));
    }

    /* Association response should be sent indirectly after a data request */
    return _mac_enqueue_and_tx(mac, ctx, src_mode, IEEE802154_FCF_TYPE_MACCMD,
                               &mac->cmd, &handle, true, true);
}

static ieee802154_mac_state_t _mac_fsm_state_idle(ieee802154_mac_t *mac,
                                                  ieee802154_mac_fsm_ev_t ev,
                                                  const ieee802154_mac_fsm_ctx_t *ctx)
{
    switch (ev) {
    case IEEE802154_MAC_FSM_EV_SCAN_START:
        if (mac->scan_req == NULL)
        {
            return IEEE802154_MAC_STATE_INVALID;
        }
        mac->state_history = mac->state;
        mac->scan_idx = 0;
        return IEEE802154_MAC_STATE_SCAN_ACTIVE;
    case IEEE802154_MAC_FSM_EV_COORD_START:
        return IEEE802154_MAC_STATE_COORDINATOR;
    case IEEE802154_MAC_FSM_EV_ASSOC_RES_RX:
    case IEEE802154_MAC_FSM_EV_MLME_ASSOC_REQ:
        if (ctx) {
            (void)_mac_assoc_request(mac, ctx);
        }
        return IEEE802154_MAC_STATE_ASSOCIATING;
    case IEEE802154_MAC_FSM_EV_MLME_POLL:
        if (ctx) {
            int res = _mac_poll_request(mac, ctx);
            if (ctx->result) {
                *ctx->result = res;
            }
        }
        return IEEE802154_MAC_STATE_IDLE;
    case IEEE802154_MAC_FSM_EV_TX_REQUEST:
        if (ctx && ctx->dst_addr && ctx->result) {
            *ctx->result = _mac_tx_request(mac, ctx->dst_mode, ctx->dst_addr);
        }
        return IEEE802154_MAC_STATE_IDLE;
    case IEEE802154_MAC_FSM_EV_SLEEP:
        return IEEE802154_MAC_STATE_SLEEP;
    case IEEE802154_MAC_FSM_EV_RX_DATA:
        if (ctx && mac->cbs.data_indication) {
            mac->cbs.data_indication(mac->cbs.mac, ctx->buf, ctx->info);
        }
        return IEEE802154_MAC_STATE_IDLE;
    case IEEE802154_MAC_FSM_EV_RX_CMD_DATA_REQ:
        if (ctx) {
            const void *src_addr = (ctx->src_mode == IEEE802154_ADDR_MODE_SHORT)
                                    ? (const void *)ctx->src
                                    : (const void *)&ctx->src_addr;
            ztimer_sleep(ZTIMER_USEC,
                         (uint32_t)IEEE802154_SIFS_SYMS * (uint32_t)mac->sym_us);
            (void)_mac_tx_request(mac, ctx->src_mode, src_addr);
        }
        return IEEE802154_MAC_STATE_IDLE;
    case IEEE802154_MAC_FSM_EV_RX_CMD_BEACON_REQ:
        return IEEE802154_MAC_STATE_IDLE;
    case IEEE802154_MAC_FSM_EV_MCPS_DATA_REQ:
        if (ctx) {
            (void)_mac_data_request(mac, ctx);
        }
        return IEEE802154_MAC_STATE_IDLE;
    default:
        return IEEE802154_MAC_STATE_INVALID;
    }
}

static ieee802154_mac_state_t _mac_fsm_state_scan_active(ieee802154_mac_t *mac,
                                                         ieee802154_mac_fsm_ev_t ev,
                                                         const ieee802154_mac_fsm_ctx_t *ctx)
{
    uint8_t idx;
    int res;
    network_uint16_t dst_addr = byteorder_htons(0xFFFFU);

    switch (ev) {
    case IEEE802154_MAC_FSM_EV_SCAN_DONE:
    {
        ieee802154_mlme_scan_req_t *req = mac->scan_req;
        ieee802154_radio_set_frame_filter_mode(&mac->submac.dev, IEEE802154_FILTER_ACCEPT);
        mac->scan_active = false;
        mac->scan_timer_pending = false;
        mac->scan_req = NULL;
        mac->scan_idx = 0;
        if (req != NULL) {
            mac->cbs.mlme_scan_confirm(mac, 0, req);
        }
        return mac->state_history;
    }
    case IEEE802154_MAC_FSM_EV_SCAN_TIMER:
        idx = mac->scan_idx;
        if (idx >= mac->scan_req->channel_count) {
            return ieee802154_mac_fsm_process_ev_ctx(mac, IEEE802154_MAC_FSM_EV_SCAN_DONE, NULL);
        }
        res = ieee802154_set_channel_number(&mac->submac, mac->scan_req->channels[idx]);
        res |= ieee802154_mac_enqueue_beacon_request(mac);
        if (res < 0)
        {
            DEBUG("IEEE802154 MAC: failed to scan channel %u\n", mac->scan_req->channels[idx]);
        }
        res = _mac_tx_request(mac, IEEE802154_ADDR_MODE_SHORT, &dst_addr);
        if (res > 0)
        {
            DEBUG("IEEE802154 MAC: failed to send beacon request none enqueud\n");
        } else if ( res < 0)
        {
            DEBUG("IEEE802154 MAC: failed to send beacon request %u\n", res);
        }
        mac->scan_idx++;
        mac->scan_timer_pending = true;
        return IEEE802154_MAC_STATE_SCAN_ACTIVE;
    case IEEE802154_MAC_FSM_EV_RX_BEACON:
        if (ctx && mac->scan_active && (mac->scan_req != NULL) &&
            (mac->scan_req->results != NULL) &&
            (mac->scan_req->results_used != NULL) &&
            (mac->scan_req->results_len > 0)) {
            size_t idx_res = *mac->scan_req->results_used;
            if (idx_res < mac->scan_req->results_len) {
                ieee802154_scan_result_t *scan_res = &mac->scan_req->results[idx_res];
                scan_res->channel = mac->submac.channel_num;
                scan_res->pan_id = ctx->src_pan.u16;
                scan_res->lqi = ctx->info->lqi;
                scan_res->rssi = ctx->info->rssi;
                scan_res->beacon_payload_len = 0;
                if (ctx->buf && ctx->buf->iol_base) {
                    int mhr_len = ieee802154_get_frame_hdr_len(ctx->buf->iol_base);
                    if ((mhr_len >= 0) && ((size_t)mhr_len <= ctx->buf->iol_len)) {
                        size_t payload_len = ctx->buf->iol_len - (size_t)mhr_len;
                        size_t copy_len = payload_len;
                        if (copy_len > IEEE802154_SCAN_BEACON_PAYLOAD_MAX) {
                            copy_len = IEEE802154_SCAN_BEACON_PAYLOAD_MAX;
                        }
                        if (copy_len > 0) {
                            const uint8_t *payload = ((const uint8_t *)ctx->buf->iol_base) +
                                                     (size_t)mhr_len;
                            memcpy(scan_res->beacon_payload, payload, copy_len);
                            scan_res->beacon_payload_len = (uint8_t)copy_len;
                        }
                    }
                }
                if (ctx->src_len == IEEE802154_SHORT_ADDRESS_LEN) {
                    scan_res->coord_addr.type = IEEE802154_ADDR_MODE_SHORT;
                    memcpy(&scan_res->coord_addr.v.short_addr, ctx->src,
                           IEEE802154_SHORT_ADDRESS_LEN);
                }
                else if (ctx->src_len == IEEE802154_LONG_ADDRESS_LEN) {
                    scan_res->coord_addr.type = IEEE802154_ADDR_MODE_EXTENDED;
                    memcpy(&scan_res->coord_addr.v.ext_addr, ctx->src,
                           IEEE802154_LONG_ADDRESS_LEN);
                }
                else {
                    scan_res->coord_addr.type = IEEE802154_ADDR_MODE_NONE;
                }
                (*mac->scan_req->results_used)++;
            }
        }
        return IEEE802154_MAC_STATE_SCAN_ACTIVE;
    case IEEE802154_MAC_FSM_EV_RX_DATA:
        if (ctx && mac->cbs.data_indication) {
            mac->cbs.data_indication(mac->cbs.mac, ctx->buf, ctx->info);
        }
        return IEEE802154_MAC_STATE_SCAN_ACTIVE;
    case IEEE802154_MAC_FSM_EV_RX_CMD_DATA_REQ:
        if (ctx) {
            const void *src_addr = (ctx->src_mode == IEEE802154_ADDR_MODE_SHORT)
                                    ? (const void *)ctx->src
                                    : (const void *)&ctx->src_addr;
            ztimer_sleep(ZTIMER_USEC,
                         (uint32_t)IEEE802154_SIFS_SYMS * (uint32_t)mac->sym_us);
            (void)_mac_tx_request(mac, ctx->src_mode, src_addr);
        }
        return IEEE802154_MAC_STATE_SCAN_ACTIVE;
    default:
        return IEEE802154_MAC_STATE_INVALID;
    }
}

static ieee802154_mac_state_t _mac_fsm_state_coordinator(ieee802154_mac_t *mac,
                                                         ieee802154_mac_fsm_ev_t ev,
                                                         const ieee802154_mac_fsm_ctx_t *ctx)
{
    switch (ev) {
    case IEEE802154_MAC_FSM_EV_RX_CMD_ASSOC_REQ:
        if (ctx && mac->cbs.mlme_associate_indication)
        {
            mac->cbs.mlme_associate_indication(mac->cbs.mac, ctx->src,
                                               (uint8_t)ctx->src_len,
                                               ctx->src_mode, ctx->capability);
        }
        if (mac->cbs.rx_request) {
            mac->cbs.rx_request(mac);
        }
        return IEEE802154_MAC_STATE_COORDINATOR;
    case IEEE802154_MAC_FSM_EV_DISASSOC_RX:
    case IEEE802154_MAC_FSM_EV_RX_CMD_DISASSOC:
        return IEEE802154_MAC_STATE_COORDINATOR;
    case IEEE802154_MAC_FSM_EV_RX_DATA:
        if (ctx && mac->cbs.data_indication) {
            mac->cbs.data_indication(mac->cbs.mac, ctx->buf, ctx->info);
        }
        return IEEE802154_MAC_STATE_COORDINATOR;
    case IEEE802154_MAC_FSM_EV_RX_CMD_DATA_REQ:
        if (ctx) {
            const void *src_addr = (ctx->src_mode == IEEE802154_ADDR_MODE_SHORT)
                                    ? (const void *)ctx->src
                                    : (const void *)&ctx->src_addr;
            ztimer_sleep(ZTIMER_USEC,
                         (uint32_t)IEEE802154_SIFS_SYMS * (uint32_t)mac->sym_us);
            int res = _mac_tx_request(mac, ctx->src_mode, src_addr);
            if (res < 0) {
                DEBUG("IEEE802154 MAC: failed to send data in response to data request status=%d\n", res);
            }
            if (mac->cbs.rx_request) {
                mac->cbs.rx_request(mac);
            }
        }
        return IEEE802154_MAC_STATE_COORDINATOR;
    case IEEE802154_MAC_FSM_EV_RX_CMD_BEACON_REQ:
        if (ctx) {
            _mac_enqueue_beacon(mac);
            (void)_mac_tx_request(mac, IEEE802154_ADDR_MODE_NONE, NULL);
        }
        return IEEE802154_MAC_STATE_COORDINATOR;
    case IEEE802154_MAC_FSM_EV_TX_REQUEST:
        if (ctx && ctx->dst_addr && ctx->result) {
            *ctx->result = _mac_tx_request(mac, ctx->dst_mode, ctx->dst_addr);
        }
        return IEEE802154_MAC_STATE_COORDINATOR;
    case IEEE802154_MAC_FSM_EV_MCPS_DATA_REQ:
        if (ctx) {
            (void)_mac_data_request(mac, ctx);
        }
        return IEEE802154_MAC_STATE_COORDINATOR;
    case IEEE802154_MAC_FSM_EV_MLME_ASSOC_RES:
        if (ctx) {
            (void)_mac_assoc_response(mac, ctx);
        }
        return IEEE802154_MAC_STATE_COORDINATOR;
    case IEEE802154_MAC_FSM_EV_SLEEP:
        return IEEE802154_MAC_STATE_SLEEP;
    default:
        return IEEE802154_MAC_STATE_INVALID;
    }
}

static ieee802154_mac_state_t _mac_fsm_state_device(ieee802154_mac_t *mac,
                                                    ieee802154_mac_fsm_ev_t ev,
                                                    const ieee802154_mac_fsm_ctx_t *ctx)
{
    switch (ev) {
    case IEEE802154_MAC_FSM_EV_DISASSOC_RX:
    case IEEE802154_MAC_FSM_EV_RX_CMD_DISASSOC:
        mac->assoc_pending = false;
        return IEEE802154_MAC_STATE_IDLE;
    case IEEE802154_MAC_FSM_EV_RX_DATA:
        if (ctx && mac->cbs.data_indication) {
            mac->cbs.data_indication(mac->cbs.mac, ctx->buf, ctx->info);
        }
        return IEEE802154_MAC_STATE_DEVICE;
    case IEEE802154_MAC_FSM_EV_TX_REQUEST:
        if (ctx && ctx->dst_addr && ctx->result) {
            *ctx->result = _mac_tx_request(mac, ctx->dst_mode, ctx->dst_addr);
        }
        return IEEE802154_MAC_STATE_DEVICE;
    case IEEE802154_MAC_FSM_EV_MLME_POLL:
        if (ctx) {
            int res = _mac_poll_request(mac, ctx);
            if (ctx->result) {
                *ctx->result = res;
            }
        }
        return IEEE802154_MAC_STATE_DEVICE;
    case IEEE802154_MAC_FSM_EV_MCPS_DATA_REQ:
        if (ctx) {
            (void)_mac_data_request(mac, ctx);
        }
        return IEEE802154_MAC_STATE_DEVICE;
    case IEEE802154_MAC_FSM_EV_SLEEP:
        return IEEE802154_MAC_STATE_SLEEP;
    default:
        return IEEE802154_MAC_STATE_INVALID;
    }
}

static ieee802154_mac_state_t _mac_fsm_state_associating(ieee802154_mac_t *mac,
                                                         ieee802154_mac_fsm_ev_t ev,
                                                         const ieee802154_mac_fsm_ctx_t *ctx)
{
    switch (ev) {
    case IEEE802154_MAC_FSM_EV_MLME_POLL:
        if (ctx) {
            int res = _mac_poll_request(mac, ctx);
            if (ctx->result) {
                *ctx->result = res;
            }
        }
        return IEEE802154_MAC_STATE_ASSOCIATING;
    case IEEE802154_MAC_FSM_EV_RX_CMD_ASSOC_RES:
        mac->assoc_pending = false;
        if (ctx && ctx->assoc_status == 0) {
            if (ctx->src_mode == IEEE802154_ADDR_MODE_SHORT) {
                network_uint16_t coord_short = { .u8 = { ctx->src[0], ctx->src[1] } };
                ieee802154_pib_value_t v = {
                    .type = IEEE802154_PIB_TYPE_NUI16,
                    .v.short_addr = coord_short,
                };
                ieee802154_mac_mlme_set_request(mac, IEEE802154_PIB_COORD_SHORT_ADDRESS, &v);
            }
            else if (ctx->src_mode == IEEE802154_ADDR_MODE_EXTENDED) {
                ieee802154_ext_addr_t coord_ext;
                memcpy(coord_ext.uint8, ctx->src, IEEE802154_LONG_ADDRESS_LEN);
                ieee802154_pib_value_t v = {
                    .type = IEEE802154_PIB_TYPE_EUI64,
                    .v.ext_addr = coord_ext,
                };
                ieee802154_mac_mlme_set_request(mac, IEEE802154_PIB_COORD_EXTENDED_ADDRESS, &v);
            }
            ieee802154_pib_value_t short_addr_value = {
                .type = IEEE802154_PIB_TYPE_NUI16,
                .v.short_addr = byteorder_htons(ctx->assoc_short_addr),
            };
            ieee802154_mac_mlme_set(mac, IEEE802154_PIB_SHORT_ADDR, &short_addr_value);
            if (ieee802154_set_short_addr(&mac->submac, &short_addr_value.v.short_addr) != 0)
            {
                DEBUG("IEEE802154 MAC: assoc, error setting short address\n");
            }
            (void)ieee802154_set_idle(&mac->submac);
            if (mac->cbs.mlme_associate_confirm) {
                mac->cbs.mlme_associate_confirm(mac->cbs.mac, 0,
                                                ctx->assoc_short_addr);
            }
            return IEEE802154_MAC_STATE_DEVICE;
        }
        if (ctx && mac->cbs.mlme_associate_confirm) {
            mac->cbs.mlme_associate_confirm(mac->cbs.mac, ctx->assoc_status,
                                            ctx->assoc_short_addr);
        }
        (void)ieee802154_set_idle(&mac->submac);
        return IEEE802154_MAC_STATE_IDLE;
    case IEEE802154_MAC_FSM_EV_RX_CMD_DISASSOC:
        mac->assoc_pending = false;
        return IEEE802154_MAC_STATE_IDLE;
    case IEEE802154_MAC_FSM_EV_ASSOC_TIMEOUT:
        mac->assoc_pending = false;
        if (mac->cbs.mlme_associate_confirm) {
            mac->cbs.mlme_associate_confirm(mac->cbs.mac, -ETIMEDOUT, 0xFFFF);
        }
        (void)ieee802154_set_idle(&mac->submac);
        return IEEE802154_MAC_STATE_IDLE;
    default:
        return IEEE802154_MAC_STATE_ASSOCIATING;
    }
}

static ieee802154_mac_state_t _mac_fsm_state_sleep(ieee802154_mac_t *mac,
                                                   ieee802154_mac_fsm_ev_t ev,
                                                   const ieee802154_mac_fsm_ctx_t *ctx)
{
    (void)mac;
    (void) ctx;
    switch (ev) {
    case IEEE802154_MAC_FSM_EV_WAKE:
        if (mac->state_history != IEEE802154_MAC_STATE_INVALID) {
            return mac->state_history;
        }
        return IEEE802154_MAC_STATE_IDLE;
    default:
        return IEEE802154_MAC_STATE_SLEEP;
    }
}

static int _mac_fsm_process_ev(ieee802154_mac_t *mac, ieee802154_mac_fsm_ev_t ev,
                               const ieee802154_mac_fsm_ctx_t *ctx)
{
    ieee802154_mac_state_t new_state;

    if (ev == IEEE802154_MAC_FSM_EV_RX_CMD_ASSOC_RES) {
        mac->is_coordinator = false;
    }
    else if (ev == IEEE802154_MAC_FSM_EV_SLEEP) {
        mac->state_history = mac->state;
    }

    switch (mac->state) {
    case IEEE802154_MAC_STATE_IDLE:
        new_state = _mac_fsm_state_idle(mac, ev, ctx);
        break;
    case IEEE802154_MAC_STATE_SCAN_ACTIVE:
        new_state = _mac_fsm_state_scan_active(mac, ev, ctx);
        break;
    case IEEE802154_MAC_STATE_ASSOCIATING:
        new_state = _mac_fsm_state_associating(mac, ev, ctx);
        break;
    case IEEE802154_MAC_STATE_COORDINATOR:
        new_state = _mac_fsm_state_coordinator(mac, ev, ctx);
        break;
    case IEEE802154_MAC_STATE_DEVICE:
        new_state = _mac_fsm_state_device(mac, ev, ctx);
        break;
    case IEEE802154_MAC_STATE_SLEEP:
        new_state = _mac_fsm_state_sleep(mac, ev, ctx);
        break;
    default:
        new_state = IEEE802154_MAC_STATE_INVALID;
        break;
    }

    if (new_state == IEEE802154_MAC_STATE_INVALID) {
        const char *ev_str = (ev < (sizeof(_mac_fsm_ev_str) / sizeof(_mac_fsm_ev_str[0])))
                             ? _mac_fsm_ev_str[ev]
                             : "UNKNOWN";
        const char *st_str = (mac->state < (sizeof(_mac_fsm_state_str) /
                             sizeof(_mac_fsm_state_str[0])))
                             ? _mac_fsm_state_str[mac->state]
                             : "UNKNOWN";
        if (ev_str == NULL) {
            ev_str = "UNKNOWN";
        }
        if (st_str == NULL) {
            st_str = "UNKNOWN";
        }
        DEBUG("IEEE802154 MAC: invalid FSM event %s in state %s\n", ev_str, st_str);
        return -EINVAL;
    }

    const char *ev_str = (ev < (sizeof(_mac_fsm_ev_str) / sizeof(_mac_fsm_ev_str[0])))
                            ? _mac_fsm_ev_str[ev]
                            : "UNKNOWN";
    const char *old_str = (mac->state < (sizeof(_mac_fsm_state_str) /
                            sizeof(_mac_fsm_state_str[0])))
                            ? _mac_fsm_state_str[mac->state]
                            : "UNKNOWN";
    const char *new_str = (new_state < (sizeof(_mac_fsm_state_str) /
                            sizeof(_mac_fsm_state_str[0])))
                            ? _mac_fsm_state_str[new_state]
                            : "UNKNOWN";
    if (ev_str == NULL) {
        ev_str = "UNKNOWN";
    }
    if (old_str == NULL) {
        old_str = "UNKNOWN";
    }
    if (new_str == NULL) {
        new_str = "UNKNOWN";
    }
    DEBUG("IEEE802154 MAC: FSM %s -> %s on %s\n", old_str, new_str, ev_str);
    mac->state = new_state;

    if ((ev == IEEE802154_MAC_FSM_EV_SCAN_START) &&
        (mac->state == IEEE802154_MAC_STATE_SCAN_ACTIVE)) {
        int res = ieee802154_radio_set_frame_filter_mode(&mac->submac.dev, IEEE802154_FILTER_PROMISC);
        if (res < 0)
        {
            return res;
        }
        mac->scan_active = true;
        return _mac_fsm_process_ev(mac, IEEE802154_MAC_FSM_EV_SCAN_TIMER, NULL);
    }

    if (new_state == IEEE802154_MAC_STATE_COORDINATOR &&
        ev  == IEEE802154_MAC_FSM_EV_COORD_START)
    {
        bool coord = true;
        ieee802154_pib_value_t pib_value;
        ieee802154_mac_mlme_get(mac, IEEE802154_PIB_EXTENDED_ADDRESS, &pib_value);
        network_uint16_t short_addr;
        eui_short_from_eui64(&pib_value.v.ext_addr, &short_addr);
        pib_value.type = IEEE802154_PIB_TYPE_NUI16;
        pib_value.v.short_addr = short_addr;
        ieee802154_mac_mlme_set_request(mac, IEEE802154_PIB_SHORT_ADDR, &pib_value);
        int res = ieee802154_radio_config_addr_filter(&mac->submac.dev, IEEE802154_AF_PAN_COORD,
                                                      (void *)&coord);
        if (res == -ENOTSUP) {
            DEBUG("IEEE802154 MAC: PAN_COORD not supported\n");
            return res;
        }
        mac->is_coordinator = true;
        ieee802154_pib_value_t rx_on;
        ieee802154_mac_mlme_get(mac, IEEE802154_PIB_RX_ON_WHEN_IDLE, &rx_on);

        if (rx_on.v.b && mac->cbs.rx_request) {
            mac->cbs.rx_request(mac);
        }
    }

    return 0;
}

int ieee802154_mac_fsm_process_ev_ctx(ieee802154_mac_t *mac, ieee802154_mac_fsm_ev_t ev,
                                      const ieee802154_mac_fsm_ctx_t *ctx)
{
    return _mac_fsm_process_ev(mac, ev, ctx);
}

int ieee802154_mac_fsm_request(ieee802154_mac_t *mac, ieee802154_mac_fsm_ev_t ev,
                               const ieee802154_mac_fsm_ctx_t *ctx)
{
    if (!mac) {
        return -EINVAL;
    }

    mutex_lock(&mac->submac_lock);
    int res = _mac_fsm_process_ev(mac, ev, ctx);
    mutex_unlock(&mac->submac_lock);
    return res;
}

int ieee802154_mac_tx(ieee802154_mac_t *mac, const ieee802154_ext_addr_t *dst_addr)
{
    int res = -EINVAL;
    ieee802154_mac_fsm_ctx_t ctx;

    memset(&ctx, 0, sizeof(ctx));
    ctx.dst_addr = dst_addr;
    ctx.dst_mode = IEEE802154_ADDR_MODE_EXTENDED;
    ctx.result = &res;

    if (ieee802154_mac_fsm_request(mac, IEEE802154_MAC_FSM_EV_TX_REQUEST, &ctx) < 0) {
        return -EBUSY;
    }

    return res;
}

static int _mac_enqueue_beacon(ieee802154_mac_t *mac)
{
    ieee802154_pib_value_t value;
    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_BEACON_PAYLOAD, &value);
    mac->cmd.iol_base = (void *)value.v.bytes.ptr;
    mac->cmd.iol_len = value.v.bytes.len;
    mac->cmd.iol_next = NULL;
    uint8_t handle = 0xFFU;
    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_EXTENDED_ADDRESS, &value);

    ieee802154_mac_map_push(mac, IEEE802154_FCF_TYPE_BEACON, IEEE802154_ADDR_MODE_EXTENDED,
        IEEE802154_ADDR_MODE_NONE, 0, NULL, &mac->cmd, &handle, false, false);
    return 0;
}

static int _mac_tx_request(ieee802154_mac_t *mac, ieee802154_addr_mode_t dst_mode,
                           const void *dst_addr)
{
    mutex_lock(&mac->indirect_q.lock);
    int slot = ieee802154_mac_indirectq_search_slot_locked(mac, dst_mode, dst_addr);

    if (slot < 0) {
        mutex_unlock(&mac->indirect_q.lock);
        return 1;
    }
    ieee802154_mac_txq_t *txq = &mac->indirect_q.q[slot];
    if (ieee802154_mac_tx_empty(txq) || mac->indirect_q.busy) {
        ieee802154_mac_handle_indirectq_auto_free(mac, &mac->indirect_q, slot);
        mutex_unlock(&mac->indirect_q.lock);
        return -ENOBUFS;
    }
    if (txq->key_mode == IEEE802154_ADDR_MODE_SHORT) {
        (void)txq->dst_short_addr;
    }
    else if (txq->key_mode == IEEE802154_ADDR_MODE_EXTENDED) {
        (void)txq->dst_ext_addr;
    }

    ieee802154_mac_tx_desc_t *d = ieee802154_mac_tx_peek(txq);
    if (d == NULL) {
        ieee802154_mac_handle_indirectq_auto_free(mac, &mac->indirect_q, slot);
        mutex_unlock(&mac->indirect_q.lock);
        return -ENOBUFS;
    }
    mac->indirect_q.current_slot = slot;
    mac->indirect_q.current_txq = txq;
    d->tx_state = IEEE802154_TX_STATE_IN_PROGRESS;
    mac->indirect_q.busy = true;
    mutex_unlock(&mac->indirect_q.lock);

    int r = ieee802154_send(&mac->submac, &d->iol_mhr);
    if (r != 0)
    {
        ieee802154_mac_tx_finish_current(mac, r, NULL);
        return -EIO;
    }
    return 0;
}
