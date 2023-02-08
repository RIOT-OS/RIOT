/*
 * Copyright (C) 2018 Freie Universität Berlin
 *               2018 Codecoup
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
 * @brief       BLE peripheral example using NimBLE
 *
 * Have a more detailed look at the api here:
 * https://mynewt.apache.org/latest/tutorials/ble/bleprph/bleprph.html
 *
 * More examples (not ready to run on RIOT) can be found here:
 * https://github.com/apache/mynewt-nimble/tree/master/apps
 *
 * Test this application e.g. with Nordics "nRF Connect"-App
 * iOS: https://itunes.apple.com/us/app/nrf-connect/id1054362403
 * Android: https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Andrzej Kaczmarek <andrzej.kaczmarek@codecoup.pl>
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nimble_riot.h"
#include "nimble_autoadv.h"

#include "thread.h"
#include "shell.h"
#include "ztimer.h"

#include "periph/i2c.h"

#include "bmi160.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

/* Macros for frames to be read */

#define ACC_FRAMES	10 /* 10 Frames are available every 100ms @ 100Hz */
#define GYR_FRAMES	10
#define MAG_FRAMES	10
/* 10 frames containing a 1 byte header, 6 bytes of accelerometer,
 * 6 bytes of gyroscope and 8 bytes of magnetometer data. This results in
 * 21 bytes per frame. Additional 40 bytes in case sensor time readout is enabled */
#define FIFO_SIZE	250

/* Variable declarations */

struct bmi160_dev bmi;

uint8_t fifo_buff[FIFO_SIZE];
struct bmi160_fifo_frame fifo_frame;
struct bmi160_sensor_data gyro_data[GYR_FRAMES], accel_data[ACC_FRAMES];

int8_t rslt;

int dev = I2C_DEV(0);

int8_t user_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    // printf("i2c_read_regs(%d, %x, %x, ...)\n", dev, dev_addr, reg_addr);
    return i2c_read_regs(dev, dev_addr, reg_addr, data, len, 0);
}

int8_t user_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    // printf("i2c_write_regs(%d, %x, %x, ...)\n", dev, dev_addr, reg_addr);
    return i2c_write_regs(dev, dev_addr, reg_addr, data, len, 0);
}

void user_delay(uint32_t period)
{
    ztimer_sleep(ZTIMER_MSEC, period);
}

/* accel params and conversion constants */
#define AC 2048.0 // for 16G
// #define AC 16384.0 // for 2G
/* gyro params and conversion constants */
#define GC 16.4 // for 2000 DPS
// #define GC 131.2 // for 250 DPS

#define GATT_DEVICE_INFO_UUID                   0x180A
#define GATT_MANUFACTURER_NAME_UUID             0x2A29
#define GATT_MODEL_NUMBER_UUID                  0x2A24

#define STR_ANSWER_BUFFER_SIZE 100

/* UUID = 1bce38b3-d137-48ff-a13e-033e14c7a335 */
static const ble_uuid128_t gatt_svr_svc_rw_demo_uuid
        = BLE_UUID128_INIT(0x35, 0xa3, 0xc7, 0x14, 0x3e, 0x03, 0x3e, 0xa1, 0xff,
                0x48, 0x37, 0xd1, 0xb3, 0x38, 0xce, 0x1b);

/* UUID = 35f28386-3070-4f3b-ba38-27507e991762 */
static const ble_uuid128_t gatt_svr_chr_rw_demo_write_uuid
        = BLE_UUID128_INIT(0x62, 0x17, 0x99, 0x7e, 0x50, 0x27, 0x38, 0xba, 0x3b,
                0x4f, 0x70, 0x30, 0x86, 0x83, 0xf2, 0x35);

/* UUID = ccdd113f-40d5-4d68-86ac-a728dd82f4aa */
static const ble_uuid128_t gatt_svr_chr_rw_demo_readonly_uuid
        = BLE_UUID128_INIT(0xaa, 0xf4, 0x82, 0xdd, 0x28, 0xa7, 0xac, 0x86, 0x68,
                0x4d, 0xd5, 0x40, 0x3f, 0x11, 0xdd, 0xcc);

static char rm_demo_write_data[64] = "Get it done!";

static int gatt_svr_chr_access_device_info_manufacturer(
        uint16_t conn_handle, uint16_t attr_handle,
        struct ble_gatt_access_ctxt *ctxt, void *arg);

static int gatt_svr_chr_access_device_info_model(
        uint16_t conn_handle, uint16_t attr_handle,
        struct ble_gatt_access_ctxt *ctxt, void *arg);

static int gatt_svr_chr_access_rw_demo(
        uint16_t conn_handle, uint16_t attr_handle,
        struct ble_gatt_access_ctxt *ctxt, void *arg);

static char str_answer[STR_ANSWER_BUFFER_SIZE];

