/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for HTU21D humidity and temperature sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * The test application demonstrates the use of the HTU21D. Either
 * single measurement steps or the blocking function #htu21d_read
 * can be used. Blocking function is activated by defining
 * USE_HTU21D_BLOCKING.
 * ```
 * CFLAGS="-DUSE_HTU21D_BLOCKING" make -C tests/driver_htu21d/ BOARD=...
 * ```
 */

#include <stdio.h>
#include <stdlib.h>

#include "thread.h"
#include "xtimer.h"

#include "htu21d.h"
#include "htu21d_params.h"

#define SLEEP   (100 * US_PER_MS)

int main(void)
{
    htu21d_t dev;

    puts("HTU21D driver test application\n");
    puts("Initializing HTU21D sensor");

    /* initialize the sensor with default configuration parameters */
    if (htu21d_init(&dev, &htu21d_params[0]) == HTU21D_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    while (1) {
        int16_t tmp;
        int16_t hum;
        int res;

        /* wait between measuremens or do something else */
        xtimer_usleep(SLEEP);

#ifdef USE_HTU21D_BLOCKING
        /* read and print data with single blocking function */
        res = htu21d_read(&dev, &tmp, &hum, 0, 0);
#else
        /* trigger temperature management */
        res = htu21d_measure_tmp(&dev);
        if (res > 0) {
            /* wait the returned measurement time or do something else */
            xtimer_usleep(res * US_PER_MS);
            /* fetch the data */
            res = htu21d_fetch_tmp (&dev, &tmp, NULL);
        }
        /* trigger relative humidity management */
        res = htu21d_measure_hum(&dev);
        if (res > 0) {
            /* wait the returned measurement time or do something else */
            xtimer_usleep(res * US_PER_MS);
            /* fetch the data */
            res = htu21d_fetch_hum (&dev, &hum, NULL);
        }
#endif
        if (res == HTU21D_OK) {
            /* print resulting floats as integers */
            int16_t tmp_abs = abs(tmp);
            printf("temp [°C/100]: %c%d.%02d, ", (tmp < 0) ? '-' : '+',
                   tmp_abs/100, tmp_abs - (tmp_abs / 100) * 100);
            printf("hum [%%/100]: +%d.%02d\n",
                   hum/100, hum - (hum / 100) * 100);
        }
    }

    return 0;
}
