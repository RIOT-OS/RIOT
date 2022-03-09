/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       (Mock-up) BLE heart rate sensor example
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "assert.h"
#include "event/timeout.h"
#include "nimble_riot.h"
#include "nimble_autoadv.h"
#include "net/bluetil/ad.h"
#include "timex.h"

#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define HRS_FLAGS_DEFAULT       (0x01)      /* 16-bit BPM value */
#define SENSOR_LOCATION         (0x02)      /* wrist sensor */
#define UPDATE_INTERVAL         (250U)
#define BPM_MIN                 (80U)
#define BPM_MAX                 (210U)
#define BPM_STEP                (2)
#define BAT_LEVEL               (42U)

static const char *_manufacturer_name = "Unfit Byte Inc.";
static const char *_model_number = "2A";
static const char *_serial_number = "a8b302c7f3-29183-x8";
static const char *_fw_ver = "13.7.12";
static const char *_hw_ver = "V3B";

static struct __attribute__((packed)) {
    uint8_t flags;
    uint16_t bpm;
} _hr_data = { HRS_FLAGS_DEFAULT, (BPM_MIN + BPM_STEP) };

static event_queue_t _eq;
static event_t _update_evt;
static event_timeout_t _update_timeout_evt;

static uint16_t _conn_handle;
static uint16_t _hrs_val_handle;

static int step = BPM_STEP;

static int _hrs_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg);

static int _devinfo_handler(uint16_t conn_handle, uint16_t attr_handle,
                            struct ble_gatt_access_ctxt *ctxt, void *arg);

static int _bas_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg);

static void _start_updating(void);
static void _stop_updating(void);

/* GATT service definitions */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /* Heart Rate Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_GATT_SVC_HRS),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_HEART_RATE_MEASURE),
            .access_cb = _hrs_handler,
            .val_handle = &_hrs_val_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_BODY_SENSE_LOC),
            .access_cb = _hrs_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* no more characteristics in this service */
        }, }
    },
    {
        /* Device Information Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_GATT_SVC_DEVINFO),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_MANUFACTURER_NAME),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_MODEL_NUMBER_STR),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_SERIAL_NUMBER_STR),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_FW_REV_STR),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_HW_REV_STR),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* no more characteristics in this service */
        }, }
    },
    {
        /* Battery Level Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_GATT_SVC_BAS),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_BATTERY_LEVEL),
            .access_cb = _bas_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* no more characteristics in this service */
        }, }
    },
    {
        0, /* no more services */
    },
};

static int _hrs_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    if (ble_uuid_u16(ctxt->chr->uuid) != BLE_GATT_CHAR_BODY_SENSE_LOC) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    puts("[READ] heart rate service: body sensor location value");

    uint8_t loc = SENSOR_LOCATION;
    int res = os_mbuf_append(ctxt->om, &loc, sizeof(loc));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static int _devinfo_handler(uint16_t conn_handle, uint16_t attr_handle,
                            struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    const char *str;

    switch (ble_uuid_u16(ctxt->chr->uuid)) {
        case BLE_GATT_CHAR_MANUFACTURER_NAME:
            puts("[READ] device information service: manufacturer name value");
            str = _manufacturer_name;
            break;
        case BLE_GATT_CHAR_MODEL_NUMBER_STR:
            puts("[READ] device information service: model number value");
            str = _model_number;
            break;
        case BLE_GATT_CHAR_SERIAL_NUMBER_STR:
            puts("[READ] device information service: serial number value");
            str = _serial_number;
            break;
        case BLE_GATT_CHAR_FW_REV_STR:
            puts("[READ] device information service: firmware revision value");
            str = _fw_ver;
            break;
        case BLE_GATT_CHAR_HW_REV_STR:
            puts("[READ] device information service: hardware revision value");
            str = _hw_ver;
            break;
        default:
            return BLE_ATT_ERR_UNLIKELY;
    }

    int res = os_mbuf_append(ctxt->om, str, strlen(str));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static int _bas_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    puts("[READ] battery level service: battery level value");

    uint8_t level = BAT_LEVEL;  /* this battery will never drain :-) */
    int res = os_mbuf_append(ctxt->om, &level, sizeof(level));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static int gap_event_cb(struct ble_gap_event *event, void *arg)
{
    (void)arg;

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status) {
                _stop_updating();
                nimble_autoadv_start();
                return 0;
            }
            _conn_handle = event->connect.conn_handle;
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            _stop_updating();
            nimble_autoadv_start();
            break;

        case BLE_GAP_EVENT_SUBSCRIBE:
            if (event->subscribe.attr_handle == _hrs_val_handle) {
                if (event->subscribe.cur_notify == 1) {
                    _start_updating();
                }
                else {
                    _stop_updating();
                }
            }
            break;
    }

    return 0;
}

