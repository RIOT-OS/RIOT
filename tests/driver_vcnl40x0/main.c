/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for Vishay VCNL40X0 proximity and ambient light sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * The test application demonstrates the use of the VCNL40X0 driver. It uses a
 * VCNL4010, VCNL4020, or VCNL3020 sensor connected to I2C_DEV(0) with
 * following configuration parameters:
 *
 * - Ambient light measurement with
 *      - a rate of 10 per second (#VCNL40X0_ALS_RATE_10) and
 *      - averaging over 32 conversions (#VCNL40X0_ALS_AVG_32)
 *
 * - Proximity measurement with
 *      - a rate of 31.25 per seconds (#VCNL40X0_PRX_RATE_31_25) and
 *      - an IR LED current of 20 mA.
 *
 * The application fetches data periodically every 100 ms.
 */

#ifndef VCNL40X0_PARAM_ALS_RATE
#define VCNL40X0_PARAM_ALS_RATE     (VCNL40X0_ALS_RATE_10)
#endif
#ifndef VCNL40X0_PARAM_PRX_RATE
#define VCNL40X0_PARAM_PRX_RATE     (VCNL40X0_PRX_RATE_31_25)
#endif

#include <stdio.h>

#include "thread.h"
#include "xtimer.h"

#include "vcnl40x0.h"
#include "vcnl40x0_params.h"

#define SLEEP   (100 * US_PER_MS)

int main(void)
{
    vcnl40x0_t dev;

    puts("VCNL40X0 proximity and ambient light sensor driver test application\n");
    puts("Initializing VCNL40X0 sensor");

    /* initialize the sensor with default configuration parameters */
    if (vcnl40x0_init(&dev, &vcnl40x0_params[0]) == VCNL40X0_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    uint16_t min_prox =  UINT16_MAX;    /* proximity offset value */

    while (1) {
        uint16_t prox;
        uint16_t als;

        /* wait longer than period of VCNL40X0 DOR */
        xtimer_usleep(SLEEP);

        /* read and print data */
        if (vcnl40x0_read_prox(&dev, &prox) == VCNL40X0_OK) {

            /*
             * Simple proximity offset correction:
             * A number of disturbing effects such as DC noise, sensor
             * coverage, or surrounding objects cause an offset in the
             * measured proximity values. Find the minimum value ever
             * measured and subtract it from the current reading. The
             * minimum value is assumed to be 0 (no proximity).
             */
            min_prox = (prox < min_prox) ? prox : min_prox;
            prox -= min_prox;

            printf("Proximity [cnts]: %5" PRIu16 ",  ", prox);
        }

        if (vcnl40x0_read_als(&dev, &als) == VCNL40X0_OK) {
            printf("ALS [lux] %5" PRIu16 ".%02" PRIu16, als/4, (als % 4) * 25);
        }

        printf("\n");
    }
    return 0;
}
