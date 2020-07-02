/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for ST VL6180X Ranging and Ambient Light Sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * The test application demonstrates the usage of different functions of the
 * ST VL6180X sensor driver dependent on used driver variant which differ in
 * functionality and size:
 *
 * Module Name     | Driver Variant
 * :---------------|:-------------------------------------------
 * `vl6180x`       | Driver with standard functionality and larger size
 * `vl6180x_basic` | Driver with only basic functionality and smaller size
 *
 * Both driver variants provide
 *
 * - continuous ranging and ambient light sensing,
 * - polling for new measurement results, and
 * - SAUL capabilies.
 *
 * In addition to these basic functionalities, the standard driver `vl6180x`
 * provides interrupt handling, single shot measurements, a number of
 * configuration functions and power down / power up functionality.
 *
 * See the online documentation for detailed information about the driver.
 *
 * What driver variant is used can be defined at make command line. Using the
 * make command
 * ```
 *     make flash -C tests/driver_vl6180x BOARD=...
 * ```
 * the standard driver variant `vl6180x` is used with the following default
 * configuration parameters:
 *
 * - periodic check of the availability of new data every 100 ms
 * - a ranging inter-measurement period of 100 ms
 * - a ranging maximum convergence time of 50 ms
 * - an ambient light sensing (ALS) inter-measurement period of 500 ms
 * - an ambient light sensing (ALS) integration time of 100 ms
 * - an ambient light sensing (ALS) analogue light channel gain of 1.0
 * - an ambient light sensing period of 500 ms
 * - a ranging period of 100 ms.
 *
 * To use the basic driver variant, the `vl6180x_basic` module has
 * to be specified at make command line
 * ```
 *      USEMODULE=vl6180x_basic make flash -C tests/driver_vl6180x BOARD=...
 * ```
 * The default configuration parameters used for the test application with the
 * basic driver variant are the same as for the standard driver variant.
 *
 * To use interrupts to fetch new data instead of polling for new data
 * periodically, the standard driver variant `vl6180x` has to be used and the
 * pin connected with the interrupt signal GPIO1 of the sensor has to be
 * defined by configuration paramater `VL6180X_PARAM_PIN_INT`, e.g.,
 * ```
 *      CFLAGS="-DVL6180X_PARAM_PIN_INT=\(GPIO_PIN\(0,1\)\)" \
 *      make flash -C tests/driver_vl6180x BOARD=...
 * ```
 * To test the power down and power up functionality, the pin connected with
 * the signal GPIO0/CE of the sensor has to be defined by configuration
 * paramater `VL6180X_PARAM_PIN_SHUTDOWN
 * `, e.g.,
 * ```
 *      CFLAGS="-DVL6180X_PARAM_PIN_SHUTDOWN=\(GPIO_PIN\(0,2\)\)" \
 *      make flash -C tests/driver_vl6180x BOARD=...
 * ```
 */

#include <stdio.h>

#include "thread.h"
#include "xtimer.h"

#include "vl6180x.h"
#include "vl6180x_params.h"

#ifndef MODULE_VL6180X_BASIC
/* Reference to main thread */
static kernel_pid_t main_pid;

/*
 * Message queue is required to send a message from interrupt context. Since
 * we only want to know that an interrupt occured but not how many, a queue
 * size of one element is engough.
 */
static msg_t main_queue[1];

static void isr (void *arg)
{
    (void)arg;
    /*
     * The ISR function is executed in the interrupt context. It must not be
     * blocking or time-consuming and must not access the sensor directly
     * via I2C.
     *
     * Therefore, the ISR function only indicates to the waiting thread that
     * an interrupt has occurred which needs to be handled in the thread
     * context.
     */
    msg_t msg;
    msg_send(&msg, main_pid);
}

static const char *rng_errors[] = {
    "No error",                         /**< VL6180X_RNG_OK */
    "VCSEL continuity Test",            /**< VL6180X_RNG_VCSEL_CONT_TEST */
    "VCSEL watchdog test",              /**< VL6180X_RNG_VCSEL_WD_TEST */
    "VCSEL watchdog",                   /**< VL6180X_RNG_VCSEL_WD */
    "PLL1 lock",                        /**< VL6180X_RNG_PLL1_LOCK */
    "PLL2 lock",                        /**< VL6180X_RNG_PLL2_LOCK */
    "Early convergence estimate",       /**< VL6180X_RNG_EARLY_CONV_EST */
    "Maximum convergence time reached", /**< VL6180X_RNG_MAX_CONV */
    "No target, ignore",                /**< VL6180X_RNG_NO_TARGET */
    "Unknown",
    "Unknown",
    "Maximum SNR reached",              /**< VL6180X_RNG_MAX_SNR */
    "Raw ranging algorithm underflow",  /**< VL6180X_RNG_RAW_ALGO_UNDERFLOW */
    "Raw ranging algorithn overflow",   /**< VL6180X_RNG_RAW_ALGO_OVERFLOW */
    "Ranging algorithm underflow",      /**< VL6180X_RNG_ALGO_UNDERFLOW */
    "Ranging algorithm overflow",       /**< VL6180X_RNG_ALGO_OVERFLOW */
};

