/*
 * Copyright (C) 2019-2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_rpble
 * @{
 *
 * @file
 * @brief       RPL-over-BLE (rpble) implementation for NimBLE and GNRC
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <limits.h>
#include <errno.h>

#include "random.h"
#include "net/bluetil/ad.h"
#include "net/bluetil/addr.h"
#include "net/gnrc/rpl.h"

#include "nimble_netif.h"
#include "nimble_netif_conn.h"
#include "nimble_rpble.h"
#include "nimble_scanner.h"
#include "host/ble_gap.h"
#include "nimble/nimble_port.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/* parent state */
#define PARENT_NONE         -1
#define PARENT_ROOT         -2
#define SCORE_NONE          0U

/* RPL meta data context fields */
#define AD_SVC_DATA_LEN     23
#define POS_INST_ID         2
#define POS_DODAG_ID        3
#define POS_VERSION         19
#define POS_RANK            20
#define POS_FREE_SLOTS      22

/* keep the timing parameters for connections and advertisements */
static nimble_netif_accept_cfg_t _accept_params;
static nimble_netif_connect_cfg_t _conn_params;

/* local RPL context */
static nimble_rpble_ctx_t _local_rpl_ctx;
static int _current_parent = PARENT_NONE;
struct {
    ble_addr_t addr;        /**< address of highest scored potential parent */
    uint16_t score;         /* 0 := not used, larger is better! */
} _psel = { { 0 }, SCORE_NONE };

/* eval event used for periodical state updates */
static uint32_t _eval_itvl;
static struct ble_npl_callout _evt_eval;

static nimble_netif_eventcb_t _eventcb = NULL;

static uint16_t _psel_score(uint16_t rank, uint8_t free)
{
    return (UINT16_MAX - rank + free);
}

static void _children_accept(void)
{
    int res;
    (void)res;

    /* stop advertising while we are updating the context */
    nimble_netif_accept_stop();

    /* we only start advertising (accepting) if we do have an active parent and
     * if we have resources for new connections */
    if ((_current_parent == PARENT_NONE) ||
        (nimble_netif_conn_count(NIMBLE_NETIF_UNUSED) == 0)) {
        return;
    }

    /* compile RPL meta data into AD service data TLV field */
    uint8_t sd[AD_SVC_DATA_LEN];
    byteorder_htolebufs(sd, BLE_GATT_SVC_IPSS);
    sd[POS_INST_ID] = _local_rpl_ctx.inst_id;
    memcpy(&sd[POS_DODAG_ID], _local_rpl_ctx.dodag_id, 16);
    sd[POS_VERSION] = _local_rpl_ctx.version;
    byteorder_htolebufs(&sd[POS_RANK], _local_rpl_ctx.rank);
    sd[POS_FREE_SLOTS] = (uint8_t)nimble_netif_conn_count(NIMBLE_NETIF_UNUSED);

    /* generate the new advertising data */
    bluetil_ad_t ad;
    uint8_t buf[BLE_HS_ADV_MAX_SZ];
    res = bluetil_ad_init_with_flags(&ad, buf, BLE_HS_ADV_MAX_SZ,
                                     BLUETIL_AD_FLAGS_DEFAULT);
    assert(res == BLUETIL_AD_OK);
    res = bluetil_ad_add(&ad, BLE_GAP_AD_SERVICE_DATA, sd, sizeof(sd));
    assert(res == BLUETIL_AD_OK);

    /* start advertising this node */
    res = nimble_netif_accept(ad.buf, ad.pos, &_accept_params);
    assert(res == 0);
}

