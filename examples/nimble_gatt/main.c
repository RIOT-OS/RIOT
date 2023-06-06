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
#include "xtimer.h"

#include "periph/i2c.h"

#include "bmi160.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include <stdint.h>
#include "event/timeout.h"
#include "net/bluetil/ad.h"
#include "timex.h"
/* Macros for frames to be read */

#define ACC_FRAMES	1 /* 10 Frames are available every 100ms @ 100Hz */ //Datasheet says you can take measurementes at 1600 Hz
/* 10 frames containing a 1 byte header, 6 bytes of accelerometer,
 * 6 bytes of gyroscope and 8 bytes of magnetometer data. This results in
 * 21 bytes per frame. Additional 40 bytes in case sensor time readout is enabled */
#define FIFO_SIZE	250

#define UPDATE_INTERVAL         (1000U)
#define TAMANHO_MEDIDA          (3*sizeof(int16_t)+sizeof(int))
#define CONTADOR                15 //QUANTIDADE DE MEDIDAS DO DUMP

static event_queue_t _eq;
static event_t _update_evt;
static event_timeout_t _update_timeout_evt;

static uint16_t _conn_handle;
static uint16_t _hrs_val_handle;

char dump_array[CONTADOR*TAMANHO_MEDIDA]; //15 mensagens de 10 bytes cada, pois o IOS menor que 10 não aceita mais que 158 bytes por envio via bluetooth
char solo_measurement[TAMANHO_MEDIDA];

/* Variable declarations */

struct bmi160_dev bmi;

uint8_t fifo_buff[FIFO_SIZE];
struct bmi160_fifo_frame fifo_frame;
struct bmi160_sensor_data accel_data[ACC_FRAMES];

int8_t rslt;

// typedef struct
// {
//     int16_t accelerometerX;
//     int16_t accelerometerY;
//     int16_t accelerometerZ;
//     uint16_t dummy;
// } reading_t;

typedef struct
{
    int16_t X_axis;
    int16_t Y_axis;
    int16_t Z_axis;
    int  timestamp;
} leitura;

#define MAX_READINGS 11000

leitura readings_buffer[MAX_READINGS];
size_t rlen = 0;

int dump_index=0;

void init_bmiSensor(void);
void init_and_run_BLE(void);
//reading_t get_reading(void);
void right_shift_readings_buffer(void);
void do_read(void);
void log_readings(void);
void read_and_show_Acc_values(void);
void acquire_ACC_Values(void);
static void _hr_update(event_t *e);
void make_data_package(uint16_t index);
void make_dump_package(int initial_index);

uint16_t latest_measurement = 0; //Ring buffer index
uint16_t get_ring_index(void);
void add_ring_index(void);


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

#define STR_ANSWER_BUFFER_SIZE 4096

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

/* UUID = ccdd113f-40d5-4d68-86ac-a728dd82f4ab */
static const ble_uuid128_t latest_mesurement_uuid
        = BLE_UUID128_INIT(0xab, 0xf4, 0x82, 0xdd, 0x28, 0xa7, 0xac, 0x86, 0x68,
                0x4d, 0xd5, 0x40, 0x3f, 0x11, 0xdd, 0xcc);

                
static char rm_demo_write_data[64] = "This is the command characteristic";

static int gatt_svr_chr_access_device_info_manufacturer(
    uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg);

static int gatt_svr_chr_access_device_info_model(
    uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg);

static int gatt_svr_chr_access_rw_demo(
    uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg);

