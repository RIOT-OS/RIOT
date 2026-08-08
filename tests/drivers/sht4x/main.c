/*
 * SPDX-FileCopyrightText: 2026 RIOT Contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sht4x.h"
#include "sht4x_params.h"
#include "ztimer.h"

int main(void)
{
    sht4x_dev_t dev;
    int res;
    int16_t temp;
    int16_t hum;

    puts("SHT4X test application\n");

    if ((res = sht4x_init(&dev, &sht4x_params[0])) != SHT4X_OK) {
        puts("Initialization failed\n");
        return 1;
    }

    while (1) {
        if ((res = sht4x_read(&dev, &temp, &hum)) == SHT4X_OK) {
            printf("Temperature [°C]: %d.%02d\n"
                   "Relative Humidity [%%]: %d\n"
                   "+-------------------------------------+\n",
                   temp / 100, abs(temp % 100), hum);
        }
        else {
            printf("Could not read data from sensor, error %d\n", res);
        }

        ztimer_sleep(ZTIMER_MSEC, 1000);
    }

    return 0;
}
