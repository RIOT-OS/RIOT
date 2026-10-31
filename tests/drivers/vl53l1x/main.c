/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>

#include "mutex.h"
#include "thread.h"
#include "ztimer.h"

#include "vl53l1x.h"
#include "vl53l1x_params.h"

static void isr(void *arg)
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

    if (vl53l1x_init(&dev, &vl53l1x_params[0]) == 0) {
        printf("[OK]\n");
    }
    else {
        printf("[Failed]\n");
        return 1;
    }

#if !IS_USED(MODULE_VL53L1X_BASIC)
    /* optional read and print the ROI */
    vl53l1x_roi_t roi;
    if (vl53l1x_get_roi(&dev, &roi) == 0) {
        printf("old ROI [x_size=%d, y_size=%d]\n", roi.x_size, roi.y_size);
    }
    roi.x_size = 8;
    roi.y_size = 8;
    if (vl53l1x_set_roi(&dev, &roi) == 0 &&
        vl53l1x_get_roi(&dev, &roi) == 0) {
        printf("new ROI [x_size=%d, y_size=%d]\n", roi.x_size, roi.y_size);
    }
#endif /* !IS_USED(MODULE_VL53L1X_BASIC) */

    mutex_t mtx = MUTEX_INIT_LOCKED;

    /* if interrupt pin is defined, enable the interrupt */
    if (vl53l1x_params[0].pin_int != GPIO_UNDEF) {
#if IS_USED(MODULE_VL53L1X_BASIC)
        /* only data ready interrupts are supported and cfg has to be NULL */
        vl53l1x_int_config(&dev, VL53L1X_INT_DATA_READY, 0, isr, &mtx);
#else
        /* generate interrupts when distance is between 200 mm and 400 mm */
        vl53l1x_thresh_config_t cfg = {
            .mode = VL53L1X_THRESH_IN,
            .high = 400,
            .low  = 200,
        };
        vl53l1x_int_config(&dev, VL53L1X_INT_DIST_THRESH, &cfg, isr, &mtx);
#endif /* IS_USED(MODULE_VL53L1X_BASIC) */
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

#if IS_USED(MODULE_VL53L1X_BASIC)
        uint16_t mm;

        if (vl53l1x_data_ready(&dev) == 0 &&
            vl53l1x_read_mm(&dev, &mm) == 0) {

            printf("distance=%" PRIi16 " [mm]\n", mm);
        }
#else /* IS_USED(MODULE_VL53L1X_BASIC) */

        vl53l1x_data_t data;

        if (vl53l1x_data_ready(&dev) == 0 &&
            vl53l1x_read_data(&dev, &data) == 0) {
            printf("distance=%" PRIi16 " [mm] status=%u "
                   "signal=%" PRIu16 " [kcps] ambient=%" PRIu16 " [kcps]\n",
                   data.distance, data.status,
                   data.signal_rate, data.ambient_rate);
        }
#endif /* IS_USED(MODULE_VL53L1X_BASIC) */
    }

    return 0;
}