static int send_latest_measurements(
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
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                /* Characteristic: * Manufacturer name */
                .uuid = BLE_UUID16_DECLARE(GATT_MANUFACTURER_NAME_UUID),
                .access_cb = gatt_svr_chr_access_device_info_manufacturer,
                .flags = BLE_GATT_CHR_F_READ,
            },
            {
                /* Characteristic: Model number string */
                .uuid = BLE_UUID16_DECLARE(GATT_MODEL_NUMBER_UUID),
                .access_cb = gatt_svr_chr_access_device_info_model,
                .flags = BLE_GATT_CHR_F_READ,
            },
            {
                0, /* No more characteristics in this service */
            },
        }},
    {/* Service: Read/Write Demo */
     .type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = (ble_uuid_t *)&gatt_svr_svc_rw_demo_uuid.u,
     .characteristics = (struct ble_gatt_chr_def[]){
         {
             /* Characteristic: Read/Write Demo write */
             .uuid = (ble_uuid_t *)&gatt_svr_chr_rw_demo_write_uuid.u,
             .access_cb = gatt_svr_chr_access_rw_demo,
             .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
         },
         {
             /* Characteristic: Read/Write Demo read only */
             .uuid = (ble_uuid_t *)&gatt_svr_chr_rw_demo_readonly_uuid.u,
             .access_cb = gatt_svr_chr_access_rw_demo,
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
             0, /* No more characteristics in this service */
         },
     }},
    {
        0, /* No more services */
    },
};

