/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
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
 * @brief       Test application for the BME280 temperature, pressure
 *              and humidity sensor.
 *
 * @author      Kees Bakker <kees@sodaq.com>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "bme280_params.h"
#include "bme280.h"
#include "xtimer.h"

#define MAINLOOP_DELAY  (2 * 1000 * 1000u)      /* 2 seconds delay between printf's */

int main(void)
{
    bme280_t dev;
    int result;

    puts("BME280 test application\n");

    printf("+------------Initializing------------+\n");
    result = bme280_init(&dev, &bme280_params[0]);
    if (result == -1) {
        puts("[Error] The given i2c is not enabled");
        return 1;
    }

    if (result == -2) {
        printf("[Error] The sensor did not answer correctly at address 0x%02X\n", bme280_params[0].i2c_addr);
        return 1;
    }

    printf("Initialization successful\n\n");

    printf("+------------Calibration Data------------+\n");
    printf("dig_T1: %u\n", dev.calibration.dig_T1);
    printf("dig_T2: %i\n", dev.calibration.dig_T2);
    printf("dig_T3: %i\n", dev.calibration.dig_T3);

    printf("dig_P1: %u\n", dev.calibration.dig_P1);
    printf("dig_P2: %i\n", dev.calibration.dig_P2);
    printf("dig_P3: %i\n", dev.calibration.dig_P3);
    printf("dig_P4: %i\n", dev.calibration.dig_P4);
    printf("dig_P5: %i\n", dev.calibration.dig_P5);
    printf("dig_P6: %i\n", dev.calibration.dig_P6);
    printf("dig_P7: %i\n", dev.calibration.dig_P7);
    printf("dig_P8: %i\n", dev.calibration.dig_P8);
    printf("dig_P9: %i\n", dev.calibration.dig_P9);

    printf("dig_H1: %u\n", dev.calibration.dig_H1);
    printf("dig_H2: %i\n", dev.calibration.dig_H2);
    printf("dig_H3: %i\n", dev.calibration.dig_H3);
    printf("dig_H4: %i\n", dev.calibration.dig_H4);
    printf("dig_H5: %i\n", dev.calibration.dig_H5);
    printf("dig_H6: %i\n", dev.calibration.dig_H6);

    printf("\n+--------Starting Measurements--------+\n");
    while (1) {
        int16_t temperature;
        uint32_t pressure;
        uint16_t humidity;

        /* Get temperature in centi degrees Celsius */
        temperature = bme280_read_temperature(&dev);

        /* Get pressure in Pa */
        pressure = bme280_read_pressure(&dev);

        /* Get pressure in %rH */
        humidity = bme280_read_humidity(&dev);

        printf("Temperature [°C]: %d.%d\n"
               "Pressure [Pa]: %lu\n"
               "Humidity [%%rH]: %u.%02u\n"
               "\n+-------------------------------------+\n",
               temperature / 100, (temperature % 100) / 10,
               (unsigned long)pressure,
               (unsigned int)(humidity / 100), (unsigned int)(humidity % 100));

        xtimer_usleep(MAINLOOP_DELAY);
    }

    return 0;
}
