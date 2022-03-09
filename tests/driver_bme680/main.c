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
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "bme680.h"
#include "bme680_params.h"
#include "mutex.h"
#include "timex.h"
#include "ztimer.h"

#define BME680_TEST_PERIOD_MS   (5 * MS_PER_SEC)    /* 5s */

ztimer_t timer;

static void _timer_cb(void *arg)
{
    ztimer_set(ZTIMER_MSEC, &timer, BME680_TEST_PERIOD_MS);
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
        BME680_SENSOR(&dev[i]).amb_temp = 25;

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
    ztimer_set(ZTIMER_MSEC, &timer, BME680_TEST_PERIOD_MS);

    while (1)
    {
        struct bme680_field_data data;

        for (unsigned i = 0; i < BME680_NUMOF; i++) {
            /* trigger one measuerment */
            bme680_force_measurement(&dev[i]);
            /* get the duration for the measurement */
            int duration = bme680_get_duration(&dev[i]);
            /* wait for the duration */
            ztimer_sleep(ZTIMER_MSEC, duration);
            /* read the data */
            int res = bme680_get_data(&dev[i], &data);

            if (res == 0 && dev[i].sensor.new_fields) {
#ifndef MODULE_BME680_FP
                printf("[bme680]: dev=%u, "
                       "T = %02d.%02d degC, "
                       "P = %" PRIu32 " Pa, H = %02" PRIu32 ".%03" PRIu32 " %%",
                       i, data.temperature / 100, data.temperature % 100,
                       data.pressure,
                       data.humidity / 1000, data.humidity % 1000);
                /* Avoid using measurements from an unstable heating setup */
                if (data.status & BME680_GASM_VALID_MSK) {
                    printf(", G = %" PRIu32 " ohms", data.gas_resistance);
                }
#else
                printf("[bme680]: dev=%u T = %.2f degC, P = %.2f Pa, H %.3f %%",
                       i, data.temperature, data.pressure, data.humidity);
                /* Avoid using measurements from an unstable heating setup */
                if (data.status & BME680_GASM_VALID_MSK) {
                    printf(", G = %.0f ohms", data.gas_resistance);
                }
#endif
                printf("\n");
            }
            else if (res == 0) {
                printf("[bme680]: no new data\n");
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
