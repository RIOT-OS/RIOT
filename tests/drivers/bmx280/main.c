/*
 * SPDX-FileCopyrightText: 2016 Kees Bakker, SODAQ
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the BMX280 temperature, pressure, and
 *              humidity sensor driver
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "bmx280_params.h"
#include "bmx280.h"
#include "xtimer.h"
#include "fmt.h"

#define MAINLOOP_DELAY  (2)         /* read sensor every 2 seconds */

int main(void)
{
    bmx280_t dev;

    puts("BMX280 test application\n");

    puts("+------------Initializing------------+");
    switch (bmx280_init(&dev, &bmx280_params[0])) {
        case BMX280_ERR_BUS:
            puts("[Error] Something went wrong when using the I2C bus");
            return 1;
        case BMX280_ERR_NODEV:
            puts("[Error] Unable to communicate with any BMX280 device");
            return 1;
        default:
            /* all good -> do nothing */
            break;
    }

    puts("Initialization successful\n");

    puts("+------------Calibration Data------------+");
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

#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
    printf("dig_H1: %u\n", dev.calibration.dig_H1);
    printf("dig_H2: %i\n", dev.calibration.dig_H2);
    printf("dig_H3: %i\n", dev.calibration.dig_H3);
    printf("dig_H4: %i\n", dev.calibration.dig_H4);
    printf("dig_H5: %i\n", dev.calibration.dig_H5);
    printf("dig_H6: %i\n", dev.calibration.dig_H6);
#endif

    puts("\n+--------Starting Measurements--------+");
    while (1) {
        /* read temperature, pressure [and humidity] values */
        int16_t temperature = bmx280_read_temperature(&dev);
        uint32_t pressure = bmx280_read_pressure(&dev);
#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
        uint16_t humidity = bme280_read_humidity(&dev);
#endif

        /* format values for printing */
        char str_temp[8];
        size_t len = fmt_s16_dfp(str_temp, temperature, -2);
        str_temp[len] = '\0';
#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
        char str_hum[8];
        len = fmt_s16_dfp(str_hum, humidity, -2);
        str_hum[len] = '\0';
#endif

        /* print values to STDIO */
        printf("Temperature [°C]: %s\n", str_temp);
        printf("   Pressure [Pa]: %" PRIu32 "\n", pressure);
#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
        printf("  Humidity [%%rH]: %s\n", str_hum);
#endif
        puts("\n+-------------------------------------+\n");

        xtimer_sleep(MAINLOOP_DELAY);
    }

    return 0;
}
