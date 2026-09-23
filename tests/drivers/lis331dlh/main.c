/*
 * SPDX-FileCopyrightText: 2026 Arseny Sukhman
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the LIS331DLH accelerometer driver
 *
 * @author      Arseny Sukhman
 *
 * @}
 */

#include <stdio.h>

#include "lis331dlh.h"
#include "lis331dlh_params.h"
#include "ztimer.h"

int main(void)
{
    lis331dlh_t dev;
    lis331dlh_data_t data;

    puts("LIS331DLH accelerometer test");

    if (lis331dlh_init(&dev, &lis331dlh_params[0]) < 0) {
        puts("Initialization failed");
        return 1;
    }

    while (1) {
        if (lis331dlh_read(&dev, &data) == 0) {
            printf("x: %i mg, y: %i mg, z: %i mg\n", data.x, data.y, data.z);
        }
        else {
            puts("Read failed");
        }
        ztimer_sleep(ZTIMER_MSEC, 1000);
    }

    return 0;
}
