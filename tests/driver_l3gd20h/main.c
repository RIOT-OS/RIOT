/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for the L3GD20H gyroscope driver
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * The test application demonstrates the use of the L3GD20H. It uses the
 * default configuration parameters:
 *
 * - Measurement mode #L3GD20H_MODE_100_25 with an ODR of 100 Hz and
 *   a LPF2 cutoff frequency of 25 Hz when LPF2 is enabled.
 * - I2C interface is used for configuration
 *
 * The application can use different approaches to get new data:
 *
 * - using the #l3gd20h_read function at a lower rate than the the ODR
 * - using the data-ready interrupt #L3GD20H_INT_DATA_READY
 * - using the event interrupt #L3GD20H_INT_EVENT
 *
 * To use one of the interrupts, module `l3gd20h_full` has to be used.
 * Furthermore, either `USE_L3GD20H_INT_DATA_READY` or
 * `USE_L3GD20H_INT_EVENT` has to be defined in CFLAGS variable of the
 * make command, for example:
 *
 * ```
 * USEMODULE=l3gd20h_full CFLAGS="-DUSE_L3GD20H_INT_DATA_READY" \
 * make flash BOARD=... -C tests/driver_l3gd20h
 * ```
 * ```
 * USEMODULE=l3gd20h_full CFLAGS="-DUSE_L3GD20H_INT_EVENT" \
 * make flash BOARD=... -C tests/driver_l3gd20h
 * ```
*/

#include <stdio.h>

#include "thread.h"
#include "xtimer.h"

#include "l3gd20h.h"
#include "l3gd20h_params.h"

#if (USE_L3GD20H_INT_DATA_READY || USE_L3GD20H_INT_EVENT) && !MODULE_L3GD20H_FULL
#error To use interrupt handling the *l3gd20h_full* module has to be enabled
#endif

#define SLEEP   (100 * US_PER_MS)

kernel_pid_t p_main;

#if USE_L3GD20H_INT_EVENT
static void l3gd20h_int1_isr (void *arg)
{
    (void)arg;

    /* send a message to trigger main thread to handle the interrupt */
    msg_t msg = { .content.value = L3GD20H_INT_EVENT };
    msg_send(&msg, p_main);
}
#endif

#if USE_L3GD20H_INT_DATA_READY
static void l3gd20h_int2_isr (void *arg)
{
    (void)arg;
    /* send a message to trigger main thread to handle the interrupt */
    msg_t msg = { .content.value = L3GD20H_INT_DATA_READY };
    msg_send(&msg, p_main);
}
#endif

#if USE_L3GD20H_INT_EVENT
int handle_event (const l3gd20h_t *dev)
{
    l3gd20h_int_event_source_t source;

    /* get the source of INT1 signal */
    l3gd20h_get_int_event_source (dev, &source);

    /* display the event */
    if (!source.active) {
        return -1;
    }
    if (source.x_low) {
        printf("x below ");
    }
    if (source.y_low) {
        printf("y below ");
    }
    if (source.z_low) {
        printf("z below ");
    }
    if (source.x_high) {
        printf("x above ");
    }
    if (source.y_high) {
        printf("y above ");
    }
    if (source.z_high) {
        printf("z above ");
    }
    printf("defined thresholds: ");

    return 0;
}
#endif

int read_data (const l3gd20h_t *dev)
{
#if USE_L3GD20H_FIFO

    l3gd20h_data_fifo_t data;

    int num = l3gd20h_read_fifo (dev, data);

    printf("FIFO samples num=%d\n", num);
    for (int i = 0; i < num; i++) {
        printf("gyro [mdps] x: %+8" PRIi32 " y: %+8" PRIi32 " z: %+8" PRIi32 "\n",
               data[i].x, data[i].y, data[i].z);
    }

    return num;

#else /* USE_L3GD20H_FIFO */

    l3gd20h_data_t data;
    l3gd20h_read(dev, &data);

    printf("gyro [mdps] x: %+8" PRIi32 " y: %+8" PRIi32 " z: %+8" PRIi32 "\n",
           data.x, data.y, data.z);

    return 1;

#endif /* USE_L3GD20H_FIFO */
}

