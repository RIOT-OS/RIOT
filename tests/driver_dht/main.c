/*
 * Copyright (C) 2015 Ludwig Knüpfer, Christian Mehlis
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
 * @brief       Test application for the dht humidity and temperature sensor driver
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#ifndef DHT_TYPE
#error "DHT_TYPE not defined"
#endif

#ifndef DHT_GPIO
#error "DHT_GPIO not defined"
#endif

#include <stdio.h>

#include "xtimer.h"

#include "dht.h"

int main(void)
{
    dht_t dev;

    puts("DHT temperature and humidity sensor test application\n");

    printf("Initializing DHT sensor at GPIO_%ld... ", (long)DHT_GPIO);
    if (dht_init(&dev, DHT_TYPE, DHT_GPIO) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    dht_data_t data;
    float temp, hum;
    while (1) {

        if (dht_read_raw(&dev, &data) == -1) {
            puts("error reading data");
        }
        dht_parse(&dev, &data, &hum, &temp);
        printf("raw relative humidity: %i\nraw temperature: %i C\n", data.humidity, data.temperature);
        printf("relative humidity: %i\ntemperature: %i C\n", (int) hum, (int) temp);
        xtimer_usleep(2000 * MS_IN_USEC);
    }

    return 0;
}
