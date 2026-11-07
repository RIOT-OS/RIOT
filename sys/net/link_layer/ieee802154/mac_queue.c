/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <string.h>

#include "mac_queue.h"
#include "mac_fsm.h"
#include "mac_pib.h"
#include "net/ieee802154.h"
#include "net/ieee802154/radio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static uint8_t ieee80214_addr_len_from_mode(ieee802154_addr_mode_t mode);
static int _enqueue_data_tx(ieee802154_mac_t *mac,
                            uint8_t frame_type,
                            ieee802154_mac_txq_t *txq,
                            ieee802154_addr_mode_t src_mode,
                            ieee802154_addr_mode_t dst_mode,
                            const uint16_t *dst_panid,
                            const void *dst_addr,
                            iolist_t *msdu,
                            const uint8_t *msdu_handle,
                            bool ack_req,
                            bool indirect);
static uint8_t ieee80214_addr_len_from_mode(ieee802154_addr_mode_t mode)
{
    switch (mode) {
    case IEEE802154_ADDR_MODE_NONE:
        return 0;
    case IEEE802154_ADDR_MODE_SHORT:
        return IEEE802154_SHORT_ADDRESS_LEN;
    case IEEE802154_ADDR_MODE_EXTENDED:
        return IEEE802154_LONG_ADDRESS_LEN;
    default:
        return 0;
    }
}

bool ieee802154_mac_tx_full(const ieee802154_mac_txq_t *txq)
{
    return txq->cnt >= IEEE802154_MAC_TXQ_LEN;
}

bool ieee802154_mac_tx_empty(const ieee802154_mac_txq_t *txq)
{
    return (txq->cnt == 0);
}

ieee802154_mac_tx_desc_t *ieee802154_mac_tx_reserve(ieee802154_mac_txq_t *txq)
{
    if (!txq || ieee802154_mac_tx_full(txq)) {
        return NULL;
    }

    ieee802154_mac_tx_desc_t *d = &txq->q[txq->tail];
    memset(d, 0, sizeof(*d));
    d->in_use = true;
    return d;
}

void ieee802154_mac_tx_commit(ieee802154_mac_txq_t *txq)
{
    txq->tail = (uint8_t)((txq->tail + 1) % IEEE802154_MAC_TXQ_LEN);
    txq->cnt++;
}

ieee802154_mac_tx_desc_t *ieee802154_mac_tx_peek(ieee802154_mac_txq_t *txq)
{
    if (!txq || ieee802154_mac_tx_empty(txq)) {
        return NULL;
    }

    ieee802154_mac_tx_desc_t *d = &txq->q[txq->head];

    if (!d->in_use) {
        return NULL;
    }

    return d;
}

void ieee802154_mac_tx_pop(ieee802154_mac_txq_t *txq)
{
    if (!txq || ieee802154_mac_tx_empty(txq)) {
        return;
    }

    ieee802154_mac_tx_desc_t *d = &txq->q[txq->head];
    memset(d, 0, sizeof(*d));

    txq->head = (uint8_t)((txq->head + 1) % IEEE802154_MAC_TXQ_LEN);
    txq->cnt--;
}

int ieee802154_indirectq_alloc_slot(ieee802154_mac_indirect_q_t *indirect_q)
{
    if (indirect_q->free_mask == 0) {
        return -1;
    }

    /* first free slot */
    uint8_t slot = __builtin_ctz(indirect_q->free_mask);
    /* 0 means used */
    indirect_q->free_mask &= ~(1U << slot);
    return slot;
}

void ieee802154_indirectq_free_slot(ieee802154_mac_indirect_q_t *indirect_q, uint8_t slot)
{
    memset(&indirect_q->q[slot], 0, sizeof(ieee802154_mac_txq_t));
    indirect_q->free_mask |= (1U << slot);
}

bool ieee802154_indirectq_empty(const ieee802154_mac_indirect_q_t *indirect_q)
{
    /* all slots free */
    return indirect_q->free_mask == ((1U << IEEE802154_MAC_TX_INDIRECTQ_SIZE) - 1U);
}

uint16_t ieee802154_indirect_get_deadline(ieee802154_mac_t *mac)
{
    uint32_t unit_period_us = (uint32_t)IEEE802154_MAC_FRAME_TIMEOUT * (uint32_t)mac->sym_us;
    /* round up to handle too early timeouts */
    uint32_t unit_period_ms = (unit_period_us + 999U) / 1000U;
    uint16_t unit_period_ticks = (uint16_t)(
        (unit_period_ms + IEEE802154_MAC_TICK_INTERVAL_MS - 1U) / IEEE802154_MAC_TICK_INTERVAL_MS);
    return (mac->indirect_q.tick + unit_period_ticks);
}

