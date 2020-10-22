/*
 * Copyright (C) 2019 Freie Universität Berlin
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
static struct ble_gap_disc_params _scan_params = { 0 };

static int _on_scan_evt(struct ble_gap_event *event, void *arg)
{
    /* only interested in the DISC event */
    if (event->type == BLE_GAP_EVENT_DISC) {
        _disc_cb(event->disc.event_type, &event->disc.addr, event->disc.rssi,
                 event->disc.data, (size_t)event->disc.length_data);
    }
    else {
        /* this should never happen */
        DEBUG("[scanner] unknown event triggered (%i)\n", (int)event->type);
        assert(0);
    }

    return 0;
}

int nimble_scanner_start(void)
{
    if (ble_gap_disc_active() == 0) {
        int res = ble_gap_disc(nimble_riot_own_addr_type, BLE_HS_FOREVER,
                               &_scan_params, _on_scan_evt, NULL);
        if (res != 0) {
            DEBUG("[scanner] err: start failed (%i)\n", res);
            return NIMBLE_SCANNER_ERR;
        }
    }

    return NIMBLE_SCANNER_OK;
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

int nimble_scanner_status(void)
{
    return (ble_gap_disc_active())
        ? NIMBLE_SCANNER_SCANNING
        : NIMBLE_SCANNER_STOPPED;
}

int nimble_scanner_init(const struct ble_gap_disc_params *params,
                        nimble_scanner_cb disc_cb)
{
    assert(disc_cb);

    if (params) {
        memcpy(&_scan_params, params, sizeof(_scan_params));
    }
    else {
        memset(&_scan_params, 0, sizeof(_scan_params));
    }
    _disc_cb = disc_cb;

    return NIMBLE_SCANNER_OK;
}