static const char *als_errors[] = {
    "No error",                         /**< VL6180X_ALS_OK */
    "ALS measurement overflow",         /**< VL6180X_ALS_OVERFLOW */
    "ALS measurement underflow",        /**< VL6180X_ALS_UNDERFLOW */
};

#endif /* !MODULE_VL6180X_BASIC */

int main(void)
{
    /* Initialize the sensor */
    vl6180x_t dev;

    /* initialize the sensor  */
    puts("VL6180X Time-of-Flight distance sensor\n");
    puts("Initializing VL6180X sensor");

    if (vl6180x_init(&dev, &vl6180x_params[0]) == VL6180X_OK) {
        printf("[OK]\n");
    }
    else {
        printf("[Failed]\n");
        return 1;
    }

    uint8_t  rng;
    uint16_t als;
    uint16_t lux;

#ifdef MODULE_VL6180X_BASIC

    while (1) {
        /* just wait 100 ms if module vl6180x_basic is used */
        xtimer_usleep(100 * US_PER_MS);

        if (vl6180x_rng_data_ready(&dev) == VL6180X_OK &&
            vl6180x_rng_read(&dev, &rng) == VL6180X_OK) {
            printf("Range: %u [mm]\n", rng);
        }

        if (vl6180x_als_data_ready(&dev) == VL6180X_OK &&
            vl6180x_als_read(&dev, &als, &lux) == VL6180X_OK) {
            printf("Ambient: %u [cnts], %u [lux]\n", als, lux);
            printf("+-----------------------------------------+\n");
        }
    }

#else /* MODULE_VL6180X_BASIC */

    /* save the main thread pid */
    main_pid = sched_active_pid;

    /* initialize the message queue */
    msg_init_queue(main_queue, 1);

    /* if interrupt pin is defined, enable the interrupt */
    if (vl6180x_params[0].pin_int != GPIO_UNDEF) {
        /* generate interrrupts when distance is less than 100 mm */
        vl6180x_int_config_t cfg = {
            .rng_int_mode = VL6180X_INT_LEVEL_LOW,
            .rng_thresh_low = 100,
            .als_int_mode = VL6180X_DATA_RDY,
        };
        vl6180x_int_config(&dev, &cfg, isr, 0);
    }

    unsigned count = 0;

    while (1) {

        /* if interrupt pin is defined, wait for the interrupt */
        if (vl6180x_params[0].pin_int != GPIO_UNDEF) {
            msg_t msg;
            msg_receive(&msg);
        }
        else {
            /* otherwise wait 100 ms. */
            xtimer_usleep(100 * US_PER_MS);
        }

        if (vl6180x_rng_data_ready(&dev) == VL6180X_OK) {
            int status = vl6180x_rng_read(&dev, &rng);
            switch (status) {
                case VL6180X_OK:
                    printf("Range: %u [mm]\n", rng);
                    break;
                case VL6180X_ERROR_RNG:
                    printf("Range: %s\n", rng_errors[vl6180x_rng_status(&dev)]);
                    break;
                default:
                    printf("Range: error %d\n", status);
            }
        }

        if (vl6180x_als_data_ready(&dev) == VL6180X_OK) {
            int status = vl6180x_als_read(&dev, &als, &lux);
            switch (status) {
                case VL6180X_OK:
                    printf("Ambient: %u [cnts], %u [lux]\n", als, lux);
                    break;
                case VL6180X_ERROR_ALS:
                    printf("Ambient: %s\n", als_errors[vl6180x_als_status(&dev)]);
                    break;
                default:
                    printf("Ambient: error %d\n", status);
            }
            printf("+-----------------------------------------+\n");
        }

        /*
         * if shutdown pin is defined, the sensor is powered down and up
         * again after 5 seconds every 50 cycles
         */
        if (vl6180x_params[0].pin_shutdown != GPIO_UNDEF) {
            count++;
            if ((count % 50) == 0) {
                vl6180x_power_down(&dev);
                printf("Sensor powered down\n");
                xtimer_sleep(5);
                vl6180x_power_up(&dev);
                printf("Sensor powered up\n");
            }
        }
    }
#endif /* MODULE_VL6180X_BASIC */

    return 0;
}
