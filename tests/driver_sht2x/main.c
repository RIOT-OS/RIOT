/*
 * Copyright (C) 2017 Georgios Psimenos (gp7g14@soton.ac.uk)
 * Copyright (C) 2018 Steffen Robertz (steffen.robertz@rwth-aachen.de)
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
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
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

    printf("Initializing SHT2X at I2C_DEV(%i)...\n", sht2x_params->i2c);
    if (sht2x_init(&dev, sht2x_params) == SHT2X_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }
    /* Enable Chip heater to see changes */
    puts("Enabling Chip Heater...");
    if (sht2x_write_settings(&dev, SHT2X_RES_12_14BIT | SHT2X_USER_HEATER_MASK) == SHT2X_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
    }

    /* periodically read temperature and humidity values */
    while (1) {
        sht2x_read_temp(&dev);
        sht2x_read_humidity(&dev);
        printf("Currently %d,%d C at %d,%d RH\n", dev.values.temp / 100, dev.values.temp % 100, dev.values.rel_humidity / 100, dev.values.rel_humidity % 100);
        xtimer_sleep(1);
    }

    /* should never be reached */
    return 0;
}
