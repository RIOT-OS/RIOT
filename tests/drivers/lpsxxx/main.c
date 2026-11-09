/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the LPS331AP/LPS25HB/LPS22HB/LPS22HH
 *              pressure sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "lpsxxx.h"
#include "lpsxxx_params.h"

int main(void)
{
    lpsxxx_t dev;

    printf("Test application for %s pressure sensor\n\n", LPSXXX_SAUL_NAME);
    printf("Initializing %s sensor\n", LPSXXX_SAUL_NAME);
    if (lpsxxx_init(&dev, &lpsxxx_params[0]) != LPSXXX_OK) {
        puts("Initialization failed");
        return 1;
    }

    uint16_t pres;
    int16_t temp;
    while (1) {
        lpsxxx_enable(&dev);
        xtimer_sleep(1); /* wait a bit for the measurements to complete */

        lpsxxx_read_temp(&dev, &temp);
        lpsxxx_read_pres(&dev, &pres);
        lpsxxx_disable(&dev);

        /* the sign apart: the fraction of -1.50 would print as -1.-50 */
        unsigned temp_abs = (temp < 0) ? -temp : temp;

        printf("Pressure value: %ihPa - Temperature: %s%u.%02u°C\n",
               pres, (temp < 0) ? "-" : "", temp_abs / 100, temp_abs % 100);
    }

    return 0;
}
