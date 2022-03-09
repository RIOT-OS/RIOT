/*
 * Copyright (C) 2018-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_autoconn
 * @{
 *
 * @file
 * @brief       Autoconn connection manager implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "assert.h"
#include "random.h"
#include "net/bluetil/ad.h"
#include "net/bluetil/addr.h"
#include "nimble_netif.h"
#include "nimble_netif_conn.h"
#include "nimble_scanner.h"
#include "nimble_autoconn.h"
#include "nimble_autoconn_params.h"

#include "host/ble_hs.h"
#include "nimble/nimble_port.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#if defined(MODULE_NIMBLE_AUTOCONN_IPSP)
#define SVC_FILTER      BLE_GATT_SVC_IPSS
#elif defined(MODULE_NIMBLE_AUTOCONN_NDNSP)
#define SVC_FILTER      BLE_GATT_SVC_NDNSS
#else
#error "NimBLE autoconn: please select a fitting submodule"
#endif

enum {
    STATE_SCAN,
    STATE_ADV,
    STATE_IDLE,
    STATE_CONN,
};

static volatile uint8_t _state = STATE_IDLE;
static volatile uint8_t _enabled = 0;

static bluetil_ad_t _ad;
static uint8_t _ad_buf[BLE_HS_ADV_MAX_SZ];

static nimble_netif_accept_cfg_t _accept_params;
static nimble_netif_connect_cfg_t _conn_params;

static struct ble_npl_callout _state_evt;
static ble_npl_time_t _timeout_adv_period;
static ble_npl_time_t _timeout_scan_period;
static ble_npl_time_t _period_jitter;

static nimble_netif_eventcb_t _eventcb = NULL;

/* this is run inside the NimBLE host thread */
static void _on_state_change(struct ble_npl_event *ev)
{
    (void)ev;
    ble_npl_time_t offset;
    offset = (ble_npl_time_t)random_uint32_range(0, (uint32_t)_period_jitter);

    if (_state == STATE_SCAN) {
        /* stop scanning */
        nimble_scanner_stop();
        /* start advertising/accepting */
        int res = nimble_netif_accept(_ad.buf, _ad.pos, &_accept_params);
        assert((res == 0) || (res == -ENOMEM));
        (void)res;

        /* schedule next state change */
        _state = STATE_ADV;
        ble_npl_callout_reset(&_state_evt, (_timeout_adv_period + offset));
    }
    else if (_state == STATE_ADV) {
        /* stop advertising/accepting */
        nimble_netif_accept_stop();
        /* start scanning */
        nimble_scanner_start();
        _state = STATE_SCAN;
        ble_npl_callout_reset(&_state_evt, (_timeout_scan_period + offset));
    }
}

static void _activate(void)
{
    if (_enabled && (_state == STATE_IDLE) &&
        (nimble_netif_conn_count(NIMBLE_NETIF_UNUSED) > 0)) {
        _state = STATE_SCAN;
        _on_state_change(NULL);
        DEBUG("[autoconn] ACTIVATED\n");
    }
}

static void _deactivate(void)
{
    if ((_state == STATE_ADV) || (_state == STATE_SCAN)) {
        ble_npl_callout_stop(&_state_evt);
        nimble_scanner_stop();
        nimble_netif_accept_stop();
        _state = STATE_IDLE;
        DEBUG("[autoconn] DEACTIVATED\n");
    }
}

static int _filter_uuid(const bluetil_ad_t *ad)
{
    bluetil_ad_data_t incomp;
    if (bluetil_ad_find(ad, BLE_GAP_AD_UUID16_INCOMP, &incomp) == BLUETIL_AD_OK) {
        uint16_t filter_uuid = SVC_FILTER;
        for (unsigned i = 0; i < incomp.len; i += 2) {
            if (memcmp(&filter_uuid, &incomp.data[i], 2) == 0) {
                return 1;
            }
        }
    }

    return 0;
}

static void _on_scan_evt(uint8_t type, const ble_addr_t *addr,
                         const nimble_scanner_info_t *info,
                         const uint8_t *ad_buf, size_t ad_len)
{
    (void)info;

#if IS_USED(MODULE_NIMBLE_AUTOCONN_EXT)
    if ((type != (NIMBLE_SCANNER_EXT_ADV | BLE_HCI_ADV_CONN_MASK)) ||
        (info->status != BLE_GAP_EXT_ADV_DATA_STATUS_COMPLETE)) {
        return;
    }
#else
    /* we are only interested in ADV_IND packets, the rest can be dropped right
     * away */
    if (type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND) {
        return;
    }
#endif

    bluetil_ad_t ad = {
        .buf  = (uint8_t *)ad_buf,
        .pos  = ad_len,
        .size = ad_len
    };

    /* for connection checking we need the address in network byte order */
    uint8_t addrn[BLE_ADDR_LEN];
    bluetil_addr_swapped_cp(addr->val, addrn);

    if (_filter_uuid(&ad) && !nimble_netif_conn_connected(addrn)) {
        nimble_scanner_stop();
        DEBUG("[autoconn] SCAN success, initiating connection\n");
        _state = STATE_CONN;
        int res = nimble_netif_connect(addr, &_conn_params);
        assert(res >= 0);
        (void)res;
    }
}

