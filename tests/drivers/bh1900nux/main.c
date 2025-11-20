/*
 * SPDX-FileCopyrightText: 2020 Nalys
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the BH1900NUX Temperature sensor
 *
 * @author      Wouter Symons <wsymons@nalys-group.com>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "bh1900nux.h"
#include "bh1900nux_params.h"

#define RATE        (200LU * US_PER_MS)      /* 200ms */

int main(void)
{
    int res = 0;
    int16_t temp = 0;
    bh1900nux_t dev;
    xtimer_ticks32_t last = xtimer_now();

    puts("bh1900nux temperature sensor test\n");

    /* initialize the device */
    res = bh1900nux_init(&dev, &bh1900nux_params[0]);
    if (res != BH1900NUX_OK) {
        puts("error: unable to initialize sensor [I2C initialization error]");
        return 1;
    }

    /* periodically read the sensor */
    while (1) {
        bh1900nux_read(&dev, &temp);
        printf("temperature: %d.%d deg. C\n", (int)temp / 1000, temp % 1000);
        xtimer_periodic_wakeup(&last, RATE);
    }

    return 0;
}
