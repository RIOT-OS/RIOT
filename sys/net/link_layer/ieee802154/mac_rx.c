/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <string.h>
#include <errno.h>

#include "mutex.h"
#include "byteorder.h"

#include "net/ieee802154/mac.h"
#include "mac_fsm.h"
#include "mac_queue.h"
#include "mac_pib.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static bool _mac_rx_decode_frame(ieee802154_mac_t *mac, iolist_t *buf, int len,
                                 int mhr_len, uint8_t frame_type,
                                 ieee802154_mac_fsm_ctx_t *ctx,
                                 ieee802154_mac_fsm_ev_t *ev,
                                 bool *do_fsm)
{
    uint8_t cmd_type;

    switch (frame_type) {
    case IEEE802154_FCF_TYPE_BEACON:
        if (mac->scan_active) {
            *ev = IEEE802154_MAC_FSM_EV_RX_BEACON;
        }
        else {
            *do_fsm = false;
        }
        break;
    case IEEE802154_FCF_TYPE_DATA:
        *ev = IEEE802154_MAC_FSM_EV_RX_DATA;
        break;
    case IEEE802154_FCF_TYPE_MACCMD:
        cmd_type = ((const uint8_t *)buf->iol_base)[mhr_len];
        ctx->cmd_type = cmd_type;
        switch(cmd_type)
        {
        case IEEE802154_CMD_DATA_REQ:
            DEBUG("IEEE802154 MAC: IEEE802154_CMD_DATA_REQ\n");
            *ev = IEEE802154_MAC_FSM_EV_RX_CMD_DATA_REQ;
            break;
        case IEEE802154_CMD_BEACON_REQ:
            DEBUG("IEEE802154 MAC: IEEE802154_CMD_BEACON_REQ\n");
            *ev = IEEE802154_MAC_FSM_EV_RX_CMD_BEACON_REQ;
            break;
        case IEEE802154_CMD_ASSOCIATION_REQ:
            DEBUG("IEEE802154 MAC: IEEE802154_CMD_ASSOCIATION_REQ\n");
            *ev = IEEE802154_MAC_FSM_EV_RX_CMD_ASSOC_REQ;
            if ((size_t)len >= (size_t)mhr_len + 2U) {
                const uint8_t *pl = ((const uint8_t *)buf->iol_base) + mhr_len;
                ctx->capability.u8 = pl[1];
            }
            break;
        case IEEE802154_CMD_ASSOCIATION_RES:
            DEBUG("IEEE802154 MAC: IEEE802154_CMD_ASSOCIATION_RES\n");
            *ev = IEEE802154_MAC_FSM_EV_RX_CMD_ASSOC_RES;
            if ((size_t)len >= (size_t)mhr_len + 4U) {
                const uint8_t *pl = ((const uint8_t *)buf->iol_base) + mhr_len;
                /* pl[0] is command ID */
                le_uint16_t short_addr_le = { .u8 = { pl[1], pl[2] } };
                ctx->assoc_short_addr = byteorder_ltohs(short_addr_le);
                ctx->assoc_status = pl[3];
            }
            break;
        case IEEE802154_CMD_DISASSOCIATION:
            DEBUG("IEEE802154 MAC: IEEE802154_CMD_DISASSOCIATION\n");
            *ev = IEEE802154_MAC_FSM_EV_RX_CMD_DISASSOC;
            break;
        default:
            DEBUG("IEEE802154 MAC: unknown command id\n");
            *do_fsm = false;
            break;
        }
        break;
    default:
        DEBUG("IEEE802154 MAC: unknown FCF_TYPE: %d\n", frame_type);
        return false;
    }

    return true;
}

