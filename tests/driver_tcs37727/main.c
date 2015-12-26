/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
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
 * @brief       Test application for the TCS37727 sensor driver.
 *
 * @author      Felix Siebel <f.siebel@phytec.de>
 *
 * @}
 */

#ifndef TEST_TCS37727_I2C
#error "TEST_TCS37727_I2C not defined"
#endif
#ifndef TEST_TCS37727_ADDR
#error "TEST_TCS37727_ADDR not defined"
#endif

#include <stdio.h>

#include "xtimer.h"
#include "tcs37727.h"

#define SLEEP       (1000 * 1000U)

int main(void)
{
    tcs37727_t dev;
    tcs37727_data_t data;

    puts("TCS37727 RGBC Data; Sensor driver test application\n");
    printf("Initializing TCS37727 sensor at I2C_%i... ", TEST_TCS37727_I2C);

    if (tcs37727_init(&dev, TEST_TCS37727_I2C, TEST_TCS37727_ADDR,
                      TCS37727_ATIME_DEFAULT) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    if (tcs37727_set_rgbc_active(&dev)) {
        puts("Measurement start failed.");
        return -1;
    }

    while (1) {
        tcs37727_read(&dev, &data);
        printf("R: %5"PRIu32" G: %5"PRIu32" B: %5"PRIu32" C: %5"PRIu32"\n",
               data.red, data.green, data.blue, data.clear);
        printf("CT : %5"PRIu32" Lux: %6"PRIu32" AGAIN: %2d ATIME %d\n",
               data.ct, data.lux, dev.again, dev.atime_us);

        xtimer_usleep(SLEEP);
    }

    return 0;
}
