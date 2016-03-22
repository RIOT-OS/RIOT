/*
 * Copyright (C) 2016 Inria
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
 * @brief       Test application for the BMP180 pressure and temperature sensor
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#ifndef TEST_I2C
#error "TEST_I2C not defined"
#endif

#ifndef TEST_MEASURE_OVERSAMPLING
#error "TEST_MEASURE_OVERSAMPLING not defined"
#endif

#ifndef TEST_ALTITUDE
#error "TEST_ALTITUDE not defined"
#endif

#include <stdio.h>
#include <inttypes.h>

#include "bmp180.h"
#include "xtimer.h"
#include "board.h"

#define SLEEP_2S   (2 * 1000 * 1000u) /* 2 seconds delay between printf */

int main(void)
{
    bmp180_t dev;
    int32_t temperature, pressure, altitude, pressure_0;
    int result;

    puts("BMP180 test application\n");

    printf("+------------Initializing------------+\n");
    result = bmp180_init(&dev, TEST_I2C, TEST_MEASURE_OVERSAMPLING);
    if (result == -1) {
        puts("[Error] The given i2c is not enabled");
        return 1;
    }
    else if (result == -2) {
        puts("[Error] The sensor did not answer correctly on the given address");
        return 1;
    }
    else {
        printf("Initialization successful\n\n");
    }

    printf("+------------Calibration------------+\n");
    printf("AC1: %i\n", dev.calibration.ac1);
    printf("AC2: %i\n", dev.calibration.ac2);
    printf("AC3: %i\n", dev.calibration.ac3);
    printf("AC4: %i\n", dev.calibration.ac4);
    printf("AC5: %i\n", dev.calibration.ac5);
    printf("AC6: %i\n", dev.calibration.ac6);
    printf("B1: %i\n", dev.calibration.b1);
    printf("B2: %i\n", dev.calibration.b2);
    printf("MB: %i\n", dev.calibration.mb);
    printf("MC: %i\n", dev.calibration.mc);
    printf("MD: %i\n", dev.calibration.md);
    printf("\n+--------Starting Measurements--------+\n");
    while (1) {
        /* Get temperature in deci degrees celsius */
        bmp180_read_temperature(&dev, &temperature);

        /* Get pressure in Pa */
        bmp180_read_pressure(&dev, &pressure);

        /* Get pressure at sealevel in Pa */
        bmp180_sealevel_pressure(&dev, (int32_t)TEST_ALTITUDE, &pressure_0);

        /* Get altitude in meters */
        bmp180_altitude(&dev, pressure_0, &altitude);

        printf("Temperature [°C]: %.1f\n"
               "Pressure [hPa]: %.2f\n"
               "Pressure at see level [hPa]: %.2f\n"
               "Altitude [m]: %i\n"
               "\n+-------------------------------------+\n",
               (double)temperature / 10.0, (double)pressure / 100.0,
               (double)pressure_0 / 100.0, (int)altitude);

        xtimer_usleep(SLEEP_2S);
    }

    return 0;
}