static void _evt_dbg(const char *msg, int handle, const uint8_t *addr)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        printf("[autoconn] %s (%i|", msg, handle);
        if (addr) {
            bluetil_addr_print(addr);
        }
        else {
            printf("n/a");
        }
        puts(")");
    }
}

static void _on_netif_evt(int handle, nimble_netif_event_t event,
                          const uint8_t *addr)
{
    switch (event) {
        case NIMBLE_NETIF_ACCEPTING:
            _evt_dbg("ACCEPTING", handle, addr);
            break;
        case NIMBLE_NETIF_ACCEPT_STOP:
            _evt_dbg("ACCEPT_STOP", handle, addr);
            break;
        case NIMBLE_NETIF_INIT_MASTER:
            _evt_dbg("CONN_INIT master", handle, addr);
            break;
        case NIMBLE_NETIF_INIT_SLAVE:
            _evt_dbg("CONN_INIT slave", handle, addr);
            _state = STATE_CONN;
            break;
        case NIMBLE_NETIF_CONNECTED_MASTER:
            _evt_dbg("CONNECTED master", handle, addr);
            assert(_state == STATE_CONN);
            _state = STATE_IDLE;
            break;
        case NIMBLE_NETIF_CONNECTED_SLAVE:
            _evt_dbg("CONNECTED slave", handle, addr);
            _state = STATE_IDLE;
            break;
        case NIMBLE_NETIF_CLOSED_MASTER:
            _evt_dbg("CLOSED master", handle, addr);
            break;
        case NIMBLE_NETIF_CLOSED_SLAVE:
            _evt_dbg("CLOSED slave", handle, addr);
            break;
        case NIMBLE_NETIF_ABORT_MASTER:
            _evt_dbg("ABORT master", handle, addr);
            assert(_state == STATE_CONN);
            _state = STATE_IDLE;
            break;
        case NIMBLE_NETIF_ABORT_SLAVE:
            _evt_dbg("ABORT slave", handle, addr);
            _deactivate();
            break;
        case NIMBLE_NETIF_CONN_UPDATED:
            _evt_dbg("UPDATED", handle, addr);
            break;
        default:
            /* this should never happen */
            assert(0);
    }

    /* pass events to high-level user if someone registered for them */
    if (_eventcb) {
        _eventcb(handle, event, addr);
    }

    /* if active and if free slots are available, search for new peers */
    _activate();
}

static int _conn_update(nimble_netif_conn_t *conn, int handle, void *arg)
{
    (void)conn;
    nimble_netif_update(handle, (const struct ble_gap_upd_params *)arg);
    return 0;
}

int nimble_autoconn_init(const nimble_autoconn_params_t *params,
                         const uint8_t *ad, size_t adlen)
{
    /* register our event callback */
    nimble_netif_eventcb(_on_netif_evt);
    /* setup state machine timer (we use NimBLEs callouts for this) */
    ble_npl_callout_init(&_state_evt, nimble_port_get_dflt_eventq(),
                         _on_state_change, NULL);
    /* at last, set the given parameters */
    return nimble_autoconn_update(params, ad, adlen);
}

void nimble_autoconn_eventcb(nimble_netif_eventcb_t cb)
{
    _eventcb = cb;
}