bool ieee802154_mac_frame_is_expired(uint16_t now_tick, uint16_t deadline_tick)
{
    return ((int16_t)(now_tick - deadline_tick)) >= 0;
}

static int _assoc_find_ext_locked(const ieee802154_mac_t *mac,
                                  const ieee802154_ext_addr_t *ext_addr)
{
    for (int i = 0; i < IEEE802154_MAC_ASSOC_TABLE_SIZE; i++) {
        const ieee802154_mac_assoc_entry_t *entry = &mac->assoc_table[i];

        if (entry->valid &&
            memcmp(entry->ext_addr.uint8, ext_addr->uint8,
                   IEEE802154_LONG_ADDRESS_LEN) == 0) {
            return i;
        }
    }
    return -1;
}

static int _assoc_find_short_locked(const ieee802154_mac_t *mac,
                                    ieee802154_short_addr_t short_addr)
{
    for (int i = 0; i < IEEE802154_MAC_ASSOC_TABLE_SIZE; i++) {
        const ieee802154_mac_assoc_entry_t *entry = &mac->assoc_table[i];

        if (entry->valid && (entry->short_addr.u16 == short_addr.u16)) {
            return i;
        }
    }
    return -1;
}

void ieee802154_mac_assoc_update(ieee802154_mac_t *mac,
                                 const ieee802154_ext_addr_t *ext_addr,
                                 ieee802154_short_addr_t short_addr)
{
    if (!mac || !ext_addr) {
        return;
    }

    uint16_t short_host = byteorder_ntohs(short_addr);
    if ((short_host == 0xffffU) || (short_host == 0xfffeU)) {
        return;
    }

    mutex_lock(&mac->indirect_q.lock);
    int slot = _assoc_find_ext_locked(mac, ext_addr);
    if (slot < 0) {
        slot = _assoc_find_short_locked(mac, short_addr);
    }
    if (slot < 0) {
        for (int i = 0; i < IEEE802154_MAC_ASSOC_TABLE_SIZE; i++) {
            if (!mac->assoc_table[i].valid) {
                slot = i;
                break;
            }
        }
    }
    if (slot < 0) {
        slot = 0;
    }

    mac->assoc_table[slot].ext_addr = *ext_addr;
    mac->assoc_table[slot].short_addr = short_addr;
    mac->assoc_table[slot].valid = true;
    mutex_unlock(&mac->indirect_q.lock);
}

static bool _assoc_ext_matches_short_locked(const ieee802154_mac_t *mac,
                                            const ieee802154_ext_addr_t *ext_addr,
                                            ieee802154_short_addr_t short_addr)
{
    int slot = _assoc_find_ext_locked(mac, ext_addr);

    return (slot >= 0) &&
           (mac->assoc_table[slot].short_addr.u16 == short_addr.u16);
}

void ieee802154_mac_indirect_fp_update(ieee802154_mac_t *mac,
                                       ieee802154_addr_mode_t dst_mode,
                                       const void *dst_addr,
                                       bool pending)
{
#ifdef IEEE802154_MAC_INDIRECT_ENABLE
#  ifdef IEEE802154_MAC_HAS_SRC_ADDR_MATCH
    if (dst_mode == IEEE802154_ADDR_MODE_SHORT) {
        if (dst_addr) {
            const network_uint16_t *short_addr = (const network_uint16_t *)dst_addr;
            ieee802154_submac_config_src_address_match(&mac->submac,
                                                      pending ? IEEE802154_SRC_MATCH_SHORT_ADD
                                                              : IEEE802154_SRC_MATCH_SHORT_CLEAR,
                                                      short_addr);
        }
    }
    else if (dst_mode == IEEE802154_ADDR_MODE_EXTENDED) {
        const ieee802154_ext_addr_t *ext = (const ieee802154_ext_addr_t *)dst_addr;
        if (ext) {
            ieee802154_submac_config_src_address_match(&mac->submac,
                                                      pending ? IEEE802154_SRC_MATCH_EXT_ADD
                                                              : IEEE802154_SRC_MATCH_EXT_CLEAR,
                                                      ext);
            int slot = _assoc_find_ext_locked(mac, ext);
            if (slot >= 0) {
                ieee802154_submac_config_src_address_match(&mac->submac,
                                                          pending ? IEEE802154_SRC_MATCH_SHORT_ADD
                                                                  : IEEE802154_SRC_MATCH_SHORT_CLEAR,
                                                          &mac->assoc_table[slot].short_addr);
            }
        }
    }
#  else
    (void) dst_mode;
    (void)dst_addr;
    bool any_pending = pending;
    if (!pending) {
        any_pending = !ieee802154_indirectq_empty(&mac->indirect_q);
    }
    ieee802154_submac_config_src_address_match(&mac->submac, IEEE802154_SRC_MATCH_EN, &any_pending);
#  endif
#else
    (void)mac;
    (void)dst_mode;
    (void)dst_addr;
    (void)pending;
#endif
}

