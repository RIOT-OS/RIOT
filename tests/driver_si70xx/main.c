/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
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
 * @brief       Test application for the Si7006/13/20/21 sensor driver
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#ifndef TEST_I2C
#error "TEST_I2C not defined"
#endif

#ifndef TEST_I2C_ADDR
#error "TEST_I2C_ADDR not defined"
#endif

#ifndef TEST_PIN_EN
#error "TEST_PIN_EN not defined"
#endif

#include <stdio.h>

#include "periph/gpio.h"

#include "xtimer.h"

#include "si70xx.h"

int main(void)
{
    si70xx_t dev;

    puts("SI7021 temperature and humidity sensor test application\n");

    /* enable the sensor if test pin given */
    if (TEST_PIN_EN != GPIO_UNDEF) {
        printf("Toggling enable pin...");

        if (gpio_init(TEST_PIN_EN, GPIO_OUT) == 0) {
            puts("[OK]\n");
        }
        else {
            puts("[Failed]\n");
            return 1;
        }

        gpio_set(TEST_PIN_EN);
    }

    /* initialize the sensor */
    printf("Initializing sensor...");

    if (si70xx_init(&dev, TEST_I2C, TEST_I2C_ADDR) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    /* run sensor test */
    printf("Testing sensor communication...");

    if (si70xx_test(&dev) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    /* print device id */
    printf("Identified sensor as the Si70%02i\n", si70xx_get_id(&dev));

    /* read temperature and humidity every 1 seconds */
    bool both = false;

    int16_t temperature;
    uint16_t humidity;

    while (1) {
        /* rotate the way of getting the data */
        if (both) {
            si70xx_get_both(&dev, &humidity, &temperature);
        }
        else {
            temperature = si70xx_get_temperature(&dev);
            humidity = si70xx_get_relative_humidity(&dev);
        }

        both = !both;

        /* display results */
        printf("relative humidity: %d.%02d\n", humidity / 100, humidity % 100);
        printf("temperature: %d.%02d C\n", temperature / 100, temperature % 100);

        /* sleep between measurements */
        xtimer_usleep(1000 * MS_IN_USEC);
    }

    return 0;
}
