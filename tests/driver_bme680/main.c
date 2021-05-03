/*
 * Copyright (C) 2018 Mesotic SAS
 *               2020 Gunar Schorcht
 *               2020 OVGU Magdeburg
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
 * @brief       Test application for the bme680_spi and bme680_i2c driver.
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Jana Eisoldt <jana.eisoldt@st.ovgu.de>
 * @}
 */

#include <stdio.h>

#include "board.h"

#if MODULE_BME680_I2C
#include "bme680_i2c.h"
#include "bme680_i2c_params.h"
#endif

#if MODULE_BME680_SPI
#include "bme680_spi.h"
#include "bme680_spi_params.h"
#endif

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

#if IS_USED(MODULE_BME680_I2C)
    bme680_i2c_t dev_i2c[BME680_I2C_NUMOF];

    for (unsigned i = 0; i < BME680_I2C_NUMOF; i++) {
        /*
         * We use a fix temperature here. The ambient temperature could be
         * determined by performing a few temperature readings without
         * operating the gas sensor or by another temperature sensor. Function
         * bme680_set_ambient_temp can be used at any time to change it.
         */
        (&dev_i2c[i])->common.params.ambient_temp = 25;

        printf("Initialize BME680 sensor %u ... ", i);
        if (bme680_i2c_init(&dev_i2c[i], &bme680_i2c_params[i]) < 0) {
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

        for (unsigned i = 0; i < BME680_I2C_NUMOF; i++) {

            int res = bme680_i2c_read(&dev_i2c[i], &data);

            if (res == 0)
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

#endif

#if IS_USED(MODULE_BME680_SPI)
    bme680_spi_t dev_spi[BME680_SPI_NUMOF];

    for (unsigned i = 0; i < BME680_SPI_NUMOF; i++) {
        /*
         * We use a fix temperature here. The ambient temperature could be
         * determined by performing a few temperature readings without
         * operating the gas sensor or by another temperature sensor. Function
         * bme680_set_ambient_temp can be used at any time to change it.
         */
        (&dev_spi[i])->common.params.ambient_temp = 25;

        printf("Initialize BME680 sensor %u ... ", i);
        if (bme680_spi_init(&dev_spi[i], &bme680_spi_params[i]) < 0) {
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

        for (unsigned i = 0; i < BME680_SPI_NUMOF; i++) {

            int res = bme680_spi_read(&dev_spi[i], &data);

            if (res == 0)
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

#endif
    /* Should never reach here */
    return 0;
}
