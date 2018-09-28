/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for the Sensirion SHT30/SHT31/SHT35 device driver
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * The test application demonstrates the use of the CCS811. It uses the default
 * configuration parameters, that is, the measurement mode #CCS811_MODE_1S with
 * one measurement per second.
 *
 * The application can use both approaches to wait for new data
 *
 * - using the data-ready interrupt #CCS811_INT_DATA_READY or
 * - using the data-ready status function #ccs811_data_ready
 *
 * To use the data-ready interrupt, use module ```ccs811_full``` and
 * define ```USE_CCS811_DATA_READY_INT``` in CFLAGS variable of the
 * make command, for example:
 *
 * ```
 * USEMODULE=ccs811_full CFLAGS="-DUSE_CCS811_DATA_READY_INT" \
 * make flash BOARD=... -C tests/driver_ccs811
 * ```
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "xtimer.h"

#include "ccs811.h"
#include "ccs811_params.h"

#if USE_CCS811_DATA_READY_INT && !MODULE_CCS811_FULL
#error To use interrupt handling the *ccs811_full* module has to be enabled
#endif

#define CCS811_LOW  600
#define CCS811_HIGH 1000

kernel_pid_t p_main;

#ifdef USE_CCS811_DATA_READY_INT
static void ccs811_isr (void *arg)
{
    /* send a message to trigger main thread to handle the interrupt */
    msg_t msg;
    msg_send(&msg, p_main);
}
#endif

int main(void)
{
    ccs811_t sensor;

    puts("CCS811 test application\n");

    printf("+------------Initializing------------+\n");

#ifdef USE_CCS811_DATA_READY_INT
    gpio_init_int (ccs811_params[0].int_pin, GPIO_IN, GPIO_FALLING,
                   ccs811_isr, 0);
#endif /* USE_CCS811_DATA_READY_INT */

    /* initialize the sensor with default configuration parameters */
    if (ccs811_init (&sensor, &ccs811_params[0]) != CCS811_OK) {
        puts("Initialization failed\n");
        return 1;
    }

#ifdef USE_CCS811_DATA_READY_INT
    /* activate data ready interrupt */
    if (ccs811_set_int_mode (&sensor, CCS811_INT_DATA_READY) != CCS811_OK) {
        puts("Activating interrupt failed\n");
        return 1;
    }
#endif /* USE_CCS811_DATA_READY_INT */

    /* save the pid of main thread */
    p_main = sched_active_pid;

    printf("\n+--------Starting Measurements--------+\n");

    while (1) {

        uint16_t tvoc;
        uint16_t eco2;

#ifdef USE_CCS811_DATA_READY_INT

        /* wait for data ready interrupt */
        msg_t msg;
        msg_receive(&msg);

#else /* USE_CCS811_DATA_READY_INT */

        /* wait for new data by means of the data-ready status function */
        while (ccs811_data_ready (&sensor) != CCS811_OK) {
            /* sleep 10 ms */
            xtimer_usleep(10000);
        }

#endif /* USE_CCS811_DATA_READY_INT */

        /* read the data */
        if (ccs811_read_iaq(&sensor, &tvoc, &eco2, 0, 0) != CCS811_OK) {
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
