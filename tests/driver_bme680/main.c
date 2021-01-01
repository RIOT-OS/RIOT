/*
 * Copyright (C) 2018 Mesotic SAS
 *               2020 Gunar Schorcht
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
 * @brief       Test application for the bme680_driver package.
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Jana Eisoldt <jana.eisoldt@st.ovgu.de>
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "bme680.h"
#include "bme680_params.h"
#include "mutex.h"
#include "xtimer.h"
#define ENABLE_DEBUG 1
#include "debug.h"

#define BME680_TEST_PERIOD_US   (5 * US_PER_SEC)

xtimer_t timer;

static void _timer_cb(void *arg)
{
    xtimer_set(&timer, BME680_TEST_PERIOD_US);
    mutex_unlock(arg);
}

int main(void)
{
    mutex_t timer_mtx = MUTEX_INIT_LOCKED;

    bme680_t dev[BME680_NUMOF];

    for (unsigned i = 0; i < BME680_NUMOF; i++) {
        /*
         * We use a fix temperature here. The ambient temperature could be
         * determined by performing a few temperature readings without
         * operating the gas sensor or by another temperature sensor. Function
         * bme680_set_ambient_temp can be used at any time to change it.
         */
        (&dev[i])->params.ambient_temp = 25;

        printf("Initialize BME680 sensor %u ... ", i);
        if (bme680_init(&dev[i], &bme680_params[i]) != BME680_OK) {
            puts("failed");
            return -1;
        }
        else {
            puts("OK");
        }
    }

    timer.callback = _timer_cb;
    timer.arg = &timer_mtx;
    xtimer_set(&timer, BME680_TEST_PERIOD_US);

    while (1)
    {
        bme680_data_t data;

        for (unsigned i = 0; i < BME680_NUMOF; i++) {

            int res = bme680_read(&dev[i], &data);

            if (res == BME680_OK)
                {
                printf("[bme680]: dev=%u, "
                       "T = %02lu.%02lu degC, "
                       "P = %" PRIu32 " Pa, H = %02" PRIu32 ".%03" PRIu32 " %%",
                       i, data.temperature / 100, data.temperature % 100,
                       data.pressure,
                       data.humidity / 1000, data.humidity % 1000);
                /* Avoid using measurements from an unstable heating setup */
                if (data.gas_status == 1) {
                    printf(", G = %" PRIu32 " ohms", data.gas_resistance);
                }
                printf("\n");
            }
            else {
                printf("[bme680]: read data failed with reason %d\n", res);
            }
        }
        printf("+-----------------------------------------+\n");
        mutex_lock(&timer_mtx);
    }
    /* Should never reach here */
    return 0;
}
