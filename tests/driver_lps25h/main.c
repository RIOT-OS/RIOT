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

#include <stdio.h>

#include "xtimer.h"
#include "lps25h.h"
#include "lps25h_params.h"

int main(void)
{
    lps25h_t dev;
    int16_t temp;
    uint16_t pres;

    puts("LPS25H pressure sensor test application");

    puts("Initializing LPS25H sensor");
    if (lps25h_init(&dev, &lps25h_params[0]) == 0) {
        printf("[ OK ]\n");
    }
    else {
        printf("[ FAILED ]\n");
        return 1;
    }

    while (1) {
        temp = lps25h_read_temperature(&dev);
        pres = lps25h_read_pressure(&dev);

        printf("Pressure: %3i hPa - Temperature: %3i °C\n", pres, temp);

        xtimer_sleep(1);
    }

    return 0;
}