/* define several bluetooth services for our device */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    /*
     * access_cb defines a callback for read and write access events on
     * given characteristics
     */
    {
        /* Service: Device Information */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATT_DEVICE_INFO_UUID),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: * Manufacturer name */
            .uuid = BLE_UUID16_DECLARE(GATT_MANUFACTURER_NAME_UUID),
            .access_cb = gatt_svr_chr_access_device_info_manufacturer,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            /* Characteristic: Model number string */
            .uuid = BLE_UUID16_DECLARE(GATT_MODEL_NUMBER_UUID),
            .access_cb = gatt_svr_chr_access_device_info_model,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },
    {
        /* Service: Read/Write Demo */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = (ble_uuid_t*) &gatt_svr_svc_rw_demo_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: Read/Write Demo write */
            .uuid = (ble_uuid_t*) &gatt_svr_chr_rw_demo_write_uuid.u,
            .access_cb = gatt_svr_chr_access_rw_demo,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            /* Characteristic: Read/Write Demo read only */
            .uuid = (ble_uuid_t*) &gatt_svr_chr_rw_demo_readonly_uuid.u,
            .access_cb = gatt_svr_chr_access_rw_demo,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },
    {
        0, /* No more services */
    },
};

static int gatt_svr_chr_access_device_info_manufacturer(
        uint16_t conn_handle, uint16_t attr_handle,
        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    puts("service 'device info: manufacturer' callback triggered");

    (void) conn_handle;
    (void) attr_handle;
    (void) arg;

    snprintf(str_answer, STR_ANSWER_BUFFER_SIZE,
             "This is RIOT! (Version: %s)\n", RIOT_VERSION);
    puts(str_answer);

    int rc = os_mbuf_append(ctxt->om, str_answer, strlen(str_answer));

    puts("");

    return rc;
}

static int gatt_svr_chr_access_device_info_model(
        uint16_t conn_handle, uint16_t attr_handle,
        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    puts("service 'device info: model' callback triggered");

    (void) conn_handle;
    (void) attr_handle;
    (void) arg;

    snprintf(str_answer, STR_ANSWER_BUFFER_SIZE,
             "You are running RIOT on a(n) %s board, "
             "which features a(n) %s MCU.", RIOT_BOARD, RIOT_MCU);
    puts(str_answer);

    int rc = os_mbuf_append(ctxt->om, str_answer, strlen(str_answer));

    puts("");

    return rc;
}

static int gatt_svr_chr_access_rw_demo(
        uint16_t conn_handle, uint16_t attr_handle,
        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    puts("service 'rw demo' callback triggered");

    (void) conn_handle;
    (void) attr_handle;
    (void) arg;

    int rc = 0;

    ble_uuid_t* write_uuid = (ble_uuid_t*) &gatt_svr_chr_rw_demo_write_uuid.u;
    ble_uuid_t* readonly_uuid = (ble_uuid_t*) &gatt_svr_chr_rw_demo_readonly_uuid.u;

    if (ble_uuid_cmp(ctxt->chr->uuid, write_uuid) == 0) {

        puts("access to characteristic 'rw demo (write)'");

        switch (ctxt->op) {

            case BLE_GATT_ACCESS_OP_READ_CHR:
                puts("read from characteristic");
                printf("current value of rm_demo_write_data: '%s'\n",
                       rm_demo_write_data);

                /* send given data to the client */
                rc = os_mbuf_append(ctxt->om, &rm_demo_write_data,
                                    strlen(rm_demo_write_data));

                break;

            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                puts("write to characteristic");

                printf("old value of rm_demo_write_data: '%s'\n",
                       rm_demo_write_data);

                uint16_t om_len;
                om_len = OS_MBUF_PKTLEN(ctxt->om);

                /* read sent data */
                rc = ble_hs_mbuf_to_flat(ctxt->om, &rm_demo_write_data,
                                         sizeof rm_demo_write_data, &om_len);
                /* we need to null-terminate the received string */
                rm_demo_write_data[om_len] = '\0';

                printf("new value of rm_demo_write_data: '%s'\n",
                       rm_demo_write_data);

                break;

            case BLE_GATT_ACCESS_OP_READ_DSC:
                puts("read from descriptor");
                break;

            case BLE_GATT_ACCESS_OP_WRITE_DSC:
                puts("write to descriptor");
                break;

            default:
                puts("unhandled operation!");
                rc = 1;
                break;
        }

        puts("");

        return rc;
    }
    else if (ble_uuid_cmp(ctxt->chr->uuid, readonly_uuid) == 0) {

        puts("access to characteristic 'rw demo (read-only)'");

        if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
            char random_digit;
            /* get random char between '0' and '9' */
            random_digit = 48 + (rand() % 10);

            snprintf(str_answer, STR_ANSWER_BUFFER_SIZE,
                     "new random number: %c", random_digit);
            puts(str_answer);

            rc = os_mbuf_append(ctxt->om, &str_answer, strlen(str_answer));

            puts("");

            return rc;
        }

        return 0;
    }

    puts("unhandled uuid!");
    return 1;
}