static void _start_updating(void)
{
    event_timeout_set(&_update_timeout_evt, UPDATE_INTERVAL);
    puts("[NOTIFY_ENABLED] heart rate service");
}

static void _stop_updating(void)
{
    event_timeout_clear(&_update_timeout_evt);
    puts("[NOTIFY_DISABLED] heart rate service");
}

static void _hr_update(event_t *e)
{
    (void)e;
    struct os_mbuf *om;

    /* our mock-up heart rate is going up and down */
    if ((_hr_data.bpm == BPM_MIN) || (_hr_data.bpm == BPM_MAX)) {
        step *= -1;
    }
    _hr_data.bpm += step;

    printf("[NOTIFY] heart rate service: measurement %i\n", (int)_hr_data.bpm);

    /* send heart rate data notification to GATT client */
    om = ble_hs_mbuf_from_flat(&_hr_data, sizeof(_hr_data));
    assert(om != NULL);
    int res = ble_gattc_notify_custom(_conn_handle, _hrs_val_handle, om);
    assert(res == 0);
    (void)res;

    /* schedule next update event */
    event_timeout_set(&_update_timeout_evt, UPDATE_INTERVAL);
}

int main(void)
{
    puts("NimBLE Heart Rate Sensor Example");

    int res = 0;
    (void)res;

    /* setup local event queue (for handling heart rate updates) */
    event_queue_init(&_eq);
    _update_evt.handler = _hr_update;
    event_timeout_ztimer_init(&_update_timeout_evt, ZTIMER_MSEC, &_eq, &_update_evt);

    /* verify and add our custom services */
    res = ble_gatts_count_cfg(gatt_svr_svcs);
    assert(res == 0);
    res = ble_gatts_add_svcs(gatt_svr_svcs);
    assert(res == 0);

    /* set the device name */
    ble_svc_gap_device_name_set(NIMBLE_AUTOADV_DEVICE_NAME);
    /* reload the GATT server to link our added services */
    ble_gatts_start();

    struct ble_gap_adv_params advp;
    memset(&advp, 0, sizeof(advp));

    advp.conn_mode = BLE_GAP_CONN_MODE_UND;
    advp.disc_mode = BLE_GAP_DISC_MODE_GEN;
    advp.itvl_min  = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    advp.itvl_max  = BLE_GAP_ADV_FAST_INTERVAL1_MAX;

    /* set advertise params */
    nimble_autoadv_set_ble_gap_adv_params(&advp);

    /* configure and set the advertising data */
    uint16_t hrs_uuid = BLE_GATT_SVC_HRS;
    nimble_autoadv_add_field(BLE_GAP_AD_UUID16_INCOMP, &hrs_uuid, sizeof(hrs_uuid));

    nimble_auto_adv_set_gap_cb(&gap_event_cb, NULL);

    /* start to advertise this node */
    nimble_autoadv_start();

    /* run an event loop for handling the heart rate update events */
    event_loop(&_eq);

    return 0;
}
