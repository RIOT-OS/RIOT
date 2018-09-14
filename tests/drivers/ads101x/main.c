/*
 * Copyright (C) 2017 OTA keys S.A.
 *               2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#include "xtimer.h"
#include "timex.h"
#include "ads101x.h"
#include "ads101x_params.h"
#include "ads101x_regs.h"

#define SLEEP       (100 * US_PER_MS)

static ads101x_t dev;
static ads101x_alert_t alert_dev;

static void alert_cb(void *arg)
{
    (void)arg;
    puts("\n[Alert!]");
}

int main(void)
{
    int16_t data;

    puts("ADS101X analog to digital driver test application\n");
    printf("Initializing ADS101x analog to digital at I2C_DEV(%i)... ",
           ads101x_params->i2c);

    if (ads101x_init(&dev, ads101x_params) == ADS101X_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    printf("Initializing ADS101x alert at I2C_DEV(%i)... ",
           ads101x_alert_params->i2c);

    if (ads101x_alert_init(&alert_dev, ads101x_alert_params) == ADS101X_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    printf("Enabling alert interrupt: ");
    if (ads101x_enable_alert(&alert_dev, alert_cb, NULL) == ADS101X_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    while (1) {
        printf("Raw analog read. CH0: ");
        ads101x_set_mux_gain(&dev, ADS101X_AIN0_SINGM | ADS101X_PGA_FSR_2V048);
        if (ads101x_read_raw(&dev, &data) == ADS101X_OK) {
            printf("%d", data);
        }
        else {
            puts("[Fail]");
            continue;
        }

        printf(" CH1: ");
        ads101x_set_mux_gain(&dev, ADS101X_AIN1_SINGM | ADS101X_PGA_FSR_2V048);
        if (ads101x_read_raw(&dev, &data) == ADS101X_OK) {
            printf("%d", data);
        }
        else {
            puts("[Fail]");
            continue;
        }

        printf(" CH2: ");
        ads101x_set_mux_gain(&dev, ADS101X_AIN2_SINGM | ADS101X_PGA_FSR_2V048);
        if (ads101x_read_raw(&dev, &data) == ADS101X_OK) {
            printf("%d", data);
        }
        else {
            puts("[Fail]");
            continue;
        }

        printf(" CH3: ");
        ads101x_set_mux_gain(&dev, ADS101X_AIN3_SINGM | ADS101X_PGA_FSR_2V048);
        if (ads101x_read_raw(&dev, &data) == ADS101X_OK) {
            printf("%d", data);
        }
        else {
            puts("[Fail]");
            continue;
        }

        puts("");

        xtimer_usleep(SLEEP);
    }

    return 0;
}