int main(void)
{

    (void) puts("Welcome to RIOT!");

    i2c_init(dev);
    i2c_acquire(dev);
    
    /* Initialize your host interface to the BMI160 */

    /* This example uses I2C as the host interface */
    bmi.id = BMI160_I2C_ADDR;
    bmi.read = user_i2c_read;
    bmi.write = user_i2c_write;
    bmi.delay_ms = user_delay;
    bmi.intf = BMI160_I2C_INTF;

    rslt = bmi160_init(&bmi);
    if (rslt == BMI160_OK) {
        printf("Success initializing BMI160 - Chip ID 0x%X\n", bmi.chip_id);
    } else if (rslt == BMI160_E_DEV_NOT_FOUND) {
        printf("Error initializing BMI160: device not found\n");
        return 1;
    } else {
        printf("Error initializing BMI160 - %d\n", rslt);
        return 1;
    }

    /* Select the Output data rate, range of accelerometer sensor */
    bmi.accel_cfg.odr = BMI160_ACCEL_ODR_1600HZ;
    bmi.accel_cfg.range = BMI160_ACCEL_RANGE_16G;
    // bmi.accel_cfg.range = BMI160_ACCEL_RANGE_2G;
    bmi.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;

    /* Select the power mode of accelerometer sensor */
    bmi.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

    /* Select the Output data rate, range of Gyroscope sensor */
    bmi.gyro_cfg.odr = BMI160_GYRO_ODR_3200HZ;
    bmi.gyro_cfg.range = BMI160_GYRO_RANGE_2000_DPS;
    // bmi.gyro_cfg.range = BMI160_GYRO_RANGE_250_DPS;
    bmi.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;

    /* Select the power mode of Gyroscope sensor */
    bmi.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE;

    /* Set the sensor configuration */
    rslt = bmi160_set_sens_conf(&bmi);
    if (rslt != BMI160_OK) {
        printf("Error configuring BMI160 - %d\n", rslt);
        return 1;
    }

    /* Link the FIFO memory location */
    fifo_frame.data = fifo_buff;
    fifo_frame.length = FIFO_SIZE;
    bmi.fifo = &fifo_frame;
    /* Clear all existing FIFO configurations */
    rslt = bmi160_set_fifo_config(BMI160_FIFO_CONFIG_1_MASK , BMI160_DISABLE, &bmi);
    if (rslt != BMI160_OK) {
        printf("Error clearing fifo - %d\n", rslt);
        return 1;
    }

    uint8_t fifo_config = BMI160_FIFO_HEADER |  BMI160_FIFO_ACCEL | BMI160_FIFO_GYRO;
    rslt = bmi160_set_fifo_config(fifo_config, BMI160_ENABLE, &bmi);
    if (rslt != BMI160_OK) {
        printf("Error enabling fifo - %d\n", rslt);
        return 1;
    }
    /* Check rslt for any error codes */
    i2c_release(dev);

    int cont =0;
    while(rslt == 0 && cont < 10) {
        /* Wait for 100ms for the FIFO to fill */
        user_delay(100);

        /* It is VERY important to reload the length of the FIFO memory as after the
         * call to bmi160_get_fifo_data(), the bmi.fifo->length contains the
         * number of bytes read from the FIFO */
        bmi.fifo->length = FIFO_SIZE;
        i2c_acquire(dev);
        rslt = bmi160_get_fifo_data(&bmi);
        if (rslt != BMI160_OK) {
            printf("Error getting fifo data - %d\n", rslt);
            return 1;
        }
        i2c_release(dev);

        uint8_t gyr_inst = GYR_FRAMES, acc_inst = ACC_FRAMES;
        rslt = bmi160_extract_gyro(gyro_data, &gyr_inst, &bmi);
        if (rslt != BMI160_OK) {
            printf("Error extracting gyro data - %d\n", rslt);
            return 1;
        }

        rslt = bmi160_extract_accel(accel_data, &acc_inst, &bmi);
        if (rslt != BMI160_OK) {
            printf("Error extracting accel data - %d\n", rslt);
            return 1;
        }

        for (size_t i = 0; i < acc_inst && i < gyr_inst; i++) {
            printf("Accel & gyro txyz is:     ");
            printf("%"PRIu32" %6.2f %6.2f %6.2f    ",
                accel_data[i].sensortime,
                accel_data[i].x / AC,
                accel_data[i].y / AC,
                accel_data[i].z / AC);
            printf("%"PRIu32" %6.2f %6.2f %6.2f    ",
                gyro_data[i].sensortime,
                gyro_data[i].x / AC,
                gyro_data[i].y / AC,
                gyro_data[i].z / AC);
            printf("\n");
        }

        cont++;
    }

    /*rm_demo_write_data[64] = (char)(accel_data[0].x/ AC);*/

    printf("passou por aqui5\n");

    puts("NimBLE GATT Server Example");

    int rc = 0;
    (void)rc;

    /* verify and add our custom services */
    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    assert(rc == 0);
    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    assert(rc == 0);

    /* set the device name */
    ble_svc_gap_device_name_set(NIMBLE_AUTOADV_DEVICE_NAME);
    /* reload the GATT server to link our added services */
    ble_gatts_start();

    /* start to advertise this node */
    nimble_autoadv_start();

    return 0;
}