void ieee802154_mac_handle_indirectq_auto_free(ieee802154_mac_t *mac,
                                               ieee802154_mac_indirect_q_t *indirect_q,
                                               uint8_t slot)
{
    ieee802154_mac_txq_t *txq = &indirect_q->q[slot];

    if (ieee802154_mac_tx_empty(txq)) {
        const ieee802154_addr_mode_t key_mode = txq->key_mode;
        const void *dst_addr = NULL;
        network_uint16_t dst_short = { .u16 = 0 };
        ieee802154_ext_addr_t dst_ext;

        if (key_mode == IEEE802154_ADDR_MODE_SHORT) {
            dst_short = txq->dst_short_addr;
            dst_addr = &dst_short;
        }
        else if (key_mode == IEEE802154_ADDR_MODE_EXTENDED) {
            dst_ext = txq->dst_ext_addr;
            dst_addr = &dst_ext;
        }
        ieee802154_indirectq_free_slot(indirect_q, slot);
        ieee802154_mac_indirect_fp_update(mac, key_mode, dst_addr, false);
    }
    else {
        ieee802154_mac_tx_desc_t *d = ieee802154_mac_tx_peek(txq);
        if (d) {
            txq->deadline_tick = &d->deadline_tick;
        }
    }
}

static int _enqueue_data_tx(ieee802154_mac_t *mac,
                            uint8_t frame_type,
                            ieee802154_mac_txq_t *txq,
                            ieee802154_addr_mode_t src_mode,
                            ieee802154_addr_mode_t dst_mode,
                            const uint16_t *dst_panid,
                            const void *dst_addr,
                            iolist_t *msdu,
                            const uint8_t *msdu_handle,
                            bool ack_req,
                            bool indirect)
{
    ieee802154_mac_tx_desc_t *dsc = ieee802154_mac_tx_reserve(txq);
    if (!dsc) {
        return -ENOBUFS;
    }
    dsc->handle = *msdu_handle;
    dsc->type = frame_type;
    dsc->ack = ack_req;
    dsc->indirect = indirect;
    dsc->tx_state = IEEE802154_TX_STATE_QUEUED;

    /* src addr selection */
    const void *src = NULL;
    ieee802154_pib_value_t src_v;
    if (src_mode == IEEE802154_ADDR_MODE_SHORT) {
        ieee802154_mac_mlme_get(mac, IEEE802154_PIB_SHORT_ADDR, &src_v);
        src = &src_v.v.short_addr;
    }
    else if (src_mode == IEEE802154_ADDR_MODE_EXTENDED) {
        ieee802154_mac_mlme_get(mac, IEEE802154_PIB_EXTENDED_ADDRESS, &src_v);
        src = &src_v.v.ext_addr;
    }

    uint8_t src_len = ieee80214_addr_len_from_mode(src_mode);
    uint8_t dst_len = ieee80214_addr_len_from_mode(dst_mode);
    ieee802154_pib_value_t src_panid;
    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_PAN_ID, &src_panid);
    le_uint16_t src_pan = byteorder_btols(byteorder_htons(src_panid.v.u16));
    le_uint16_t dst_pan = {0};
    if (dst_panid) {
        dst_pan = byteorder_btols(byteorder_htons(*dst_panid));
    }

    ieee802154_pib_value_t dsn;
    ieee802154_mac_mlme_get(mac, IEEE802154_PIB_DSN, &dsn);

    uint8_t flags = frame_type;
    if ((frame_type != IEEE802154_FCF_TYPE_BEACON) && ack_req) {
        flags |= IEEE802154_FCF_ACK_REQ;
    }

    bool was_empty = ieee802154_mac_tx_empty(txq);
    size_t mhr_len = ieee802154_set_frame_hdr(dsc->mhr,
                                              src, (size_t)src_len,
                                              dst_len ? dst_addr : NULL, (size_t)dst_len,
                                              src_pan, dst_pan,
                                              flags,
                                              dsn.v.u8);
    if (mhr_len == 0 || mhr_len > (int)sizeof(dsc->mhr)) {
        dsc->in_use = false;
        return -EINVAL;
    }

    dsc->iol_msdu = msdu;
    dsc->iol_mhr.iol_base = &dsc->mhr;
    dsc->iol_mhr.iol_len = mhr_len;
    dsc->iol_mhr.iol_next = dsc->iol_msdu;
    if (msdu && (msdu->iol_len == 0)) {
        /* Avoid zero-length iolist entries with NULL base. */
        dsc->iol_mhr.iol_next = NULL;
    }

    /* DSN++ */
    ieee802154_pib_value_t dsn_new = {
        .type = IEEE802154_PIB_TYPE_U8,
        .v.u8 = dsn.v.u8 + 1,
    };
    ieee802154_mac_mlme_set(mac, IEEE802154_PIB_DSN, &dsn_new);
    uint16_t deadline = ieee802154_indirect_get_deadline(mac);
    dsc->deadline_tick = deadline;
    if (was_empty) {
        txq->deadline_tick = &dsc->deadline_tick;
    }
    ieee802154_mac_tx_commit(txq);
    if (indirect) {
        ieee802154_mac_indirect_fp_update(mac, dst_mode, dst_addr, true);
    }
    return 0;
}

