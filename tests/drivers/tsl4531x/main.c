/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the TSL4531x Lux sensor
 *
 * @author      Daniel Petry <daniel.petry@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "xtimer.h"
#include "periph/i2c.h"

#include "tsl4531x.h"
#include "tsl4531x_params.h"

#define _100ms_in_us   (100 * 1000u) /* 1 second delay between printf */

int main(void)
{
    tsl4531x_t dev;
    int err;

    puts("TSL4531x test application. Initializing...");

    if ((err = tsl4531x_init(&dev, tsl4531x_params)) < 0) {
        printf("[Error] Device not initialised. Error code: %d\n", err);
        return 1;
    }

    puts("[Info] Initialized, beginning test.");

    while (1) {

        uint16_t lux;

        printf("-------------------------------------------------------------");
        printf("-------------------------\n");

        /* Set into high power mode */
        tsl4531x_set_low_power_mode(&dev, false);

        /* Test simple read - high power mode */
        lux = tsl4531x_simple_read(&dev);
        printf("Illuminance       | High power mode | Synchronous read                 |");
        printf(" [lx] | %u\n", lux);

        /* Determine the actual integration time - how long does it take for a
           value to change?
           Note that if the sensor value doesn't change between integration
           cycles, this will sum the previous integration times. This mostly
           won't happen, but it's best to let this run for a few cycles and take
           the minimum. */
        uint16_t lux_last = lux;
        uint8_t changes = 0;
        uint32_t change_times[2];
        while (changes < 2) {
            lux = tsl4531x_get_sample(&dev);
            if (lux != lux_last) {
                lux_last = lux;
                change_times[changes] = xtimer_now_usec();
                changes++;
            }
        }

        printf("Sample ready time | High power mode | From device                      |");
        printf(" [us] | %lu\n", (unsigned long)(change_times[1] - change_times[0]));

        /* Set into low power mode */
        tsl4531x_set_low_power_mode(&dev, true);

        /* This tests what happens when you read without asking for a sample in
           low power mode. */
        lux = tsl4531x_get_sample(&dev);
        printf("Illuminance       | Low power mode  | Immediate read after mode change |");
        printf(" [lx] | %u\n", lux);
        xtimer_usleep(tsl4531x_time_until_sample_ready(&dev));
        lux = tsl4531x_get_sample(&dev);
        printf("Illuminance       | Low power mode  | One cycle time after mode change |");
        printf(" [lx] | %u\n", lux);

        /* Test synchronous read - low power mode */
        lux = tsl4531x_simple_read(&dev);
        printf("Illuminance       | Low power mode  | Synchronous read                 |");
        printf(" [lx] | %u\n", lux);

        /* Test asynchronous read - low power mode */
        tsl4531x_start_sample(&dev);

        /* Verify that the stated time until sample ready is reasonable. */
        uint32_t t = tsl4531x_time_until_sample_ready(&dev);
        xtimer_usleep(t);
        lux = tsl4531x_get_sample(&dev);
        printf("Illuminance       | Low power mode  | Asynchronous read                |");
        printf(" [lx] | %u\n", lux);
        printf("Sample ready time | Low power mode  | From driver                      |");
        printf(" [us] | %lu\n", (unsigned long)t);

        xtimer_usleep(_100ms_in_us);
    }

    return 0;
}
