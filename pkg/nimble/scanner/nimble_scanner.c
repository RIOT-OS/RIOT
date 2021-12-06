/*
 * Copyright (C) 2019-2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_scanner
 * @{
 *
 * @file
 * @brief       Implementation of a scanner abstraction for NimBLE
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "nimble_riot.h"
#include "nimble_scanner.h"

#include "host/ble_gap.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static nimble_scanner_cb _disc_cb = NULL;

#if MYNEWT_VAL_BLE_EXT_ADV
static struct ble_gap_ext_disc_params _scan_params = { 0 };
static uint8_t _scan_flags = 0;
#else
static struct ble_gap_disc_params _scan_params = { 0 };
#endif

/* duration of the scanning procedure */
static int32_t _scan_duration = BLE_HS_FOREVER;

static int _on_scan_evt(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
#if MYNEWT_VAL_BLE_EXT_ADV
    case BLE_GAP_EVENT_EXT_DISC: {
        uint8_t type;
        nimble_scanner_info_t info;
        info.rssi = event->ext_disc.rssi;

        if (event->ext_disc.props & BLE_HCI_ADV_LEGACY_MASK) {
            type = event->ext_disc.legacy_event_type;
            info.status = NIMBLE_SCANNER_COMPLETE;
            info.phy_pri = BLE_HCI_LE_PHY_1M;
            info.phy_sec = BLE_HCI_LE_PHY_1M;
        }
        else {
            type = (event->ext_disc.props | NIMBLE_SCANNER_EXT_ADV);
            info.status = event->ext_disc.data_status;
            info.phy_pri = event->ext_disc.prim_phy;
            info.phy_sec = event->ext_disc.sec_phy;
        }

        _disc_cb(type, &event->ext_disc.addr, &info,
                 event->ext_disc.data, event->ext_disc.length_data);
        break;
    }
#else
    case BLE_GAP_EVENT_DISC: {
        nimble_scanner_info_t info = {
            .status = NIMBLE_SCANNER_COMPLETE,
            .phy_pri = BLE_HCI_LE_PHY_1M,
            .phy_sec = BLE_HCI_LE_PHY_1M,
            .rssi = event->disc.rssi,
        };
        _disc_cb(event->disc.event_type, &event->disc.addr, &info,
                 event->disc.data, (size_t)event->disc.length_data);
        break;
    }
#endif
    case BLE_GAP_EVENT_DISC_COMPLETE:
        DEBUG("[scanner] scan cycle completed\n");
        break;
    default:
        /* this should never happen */
        DEBUG("[scanner] unknown event triggered (%i)\n", (int)event->type);
        assert(0);
    }

    return 0;
}

int nimble_scanner_start(void)
{
    if (ble_gap_disc_active() == 0) {
#if MYNEWT_VAL_BLE_EXT_ADV
        uint8_t dups = (_scan_flags & NIMBLE_SCANNER_FILTER_DUPS) ? 1 : 0;
        uint8_t limited = (_scan_flags & NIMBLE_SCANNER_LIMITED) ? 1 : 0;
        const struct ble_gap_ext_disc_params *uncoded =
            (_scan_flags & NIMBLE_SCANNER_PHY_1M) ? &_scan_params : NULL;
        const struct ble_gap_ext_disc_params *coded =
            (_scan_flags & NIMBLE_SCANNER_PHY_CODED) ? &_scan_params : NULL;

        int32_t dur = (_scan_duration == BLE_HS_FOREVER) ? 0
                                                         : _scan_duration / 10;

        int res = ble_gap_ext_disc(nimble_riot_own_addr_type,
                                   dur, 0, dups,
                                   BLE_HCI_SCAN_FILT_NO_WL, limited,
                                   uncoded, coded,
                                   _on_scan_evt, NULL);
#else
        int res = ble_gap_disc(nimble_riot_own_addr_type, _scan_duration,
                               &_scan_params, _on_scan_evt, NULL);
#endif
        if (res != 0) {
            DEBUG("[scanner] err: start failed (%i)\n", res);
            return -ECANCELED;
        }
    }

    return 0;
}

void nimble_scanner_stop(void)
{
    if (ble_gap_disc_active() == 1) {
        int res = ble_gap_disc_cancel();
        /* the above should always succeed */
        assert(res == 0);
        (void)res;
    }
}

void nimble_scanner_set_scan_duration(int32_t duration_ms)
{
    _scan_duration = duration_ms;
    if (ble_gap_disc_active()) {
        nimble_scanner_stop();
        nimble_scanner_start();
    }
}

int nimble_scanner_init(const nimble_scanner_cfg_t *params,
                        nimble_scanner_cb disc_cb)
{
    assert(params);
    assert(disc_cb);

    _scan_params.itvl = BLE_GAP_SCAN_ITVL_MS(params->itvl_ms);
    _scan_params.window = BLE_GAP_SCAN_WIN_MS(params->win_ms);
    _scan_params.passive = (params->flags & NIMBLE_SCANNER_PASSIVE) ? 1 : 0;
#if MYNEWT_VAL_BLE_EXT_ADV
    _scan_flags = params->flags;
#else
    _scan_params.limited = (params->flags & NIMBLE_SCANNER_LIMITED) ? 1 : 0;
    _scan_params.filter_duplicates =
                           (params->flags & NIMBLE_SCANNER_FILTER_DUPS) ? 1 : 0;
#endif

    _disc_cb = disc_cb;

    return 0;
}