static int _indirectq_search_slot_locked(ieee802154_mac_t *mac,
                                         ieee802154_addr_mode_t dst_mode,
                                         const void *dst_addr)
{
    ieee802154_mac_indirect_q_t *indirect_q = &mac->indirect_q;
    if (!dst_addr) {
        for (int i = 0; i < IEEE802154_MAC_TX_INDIRECTQ_SIZE; i++) {
            if (!indirect_q->q[i].has_dst_addr &&
                !ieee802154_mac_tx_empty(&indirect_q->q[i])) {
                return i;
            }
        }
        return -1;
    }

    network_uint16_t short_addr = { .u16 = 0 };
    const ieee802154_ext_addr_t *ext_addr = NULL;
    if (dst_mode == IEEE802154_ADDR_MODE_SHORT) {
        short_addr = *(const network_uint16_t *)dst_addr;
    }
    else if (dst_mode == IEEE802154_ADDR_MODE_EXTENDED) {
        ext_addr = (const ieee802154_ext_addr_t *)dst_addr;
    }

    for (int i = 0; i < IEEE802154_MAC_TX_INDIRECTQ_SIZE; i++) {
        ieee802154_mac_txq_t *q = &indirect_q->q[i];
        if (!q->has_dst_addr) {
            continue;
        }
        if (q->key_mode == IEEE802154_ADDR_MODE_EXTENDED) {
            if (ext_addr &&
                memcmp(q->dst_ext_addr.uint8, ext_addr->uint8,
                       IEEE802154_LONG_ADDRESS_LEN) == 0) {
                return i;
            }
            if ((dst_mode == IEEE802154_ADDR_MODE_SHORT) &&
                _assoc_ext_matches_short_locked(mac, &q->dst_ext_addr, short_addr)) {
                return i;
            }
        }
        else if (q->key_mode == IEEE802154_ADDR_MODE_SHORT) {
            if (q->dst_short_addr.u16 == short_addr.u16) {
                return i;
            }
            if (ext_addr &&
                _assoc_ext_matches_short_locked(mac, ext_addr, q->dst_short_addr)) {
                return i;
            }
        }
    }
    return -1;
}

int ieee802154_mac_indirectq_search_slot(ieee802154_mac_t *mac,
                                         ieee802154_addr_mode_t dst_mode,
                                         const void *dst_addr)
{
    int slot;
    mutex_lock(&mac->indirect_q.lock);
    slot = _indirectq_search_slot_locked(mac, dst_mode, dst_addr);
    mutex_unlock(&mac->indirect_q.lock);
    return slot;
}

