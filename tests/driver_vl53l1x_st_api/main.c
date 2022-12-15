/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for ST VL53L1X API driver
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * The test application provides a direct test for the ST VL53L1X API
 * driver module vl53l1x_st_api, which supports the full functionality
 * of the sensor. Although the ST VL53L1X API driver could also be tested
 * with tests/driver_vl53l1x, this test application allows the use of the
 * ST VL53L1X API driver without additional definitions and is thus covered
 * by CI compilation.
 *
 * If the configuration parameter VL53L1X_PARAM_PIN_INT is defined, interrupts
 * are used to get data instead of polling for new data. In the case of driver
 * variant vl53l1x_st_api, threshold interrupts are configured. Otherwise
 * only data interrupts are used.
 *
 * In all cases, the sensor is configured with following default parameters:
 *
 * - timing budget of 50 ms
 * - intermeasurement period of 100 ms
 */

#include <stdio.h>

#include "mutex.h"
#include "thread.h"
#include "ztimer.h"

#include "vl53l1x.h"
#include "vl53l1x_params.h"

static void isr (void *arg)
{
    /*
     * The ISR function is executed in the interrupt context. It must not be
     * blocking or time-consuming and must not access the sensor directly
     * via I2C.
     *
     * Therefore, the ISR function only indicates to the waiting thread that
     * an interrupt has occurred which needs to be handled in the thread
     * context.
     */
    mutex_unlock(arg);
}

int main(void)
{
    /* Initialize the sensor */
    vl53l1x_t dev;

    /* initialize the sensor  */
    puts("VL53L1X Time-of-Flight distance sensor\n");
    puts("Initializing VL53L1X sensor");

    if (vl53l1x_init(&dev, &vl53l1x_params[0]) == VL53L1X_OK) {
        printf("[OK]\n");
    }
    else {
        printf("[Failed]\n");
        return 1;
    }

    mutex_t mtx = MUTEX_INIT_LOCKED;

    /* if interrupt pin is defined, enable the interrupt */
    if (vl53l1x_params[0].pin_int != GPIO_UNDEF) {
        /* generate interrupts when distance is between 200 mm and 400 mm */
        vl53l1x_int_config_t cfg = {
            .mode = VL53L1X_INT_DIST,
            .dist_mode = VL53L1X_THRESH_IN,
            .dist_high = 400,
            .dist_low  = 200,
        };
        vl53l1x_int_config(&dev, &cfg, isr, &mtx);
    }

    while (1) {

        /* if interrupt pin is defined, wait for the interrupt */
        if (vl53l1x_params[0].pin_int != GPIO_UNDEF) {
            mutex_lock(&mtx);
        }
        else {
            /* otherwise wait 100 ms. */
            ztimer_sleep(ZTIMER_MSEC, 100);
        }

        vl53l1x_data_t data;

        if (vl53l1x_data_ready(&dev) == VL53L1X_OK &&
            vl53l1x_read_data(&dev, &data) == VL53L1X_OK) {

            printf("distance=%" PRIi16 " [mm] status=%" PRIu8 " "
                   "signal=%" PRIu16 ".%03" PRIu16 " [mcps] "
                   "ambient=%" PRIu16 ".%03" PRIu16 " [mcps]\n",
                   data.distance, data.status,
                   (uint16_t)(data.signal_rate >> 16),
                   (uint16_t)((data.signal_rate & 0xffff)/65.535),
                   (uint16_t)(data.ambient_rate >> 16),
                   (uint16_t)((data.ambient_rate & 0xffff)/65.535));
        }
    }

    return 0;
}
