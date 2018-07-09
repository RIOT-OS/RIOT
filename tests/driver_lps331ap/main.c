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

#include <stdio.h>

#include "xtimer.h"
#include "lps331ap.h"
#include "lps331ap_params.h"

#define SLEEP       (250 * 1000U)

int main(void)
{
    lps331ap_t dev;

    puts("LPS331AP pressure sensor test application\n");
    puts("Initializing LPS331AP sensor");
    if (lps331ap_init(&dev, &lps331ap_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    while (1) {
        int pres = lps331ap_read_pres(&dev);
        int temp = lps331ap_read_temp(&dev);

        int pres_abs = pres / 1000;
        pres -= pres_abs * 1000;
        int temp_abs = temp / 1000;
        temp -= temp_abs * 1000;

        printf("Pressure value: %2i.%03i bar - Temperature: %2i.%03i °C\n",
               pres_abs, pres, temp_abs, temp);

        xtimer_usleep(SLEEP);
    }

    return 0;
}
