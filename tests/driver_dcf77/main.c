/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for the DCF77 device driver
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include "mutex.h"
#include "xtimer.h"

#include "dcf77_params.h"
#include "dcf77.h"

static void dcf77_callback(dcf77_t *dev, void *arg)
{
    (void) dev;
    mutex_unlock(arg);
}

int main(void)
{
    dcf77_t sensor;
    mutex_t mutex = MUTEX_INIT_LOCKED;

    printf("DCF77 test application\n");

    /* initialize the sensor with default configuration parameters */
    if (dcf77_init(&sensor, &dcf77_params[0]) != DCF77_OK) {
        puts("Initialization failed\n");
        return -1;
    }

    printf("DCF77 Module initialized \n");

    dcf77_set_tick_cb(&sensor, dcf77_callback, &mutex);

    while (1) {
        struct tm time;
        char buffer[32];
        mutex_lock(&mutex);

        dcf77_get_time(&sensor, &time);

        strftime(buffer, sizeof(buffer), "Date: %d.%m.%Y Time: %H:%M.", &time);
        puts (buffer);

    }

    return 0;
}