int nimble_autoconn_update(const nimble_autoconn_params_t *params,
                           const uint8_t *ad, size_t adlen)
{
    int res;
    (void)res;

    if ((params == NULL) || (adlen > sizeof(_ad_buf)) ||
        ((adlen > 0) && (ad == NULL))) {
        return NIMBLE_AUTOCONN_PARAMERR;
    }

    /* scan and advertising period configuration */
    ble_npl_time_ms_to_ticks(params->period_adv_ms, &_timeout_adv_period);
    ble_npl_time_ms_to_ticks(params->period_scan_ms, &_timeout_scan_period);
    ble_npl_time_ms_to_ticks(params->period_jitter_ms, &_period_jitter);

    /* populate the connection parameters */
    memset(&_conn_params, 0, sizeof(_conn_params));
    _conn_params.scan_itvl_ms = params->scan_itvl_ms;
    _conn_params.scan_window_ms = params->scan_win_ms;
    _conn_params.conn_itvl_min_ms = params->conn_itvl_min_ms;
    _conn_params.conn_itvl_max_ms = params->conn_itvl_max_ms;
    _conn_params.conn_supervision_timeout_ms = params->conn_super_to_ms;
    _conn_params.conn_slave_latency = params->conn_latency;
    _conn_params.timeout_ms = params->conn_timeout_ms;
#if IS_USED(MODULE_NIMBLE_AUTOCONN_EXT)
    _conn_params.phy_mode = params->phy_mode;
#else
    _conn_params.phy_mode = NIMBLE_PHY_1M;
#endif
    _conn_params.own_addr_type = nimble_riot_own_addr_type;

    /* we use the same values to updated existing connections */
    struct ble_gap_upd_params conn_update_params;
    conn_update_params.itvl_min = BLE_GAP_CONN_ITVL_MS(params->conn_itvl_min_ms);
    conn_update_params.itvl_max = BLE_GAP_CONN_ITVL_MS(params->conn_itvl_max_ms);
    conn_update_params.latency = params->conn_latency;
    conn_update_params.supervision_timeout =
                        BLE_GAP_SUPERVISION_TIMEOUT_MS(params->conn_super_to_ms);
    conn_update_params.min_ce_len = 0;
    conn_update_params.max_ce_len = 0;

    /* calculate the used scan parameters */
    nimble_scanner_cfg_t scan_params;
    scan_params.itvl_ms = params->scan_itvl_ms;
    scan_params.win_ms = params->scan_win_ms;
    scan_params.flags = (NIMBLE_SCANNER_PASSIVE | NIMBLE_SCANNER_FILTER_DUPS);
#if IS_USED(MODULE_NIMBLE_AUTOCONN_EXT) && IS_USED(MODULE_NIMBLE_PHY_CODED)
    if (params->phy_mode == NIMBLE_PHY_CODED) {
        scan_params.flags |= NIMBLE_SCANNER_PHY_CODED;
    }
    else {
        scan_params.flags |= NIMBLE_SCANNER_PHY_1M;
    }
#else
    scan_params.flags |= NIMBLE_SCANNER_PHY_1M;
#endif

    /* set the advertising parameters used */
    memset(&_accept_params, 0, sizeof(_accept_params));
#if IS_USED(MODULE_NIMBLE_AUTOCONN_EXT)
    _accept_params.flags = 0;
    _accept_params.primary_phy = params->phy_mode;
    _accept_params.secondary_phy = params->phy_mode;
#else
    _accept_params.flags = NIMBLE_NETIF_FLAG_LEGACY;
    _accept_params.primary_phy = NIMBLE_PHY_1M;
    _accept_params.secondary_phy = NIMBLE_PHY_1M;
#endif
    _accept_params.adv_itvl_ms = params->adv_itvl_ms;
    _accept_params.timeout_ms = BLE_HS_FOREVER;
    _accept_params.own_addr_type = nimble_riot_own_addr_type;

    /* initialize the advertising data that will be used */
    if (adlen > 0) {
        memcpy(_ad_buf, ad, adlen);
        bluetil_ad_init(&_ad, _ad_buf, adlen, sizeof(_ad_buf));
    }
    else {
        uint16_t svc = SVC_FILTER;
        bluetil_ad_init_with_flags(&_ad, _ad_buf, sizeof(_ad_buf),
                                   BLUETIL_AD_FLAGS_DEFAULT);
        bluetil_ad_add(&_ad, BLE_GAP_AD_UUID16_INCOMP, &svc, sizeof(svc));
        if (params->node_id) {
            res = bluetil_ad_add(&_ad, BLE_GAP_AD_NAME,
                                 params->node_id, strlen(params->node_id));
            if (res != BLUETIL_AD_OK) {
                return NIMBLE_AUTOCONN_ADERR;
            }
        }
    }

    /* initialize scanner with default parameters */
    nimble_scanner_init(&scan_params, _on_scan_evt);

    /* we also need to apply the new connection parameters to all BLE
     * connections where we are in the MASTER role */
    nimble_netif_conn_foreach(NIMBLE_NETIF_GAP_MASTER, _conn_update,
                              &conn_update_params);

    return NIMBLE_AUTOCONN_OK;
}

void nimble_autoconn_enable(void)
{
    _enabled = 1;
    _activate();
}

void nimble_autoconn_disable(void)
{
    _enabled = 0;
    _deactivate();
}
