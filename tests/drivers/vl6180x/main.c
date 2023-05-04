/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for ST VL6180X Ranging and Ambient Light Sensing (ALS) module
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * ## About
 *
 * The test application demonstrates the use of different functions of
 * the ST VL6180X sensor driver depending on used pseudomodules:
 *
 * Module Name        | Functionality used
 * :------------------|:-------------------------------------------
 * `vl6180x_rng`      | Periodic range measurements enabled
 * `vl6180x_als`      | Periodic ambient light sensing (ALS) enabled
 * `vl6180x_irq`      | Data ready interrupt enabled for range and ALS measurements
 * `vl6180x_config`   | Event interrupt enabled and configured for range measurements
 * `vl6180x_shutdown` | Power-down and power-up functions used
 *
 * ## Usage
 *
 * To compile and execute the test application, use command:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * make BOARD=... -C tests/drivers/vl6180x flash term
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * By default, the `vl6180x_rng` and `vl6180x_als` modules are enabled.
 * This is, the standard variant of the test application performs periodic
 * range and ALS measurements.
 *
 * To use data ready interrupts for range and ALS measurements instead of
 * polling for new data, the `vl6180x_irq` module has to be used:
 *
 * The MCU GPIO pin connected with the interrupt signal GPIO1 of the sensor
 * has to be defined by the configuration parameter `VL6180X_PARAM_PIN_INT`,
 * for example:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * CFLAGS="-DVL6180X_PARAM_INT_PIN=\(GPIO_PIN\(0,5\)\)" \
 * USEMODULE=vl6180x_irq \
 * make BOARD=... -C tests/drivers/vl6180x flash term
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * If the module `vl6180x_config` is used additionally, the event interrupt
 * is configured and used for range measurements:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * CFLAGS="-DVL6180X_PARAM_INT_PIN=\(GPIO_PIN\(0,5\)\)" \
 * USEMODULE='vl6180x_irq vl6180x_config'\
 * make BOARD=... -C tests/drivers/vl6180x flash term
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * To test the power-down and power-up functionality, the `vl6180x_shutdown`
 * module has to be used. The MCU GPIO pin connected with the signal GPIO0/CE
 * of the sensor has to be defined by configuration parameter
 * `VL6180X_PARAM_PIN_SHUTDOWN`, for example:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * CFLAGS="-VL6180X_PARAM_SHUTDOWN_PIN=\(GPIO_PIN\(0,6\)\)" \
 * USEMODULE=vl6180x_shutdown \
 * make BOARD=... -C tests/drivers/vl6180x flash term
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include <stdio.h>

#include "thread.h"
#include "ztimer.h"

#include "vl6180x.h"
#include "vl6180x_params.h"

#if IS_USED(MODULE_VL6180X_IRQ)

#if IS_USED(MODULE_VL6180X_RNG)
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
#endif

#if IS_USED(MODULE_VL6180X_ALS)
static const char *als_errors[] = {
    "No error",                         /**< VL6180X_ALS_OK */
    "ALS measurement overflow",         /**< VL6180X_ALS_OVERFLOW */
    "ALS measurement underflow",        /**< VL6180X_ALS_UNDERFLOW */
};
#endif

#endif /* IS_USED(MODULE_VL6180X_IRQ) */

