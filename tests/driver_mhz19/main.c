/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the MH-Z19 CO2 sensor.
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "mhz19.h"
#include "mhz19_params.h"
#include "xtimer.h"

int main(void)
{
    mhz19_t dev;

    printf("MH-Z19 test application\n");

    printf("------- Initialization -------\n");
    if (mhz19_init(&dev, (mhz19_params_t *)&mhz19_params[0]) != MHZ19_OK) {
        printf("[Error] Initialization failed\n");
        return 1;
    }
    printf("[Ok] Initialization successfull\n\n");


    printf("------- Starting measurements -------\n");
    while (1) {
        int16_t ppm = mhz19_get_ppm(&dev);

        if (ppm < 0) {
            printf("[Error] Measurement timed out\n");
        } else if (ppm > 2000) {
            printf("[Error] Measurement above the range of the sensor (%dppm)\n", (int)ppm);
        } else {
            printf("[Ok] Measured %dppm of CO2\n", (int)ppm);
        }

        xtimer_sleep(2);
    }
}
