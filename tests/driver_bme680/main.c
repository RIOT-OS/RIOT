/*
 * Copyright (C) 2018 Dylan Laduranty, Mesotic SAS
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
 * @brief       Test application for the BME680 temperature, pressure
 *              and humidity and gas sensor.
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "bme680.h"
#include "bme680_params.h"
#include "xtimer.h"

#define MAINLOOP_DELAY  (15 * 1000 * 1000u)
#define HEATR_DUR        2000
#define AMB_TEMP        25
#define HEATER_TEMP     200

int main(void)
{
    bme680_t dev;
    bme680_data_t data;
    int result;
    uint32_t duration;

    puts("BME680 test application\n");

    result = bme680_init(&dev, &bme680_params[0]);
    if (result == BME680_COM_FAIL) {
        puts("[Error]: I2C communication failed");
        return -1;
    }

    if (result == BME680_BAD_ID) {
        printf("[Error]: bad ID read from sensor 0x%02X\n",
               bme680_params[0].i2c_addr);
        return -2;
    }

    printf("Initialization successful\n\n");
    dev.tph_sett.os_hum = BME680_OS_1X;
    dev.tph_sett.os_pres = BME680_OS_16X;
    dev.tph_sett.os_temp = BME680_OS_2X;
    /* Set the remaining gas sensor settings and link the heating profile */
    dev.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    dev.gas_sett.heatr_dur = HEATR_DUR;
    dev.amb_temp = AMB_TEMP;

    dev.gas_sett.heatr_temp = HEATER_TEMP;

    bme680_get_profile_dur(&dev, &duration);
    duration += dev.gas_sett.heatr_dur * US_PER_MS;
    printf("measurement duration %lu [ms]\n", duration / 1000 );
    bme680_set_sensor_settings(&dev, BME680_OST_SEL | BME680_OSP_SEL |
                               BME680_OSH_SEL | BME680_GAS_SENSOR_SEL);
    printf("Prepare measurement...\n");
    while(1) {
    bme680_set_sensor_mode(&dev, BME680_FORCED_MODE);
    xtimer_usleep(duration);
    result = bme680_get_sensor_data(&dev, &data);
    if (result != 0) {
        printf("[Error] fail retrieve value:%d", result);
    }

    printf("Temperature:%d.%d[°C]\nHumidity:%lu[%%]\nPressure:%lu[Pa]\n",
           data.temperature / 100, data.temperature % 100 ,data.humidity / 1000,
           data.pressure);

    printf("Gas:%lu[Ohms]\n\n", data.gas_resistance);
    xtimer_usleep(MAINLOOP_DELAY);
    }

}