static void _on_scan_evt(uint8_t type, const ble_addr_t *addr,
                         const nimble_scanner_info_t *info,
                         const uint8_t *ad, size_t ad_len)
{
    int res;
    (void)info;

#if IS_USED(MODULE_NIMBLE_RPBLE_EXT)
    if ((type != (NIMBLE_SCANNER_EXT_ADV | BLE_HCI_ADV_CONN_MASK)) ||
        (info->status != BLE_GAP_EXT_ADV_DATA_STATUS_COMPLETE)) {
        return;
    }
#else
    /* filter out all non-connectible advertisements */
    if (type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND) {
        return;
    }
#endif

    /* check if scanned node does actually speak rpble */
    bluetil_ad_data_t sd_field;
    bluetil_ad_t ads = { .buf = (uint8_t *)ad, .pos = ad_len, .size = ad_len };
    res = bluetil_ad_find(&ads, BLE_GAP_AD_SERVICE_DATA, &sd_field);
    if (res != BLUETIL_AD_OK) {
        return;
    }
    if ((sd_field.len != AD_SVC_DATA_LEN) ||
        (byteorder_lebuftohs(sd_field.data) != BLE_GATT_SVC_IPSS)) {
        return;
    }

    /**
     * @note    Here we need to improve the filtering: so far, we consider every
     *          node we see that is capable of rplbe to be a parent. We should
     *          however also filter for instance ID and possibly the DODAG ID as
     *          well. On top, we should probably only consider nodes with >=
     *          version as parent
     */

    /* score and compare advertising peer */
    uint16_t rank = byteorder_lebuftohs(&sd_field.data[POS_RANK]);
    uint8_t free = sd_field.data[POS_FREE_SLOTS];
    uint16_t score = _psel_score(rank, free);

    /* our currently preferred parent might have updated its score in the mean
     * time, so we need to check that */
    if (memcmp(&_psel.addr, addr, sizeof(ble_addr_t)) == 0) {
        _psel.score = score;
        return;
    }

    /* we consider only parents with a lower rank and remember the one with the
     * best score */
    if (((_local_rpl_ctx.rank == 0) || (_local_rpl_ctx.rank > rank)) &&
        (score > _psel.score)) {
        _psel.score = score;
        memcpy(&_psel.addr, addr, sizeof(ble_addr_t));
    }
}

static void _parent_find(void)
{
    _psel.score = SCORE_NONE;
    _current_parent = PARENT_NONE;
    nimble_scanner_start();
    ble_npl_callout_reset(&_evt_eval, _eval_itvl);
}

static void _parent_find_stop(void)
{
    ble_npl_callout_stop(&_evt_eval);
    nimble_scanner_stop();
}

static void _parent_connect(struct ble_npl_event *ev)
{
    (void)ev;

    /* for now, we only try to connect to a parent if we have none */
    assert(_current_parent == PARENT_NONE);
    /* just in case this event is triggered while we were configured to be the
     * RPL root */
    if (_local_rpl_ctx.role == GNRC_RPL_ROOT_NODE) {
        return;
    }

    /* reset timer and stop scanner  */
    _parent_find_stop();

    if (_psel.score == SCORE_NONE) {
        /* no potential parent found, restarting search for one */
        _parent_find();
        return;
    }

    /* try to connect to parent */
    int res = nimble_netif_connect(&_psel.addr, &_conn_params);
    if (res < 0) {
        _parent_find();
        return;
    }
    _current_parent = res;
}

static void _parent_lost(void)
{
    nimble_netif_accept_stop();

    if (_local_rpl_ctx.rank != 0) {
        gnrc_rpl_addr_unreachable(&_local_rpl_ctx.parent_addr);
    }

    /* back to 0, now we need to find a new parent... */
    _parent_find();
}

static void _on_netif_evt(int handle, nimble_netif_event_t event,
                          const uint8_t *addr)
{
    (void)addr;

    switch (event) {
        case NIMBLE_NETIF_CONNECTED_MASTER:
            /* parent selected */
            assert(_current_parent == handle);
            /* send a DIS once connected to a (new) parent) */
            gnrc_rpl_send_DIS(NULL, (ipv6_addr_t *) &ipv6_addr_all_rpl_nodes,
                              NULL, 0);
            break;
        case NIMBLE_NETIF_CONNECTED_SLAVE:
            /* child added */
            _children_accept();
            break;
        case NIMBLE_NETIF_CLOSED_MASTER:
            /* parent lost */
            _parent_lost();
        case NIMBLE_NETIF_CLOSED_SLAVE:
            /* child lost */
            _children_accept();
            break;
        case NIMBLE_NETIF_ABORT_MASTER:
            /* parent selection aborted */
            _parent_lost();
            break;
        case NIMBLE_NETIF_ABORT_SLAVE:
            /* child selection aborted */
            _children_accept();
            break;
        default:
            /* nothing to do for all other events */
            break;
    }

    /* pass events to high-level user if activated */
    if (_eventcb) {
        _eventcb(handle, event, addr);
    }
}

int nimble_rpble_init(const nimble_rpble_cfg_t *cfg)
{
    /* initialize state */
    memset(&_local_rpl_ctx, 0, sizeof(_local_rpl_ctx));

    /* initialize the eval event */
    ble_npl_callout_init(&_evt_eval, nimble_port_get_dflt_eventq(),
                         _parent_connect, NULL);

    return nimble_rpble_param_update(cfg);
}

