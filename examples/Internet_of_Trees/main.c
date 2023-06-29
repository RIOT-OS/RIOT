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
#include <stdlib.h>
#include <string.h>

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

#include "bmx280.h"
#include "bmx280_params.h"

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

// static struct __attribute__((packed)) {
//     uint8_t flags;
//     uint16_t bpm;
// } _hr_data = { HRS_FLAGS_DEFAULT, 10 };

static event_queue_t _eq;
static event_t _update_evt;
static event_timeout_t _update_timeout_evt;

static bmx280_t dev;

static uint16_t _conn_handle;
static uint16_t _hrs_val_handle;

char sensor_measurements[100];

static int _hrs_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg);

static int _devinfo_handler(uint16_t conn_handle, uint16_t attr_handle,
                            struct ble_gatt_access_ctxt *ctxt, void *arg);

static int _bas_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg);

static int send_latest_measurements(
    uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    int rc = os_mbuf_append(ctxt->om, sensor_measurements, sizeof(sensor_measurements));

    return rc;
}

static void _start_updating(void);
static void _stop_updating(void);

/* UUID = ccdd113f-40d5-4d68-86ac-a728dd82f4ab */
static const ble_uuid128_t latest_mesurement_uuid
        = BLE_UUID128_INIT(0xab, 0xf4, 0x82, 0xdd, 0x28, 0xa7, 0xac, 0x86, 0x68,
                0x4d, 0xd5, 0x40, 0x3f, 0x11, 0xdd, 0xcc);

/* GATT service definitions */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /* Heart Rate Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_GATT_SVC_ESS),
        .characteristics = (struct ble_gatt_chr_def[]) { 
            {
                .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_HEART_RATE_MEASURE),
                .access_cb = _hrs_handler,
                .val_handle = &_hrs_val_handle,
                .flags = BLE_GATT_CHR_F_NOTIFY,
            }, 
            {
                .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_BODY_SENSE_LOC),
                .access_cb = _hrs_handler,
                .flags = BLE_GATT_CHR_F_READ,
            },
            {
            /* Characteristic: Read only latest measurement */
                .uuid = (ble_uuid_t *)&latest_mesurement_uuid.u,
                .access_cb = send_latest_measurements,
                .val_handle = &_hrs_val_handle,
                .flags = BLE_GATT_CHR_F_NOTIFY,
            },  
            {
                0, /* no more characteristics in this service */
            }, 
        }
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

    uint32_t pressure = bmx280_read_pressure(&dev);
    memcpy(sensor_measurements, &pressure, sizeof(uint32_t));
    uint32_t temperature = bmx280_read_temperature(&dev);
    memcpy(sensor_measurements + sizeof(uint32_t), &temperature, sizeof(uint32_t));
    uint32_t humidity = bme280_read_humidity(&dev);
    memcpy(sensor_measurements +2*sizeof(uint32_t), &humidity, sizeof(uint32_t));

    /* send heart rate data notification to GATT client */
    om = ble_hs_mbuf_from_flat(sensor_measurements, sizeof(sensor_measurements));
    assert(om != NULL);
    int res = ble_gattc_notify_custom(_conn_handle, _hrs_val_handle, om);
    assert(res == 0);
    (void)res;

    /* schedule next update event */
    event_timeout_set(&_update_timeout_evt, UPDATE_INTERVAL);

    printf("   Pressure    [Pa]:      %" PRIu32 "\n", pressure);
    printf("   Temperature [°C]:      %" PRIu32 "\n", temperature);
    printf("   Pressure    [Percent]: %" PRIu32 "\n", humidity);
}

void make_data_package(bmx280_t* dev)
{
    uint32_t pressure = bmx280_read_pressure(dev);
    memcpy(sensor_measurements, &pressure, sizeof(uint32_t));
}

#include <stdio.h>
#include "periph/gpio.h"
#include "xtimer.h"
#include "scd30.h"
#include "scd30_params.h"
#include "scd30_internal.h"

#define MEASUREMENT_INTERVAL_SECS (2)

scd30_t scd30_dev;
scd30_params_t params = SCD30_PARAMS;
scd30_measurement_t result;

#define TEST_ITERATIONS 10

int main(void)
{
    printf("SCD30 Test:\n");
    int i = 0;

    scd30_init(&scd30_dev, &params);
    uint16_t pressure_compensation = SCD30_DEF_PRESSURE;
    uint16_t value = 0;
    uint16_t interval = MEASUREMENT_INTERVAL_SECS;

    scd30_set_param(&scd30_dev, SCD30_INTERVAL, MEASUREMENT_INTERVAL_SECS);
    scd30_set_param(&scd30_dev, SCD30_START, pressure_compensation);

    scd30_get_param(&scd30_dev, SCD30_INTERVAL, &value);
    printf("[test][dev-%d] Interval: %u s\n", i, value);
    scd30_get_param(&scd30_dev, SCD30_T_OFFSET, &value);
    printf("[test][dev-%d] Temperature Offset: %u.%02u C\n", i, value / 100u,
           value % 100u);
    scd30_get_param(&scd30_dev, SCD30_A_OFFSET, &value);
    printf("[test][dev-%d] Altitude Compensation: %u m\n", i, value);
    scd30_get_param(&scd30_dev, SCD30_ASC, &value);
    printf("[test][dev-%d] ASC: %u\n", i, value);
    scd30_get_param(&scd30_dev, SCD30_FRC, &value);
    printf("[test][dev-%d] FRC: %u ppm\n", i, value);
    puts("NimBLE Heart Rate Sensor Example");
    
    while (i < TEST_ITERATIONS) {
        xtimer_sleep(1);
        scd30_read_triggered(&scd30_dev, &result);
        printf(
            "[scd30_test-%d] Triggered measurements co2: %.02fppm,"
            " temp: %.02f°C, hum: %.02f%%. \n", i, result.co2_concentration,
            result.temperature, result.relative_humidity);
        i++;
    }

    i = 0;
    scd30_start_periodic_measurement(&scd30_dev, &interval,
                                     &pressure_compensation);

    while (i < TEST_ITERATIONS) {
        xtimer_sleep(MEASUREMENT_INTERVAL_SECS);
        scd30_read_periodic(&scd30_dev, &result);
        printf(
            "[scd30_test-%d] Continuous measurements co2: %.02fppm,"
            " temp: %.02f°C, hum: %.02f%%. \n", i, result.co2_concentration,
            result.temperature, result.relative_humidity);
        i++;
    }

    scd30_stop_measurements(&scd30_dev);

    printf("Nimble GATT application\n\r");
    switch (bmx280_init(&dev, &bmx280_params[0])) {
        case BMX280_ERR_BUS:
            puts("[Error] Something went wrong when using the I2C bus");
            return 1;
        case BMX280_ERR_NODEV:
            puts("[Error] Unable to communicate with any BMX280 device");
            return 1;
        default:
            /* all good -> do nothing */
            break;
    }

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
    //uint16_t hrs_uuid = BLE_GATT_SVC_ESS;
    nimble_autoadv_add_field(BLE_GAP_AD_UUID16_INCOMP, &latest_mesurement_uuid, sizeof(latest_mesurement_uuid));

    nimble_auto_adv_set_gap_cb(&gap_event_cb, NULL);

    /* start to advertise this node */
    nimble_autoadv_start();

    /* run an event loop for handling the heart rate update events */
    event_loop(&_eq);

    return 0;
}
