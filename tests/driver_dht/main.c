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

#include <stdio.h>

#include "xtimer.h"
#include "dht.h"
#include "dht_params.h"

extern dht_t dht_devs[];

int main(void)
{
    dht_data_t data;
    float temp, hum;

    puts("DHT temperature and humidity sensor test application\n");

    /* periodically read temp and humidity values */
    while (1) {
        for (unsigned i = 0; i < DHT_NUMOF; i++) {
            dht_t *dev = &dht_devs[i];

            if (dht_read_raw(dev, &data) == -1) {
                puts("error reading data");
            }
            dht_parse(dev, &data, &hum, &temp);
            printf("raw relative humidity: %i\nraw temperature: %i C\n", data.humidity, data.temperature);
            printf("relative humidity: %i\ntemperature: %i C\n", (int) hum, (int) temp);
            xtimer_usleep(2000 * MS_IN_USEC);
        }
    }

    return 0;
}
