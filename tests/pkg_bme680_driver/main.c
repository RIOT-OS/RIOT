/*
 * Copyright (C) 2018 Mesotic SAS
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
 * @brief       Test application for the bme680_driver package.
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <stdio.h>

#include "bme680.h"
#include "bme680_defs.h"
#include "bme680_hal.h"

#ifdef MODULE_PERIPH_SPI
#include "periph/spi.h"
#endif

#include "xtimer.h"


int main(void)
{
    struct bme680_dev dev;

    /* You may assign a chip select identifier to be handled later */
    dev.dev_id = 0;

    /* amb_temp can be set to 25 prior to configuring the gas sensor 
     * or by performing a few temperature readings without operating the gas sensor.
     */
    dev.amb_temp = 25;
    dev.delay_ms = ms_sleep;

#ifdef MODULE_PERIPH_I2C
    dev.intf = BME680_I2C_INTF;
    dev.read = i2c_read_hal;
    dev.write = i2c_write_hal;
#else
    dev.intf = BME680_SPI_INTF;
    dev.read = spi_read_hal;
    dev.write = spi_write_hal;
    spi_init_cs(SPI_DEV(dev.dev_id), SPI_NSS_PIN);
#endif

    printf("Intialize BME680 sensor...");
    if(bme680_init(&dev) != BME680_OK) {
        puts("failed");
    }
    else {
        puts("OK");
    }


    uint8_t set_required_settings;
    uint8_t rslt;
    /* Set the temperature, pressure and humidity settings */
    dev.tph_sett.os_hum = BME680_OS_2X;
    dev.tph_sett.os_pres = BME680_OS_4X;
    dev.tph_sett.os_temp = BME680_OS_8X;
    dev.tph_sett.filter = BME680_FILTER_SIZE_3;

    /* Set the remaining gas sensor settings and link the heating profile */
    dev.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    /* Create a ramp heat waveform in 3 steps */
    dev.gas_sett.heatr_temp = 320; /* degree Celsius */
    dev.gas_sett.heatr_dur = 150; /* milliseconds */

    /* Select the power mode */
    /* Must be set before writing the sensor configuration */
    dev.power_mode = BME680_FORCED_MODE;

    /* Set the required sensor settings needed */
    set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL
                            | BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;

    /* Set the desired sensor configuration */
    rslt = bme680_set_sensor_settings(set_required_settings, &dev);

    /* Set the power mode */
    rslt = bme680_set_sensor_mode(&dev);

  /* Get the total measurement duration so as to sleep or wait till the
     * measurement is complete */
    uint16_t meas_period;
    bme680_get_profile_dur(&meas_period, &dev);

    struct bme680_field_data data;

    while(1)
    {
        dev.delay_ms(meas_period); /* Delay till the measurement is ready */

        rslt = bme680_get_sensor_data(&data, &dev);
        if(!rslt)
#ifndef BME680_FLOAT_POINT_COMPENSATION
        printf("T: %d degC, P: %ld hPa, H %ld ", data.temperature,
            data.pressure, data.humidity);
        /* Avoid using measurements from an unstable heating setup */
        if(data.status & BME680_GASM_VALID_MSK)
            printf(", G: %ld ohms", data.gas_resistance);
#else
        printf("T: %.2f degC, P: %.2f hPa, H %2.f ", data.temperature,
            data.pressure, data.humidity);
        /* Avoid using measurements from an unstable heating setup */
        if(data.status & BME680_GASM_VALID_MSK)
            printf(", G: %f ohms", data.gas_resistance);
#endif
        printf("\r\n");

        /* Trigger the next measurement if you would like to read data out
           continuously */
        if (dev.power_mode == BME680_FORCED_MODE) {
            rslt = bme680_set_sensor_mode(&dev);
        }
    }

    return 0;
}
