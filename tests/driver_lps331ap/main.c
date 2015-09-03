/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief       Test application for the LPS331AP pressure sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifndef TEST_LPS331AP_I2C
#error "TEST_LPS331AP_I2C not defined"
#endif
#ifndef TEST_LPS331AP_ADDR
#error "TEST_LPS331AP_ADDR not defined"
#endif

#include <stdio.h>

#include "xtimer.h"
#include "lps331ap.h"

#define RATE        LPS331AP_RATE_7HZ
#define SLEEP       (250 * 1000U)

int main(void)
{
    lps331ap_t dev;
    int temp, pres;
    int temp_abs, pres_abs;

    puts("LPS331AP pressure sensor test application\n");
    printf("Initializing LPS331AP sensor at I2C_%i... ", TEST_LPS331AP_I2C);
    if (lps331ap_init(&dev, TEST_LPS331AP_I2C, TEST_LPS331AP_ADDR, RATE) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    while (1) {
        pres = lps331ap_read_pres(&dev);
        temp = lps331ap_read_temp(&dev);

        pres_abs = pres / 1000;
        pres -= pres_abs * 1000;
        temp_abs = temp / 1000;
        temp -= temp_abs * 1000;

        printf("Pressure value: %2i.%03i bar - Temperature: %2i.%03i °C\n",
               pres_abs, pres, temp_abs, temp);

        xtimer_usleep(SLEEP);
    }

    return 0;
}
