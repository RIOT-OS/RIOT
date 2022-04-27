/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_autoadv
 *
 * @{
 *
 * @file
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdlib.h>

#include "nimble_riot.h"

#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "net/bluetil/ad.h"

#include "nimble_autoadv.h"

#ifndef CONFIG_NIMBLE_AUTOADV_INSTANCE
#define CONFIG_NIMBLE_AUTOADV_INSTANCE            0
#endif

/* settings for advertising procedure */
static nimble_autoadv_cfg_t _cfg;

/* buffer for _ad */
static uint8_t buf[BLE_HS_ADV_MAX_SZ];
/* advertising data struct */
static bluetil_ad_t _ad;

/* GAP callback function */
static ble_gap_event_fn *_gap_cb;
/* arguments for GAP callback function */
static void *_gap_cb_arg;

static int _gap_event_cb(struct ble_gap_event *event, void *arg)
{
    (void)arg;

    switch (event->type) {

    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status != 0) {
            /* failed, ensure advertising is restarted */
            nimble_autoadv_start(NULL);
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        nimble_autoadv_start(NULL);
        break;
    }

    return 0;
}

void nimble_autoadv_init(const nimble_autoadv_cfg_t *cfg)
{
    nimble_autoadv_reset((nimble_autoadv_cfg_t *)cfg);

    if (!CONFIG_NIMBLE_AUTOADV_START_MANUALLY) {
        nimble_autoadv_start(NULL);
    }
}

static int _ble_gap_adv_active(void)
{
#if MYNEWT_VAL_BLE_EXT_ADV
    return ble_gap_ext_adv_active(CONFIG_NIMBLE_AUTOADV_INSTANCE);
#else
    return ble_gap_adv_active();
#endif
}

int nimble_autoadv_add_field(uint8_t type, const void *data, size_t data_len)
{
    int rc = bluetil_ad_add(&_ad, type, data, data_len);

    if (rc != BLUETIL_AD_OK) {
        return rc;
    }

    if (_ble_gap_adv_active()) {
        nimble_autoadv_start(NULL);
    }

    return rc;
}

void nimble_autoadv_cfg_update(nimble_autoadv_cfg_t *cfg)
{
    memcpy(&_cfg, cfg, sizeof(nimble_autoadv_cfg_t));

    if (cfg->own_addr_type == 0xff) {
        _cfg.own_addr_type = nimble_riot_own_addr_type;
    }

    if (_ble_gap_adv_active()) {
        nimble_autoadv_start(NULL);
    }
}

void nimble_autoadv_get_cfg(nimble_autoadv_cfg_t *cfg)
{
    memcpy(cfg, &_cfg, sizeof(nimble_autoadv_cfg_t));
}

void nimble_autoadv_set_gap_cb(ble_gap_event_fn *cb, void *cb_arg)
{
    _gap_cb = cb;
    _gap_cb_arg = cb_arg;

    if (_ble_gap_adv_active()) {
        nimble_autoadv_start(NULL);
    }
}

void nimble_autoadv_start(ble_addr_t *addr)
{
    int rc;

    (void)rc;

    nimble_autoadv_stop();

#if MYNEWT_VAL_BLE_EXT_ADV
    struct ble_gap_ext_adv_params advp = {
        .connectable = (_cfg.flags & NIMBLE_AUTOADV_FLAG_CONNECTABLE) ? 1 : 0,
        .scannable = (_cfg.flags & NIMBLE_AUTOADV_FLAG_SCANNABLE) ? 1 : 0,
        .directed = addr ? 1 : 0,
        .high_duty_directed = (_cfg.flags & NIMBLE_AUTOADV_FLAG_HD_MODE) ? 1 : 0,
        .legacy_pdu = (_cfg.flags & NIMBLE_AUTOADV_FLAG_LEGACY) ? 1 : 0,
        .anonymous = (_cfg.flags & NIMBLE_AUTOADV_FLAG_ANONYMOUS) ? 1 : 0,
        .include_tx_power = 0,
        .scan_req_notif = (_cfg.flags & NIMBLE_AUTOADV_FLAG_SCAN_REQ_NOTIF) ? 1 : 0,
        .itvl_min = BLE_GAP_ADV_ITVL_MS(_cfg.adv_itvl_ms),
        .itvl_max = BLE_GAP_ADV_ITVL_MS(_cfg.adv_itvl_ms),
        .channel_map = _cfg.channel_map,
        .own_addr_type = _cfg.own_addr_type,
        .peer = *addr,
        .filter_policy = _cfg.filter_policy,
        .primary_phy = nimble_riot_get_phy_hci(_cfg.phy),
        .secondary_phy = nimble_riot_get_phy_hci(_cfg.phy),
        .tx_power = _cfg.tx_power,
        .sid = CONFIG_NIMBLE_AUTOADV_INSTANCE,
    };

    /* remove before configure */
    if (ble_gap_ext_adv_active(CONFIG_NIMBLE_AUTOADV_INSTANCE)) {
        rc = ble_gap_ext_adv_remove(CONFIG_NIMBLE_AUTOADV_INSTANCE);
        assert(rc == 0);
    }
    /* configure */
    rc = ble_gap_ext_adv_configure(CONFIG_NIMBLE_AUTOADV_INSTANCE, &advp,
                                   NULL, _gap_event_cb, _gap_cb_arg);
    assert(rc == 0);

    /* get mbuf for adv data, this is freed from the `ble_gap_ext_adv_set_data` */
    struct os_mbuf *data;

    data = os_msys_get_pkthdr(BLE_HS_ADV_MAX_SZ, 0);
    assert(data);

    /* fill mbuf with adv data */
    rc = os_mbuf_append(data, _ad.buf, _ad.pos);
    assert(rc == 0);

    /* set adv data */
    rc = ble_gap_ext_adv_set_data(CONFIG_NIMBLE_AUTOADV_INSTANCE, data);
    assert(rc == 0);

    /* set a single advertisement event, ble_gap_ext_adv units are in 10s of ms */
    int32_t dur = (_cfg.adv_duration_ms == BLE_HS_FOREVER) ? 0
                                                           : _cfg.adv_duration_ms / 10;
    rc = ble_gap_ext_adv_start(CONFIG_NIMBLE_AUTOADV_INSTANCE, dur, 0);
    assert(rc == 0);
#else
    uint8_t mode = BLE_GAP_CONN_MODE_NON;
    if (addr != NULL) {
        mode = BLE_GAP_CONN_MODE_DIR;
    }
    else if (_cfg.flags && NIMBLE_AUTOADV_FLAG_CONNECTABLE) {
        mode = BLE_GAP_CONN_MODE_UND;
    }
    uint8_t disc = (_cfg.flags && NIMBLE_AUTOADV_FLAG_SCANNABLE) ? BLE_GAP_CONN_MODE_DIR
                                                                 : BLE_GAP_CONN_MODE_UND;
    struct ble_gap_adv_params advp = {
        .conn_mode = mode,
        .disc_mode = disc,
        .itvl_min = BLE_GAP_ADV_ITVL_MS(_cfg.adv_itvl_ms),
        .itvl_max = BLE_GAP_ADV_ITVL_MS(_cfg.adv_itvl_ms),
        .channel_map = _cfg.channel_map,
        .filter_policy = _cfg.filter_policy,
        .high_duty_cycle = (_cfg.flags & NIMBLE_AUTOADV_FLAG_HD_MODE) ? 1 : 0,
    };

    rc = ble_gap_adv_set_data(_ad.buf, _ad.pos);
    assert(rc == 0);

    rc = ble_gap_adv_start(_cfg.own_addr_type, addr, _cfg.adv_duration_ms,
                           &advp, _gap_cb, _gap_cb_arg);
    assert(rc == 0);
#endif
}

static int _ble_gap_stop(void)
{
#if MYNEWT_VAL_BLE_EXT_ADV
    if (ble_gap_ext_adv_active(CONFIG_NIMBLE_AUTOADV_INSTANCE)) {
        return ble_gap_ext_adv_stop(CONFIG_NIMBLE_AUTOADV_INSTANCE);
    }
#else
    if (ble_gap_adv_active()) {
        return ble_gap_adv_stop();
    }
#endif
    return 0;
}

void nimble_autoadv_stop(void)
{
    int rc = _ble_gap_stop();

    (void)rc;
    assert(rc == BLE_HS_EALREADY || rc == 0);
}

void nimble_autoadv_reset(nimble_autoadv_cfg_t *cfg)
{
    int rc = 0;

    (void)rc;
    _gap_cb = &_gap_event_cb;
    _gap_cb_arg = NULL;

    nimble_autoadv_cfg_update(cfg);

    if (IS_ACTIVE(CONFIG_NIMBLE_AUTOADV_FLAG_FIELD) && BLUETIL_AD_FLAGS_DEFAULT != 0) {
        rc = bluetil_ad_init_with_flags(&_ad, buf, sizeof(buf), BLUETIL_AD_FLAGS_DEFAULT);
        assert(rc == BLUETIL_AD_OK);
    }
    else {
        bluetil_ad_init(&_ad, buf, 0, sizeof(buf));
    }

    if (CONFIG_NIMBLE_AUTOADV_DEVICE_NAME != NULL) {
        rc = bluetil_ad_add_name(&_ad, CONFIG_NIMBLE_AUTOADV_DEVICE_NAME);
        assert(rc == BLUETIL_AD_OK);
    }

    if (_ble_gap_adv_active()) {
        nimble_autoadv_start(NULL);
    }
}

int nimble_autoadv_get_adv_instance(void)
{
    return CONFIG_NIMBLE_AUTOADV_INSTANCE;
}
