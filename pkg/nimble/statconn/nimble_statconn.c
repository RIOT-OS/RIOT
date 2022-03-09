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

/* sanity check on the conn interval range to catch configuration errors */
#if NIMBLE_STATCONN_CONN_ITVL_MIN_MS > NIMBLE_STATCONN_CONN_ITVL_MAX_MS
#error "nimble_statconn: CONN_ITVL_MIN_MS must be <= CONN_ITVL_MAX_MS"
#endif

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
#if IS_USED(MODULE_NIMBLE_STATCONN_EXT)
    nimble_phy_t phy_mode;          /**< PHY mode used by this slot */
#endif
} slot_t;

static const uint8_t _ad[2] = { BLE_GAP_AD_FLAGS, BLUETIL_AD_FLAGS_DEFAULT };

static mutex_t _lock = MUTEX_INIT;
static slot_t _slots[NIMBLE_NETIF_MAX_CONN];

static nimble_netif_accept_cfg_t _accept_params;
static nimble_netif_connect_cfg_t _conn_params;

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
        /* try to (re)open the connection */
#if IS_USED(MODULE_NIMBLE_STATCONN_EXT)
        _conn_params.phy_mode = slot->phy_mode;
#endif
        nimble_netif_connect(&peer, &_conn_params);
    }
    else if (slot && (role == ROLE_S)) {
#if IS_USED(MODULE_NIMBLE_STATCONN_EXT)
        _accept_params.primary_phy = slot->phy_mode;
        _accept_params.secondary_phy = slot->phy_mode;
#endif
        nimble_netif_accept(_ad, sizeof(_ad), &_accept_params);
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

static int _be(uint8_t role, const uint8_t *addr,
               const nimble_statconn_cfg_t *cfg)
{
    mutex_lock(&_lock);
    slot_t *s = _get_addr(addr);
    if (s != NULL) {
        mutex_unlock(&_lock);
        return -EALREADY;
    }
    s = _get_state(UNUSED);
    if (s == NULL) {
        mutex_unlock(&_lock);
        return -ENOMEM;
    }

    s->state = (role | PENDING);
    memcpy(s->addr, addr, BLE_ADDR_LEN);
    mutex_unlock(&_lock);

#if IS_USED(MODULE_NIMBLE_STATCONN_EXT)
    if (cfg != NULL) {
        s->phy_mode = cfg->phy_mode;
    }
    else {
        s->phy_mode = NIMBLE_STATCONN_PHY_MODE;
    }
#else
    (void)cfg;
#endif

    _activate(role);
    return 0;
}

void nimble_statconn_init(void)
{
    memset(_slots, 0, sizeof(_slots));

    /* set the advertising parameters used */
    memset(&_accept_params, 0, sizeof(_accept_params));
#if IS_USED(MODULE_NIMBLE_STATCONN_EXT)
    _accept_params.flags = 0;
#else
    _accept_params.flags = NIMBLE_NETIF_FLAG_LEGACY;
    _accept_params.primary_phy = NIMBLE_PHY_1M;
    _accept_params.secondary_phy = NIMBLE_PHY_1M;
#endif
    _accept_params.adv_itvl_ms = NIMBLE_STATCONN_ADV_ITVL_MS;
    _accept_params.timeout_ms = BLE_HS_FOREVER;
    _accept_params.own_addr_type = nimble_riot_own_addr_type;

    /* set connection parameters */
    memset(&_conn_params, 0, sizeof(_conn_params));
#if !IS_USED(MODULE_NIMBLE_AUTOCONN_EXT)
    _conn_params.phy_mode = NIMBLE_PHY_1M;
#endif
    _conn_params.scan_itvl_ms = NIMBLE_STATCONN_CONN_WIN_MS;
    _conn_params.scan_window_ms = NIMBLE_STATCONN_CONN_WIN_MS;
    _conn_params.conn_itvl_min_ms = NIMBLE_STATCONN_CONN_ITVL_MIN_MS;
    _conn_params.conn_itvl_max_ms = NIMBLE_STATCONN_CONN_ITVL_MAX_MS;
    _conn_params.conn_supervision_timeout_ms = NIMBLE_STATCONN_CONN_SUPERTO_MS;
    _conn_params.conn_slave_latency = NIMBLE_STATCONN_CONN_LATENCY;
    _conn_params.timeout_ms = NIMBLE_STATCONN_CONN_TIMEOUT_MS;
    _conn_params.own_addr_type = nimble_riot_own_addr_type;

    /* register our event callback */
    nimble_netif_eventcb(_on_netif_evt);
}

void nimble_statconn_eventcb(nimble_netif_eventcb_t cb)
{
    _eventcb = cb;
}

int nimble_statconn_add_master(const uint8_t *addr,
                               const nimble_statconn_cfg_t *cfg)
{
    return _be(ROLE_S, addr, cfg);
}

int nimble_statconn_add_slave(const uint8_t *addr,
                              const nimble_statconn_cfg_t *cfg)
{
    return _be(ROLE_M, addr, cfg);
}

int nimble_statconn_rm(const uint8_t *addr)
{
    mutex_lock(&_lock);
    slot_t *s = _get_addr(addr);
    if (s == NULL) {
        mutex_unlock(&_lock);
        return -ENOTCONN;
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

    return 0;
}
