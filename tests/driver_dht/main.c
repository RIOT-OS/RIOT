/*
 * Copyright (C) 2015 Ludwig Knüpfer, Christian Mehlis
 *               2016-2017 Freie Universität Berlin
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "timex.h"
#include "fmt.h"
#include "dht.h"
#include "dht_params.h"

#define DELAY           (2 * US_PER_SEC)

int main(void)
{
    dht_t dev;
    int16_t temp, hum;
    char temp_s[10];
    char hum_s[10];

    puts("DHT temperature and humidity sensor test application\n");

    /* initialize first configured sensor */
    printf("Initializing DHT sensor...\t");
    if (dht_init(&dev, &dht_params[0]) == DHT_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    /* periodically read temp and humidity values */
    while (1) {
        if (dht_read(&dev, &temp, &hum) != DHT_OK) {
            puts("Error reading values");
            continue;
        }

        size_t n = fmt_s16_dfp(temp_s, temp, 1);
        temp_s[n] = '\0';
        n = fmt_s16_dfp(hum_s, hum, 1);
        hum_s[n] = '\0';

        printf("DHT values - temp: %s°C - relative humidity: %s%%\n",
                temp_s, hum_s);

        xtimer_usleep(DELAY);
    }

    return 0;
}
