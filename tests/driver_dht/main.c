/*
 * Copyright (C) 2015 Ludwig Knüpfer, Christian Mehlis
 *               2016 Freie Universität Berlin
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
#include "dht.h"
#include "dht_params.h"

extern dht_t dht_devs[];

int main(void)
{
    puts("DHT temperature and humidity sensor test application\n");

    /* periodically read temp and humidity values */
    while (1) {
        for (unsigned i = 0; i < DHT_NUMOF; i++) {
            dht_t *dev = &dht_devs[i];
            int16_t temp, hum;
            int16_t dec_temp, dec_hum, int_temp, int_hum;

            if (dht_read(dev, &temp, &hum) == -1) {
                puts("error reading data");
                continue;
            }

            /* split up values into integral and fractional parts for nicer
             * printing
             * TODO: this should be done in some kind of library... */
            int_temp = temp / 10;
            dec_temp = temp  - (int_temp * 10);
            int_hum = hum / 10;
            dec_hum = hum - (int_hum * 10);

            printf("DHT device #%i - ", i);
            printf("temp: %i.%i°C, ", int_temp, dec_temp);
            printf("relative humidity: %i.%i%%\n", int_hum, dec_hum);

            xtimer_usleep(2000 * MS_IN_USEC);
        }
    }

    return 0;
}
