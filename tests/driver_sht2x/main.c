/*
 * Copyright (C) 2017 Georgios Psimenos (gp7g14@soton.ac.uk)
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
 * @brief       Test application for the SHT2x sensor driver.
 *
 * @author      Georgios Psimenos <gp7g14@soton.ac.uk>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "sht2x.h"
#include "sht2x_params.h"

static sht2x_t dev;

int main(void)
{
    puts("SHT2X driver test application");

    int16_t temperature;
    int tdecimal;
    int tfraction;

    int16_t humidity;
    int hdecimal;
    int hfraction;

    printf("Initializing SHT2X at I2C_DEV(%i)...\n", sht2x_params->i2c);
    if (sht2x_init(&dev, sht2x_params) == SHT2X_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    /* periodically read temperature and humidity values */
    while (1) {
        temperature = sht2x_read_temp(&dev);
        tdecimal = temperature / 100;
        tfraction = temperature % 100;

        humidity = sht2x_read_humidity(&dev);
        hdecimal = humidity / 100;
        hfraction = humidity % 100;

        printf("Currently %i.%i °C at %i.%i%% RH\n", tdecimal, tfraction, hdecimal, hfraction);
        xtimer_sleep(1);
    }

    // should never be reached
    return 0;
}
