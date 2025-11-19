/*
 * SPDX-FileCopyrightText: 2016 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include <stdio.h>

#include "timex.h"
#include "ztimer.h"

#include "si70xx_params.h"
#include "si70xx.h"

int main(void)
{
    si70xx_t dev;

    puts("SI70XX temperature and humidity sensor test application");

    /* initialize the sensor */
    puts("Initializing sensor... ");

    if (si70xx_init(&dev, &si70xx_params[0]) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    printf("Found SI70%02d sensor, revision %d\n", si70xx_get_id(&dev),
           si70xx_get_revision(&dev));

    /* read temperature and humidity every 1 seconds */
    bool both = false;

    while (1) {
        int16_t temperature;
#if SI70XX_HAS_HUMIDITY_SENSOR
        uint16_t humidity;

        /* rotate the way of getting the data */
        if (both) {
            si70xx_get_both(&dev, &humidity, &temperature);
        }
        else {
            temperature = si70xx_get_temperature(&dev);
            humidity = si70xx_get_relative_humidity(&dev);
        }
#else /* SI70XX_HAS_HUMIDITY_SENSOR */
        temperature = si70xx_get_temperature(&dev);
#endif /* SI70XX_HAS_HUMIDITY_SENSOR */
        both = !both;

        /* display results */
#if SI70XX_HAS_HUMIDITY_SENSOR
        printf("relative humidity: %d.%02d\n", humidity / 100, humidity % 100);
#endif /* SI70XX_HAS_HUMIDITY_SENSOR */
        printf("temperature: %d.%02d C\n", temperature / 100,
               temperature % 100);

        /* sleep between measurements */
        ztimer_sleep(ZTIMER_MSEC, MS_PER_SEC);  /* 1s delay */
    }

    return 0;
}