int main(void)
{
    l3gd20h_t dev;

    p_main = sched_active_pid;

    puts("L3GD20H gyroscope driver test application\n");
    puts("Initializing L3GD20H sensor");

    /* initialize the sensor with default configuration parameters */
    if (l3gd20h_init(&dev, &l3gd20h_params[0]) == L3GD20H_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

#if MODULE_L3GD20H_FULL
    /* select LPF2 and HPF, configure HPF and reset REFERENCE by dummy read */
    int8_t ref;
    l3gd20h_select_output_filter(&dev, L3GD20H_HPF_ONLY);
    l3gd20h_config_hpf(&dev, L3GD20H_HPF_NORMAL, 0);
    l3gd20h_get_hpf_ref (&dev, &ref);
#endif

#if USE_L3GD20H_INT_EVENT
    /* init INT1 signal pin and enable the interrupt */
    gpio_init_int (l3gd20h_params[0].int1, GPIO_IN, GPIO_RISING,
                   l3gd20h_int1_isr, 0);

    /* enable event interrupts (axis movement and wake up) */
    l3gd20h_int_event_config_t int_cfg;

    l3gd20h_get_int_event_config (&dev, &int_cfg);

    int_cfg.x_high_enabled = true;
    int_cfg.y_high_enabled = true;
    int_cfg.z_high_enabled = true;
    int_cfg.x_low_enabled  = false;
    int_cfg.y_low_enabled  = false;
    int_cfg.z_low_enabled  = false;
    int_cfg.x_threshold = 1000;
    int_cfg.y_threshold = 1000;
    int_cfg.z_threshold = 1000;

    int_cfg.filter = L3GD20H_HPF_ONLY;
    int_cfg.and_or = false;
    int_cfg.duration = 0;
    int_cfg.latch = true;

    l3gd20h_set_int_event_config (&dev, &int_cfg);
    l3gd20h_enable_int(&dev, L3GD20H_INT_EVENT, true);
#endif

#if USE_L3GD20H_FIFO
    /* clear FIFO and activate FIFO Stream mode */
    l3gd20h_set_fifo_mode(&dev, L3GD20H_BYPASS, 0);
    l3gd20h_set_fifo_mode(&dev, L3GD20H_STREAM, 10);
#endif

#if USE_L3GD20H_INT_DATA_READY
    /* init INT2/DRDY signal pin and enable the interrupt */
    gpio_init_int(l3gd20h_params[0].int2, GPIO_IN, GPIO_RISING,
                  l3gd20h_int2_isr, 0);

#if USE_L3GD20H_FIFO
    l3gd20h_enable_int(&dev, L3GD20H_INT_FIFO_OVERRUN, true);
    l3gd20h_enable_int(&dev, L3GD20H_INT_FIFO_THRESHOLD, true);
#else /* USE_L3GD20H_FIFO */
    l3gd20h_enable_int(&dev, L3GD20H_INT_DATA_READY, true);
#endif /* USE_L3GD20H_FIFO */
#endif /* USE_L3GD20H_INT_DATA_READY */

    while (1) {

#if USE_L3GD20H_INT_DATA_READY || USE_L3GD20H_INT_EVENT
        /* wait for data ready interrupt */
        msg_t msg;
        msg_receive(&msg);

#if USE_L3GD20H_INT_EVENT
        /* in case of event we determine the source */
        if (msg.content.value == L3GD20H_INT_EVENT) {
            handle_event(&dev);
        }
#endif /* USE_L3GD20H_INT_EVENT */
#endif /* USE_L3GD20H_INT_DATA_READY || USE_L3GD20H_INT_EVENT */

        /* read and print data in any case */
        read_data (&dev);

#if !USE_L3GD20H_INT_DATA_READY && !USE_L3GD20H_INT_EVENT
        xtimer_usleep(SLEEP);
#endif
    }

    return 0;
}
