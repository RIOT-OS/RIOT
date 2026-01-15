/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2018 Acutam Automation, LLC
 * SPDX-FileCopyrightText: 2025 Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the ADS101x/111x ADC driver
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * @}
 */

#include <stdio.h>

#define ADS1X1X_PARAM_HIGH_LIMIT 10000

#include "ztimer.h"
#include "ads1x1x.h"
#include "ads1x1x_params.h"
#include "ads1x1x_internal.h"

#define SLEEP_MSEC      100
#define ADS101X_DATAR_TEST ADS101X_DATAR_1600 // Default data rate for ADS101x
#define ADS111X_DATAR_TEST ADS111X_DATAR_128 // Default data rate for ADS111x

static ads1x1x_t dev;
static ads1x1x_alert_t alert_dev;

static ads1x1x_params_t ads101x_params =  {
    .i2c = I2C_DEV(0),
    .addr = CONFIG_ADS1X1X_I2C_ADDR,
    .mux = ADS1X1X_PARAM_MUX,
    .pga = ADS1X1X_PARAM_PGA,
    .mode = ADS1X1X_PARAM_MODE,
    .dr = ADS101X_DATAR_TEST,
    .bits_res = ADS101X_BITS_RES
};

static ads1x1x_params_t ads111x_params =  {
    .i2c = I2C_DEV(1),
    .addr = CONFIG_ADS1X1X_I2C_ADDR,
    .mux = ADS1X1X_PARAM_MUX,
    .pga = ADS1X1X_PARAM_PGA,
    .mode = ADS1X1X_PARAM_MODE,
    .dr = ADS111X_DATAR_TEST,
    .bits_res = ADS111X_BITS_RES
};

static void alert_cb(void *arg)
{
    (void)arg;
    puts("\n[Alert!]");
}

int run_test(ads1x1x_params_t * params)
{
    int16_t data;

    if (ads1x1x_init(&dev, params) == ADS1X1X_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    printf("Initializing ADS alert at I2C_DEV(%i)... ",
           ads1x1x_alert_params->i2c);

    if (ads1x1x_alert_init(&alert_dev, ads1x1x_alert_params) == ADS1X1X_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    printf("Enabling alert interrupt: ");
    if (ads1x1x_enable_alert(&alert_dev, alert_cb, NULL, 4) == ADS1X1X_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    while (1) {
        printf("Raw analog read. CH0: ");
        if (ads1x1x_set_mux(&dev, ADS1X1X_AIN0_SINGM) < 0) {
            puts("[Fail]");
            return -1;
        };
        if (ads1x1x_read_raw(&dev, &data) == ADS1X1X_OK) {
            printf("%d", data);
        }
        else {
            puts("[Fail]");
            continue;
        }

        printf(" CH1: ");
        if (ads1x1x_set_mux(&dev, ADS1X1X_AIN1_SINGM) < 0) {
            puts("[Fail]");
            return -1;
        }
        if (ads1x1x_read_raw(&dev, &data) == ADS1X1X_OK) {
            printf("%d", data);
        }
        else {
            puts("[Fail]");
            continue;
        }

        printf(" CH2: ");
        if (ads1x1x_set_mux(&dev, ADS1X1X_AIN2_SINGM) < 0) {
            puts("[Fail]");
            return -1;
        }
        if (ads1x1x_read_raw(&dev, &data) == ADS1X1X_OK) {
            printf("%d", data);
        }
        else {
            puts("[Fail]");
            continue;
        }

        printf(" CH3: ");
        if (ads1x1x_set_mux(&dev, ADS1X1X_AIN3_SINGM) < 0) {
            puts("[Fail]");
            return -1;
        }
        if (ads1x1x_read_raw(&dev, &data) == ADS1X1X_OK) {
            printf("%d", data);
        }
        else {
            puts("[Fail]");
            continue;
        }

        puts("");

        ztimer_sleep(ZTIMER_MSEC, SLEEP_MSEC);
    }

    return 0;
}

int main(void)
{
    puts("ADS1X1X analog to digital driver test application\n");

    printf("Testing ADS101X on I2C_DEV(%i)... ", ads101x_params.i2c);
    if (run_test(&ads101x_params) < 0) {
        return -1;
    }

    printf("Testing ADS111X on I2C_DEV(%i)... ", ads111x_params.i2c);
    if (run_test(&ads111x_params) < 0) {
        return -1;
    }

    return 0;
}
