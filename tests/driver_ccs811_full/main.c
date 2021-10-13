/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for the AMS CCS811 device driver
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * The test application demonstrates the use of the CCS811 and pseudomodule
 * ```ccs811_full``` using
 *
 * - data-ready interrupt ```CCS811_INT_DATA_READY``` and
 * - default configuration parameters, that is, the measurement mode
 *   ```CCS811_MODE_1S``` with one measurement per second.
 *
 * The default configuration parameter for the interrupt pin has to be
 * overridden according to the hardware configuration by defining
 * ```CCS811_PARAM_INT_PIN``` before ```ccs811_params.h``` is included, e.g.,
 * ```
 * #define CCS811_PARAM_INT_PIN     (GPIO_PIN(0, 7))
 * ```
 * or via the CFLAGS variable in the make command.
 * ```
 * CFLAGS="-DCCS811_PARAM_INT_PIN=\(GPIO_PIN\(0,7\)\)" make -C tests/driver_ccs811 BOARD=...
 * ```
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "xtimer.h"

#include "ccs811.h"
#include "ccs811_params.h"

kernel_pid_t p_main;

static void ccs811_isr (void *arg)
{
    (void)arg;
    /* send a message to trigger main thread to handle the interrupt */
    msg_t msg;
    msg_send(&msg, p_main);
}

int main(void)
{
    ccs811_t sensor;

    puts("CCS811 test application\n");

    printf("+------------Initializing------------+\n");

    /* initialize the sensor with default configuration parameters */
    if (ccs811_init (&sensor, &ccs811_params[0]) != CCS811_OK) {
        puts("Initialization failed\n");
        return 1;
    }

    /* initialize the interrupt pin */
    gpio_init_int (ccs811_params[0].int_pin, GPIO_IN, GPIO_FALLING,
                   ccs811_isr, 0);

    /* activate data ready interrupt */
    if (ccs811_set_int_mode (&sensor, CCS811_INT_DATA_READY) != CCS811_OK) {
        puts("Activating interrupt failed\n");
        return 1;
    }

    /* save the pid of main thread */
    p_main = thread_getpid();

    printf("\n+--------Starting Measurements--------+\n");

    while (1) {

        uint16_t tvoc;
        uint16_t eco2;

        /* wait for data ready interrupt */
        msg_t msg;
        msg_receive(&msg);

        /* read the data */
        if (ccs811_read_iaq(&sensor, &tvoc, &eco2, 0, 0) == CCS811_OK) {
            /* print values */
            printf("TVOC [ppb]: %d\neCO2 [ppm]: %d\n", tvoc, eco2);
            puts("+-------------------------------------+");
        }
        else {
            printf("Could not read data from sensor\n");
        }
    }

    return 0;
}