int main(void)
{
    /* Initialize the sensor */
    vl6180x_t dev;

#if IS_USED(MODULE_VL6180X_RNG)
    uint8_t  rng;
#endif

#if IS_USED(MODULE_VL6180X_ALS)
    uint16_t als;
    uint16_t lux;
#endif

    /* initialize the sensor  */
    puts("VL6180X Time-of-Flight distance sensor\n");
    puts("Initializing VL6180X sensor");

    if (vl6180x_init(&dev, &vl6180x_params[0]) == VL6180X_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return 1;
    }

#if IS_USED(MODULE_VL6180X_SHUTDOWN)
    unsigned count = 0;
#endif

#if IS_USED(MODULE_VL6180X_IRQ) && IS_USED(MODULE_VL6180X_CONFIG)
    vl6180x_int_config_t mode;
    vl6180x_int_thresh_t thresh;

#if IS_USED(MODULE_VL6180X_RNG)
    /* interrupt when distance is less than 30 mm or greater than 100 mm */
    mode.rng_int = VL6180X_INT_OUT;
    thresh.rng_low = 30;
    thresh.rng_high = 100;
#endif /* IS_USED(MODULE_VL6180X_RNG) */

#if IS_USED(MODULE_VL6180X_ALS)
    /* interrupts when ALS data are ready */
    mode.als_int = VL6180X_INT_DRDY;
#endif /* IS_USED(MODULE_VL6180X_RNG) */

    vl6180x_int_config(&dev, thresh);
    vl6180x_int_enable(&dev, mode);

#endif /* IS_USED(MODULE_VL6180X_CONFIG) && IS_USED(MODULE_VL6180X_IRQ) */

    while (1) {

#if IS_USED(MODULE_VL6180X_IRQ)
        vl6180x_int_config_t src;

        vl6180x_int_wait(&dev, &src);
#if IS_USED(MODULE_VL6180X_RNG)
        if (src.rng_int == VL6180X_INT_DRDY) {
            int status = vl6180x_rng_read(&dev, &rng);
            switch (status) {
                case VL6180X_RNG_OK:
                    printf("RNG: %u [mm]\n", rng);
                    break;
                case -VL6180X_ERROR_RNG:
                    printf("RNG: %s\n", rng_errors[vl6180x_rng_status(&dev)]);
                    break;
                default:
                    printf("RNG: error %d\n", status);
            }
            puts("+-----------------------------------------+");
        }

        else if (src.rng_int == VL6180X_INT_OUT) {
            puts("RNG: out of window");
        }
        else if (src.rng_int == VL6180X_INT_RNG_LOW) {
            puts("RNG: low level");
        }
        else if (src.rng_int == VL6180X_INT_RNG_HIGH) {
            puts("RNG: high level");
        }
#endif /* IS_USED(MODULE_VL6180X_RNG) */

#if IS_USED(MODULE_VL6180X_ALS)
        if (src.als_int == VL6180X_INT_DRDY) {
            int status = vl6180x_als_read(&dev, &als, &lux);
            switch (status) {
                case VL6180X_OK:
                    printf("ALS: %u [cnts], %u [lux]\n", als, lux);
                    break;
                case -VL6180X_ERROR_ALS:
                    printf("ALS: %s\n", als_errors[vl6180x_als_status(&dev)]);
                    break;
                default:
                    printf("ALS: error %d\n", status);
            }
        }

        else if (src.als_int == VL6180X_INT_OUT) {
            puts("ALS: out of window");
        }
        else if (src.als_int == VL6180X_INT_LOW) {
            puts("ALS: low level");
        }
        else if (src.als_int == VL6180X_INT_HIGH) {
            puts("ALS: high level");
        }
#endif /* IS_USED(MODULE_VL6180X_ALS) */

#else /* IS_USED(MODULE_VL6180X_IRQ) */

        /* just wait 250 ms if module vl6180x_basic is used */
        ztimer_sleep(ZTIMER_MSEC, 250);

#if IS_USED(MODULE_VL6180X_ALS)
        if (vl6180x_als_data_ready(&dev) == VL6180X_OK &&
            vl6180x_als_read(&dev, &als, &lux) == VL6180X_OK) {
            printf("ALS: %u [cnts], %u [lux]\n", als, lux);
        }
#endif /* IS_USED(MODULE_VL6180X_ALS) */

#if IS_USED(MODULE_VL6180X_RNG)
         if (vl6180x_rng_data_ready(&dev) == VL6180X_OK) {
            if (vl6180x_rng_read(&dev, &rng) == VL6180X_OK) {
                printf("RNG: %u [mm]\n", rng);
            }
            else {
                puts("RNG: inf");
            }
            puts("+-----------------------------------------+");
        }
#endif /* IS_USED(MODULE_VL6180X_RNG) */

#endif /* IS_USED(MODULE_VL6180X_IRQ) */

#if IS_USED(MODULE_VL6180X_SHUTDOWN)
        /*
         * if shutdown pin is defined, the sensor is powered down and up
         * again after 5 seconds every 50 cycles
         */
        count++;
        if ((count % 50) == 0) {
            vl6180x_power_down(&dev);
            puts("Sensor powered down");
            ztimer_sleep(ZTIMER_SEC, 5);
            vl6180x_power_up(&dev);
            puts("Sensor powered up");
        }
#endif /* IS_USED(MODULE_VL6180X_SHUTDOWN) */
    }

    return 0;
}
