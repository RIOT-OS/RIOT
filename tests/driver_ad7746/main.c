/*
 * Copyright (C) 2019 HAW Hamburg
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
 * @brief       Test application for the AD7746 capacitance-to-digital
 *              converter with temperature sensor.
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "timex.h"
#include "ad7746.h"
#include "ad7746_params.h"

#define SLEEP       (1000 * US_PER_MS)

static ad7746_t dev;

int main(void)
{
    int data;

    puts("AD746 capacitance to digital driver test application\n");
    printf("Initializing AD7746 at I2C_DEV(%i)... ",
           ad7746_params->i2c);

    if (ad7746_init(&dev, ad7746_params) == AD7746_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    /* show that if changed mode data may not be available right away */
    if (ad7746_read_voltage_vdd(&dev, &data) == AD7746_OK) {
        printf("VDD : %d mV\n", data);
    }
    else {
        printf("No data available for VDD yet\n");
    }

    if (ad7746_read_temperature_int(&dev, &data) == AD7746_OK) {
        printf("Internal temperature: %d C\n", data);
    }
    else {
        printf("No data available for internal temperature yet\n");
    }

    while (1) {
        int res;
        puts("=========================");
        puts("        Measuring");
        puts("=========================");
        res = ad7746_read_capacitance_1(&dev, &data);
        if ( res == AD7746_OK) {
            printf("Capacitance %d fF\n", data);
        }
        else {
            printf("Error reading data. err: %d\n", res);
        }

        do {
            res = ad7746_read_temperature_int(&dev, &data);
        } while (res == AD7746_NODATA);

        if (res == AD7746_OK) {
            printf("Internal temperature: %d C\n", data);
        }
        else {
            printf("Error reading internal temperature\n");
        }
        puts("");
        xtimer_usleep(SLEEP);
    }

    return 0;
}