static int gatt_svr_chr_access_device_info_model(
    uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    puts("service 'device info: model' callback triggered");

    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    snprintf(str_answer, STR_ANSWER_BUFFER_SIZE,
             "You are running RIOT on a(n) %s board, "
             "which features a(n) %s MCU.",
             RIOT_BOARD, RIOT_MCU);
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

    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    int rc = 0;

    ble_uuid_t *write_uuid = (ble_uuid_t *)&gatt_svr_chr_rw_demo_write_uuid.u;
    ble_uuid_t *readonly_uuid = (ble_uuid_t *)&gatt_svr_chr_rw_demo_readonly_uuid.u;

    if (ble_uuid_cmp(ctxt->chr->uuid, write_uuid) == 0)
    {

        puts("access to characteristic 'rw demo (write)'");

        switch (ctxt->op)
        {

        case BLE_GATT_ACCESS_OP_READ_CHR:
            puts("read from characteristic");
            printf("current value of rm_demo_write_data: '%s'\n \r",
                   rm_demo_write_data);

            /* send given data to the client */
            rc = os_mbuf_append(ctxt->om, &rm_demo_write_data,
                                strlen(rm_demo_write_data));

            break;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            puts("write to characteristic");

            printf("old value of rm_demo_write_data: '%s'\n \r",
                   rm_demo_write_data);

            uint16_t om_len;
            om_len = OS_MBUF_PKTLEN(ctxt->om);

            /* read sent data */
            rc = ble_hs_mbuf_to_flat(ctxt->om, &rm_demo_write_data,
                                     sizeof rm_demo_write_data, &om_len);
            /* we need to null-terminate the received string */
            rm_demo_write_data[om_len] = '\0';

            printf("new value of rm_demo_write_data: '%s'\n \r",
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
    else if (ble_uuid_cmp(ctxt->chr->uuid, readonly_uuid) == 0)
    {

        puts("access to characteristic 'rw demo (read-only)'");

        if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR && strcmp(rm_demo_write_data, "ola") == 0)
        {

            make_dump_package(dump_index);

            if (dump_index <= 0)
            {
                dump_index+=MAX_READINGS-1;
            }
            else
            {
                printf("Posicao Inicial: %d   \n \r", dump_index);
                dump_index-=CONTADOR;
            }

            printf("Posicao Final = %d \n \r", dump_index);           

            rc = os_mbuf_append(ctxt->om, &dump_array, CONTADOR*TAMANHO_MEDIDA);
            
            return rc;
        }
        else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR && strcmp(rm_demo_write_data, "ri") == 0)
        {
            dump_index=get_ring_index();
            snprintf(str_answer, STR_ANSWER_BUFFER_SIZE,
                     "Ringbuffer index Reset to latest measurement");
            puts(str_answer);

            rc = os_mbuf_append(ctxt->om, &str_answer, strlen(str_answer));

            puts("");

            return rc;
        }
        else
        {
            snprintf(str_answer, STR_ANSWER_BUFFER_SIZE,
                     "No data available");
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

static int auxi2 = 0;
bool connected = 0; 

int main(void)
{

    (void)puts("Welcome to RIOT!");

    init_bmiSensor();
    
    /* setup local event queue (for handling heart rate updates) */
    event_queue_init(&_eq);
    _update_evt.handler = _hr_update;
    event_timeout_ztimer_init(&_update_timeout_evt, ZTIMER_MSEC, &_eq, &_update_evt);
    
    init_and_run_BLE();
    event_timeout_set(&_update_timeout_evt, UPDATE_INTERVAL);


    /* run an event loop for handling the heart rate update events */
    event_loop(&_eq);

    return 0;
}

void do_read(void)
{
    uint16_t aux = get_ring_index(); //supoes-se que o index nunca vai ser um valor proibido nunca <0 e nem >= MAX READINGS
    uint16_t i, j;
    for (i = aux, j = 0; i < (aux + ACC_FRAMES) && j < ACC_FRAMES; i++, j++)
    {
        readings_buffer[i].X_axis = accel_data[j].x;
        readings_buffer[i].Y_axis = accel_data[j].y;
        readings_buffer[i].Z_axis = accel_data[j].z;
        readings_buffer[i].timestamp = (int)(xtimer_now_usec()/1000); 
        add_ring_index(); //incrementa-se o indice do ring buffer a cada medida
    }
}

void log_readings(void)
{
    int i= get_ring_index();
        printf("[Acc_readings] readings_buffer[%d]: ", i);
        printf("Acc_x: %2.6f ", ((float)readings_buffer[i].X_axis)/ AC);
        printf("Acc_y: %2.6f ", ((float)readings_buffer[i].Y_axis)/ AC);
        printf("Acc_z: %2.6f ", ((float)readings_buffer[i].Z_axis)/ AC);
        printf("Acc_timeStamp: %d ", (readings_buffer[i].timestamp));
        printf("\n \r");
}
void read_and_show_Acc_values(void)
{
    do_read();
    // #if(LOG_LEVEL==4)
    log_readings();
    // #endif
}

void init_bmiSensor(void)
{
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
    if (rslt == BMI160_OK)
    {
        printf("Success initializing BMI160 - Chip ID 0x%X\n \r", bmi.chip_id);
    }
    else if (rslt == BMI160_E_DEV_NOT_FOUND)
    {
        printf("Error initializing BMI160: device not found\n \r");
        return;
    }
    else
    {
        printf("Error initializing BMI160 - %d\n \r", rslt);
        return;
    }

    /* Select the Output data rate, range of accelerometer sensor */
    bmi.accel_cfg.odr = BMI160_ACCEL_ODR_1600HZ;
    bmi.accel_cfg.range = BMI160_ACCEL_RANGE_16G;
    // bmi.accel_cfg.range = BMI160_ACCEL_RANGE_2G;
    bmi.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;

    /* Select the power mode of accelerometer sensor */
    bmi.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

    /* Select the Output data rate, range of Gyroscope sensor */
    //bmi.gyro_cfg.odr = BMI160_GYRO_ODR_3200HZ;
    //bmi.gyro_cfg.range = BMI160_GYRO_RANGE_2000_DPS;
    // bmi.gyro_cfg.range = BMI160_GYRO_RANGE_250_DPS;
    //bmi.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;

    /* Select the power mode of Gyroscope sensor */
    bmi.gyro_cfg.power = BMI160_GYRO_SUSPEND_MODE;

    /* Set the sensor configuration */
    rslt = bmi160_set_sens_conf(&bmi);
    if (rslt != BMI160_OK)
    {
        printf("Error configuring BMI160 - %d\n \r", rslt);
        return;
    }

    /* Link the FIFO memory location */
    fifo_frame.data = fifo_buff;
    fifo_frame.length = FIFO_SIZE;
    bmi.fifo = &fifo_frame;
    /* Clear all existing FIFO configurations */
    rslt = bmi160_set_fifo_config(BMI160_FIFO_CONFIG_1_MASK, BMI160_DISABLE, &bmi);
    if (rslt != BMI160_OK)
    {
        printf("Error clearing fifo - %d\n \r", rslt);
        return;
    }

    uint8_t fifo_config = BMI160_FIFO_HEADER | BMI160_FIFO_ACCEL | BMI160_FIFO_GYRO;
    rslt = bmi160_set_fifo_config(fifo_config, BMI160_ENABLE, &bmi);
    if (rslt != BMI160_OK)
    {
        printf("Error enabling fifo - %d\n \r", rslt);
        return;
    }
    /* Check rslt for any error codes */
    i2c_release(dev);
}

static void _start_updating(void)
{
    //event_timeout_set(&_update_timeout_evt, UPDATE_INTERVAL);
    connected= 1;
    printf("[NOTIFY_ENABLED] Streaming ON\n\r");
}

static void _stop_updating(void)
{
    //event_timeout_clear(&_update_timeout_evt);
    connected = 0;
    printf("[NOTIFY_DISABLED] Streaming OFF\n\r");
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

void init_and_run_BLE(void)
{
    printf("NimBLE GATT Pulga for BELKA \n\r");

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

    struct ble_gap_adv_params advp;
    memset(&advp, 0, sizeof(advp));

    advp.conn_mode = BLE_GAP_CONN_MODE_UND;
    advp.disc_mode = BLE_GAP_DISC_MODE_GEN;
    advp.itvl_min  = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    advp.itvl_max  = BLE_GAP_ADV_FAST_INTERVAL1_MAX;

    /* set advertise params */
    nimble_autoadv_set_ble_gap_adv_params(&advp);

    /* configure and set the advertising data */

    nimble_autoadv_add_field(BLE_GAP_AD_UUID16_INCOMP, &latest_mesurement_uuid, sizeof(latest_mesurement_uuid));

    nimble_auto_adv_set_gap_cb(&gap_event_cb, NULL);

    /* start to advertise this node */
    nimble_autoadv_start();
}

void acquire_ACC_Values(void){

    /* It is VERY important to reload the length of the FIFO memory as after the
        * call to bmi160_get_fifo_data(), the bmi.fifo->length contains the
        * number of bytes read from the FIFO */
    bmi.fifo->length = FIFO_SIZE;
    i2c_acquire(dev);
    rslt = bmi160_get_fifo_data(&bmi);
    if (rslt != BMI160_OK)
    {
        printf("Error getting fifo data - %d\n \r", rslt);
        return;
    }
    i2c_release(dev);

    uint8_t acc_inst = ACC_FRAMES;

    rslt = bmi160_extract_accel(accel_data, &acc_inst, &bmi);
    if (rslt != BMI160_OK)
    {
        printf("Error extracting accel data - %d\n \r", rslt);
        return;
    }
}

static void _hr_update(event_t *e)
{
    (void)e;
    struct os_mbuf *om;

    /* from here, we code the event */
    acquire_ACC_Values();
    do_read(); //aqui iremos adicionar os dados no final do ring buffer e incrementalo

    int auxi, auxi3;
    auxi3 = get_ring_index();
    if (auxi3 > 0)
        make_data_package(auxi3 -1); //do read já incrementa o ring buffer
    else
        make_data_package(0);
    
    if(connected){
        /* send data notification to GATT client */
        om = ble_hs_mbuf_from_flat(solo_measurement, sizeof(solo_measurement));
        assert(om != NULL);
        int res = ble_gattc_notify_custom(_conn_handle, _hrs_val_handle, om);
        assert(res == 0);
        (void)res;
    }

    auxi = solo_measurement[TAMANHO_MEDIDA -4] + (solo_measurement[TAMANHO_MEDIDA -3] <<8) + (solo_measurement[TAMANHO_MEDIDA -2] <<16) + (solo_measurement[TAMANHO_MEDIDA-1] <<24); //VERY SENSIBLE BIT SHIFT OPERATION
    printf("[NOTIFY] Latest measurement interval: %d \n \r", auxi - auxi2);
    printf("[NOTIFY] Ringbuffer Index: %d \n \r", auxi3);
    auxi2 = auxi;
    /* schedule next update event */
    event_timeout_set(&_update_timeout_evt, UPDATE_INTERVAL);
}

void make_data_package(uint16_t index)
{
    int auxi;
    int16_t auxf;

        auxf= (readings_buffer[index].X_axis);
        memcpy(solo_measurement, &auxf, sizeof(int16_t));
        auxf= (readings_buffer[index].Y_axis); 
        memcpy(solo_measurement + sizeof(int16_t) , &auxf, sizeof(int16_t));
        auxf= (readings_buffer[index].Z_axis);
        memcpy(solo_measurement + (2)*sizeof(int16_t), &auxf, sizeof(int16_t));
        auxi= (readings_buffer[index].timestamp);
        memcpy(solo_measurement + (3)*sizeof(int16_t), &auxi, sizeof(int));
}

static int gatt_svr_chr_access_device_info_manufacturer(
    uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    puts("service 'device info: manufacturer' callback triggered");

    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    snprintf(str_answer, STR_ANSWER_BUFFER_SIZE,
             "This is RIOT! (Version: %s)\n \r", RIOT_VERSION); 
    puts(str_answer);

    int rc = os_mbuf_append(ctxt->om, str_answer, strlen(str_answer));

    puts("");
    return rc;
}

void make_dump_package(int initial_index)
{
    int auxi;
    int16_t auxf; 

    for(int i=0; i < CONTADOR; i++)
    {
        if ((initial_index - i) >= 0)    
        {
            auxf= (readings_buffer[-i+initial_index].X_axis);
            memcpy(dump_array + ( 3*i )*sizeof(int16_t) + i*sizeof(int), &auxf, sizeof(int16_t));
            auxf= (readings_buffer[-i+initial_index].Y_axis); 
            memcpy(dump_array + (3*i+1)*sizeof(int16_t) + i*sizeof(int), &auxf, sizeof(int16_t));
            auxf= (readings_buffer[-i+initial_index].Z_axis);
            memcpy(dump_array + (3*i+2)*sizeof(int16_t) + i*sizeof(int), &auxf, sizeof(int16_t));
            auxi= (readings_buffer[-i+initial_index].timestamp);
            memcpy(dump_array + (3*i+3)*sizeof(int16_t) + i*sizeof(int), &auxi, sizeof(int));
        }
        else{
            auxf= (readings_buffer[MAX_READINGS-i-1].X_axis);
            memcpy(dump_array + ( 3*i )*sizeof(int16_t) + i*sizeof(int), &auxf, sizeof(int16_t));
            auxf= (readings_buffer[MAX_READINGS-i-1].Y_axis); 
            memcpy(dump_array + (3*i+1)*sizeof(int16_t) + i*sizeof(int), &auxf, sizeof(int16_t));
            auxf= (readings_buffer[MAX_READINGS-i-1].Z_axis);
            memcpy(dump_array + (3*i+2)*sizeof(int16_t) + i*sizeof(int), &auxf, sizeof(int16_t));
            auxi= (readings_buffer[MAX_READINGS-i-1].timestamp);
            memcpy(dump_array + (3*i+3)*sizeof(int16_t) + i*sizeof(int), &auxi, sizeof(int));
        }
    }
}

static int send_latest_measurements(
    uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    int rc = os_mbuf_append(ctxt->om, solo_measurement, sizeof(solo_measurement));

    puts("new service working");

    return rc;
}

uint16_t get_ring_index(void)
{
    return latest_measurement;
}

void add_ring_index(void)
{
    //latest_measurement = rlen < MAX_READINGS ? rlen++ : 0;
    if(rlen < MAX_READINGS-1)
    {
        rlen++;
        latest_measurement = rlen;
    }
    else if(rlen == MAX_READINGS-1)
    {
        latest_measurement = 0;
        rlen++;
    }
    else if (latest_measurement == MAX_READINGS -1)
    {
        latest_measurement = 0;
    }
    else 
    {
        latest_measurement++;
    }

}