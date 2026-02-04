/*
 * SPDX-FileCopyrightText: 2018 HAW-Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the SDS011 Laser Dust Sensor driver
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "xtimer.h"

#include "sds011.h"
#include "sds011_params.h"
#include "msg.h"
#include "thread.h"

#define ACTIVE_REPORTING_TEST_CNT                (20U)
#define PUT_TO_QUERY_MODE_RETRIES                (3U)
#define PUT_TO_QUERY_MODE_RETRY_TIMEOUT_MS       (100U)
#define MANUAL_QUERY_CNT                         (10U)
#define WORKING_PERIOD                           (0U)

/**
 * @brief   Allocate the device descriptor
 */
static sds011_t dev;

static char* _rmode_str(sds011_reporting_mode_t rmode)
{
    switch (rmode) {
        case SDS011_RMODE_ACTIVE:
            return "ACTIVE";
        case SDS011_RMODE_QUERY:
            return "QUERY";
        default:
            return "INVALID";
    }
}

static char* _wmode_str(sds011_working_mode_t wmode)
{
    switch (wmode) {
        case SDS011_WMODE_WORK:
            return "WORK";
        case SDS011_WMODE_SLEEP:
            return "SLEEP";
        default:
            return "INVALID";
    }
}

static void _print_measurement(sds011_data_t *data)
{
    uint16_t pm10_ug_int = data->pm_10 / 10;
    uint16_t pm10_ug_dec = data->pm_10 - 10 * pm10_ug_int;
    uint16_t pm2_5_ug_int = data->pm_2_5 / 10;
    uint16_t pm2_5_ug_dec = data->pm_2_5 - 10 * pm2_5_ug_int;
    printf("==> PM2.5: %d.%0d ug/m^3 | PM10: %d.%0d ug/m^3\n",
           pm2_5_ug_int, pm2_5_ug_dec, pm10_ug_int, pm10_ug_dec);
}

void measure_cb(sds011_data_t *data, void *ctx)
{
    msg_t msg = { .content.value = (((uint32_t)data->pm_10) << 16 | data->pm_2_5) };
    kernel_pid_t target_pid = (intptr_t)ctx;
    msg_send(&msg, target_pid);
}

int main(void)
{
    unsigned retry_cnt = 0;
    uint8_t year;
    uint8_t month;
    uint8_t day;
    sds011_reporting_mode_t rmode;
    sds011_working_mode_t wmode;
    uint8_t minutes;
    sds011_data_t data;

    puts("SDS011 test application");

    /* initialize the driver */
    if (sds011_init(&dev, &sds011_params[0]) == SDS011_OK) {
        puts("init [OK]");
    }
    else {
        puts("initialization [ERROR]");
        return -1;
    }

    printf("setting reporting mode to '%s'...\n", _rmode_str(SDS011_RMODE_QUERY));

    /* set the sensor to query mode to disable active reporting messages
       -> to work correctly, this step may need to be repeated if the automatic
          output is incoming while the reply is expected */
    while (sds011_set_reporting_mode(&dev, SDS011_RMODE_QUERY) != SDS011_OK) {
        if (retry_cnt++ >= PUT_TO_QUERY_MODE_RETRIES) {
            puts("[ERROR]");
            return -1;
        }
        xtimer_usleep(PUT_TO_QUERY_MODE_RETRY_TIMEOUT_MS * 1000);
        puts("[RETRY]");
    }

    puts("[OK]");
    puts("getting reporting mode from device...");

    if (sds011_get_reporting_mode(&dev, &rmode) == SDS011_OK) {
        printf("[OK] => mode: %s\n", _rmode_str(rmode));
        if (rmode != SDS011_RMODE_QUERY) {
            puts("mismatch! [ERROR]");
            return -1;
        }
    }
    else {
        puts("[ERROR]");
        return -1;
    }

    puts("getting firmware version...");

    if (sds011_get_fw_version(&dev, &year, &month, &day) == SDS011_OK) {
        printf("[OK] => %d.%d.%d\n", year, month, day);
    }
    else {
        puts("[ERROR]");
        return -1;
    }

    printf("setting working mode to '%s'...\n", _wmode_str(SDS011_WMODE_WORK));

    if (sds011_set_working_mode(&dev, SDS011_WMODE_WORK) == SDS011_OK) {
        puts("[OK]");
    }
    else {
        puts("[ERROR]");
        return -1;
    }

    puts("getting working mode from device...");

    if (sds011_get_working_mode(&dev, &wmode) == SDS011_OK) {
        printf("[OK] => mode: %s\n", _wmode_str(wmode));
        if (wmode != SDS011_WMODE_WORK) {
            puts("mismatch! [ERROR]");
            return -1;
        }
    }
    else {
        puts("[ERROR]");
        return -1;
    }

    printf("setting working period to %u...\n", WORKING_PERIOD);

    if (sds011_set_working_period(&dev, WORKING_PERIOD) == SDS011_OK) {
        puts("[OK]");
    }
    else {
        puts("[ERROR]");
        return 1;
    }

    if (sds011_get_working_period(&dev, &minutes) == SDS011_OK) {
        printf("[OK] => working period: %u\n", minutes);
        if (minutes != WORKING_PERIOD) {
            puts("mismatch! [ERROR]");
            return -1;
        }
    }
    else {
        puts("[ERROR]");
        return -1;
    }

    for (unsigned i = 0; i < MANUAL_QUERY_CNT; i++) {
        if (sds011_read(&dev, &data) == SDS011_OK) {
            printf("manual query %2u/%u [OK]: ", i + 1, MANUAL_QUERY_CNT);
            _print_measurement(&data);
        }
    }

    sds011_register_callback(&dev, measure_cb, (void*)(intptr_t)thread_getpid());

    printf("switching to active reporting mode for %u measurements...\n",
            ACTIVE_REPORTING_TEST_CNT);

    if (sds011_set_reporting_mode(&dev, SDS011_RMODE_ACTIVE) == SDS011_OK) {
        puts("[OK]");
    }
    else {
        puts("[ERROR]");
        return -1;
    }

    /* wait a little bit so the callback gets executed a few times */
    msg_t msg;
    for(unsigned msg_cnt = 0; msg_cnt < ACTIVE_REPORTING_TEST_CNT; msg_cnt++){
        msg_receive(&msg);
        sds011_data_t data;
        data.pm_10 = msg.content.value >> 16;
        data.pm_2_5 = msg.content.value & 0xFFFF;
        printf("msg from callback: ");
        _print_measurement(&data);
    }

    /* unregister callback */
    sds011_register_callback(&dev, NULL, NULL);

    puts("switching to sleep mode...");

    if (sds011_set_working_mode(&dev, SDS011_WMODE_SLEEP) == SDS011_OK) {
        puts("[OK]");
    }
    else {
        puts("[ERROR]");
        return -1;
    }

    puts("[SUCCESS]");
    return 0;
}
