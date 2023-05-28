/*
 * Copyright (C) 2017 Mesotic SAS
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
 * @brief       ADXL345 test application
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <stdio.h>

#include "adxl345.h"
#include "adxl345_params.h"
#include "xtimer.h"

#define SLEEP_DELAY 100 * 1000U

int main(void)
{
    adxl345_t dev;
    adxl345_data_t data;

    puts("ADXL345 test application");
    printf("Initializing ADXL345 accelerometer at I2C_DEV(%i)... ",
           adxl345_params[0].i2c);

    if (adxl345_init(&dev, &adxl345_params[0]) == ADXL345_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

   while(1) {
        adxl345_read(&dev, &data);
        printf("Acceleration [in mg]: X axis:%d Y axis:%d Z axis:%d\n",
               (int)data.x, (int)data.y, (int)data.z);
        xtimer_usleep(SLEEP_DELAY);
    }
    return 0;
}
