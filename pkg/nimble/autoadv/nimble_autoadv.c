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

/* settings for advertising procedure */
static struct ble_gap_adv_params _advp;

/* duration of the advertisement procedure */
static int32_t _adv_duration;

/* buffer for _ad */
static uint8_t buf[BLE_HS_ADV_MAX_SZ];

/* advertising data struct */
static bluetil_ad_t _ad;

/* GAP callback function */
static ble_gap_event_fn *_gap_cb;

/* arguments for GAP callback function */
static void *_gap_cb_arg;

void nimble_autoadv_start(void);

static int _gap_event_cb(struct ble_gap_event *event, void *arg)
{
    (void) arg;

    switch (event->type) {

        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status != 0) {
                // failed, ensure advertising is restarted
                nimble_autoadv_start();
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            nimble_autoadv_start();
            break;
    }

    return 0;
}

void nimble_autoadv_init(void)
{
    nimble_autoadv_reset();

    if (!NIMBLE_AUTOADV_START_MANUALLY) {
        nimble_autoadv_start();
    }
}

int nimble_autoadv_add_field(uint8_t type, const void *data, size_t data_len)
{
    int rc = bluetil_ad_add(&_ad, type, data, data_len);

    if (rc != BLUETIL_AD_OK) {
        return rc;
    }

    if (ble_gap_adv_active()) {
        nimble_autoadv_start();
    }

    return rc;
}

void nimble_autoadv_set_ble_gap_adv_params(struct ble_gap_adv_params *params)
{
    memcpy(&_advp, params, sizeof(struct ble_gap_adv_params));

    if (ble_gap_adv_active()) {
        nimble_autoadv_start();
    }
}

void nimble_auto_adv_set_adv_duration(int32_t duration_ms)
{
    _adv_duration = duration_ms;

    if (ble_gap_adv_active()) {
        nimble_autoadv_start();
    }
}

void nimble_auto_adv_set_gap_cb(ble_gap_event_fn *cb, void *cb_arg)
{
    _gap_cb = cb;
    _gap_cb_arg = cb_arg;

    if (ble_gap_adv_active()) {
        nimble_autoadv_start();
    }
}

void nimble_autoadv_start(void)
{
    int rc;
    (void) rc;

    rc = ble_gap_adv_stop();
    assert(rc == BLE_HS_EALREADY || rc == 0);

    rc = ble_gap_adv_set_data(_ad.buf, _ad.pos);
    assert(rc == 0);

    rc = ble_gap_adv_start(nimble_riot_own_addr_type, NULL, _adv_duration, &_advp, _gap_cb, _gap_cb_arg);
    assert(rc == 0);
}

void nimble_autoadv_stop(void)
{
    int rc;
    (void) rc;

    rc = ble_gap_adv_stop();
    assert(rc == BLE_HS_EALREADY || rc == 0);
}

void nimble_autoadv_reset(void)
{
    _gap_cb = &_gap_event_cb;
    _gap_cb_arg = NULL;

    _adv_duration = BLE_HS_FOREVER;

    memset(&_advp, 0, sizeof _advp);
    _advp.conn_mode = BLE_GAP_CONN_MODE_UND;
    _advp.disc_mode = BLE_GAP_DISC_MODE_GEN;

    int rc = 0;
    (void) rc;

    if (IS_ACTIVE(NIMBLE_AUTOADV_FLAG_FIELD) && BLUETIL_AD_FLAGS_DEFAULT != 0) {
        rc = bluetil_ad_init_with_flags(&_ad, buf, sizeof(buf),
                                        BLUETIL_AD_FLAGS_DEFAULT);
        assert(rc == BLUETIL_AD_OK);
    }
    else {
        bluetil_ad_init(&_ad, buf, 0, sizeof(buf));
    }

    if (NIMBLE_AUTOADV_DEVICE_NAME != NULL) {
        rc = bluetil_ad_add_name(&_ad, NIMBLE_AUTOADV_DEVICE_NAME);
        assert(rc == BLUETIL_AD_OK);
    }

    if (ble_gap_adv_active()) {
        nimble_autoadv_start();
    }
}
