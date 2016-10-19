/*
 * Copyright (C) 2015 HAW Hamburg
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
 * @brief       Test application for the LPS25H pressure sensor
 *
 * @author      René Herthel <rene.herthel@outlook.de>
 *
 * @}
 */

#ifndef TEST_LPS25H_I2C
#error "TEST_LPS25H_I2C not defined"
#endif
#ifndef TEST_LPS25H_ADDR
#error "TEST_LPS25H_ADDR not defined"
#endif

#include <stdio.h>

#include "xtimer.h"
#include "lps25h.h"

#define ODR         LPS25H_ODR_7HZ
#define SLEEP       (850 * 1000U)

int main(void)
{
    lps25h_t dev;
    int temp;
    int pres;

    printf("LPS25H pressure sensor test application\n");
    printf("Initializing LPS25H sensor at I2C_%i ... ", TEST_LPS25H_I2C);
    if (lps25h_init(&dev, TEST_LPS25H_I2C, TEST_LPS25H_ADDR, ODR) == 0) {
        printf("[ OK ]\n");
    }
    else {
        printf("[ FAIL ]\n");
        return 1;
    }

    while (1) {
        temp = lps25h_read_temp(&dev);
        pres = lps25h_read_pres(&dev);

        printf("Pressure: %3i hPa - Temperature: %3i °C\n", pres, temp);

        xtimer_usleep(SLEEP);
    }

    return 0;
}
