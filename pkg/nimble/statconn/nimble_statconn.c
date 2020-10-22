/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_statconn
 * @{
 *
 * @file
 * @brief       Statconn - static connection manager for NimBLE netif
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "assert.h"
#include "net/bluetil/addr.h"
#include "net/bluetil/ad.h"
#include "nimble_netif.h"
#include "nimble_netif_conn.h"
#include "nimble_statconn.h"

#include "host/ble_hs.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define UNUSED          (0x00)
#define PENDING         (0x80)
#define CONNECTED       (0x00)
#define ROLE_M          (0x01)
#define ROLE_S          (0x02)

typedef struct {
    uint8_t addr[BLE_ADDR_LEN];     /**< peer addr, network byte order */
    uint8_t state;                  /**< internal state */
} slot_t;

static const uint8_t _ad[2] = { BLE_GAP_AD_FLAGS, BLUETIL_AD_FLAGS_DEFAULT };

static mutex_t _lock = MUTEX_INIT;
static slot_t _slots[NIMBLE_NETIF_MAX_CONN];

static struct ble_gap_adv_params _adv_params;
static struct ble_gap_conn_params _conn_params;
static uint32_t _conn_timeout;

static nimble_netif_eventcb_t _eventcb = NULL;


static slot_t *_get_addr(const uint8_t *addr)
{
    for (unsigned i = 0; i < ARRAY_SIZE(_slots); i++) {
        if (memcmp(addr, _slots[i].addr, BLE_ADDR_LEN) == 0) {
            return &_slots[i];
        }
    }
    return NULL;
}

static slot_t *_get_state(uint8_t state)
{
    for (unsigned i = 0; i < ARRAY_SIZE(_slots); i++) {
        if (_slots[i].state == state) {
            return &_slots[i];
        }
    }
    return NULL;
}

static void _activate(uint8_t role)
{
    mutex_lock(&_lock);
    slot_t *slot = _get_state(role | PENDING);

    if (slot && (role == ROLE_M)) {
        ble_addr_t peer;
        peer.type = BLE_ADDR_RANDOM;
        bluetil_addr_swapped_cp(slot->addr, peer.val);
        nimble_netif_connect(&peer, &_conn_params, _conn_timeout);
    }
    else if (slot && (role == ROLE_S)) {
        nimble_netif_accept(_ad, sizeof(_ad), &_adv_params);
    }
    mutex_unlock(&_lock);
}

static void _update(const uint8_t *addr, uint8_t role, uint8_t state)
{
    mutex_lock(&_lock);
    slot_t *slot = _get_addr(addr);
    if (slot) {
        slot->state = (role | state);
    }
    else {
        DEBUG("[statconn] warning: state change on unknown peer address\n");
    }
    mutex_unlock(&_lock);

    _activate(role);
}

static void _on_netif_evt(int handle, nimble_netif_event_t event,
                          const uint8_t *addr)
{
    switch (event) {
        case NIMBLE_NETIF_ACCEPTING:
            break;
        case NIMBLE_NETIF_ACCEPT_STOP:
            break;
        case NIMBLE_NETIF_INIT_MASTER:
            break;
        case NIMBLE_NETIF_INIT_SLAVE:
            break;
        case NIMBLE_NETIF_CONNECTED_MASTER:
            _update(addr, ROLE_M, CONNECTED);
            break;
        case NIMBLE_NETIF_CONNECTED_SLAVE:
            _update(addr, ROLE_S, CONNECTED);
            break;
        case NIMBLE_NETIF_CLOSED_MASTER:
            _update(addr, ROLE_M, PENDING);
            break;
        case NIMBLE_NETIF_CLOSED_SLAVE:
            _update(addr, ROLE_S, PENDING);
            break;
        case NIMBLE_NETIF_ABORT_MASTER:
            _update(addr, ROLE_M, PENDING);
            break;
        case NIMBLE_NETIF_ABORT_SLAVE:
            _update(addr, ROLE_S, PENDING);
            break;
        case NIMBLE_NETIF_CONN_UPDATED:
            break;
        default:
            /* this should never happen */
            assert(0);
    }

    /* pass events to high-level user if someone registered for them */
    if (_eventcb) {
        _eventcb(handle, event, addr);
    }
}

static int _be(uint8_t role, const uint8_t *addr)
{
    mutex_lock(&_lock);
    slot_t *s = _get_addr(addr);
    if (s != NULL) {
        mutex_unlock(&_lock);
        return NIMBLE_STATCONN_INUSE;
    }
    s = _get_state(UNUSED);
    if (s == NULL) {
        mutex_unlock(&_lock);
        return NIMBLE_STATCONN_NOSLOT;
    }

    s->state = (role | PENDING);
    memcpy(s->addr, addr, BLE_ADDR_LEN);
    mutex_unlock(&_lock);
    _activate(role);
    return NIMBLE_STATCONN_OK;
}

void nimble_statconn_init(void)
{
    memset(_slots, 0, sizeof(_slots));

    /* set the advertising parameters used */
    _adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    _adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    _adv_params.itvl_min = BLE_GAP_ADV_ITVL_MS(NIMBLE_STATCONN_ADV_ITVL_MS);
    _adv_params.itvl_max = _adv_params.itvl_min;
    _adv_params.channel_map = 0;
    _adv_params.filter_policy = 0;
    _adv_params.high_duty_cycle = 0;

    /* set connection parameters */
    _conn_params.scan_itvl = BLE_GAP_SCAN_ITVL_MS(NIMBLE_STATCONN_CONN_WIN_MS);
    _conn_params.scan_window = _conn_params.scan_itvl;
    _conn_params.itvl_min = BLE_GAP_CONN_ITVL_MS(NIMBLE_STATCONN_CONN_ITVL_MS);
    _conn_params.itvl_max = _conn_params.itvl_min;
    _conn_params.latency = NIMBLE_STATCONN_CONN_LATENCY;
    _conn_params.supervision_timeout = BLE_GAP_SUPERVISION_TIMEOUT_MS(
                                               NIMBLE_STATCONN_CONN_SUPERTO_MS);
    _conn_params.min_ce_len = 0;
    _conn_params.max_ce_len = 0;
    _conn_timeout = NIMBLE_STATCONN_CONN_TIMEOUT_MS;

    /* register our event callback */
    nimble_netif_eventcb(_on_netif_evt);
}

void nimble_statconn_eventcb(nimble_netif_eventcb_t cb)
{
    _eventcb = cb;
}

int nimble_statconn_add_master(const uint8_t *addr)
{
    return _be(ROLE_S, addr);
}

int nimble_statconn_add_slave(const uint8_t *addr)
{
    return _be(ROLE_M, addr);
}

int nimble_statconn_rm(const uint8_t *addr)
{
    mutex_lock(&_lock);
    slot_t *s = _get_addr(addr);
    if (s == NULL) {
        mutex_unlock(&_lock);
        return NIMBLE_STATCONN_NOTCONN;
    }
    uint8_t role = (s->state & ROLE_M) ? ROLE_M : ROLE_S;

    if (role == ROLE_S) {
        nimble_netif_accept_stop();
    }
    s->state = UNUSED;
    memset(s->addr, 0, sizeof(BLE_ADDR_LEN));
    mutex_unlock(&_lock);

    nimble_netif_close(nimble_netif_conn_get_by_addr(addr));
    if (role == ROLE_S) {
        _activate(ROLE_S);
    }

    return NIMBLE_STATCONN_OK;
}
