/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the TCS37727 sensor driver.
 *
 * @author      Felix Siebel <f.siebel@phytec.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "tcs37727.h"
#include "tcs37727_params.h"

#define SLEEP       (1 * US_PER_SEC)

int main(void)
{
    tcs37727_t dev;
    tcs37727_data_t data;

    puts("TCS37727 RGBC Data; Sensor driver test application\n");
    printf("Initializing first configured TCS37727 sensor...");

    if (tcs37727_init(&dev, &tcs37727_params[0]) == TCS37727_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    while (1) {
        tcs37727_read(&dev, &data);
        printf("R: %5"PRIu32" G: %5"PRIu32" B: %5"PRIu32" C: %5"PRIu32"\n",
               data.red, data.green, data.blue, data.clear);
        printf("CT : %5"PRIu32" Lux: %6"PRIu32" AGAIN: %2d ATIME %"PRIu32"\n",
               data.ct, data.lux, dev.again, dev.p.atime);

        xtimer_usleep(SLEEP);
    }

    return 0;
}
