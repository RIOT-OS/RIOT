/*
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
 * @}
 */

#include <stdio.h>

#include "ztimer.h"
#include "ads1x1x.h"
#include "ads1x1x_params.h"
#include "ads1x1x_internal.h"

#define SLEEP_MSEC      100

static ads1x1x_t dev;
static ads1x1x_alert_t alert_dev;

static void alert_cb(void *arg)
{
    (void)arg;
    puts("\n[Alert!]");
}

int main(void)
{
    int16_t data;

    puts("ADS1X1X analog to digital driver test application\n");
    printf("Initializing ADS1X1X analog to digital at I2C_DEV(%i)... ",
           ads1x1x_params->i2c);

    if (ads1x1x_init(&dev, ads1x1x_params) == ADS1X1X_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    printf("Initializing ADS1X1X alert at I2C_DEV(%i)... ",
           ads1x1x_alert_params->i2c);

    if (ads1x1x_alert_init(&alert_dev, ads1x1x_alert_params) == ADS1X1X_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    printf("Enabling alert interrupt: ");
    if (ads1x1x_enable_alert(&alert_dev, alert_cb, NULL) == ADS1X1X_OK) {
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