int nimble_rpble_param_update(const nimble_rpble_cfg_t *cfg)
{
    assert(cfg);

    if (_current_parent == PARENT_NONE) {
        _parent_find_stop();
    }
    else {
        nimble_netif_accept_stop();
    }

    uint32_t itvl = random_uint32_range(cfg->eval_itvl_min_ms,
                                        cfg->eval_itvl_max_ms);
    ble_npl_time_ms_to_ticks(itvl, &_eval_itvl);

    /* accept parameter extraction */
    memset(&_accept_params, 0, sizeof(_accept_params));
#if IS_USED(MODULE_NIMBLE_RPBLE_EXT)
    _accept_params.flags = 0;
    _accept_params.primary_phy = cfg->phy_mode;
    _accept_params.secondary_phy = cfg->phy_mode;
#else
    _accept_params.flags = NIMBLE_NETIF_FLAG_LEGACY;
    _accept_params.primary_phy = NIMBLE_PHY_1M;
    _accept_params.secondary_phy = NIMBLE_PHY_1M;
#endif
    _accept_params.adv_itvl_ms = cfg->adv_itvl_ms;
    _accept_params.timeout_ms = BLE_HS_FOREVER;
    _accept_params.own_addr_type = nimble_riot_own_addr_type;

    /* connection parameter extraction */
    memset(&_conn_params, 0, sizeof(_conn_params));
    _conn_params.scan_itvl_ms = cfg->conn_scan_itvl_ms;
    _conn_params.scan_window_ms = cfg->conn_scan_win_ms;
    _conn_params.conn_itvl_min_ms = cfg->conn_itvl_min_ms;
    _conn_params.conn_itvl_max_ms = cfg->conn_itvl_max_ms;
    _conn_params.conn_supervision_timeout_ms = cfg->conn_super_to_ms;
    _conn_params.conn_slave_latency = cfg->conn_latency;
    _conn_params.timeout_ms = cfg->conn_scan_to_ms;
#if IS_USED(MODULE_NIMBLE_RPBLE_EXT)
    _conn_params.phy_mode = cfg->phy_mode;
#else
    _conn_params.phy_mode = NIMBLE_PHY_1M;
#endif
    _conn_params.own_addr_type = nimble_riot_own_addr_type;

    /* register event callback */
    nimble_netif_eventcb(_on_netif_evt);

    /* configure scanner */
    nimble_scanner_cfg_t scan_params = { 0 };
    scan_params.itvl_ms = cfg->scan_itvl_ms;
    scan_params.win_ms = cfg->scan_win_ms;
    scan_params.flags = (NIMBLE_SCANNER_PASSIVE | NIMBLE_SCANNER_FILTER_DUPS);

#if IS_USED(MODULE_NIMBLE_RPBLE_EXT) && IS_USED(MODULE_NIMBLE_PHY_CODED)
    if (cfg->phy_mode == NIMBLE_PHY_CODED) {
        scan_params.flags |= NIMBLE_SCANNER_PHY_CODED;
    }
    else {
        scan_params.flags |= NIMBLE_SCANNER_PHY_1M;
    }
#else
    scan_params.flags |= NIMBLE_SCANNER_PHY_1M;
#endif
    nimble_scanner_init(&scan_params, _on_scan_evt);

    /* start to look for parents */
    if (_current_parent == PARENT_NONE) {
        _parent_find();
    } else {
        _children_accept();
    }

    return 0;
}

int nimble_rpble_eventcb(nimble_netif_eventcb_t cb)
{
    _eventcb = cb;
    return 0;
}

int nimble_rpble_update(const nimble_rpble_ctx_t *ctx)
{
    assert(ctx != NULL);
    int ret = 0;

    /* if the update context is equal to what we have, ignore it */
    if (memcmp(&_local_rpl_ctx, ctx, sizeof(nimble_rpble_ctx_t)) == 0) {
        ret = -EALREADY;
    }
    else {
        /* save rpl context for future reference */
        memcpy(&_local_rpl_ctx, ctx, sizeof(nimble_rpble_ctx_t));
        if (ctx->role == GNRC_RPL_ROOT_NODE) {
            _current_parent = PARENT_ROOT;
            _parent_find_stop();
        }
    }

    /* advertise the updated context */
    _children_accept();
    return ret;
}