static int _indirectq_get_slot_locked(ieee802154_mac_t *mac,
                                      ieee802154_addr_mode_t dst_mode,
                                      const void *dst_addr)
{
    int slot = _indirectq_search_slot_locked(mac, dst_mode, dst_addr);
    ieee802154_mac_indirect_q_t *indirect_q = &mac->indirect_q;

    if (slot >= 0) {
        return slot;
    }
    slot = ieee802154_indirectq_alloc_slot(indirect_q);
    if (slot < 0) {
        return slot;
    }

    ieee802154_mac_txq_t *q = &indirect_q->q[slot];
    if (dst_addr) {
        q->has_dst_addr = true;
        q->dst_mode = dst_mode;
        if (dst_mode == IEEE802154_ADDR_MODE_SHORT) {
            q->dst_short_addr = *(const network_uint16_t *)dst_addr;
            q->key_mode = IEEE802154_ADDR_MODE_SHORT;
        }
        else if (dst_mode == IEEE802154_ADDR_MODE_EXTENDED) {
            q->key_mode = IEEE802154_ADDR_MODE_EXTENDED;
            q->dst_ext_addr = *(const ieee802154_ext_addr_t *)dst_addr;
        }
        else {
            q->key_mode = dst_mode;
        }
    }
    else {
        q->has_dst_addr = false;
    }
    return slot;
}

int ieee802154_mac_indirectq_get_slot(ieee802154_mac_t *mac,
                                      ieee802154_addr_mode_t dst_mode,
                                      const void *dst_addr)
{
    int slot;
    mutex_lock(&mac->indirect_q.lock);
    slot = _indirectq_get_slot_locked(mac, dst_mode, dst_addr);
    mutex_unlock(&mac->indirect_q.lock);
    return slot;
}

int ieee802154_mac_map_push(ieee802154_mac_t *mac,
                            uint8_t frame_type,
                            ieee802154_addr_mode_t src_mode,
                            ieee802154_addr_mode_t dst_mode,
                            const uint16_t *dst_panid,
                            const void *dst_addr,
                            iolist_t *msdu,
                            const uint8_t *msdu_handle,
                            bool ack_req,
                            bool indirect)
{
    mutex_lock(&mac->indirect_q.lock);
    int slot = _indirectq_get_slot_locked(mac, dst_mode, dst_addr);
    if (slot < 0) {
        mutex_unlock(&mac->indirect_q.lock);
        return -ENOBUFS;
    }

    int res = _enqueue_data_tx(mac, frame_type, &mac->indirect_q.q[slot], src_mode,
                               dst_mode, dst_panid, dst_addr, msdu,
                               msdu_handle, ack_req, indirect);
    if (res < 0) {
        DEBUG("Enqueue Data failed: %d (%s)\n", res, strerror(-res));
        mutex_unlock(&mac->indirect_q.lock);
        return res;
    }
    mutex_unlock(&mac->indirect_q.lock);
    return 0;
}

int ieee802154_mac_enqueue_data_request(ieee802154_mac_t *mac,
                                        ieee802154_addr_mode_t dst_mode,
                                        uint16_t *dst_panid,
                                        const void *dst_addr)
{
    *(uint8_t *)mac->cmd.iol_base = IEEE802154_CMD_DATA_REQ;
    mac->cmd.iol_len = 1;
    mac->cmd.iol_next = NULL;
    // TODO: check for src addressing mode
    uint8_t handle = 0xFFU;
    ieee802154_mac_map_push(mac, IEEE802154_FCF_TYPE_MACCMD, dst_mode, dst_mode,
                            dst_panid, dst_addr, &mac->cmd, &handle, true, false);
    return 0;
}

int ieee802154_mac_enqueue_beacon_request(ieee802154_mac_t *mac)
{
    *(uint8_t *)mac->cmd.iol_base = IEEE802154_CMD_BEACON_REQ;
    mac->cmd.iol_len = 1;
    mac->cmd.iol_next = NULL;
    uint8_t handle = 0xFFU;
    uint8_t panid_bytes[2] = IEEE802154_PANID_BCAST;
    uint16_t panid = ((uint16_t)panid_bytes[1] << 8) | (uint16_t)panid_bytes[0];
    ieee802154_ext_addr_t dst_addr = {.uint8 = IEEE802154_ADDR_BCAST};
    ieee802154_mac_map_push(mac, IEEE802154_FCF_TYPE_MACCMD, IEEE802154_ADDR_MODE_NONE,
                            IEEE802154_ADDR_MODE_SHORT, &panid, &dst_addr, &mac->cmd,
                            &handle, false, false);
    return 0;
}