static bool _mac_rx_prepare_ctx(ieee802154_mac_t *mac, iolist_t *buf, int len,
                                const ieee802154_rx_info_t *info,
                                ieee802154_mac_fsm_ctx_t *ctx,
                                ieee802154_mac_fsm_ev_t *ev,
                                uint8_t *frame_type, bool *do_fsm)
{
    uint8_t src[IEEE802154_LONG_ADDRESS_LEN];//, dst[IEEE802154_LONG_ADDRESS_LEN];
    int mhr_len, src_len;
    le_uint16_t src_pan;
    mhr_len = ieee802154_get_frame_hdr_len(buf->iol_base);
    uint8_t fcf0 = ((const uint8_t *)buf->iol_base)[0];
    *frame_type = fcf0 & IEEE802154_FCF_TYPE_MASK;
    *do_fsm = true;

    //dst_len = ieee802154_get_dst(buf->iol_base, dst, &dst_pan);
    src_len = ieee802154_get_src(buf->iol_base, src, &src_pan);
    if (src_len < 0 || (size_t)src_len > sizeof(src)) {
        return false;
    }

    if (mac->poll_rx_active) {
        mac->poll_rx_active = false;
    }

    memset(ctx, 0, sizeof(*ctx));
    ctx->buf = buf;
    ctx->info = info;
    ctx->src_pan = src_pan;
    ctx->src_len = src_len;
    ctx->frame_type = *frame_type;
    ctx->assoc_status = 0xFFU;
    ctx->assoc_short_addr = 0xFFFFU;
    ctx->capability.u8 = 0;
    if (src_len == IEEE802154_SHORT_ADDRESS_LEN) {
        ctx->src_mode = IEEE802154_ADDR_MODE_SHORT;
    }
    else if (src_len == IEEE802154_LONG_ADDRESS_LEN) {
        ctx->src_mode = IEEE802154_ADDR_MODE_EXTENDED;
    }
    else {
        ctx->src_mode = IEEE802154_ADDR_MODE_NONE;
    }
    if (src_len > 0) {
        memcpy(ctx->src, src, (size_t)src_len);
        if (src_len == IEEE802154_LONG_ADDRESS_LEN) {
            memcpy(ctx->src_addr.uint8, src, IEEE802154_LONG_ADDRESS_LEN);
        }
    }

    if (!_mac_rx_decode_frame(mac, buf, len, mhr_len, *frame_type, ctx, ev, do_fsm)) {
        return false;
    }

    return true;
}

void ieee802154_mac_rx_process(ieee802154_mac_t *mac, iolist_t *buf)
{
    uint8_t frame_type;
    ieee802154_mac_fsm_ev_t ev;
    bool do_fsm = true;
    ieee802154_mac_fsm_ctx_t ctx;
    mutex_lock(&mac->submac_lock);
    int len = ieee802154_get_frame_length(&mac->submac);
    if (len <= 0) {
        DEBUG("IEEE802154 MAC: RX length %d (scan_active=%d)\n", len, mac->scan_active);
        (void)ieee802154_read_frame(&mac->submac, NULL, 0, NULL);
        mutex_unlock(&mac->submac_lock);
        return;
    }
    ieee802154_rx_info_t info;
    buf->iol_len = len;
    (void)ieee802154_read_frame(&mac->submac, buf->iol_base, buf->iol_len, &info);
    mutex_unlock(&mac->submac_lock);
    if (!_mac_rx_prepare_ctx(mac, buf, len, &info, &ctx, &ev, &frame_type, &do_fsm)) {
        ieee802154_pib_value_t rx_on;

        ieee802154_mac_mlme_get(mac, IEEE802154_PIB_RX_ON_WHEN_IDLE, &rx_on);

        if (!rx_on.v.b) {
            return;
        }

        if (mac->cbs.rx_request) {
            mac->cbs.rx_request(mac);
        }
        return;
    }
    if (do_fsm) {
        (void)ieee802154_mac_fsm_process_ev_ctx(mac, ev, &ctx);
    }
    /* Free RX buffers for frames handled entirely inside MAC.
     * DATA frames are freed by data_indication callback. */
    if (frame_type != IEEE802154_FCF_TYPE_DATA || !mac->cbs.data_indication) {
        if (mac->cbs.dealloc_request) {
            mac->cbs.dealloc_request(mac, buf);
        }
    }
    ieee802154_pib_value_t rx_on;

    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_RX_ON_WHEN_IDLE, &rx_on);

    if (!rx_on.v.b) {
        return;
    }

    if (mac->cbs.rx_request) {
        mac->cbs.rx_request(mac);
    }
}

void ieee802154_mac_rx_request_process(ieee802154_mac_t *mac)
{
    mutex_lock(&mac->submac_lock);
    if (!mac->rx_retry_pending) {
        mac->rx_retry_count = 0;
    }
    mac->rx_retry_pending = false;

    int res = ieee802154_set_rx(&mac->submac);
    bool retry = (res == -EBUSY) && mac->cbs.rx_request &&
                 (mac->rx_retry_count < IEEE802154_MAC_RX_RETRY_MAX);

    if (retry) {
        mac->rx_retry_count++;
        mac->rx_retry_pending = true;
    }
    else {
        mac->rx_retry_count = 0;
    }
    mutex_unlock(&mac->submac_lock);

    if (retry) {
        mac->cbs.rx_request(mac);
    }
    else if ((res < 0) && (res != -EALREADY)) {
        DEBUG("IEEE802154 MAC: failed to switch to RX: %d\n", res);
    }
}
