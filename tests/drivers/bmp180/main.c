/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "bmp180.h"
#include "bmp180_params.h"
#include "timex.h"
#include "ztimer.h"
#include "board.h"

int main(void)
{
    bmp180_t dev;
    int result;

    puts("BMP180 test application\n");

    printf("+------------Initializing------------+\n");
    result = bmp180_init(&dev, &bmp180_params[0]);
    if (result == -BMP180_ERR_NODEV) {
        puts("[Error] The sensor did not answer correctly on the given address");
        return 1;
    }
    else if (result == -BMP180_ERR_NOCAL) {
        puts("[Error] Cannot read the sensor calibration values");
        return 1;
    }
    else {
        puts("Initialization successful\n");
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
        int16_t temperature = bmp180_read_temperature(&dev);

        /* Get pressure in Pa */
        uint32_t pressure = bmp180_read_pressure(&dev);

        /* Get pressure at sealevel in Pa */
        uint32_t pressure_0 = bmp180_sealevel_pressure(&dev, (int16_t)TEST_ALTITUDE);

        /* Get altitude in meters */
        int16_t altitude = bmp180_altitude(&dev, pressure_0);

        printf("Temperature [°C]: %i.%d\n"
               "Pressure [hPa]: %lu.%d\n"
               "Pressure at see level [hPa]: %lu.%d\n"
              "Altitude [m]: %i\n"
               "\n+-------------------------------------+\n",
               (int)(temperature / 10), abs(temperature % 10),
               (unsigned long)pressure / 100, (int)(pressure % 100),
               (unsigned long)pressure_0 / 100, (int)(pressure_0 % 100),
               (int)altitude);

        ztimer_sleep(ZTIMER_MSEC, 2 * MS_PER_SEC);
    }

    return 0;
}
