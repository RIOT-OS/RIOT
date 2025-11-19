/*
 * SPDX-FileCopyrightText: 2015 Ludwig Knüpfer
 * SPDX-FileCopyrightText: 2015 Christian Mehlis
 * SPDX-FileCopyrightText: 2016-2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "dht.h"
#include "dht_params.h"
#include "time_units.h"
#include "ztimer.h"

#define DELAY           (2 * US_PER_SEC)

int main(void)
{
    dht_t dev;
    int16_t temp, hum;

    puts("DHT temperature and humidity sensor test application\n");

    /* initialize first configured sensor */
    printf("Initializing DHT sensor...\t");
    if (dht_init(&dev, &dht_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    /* periodically read temp and humidity values */
    while (1) {
        ztimer_sleep(ZTIMER_USEC, DELAY);

        if (dht_read(&dev, &temp, &hum) != 0) {
            puts("Error reading both values");
            continue;
        }
        if (dht_read(&dev, &temp, NULL) != 0) {
            puts("Error reading just temperature");
            continue;
        }
        if (dht_read(&dev, NULL, &hum) != 0) {
            puts("Error reading just humidity");
            continue;
        }

        printf("DHT values - temp: %d.%d°C - relative humidity: %d.%d%%\n",
               temp/10, temp%10, hum/10, hum%10);
    }

    return 0;
}
