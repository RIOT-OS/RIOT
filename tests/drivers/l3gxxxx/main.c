/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for the L3Gxxxx 3-axis gyroscope sensor family driver
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * # About
 *
 * This is a manual test application for the driver of the L3GXXXX 3-axis
 * gyroscope sensor family. It demonstrates the use of a L3Gxxxx sensor.
 * The different functionalities of the driver are enabled and tested by
 * using different modules.
 *
 * # Usage
 *
 * The used sensor must be specified by setting the variable DRIVER to
 * `l3gd20h`, `l3gd20`, `l3g4200d_ng`, `i3g4250d` or `a3g4250d` which enables
 * the corresponding pseudomodule:
 * ```
 * DRIVER=l3gd20h BOARD=... make -C tests/drivers/l3gxxxx
 * ```
 * If the DRIVER variable is not set, `l3gd20h` is used.
 *
 * By default, the application will initialize the gyroscope with the default
 * configuration parameter set as defined in the `l3gxxxx_params.h` file:
 *
 * Configuration Parameter     | Default value
 * :---------------------------|:----------------------
 * Output data rate            | 100 Hz
 * LPF2 cutoff frequency       | 25 Hz
 * Full scale                  | 245 dps
 * Filter selection            | HPF and LPF2 are used
 * HPF mode                    | Normal with autoreset
 * HPF cutoff frequency        | 8 Hz
 *
 * The default configuration uses the I2C interface (module `l3gxxxx_i2c`)
 * of the sensor and polls the data periodically every 200 ms. If the SPI
 * interface of the sensor should be used, the module `l3gxxxx_spi` has to
 * be used:
 * ```
 * DRIVER=l3gd20h USEMODULE=l3gxxxx_spi BOARD=... make -C tests/drivers/l3gxxxx
 * ```
 *
 * The application is configurable by using different pseudomodules to test
 * most of the functionalities of the L3Gxxxx driver.
 *
 * - Module `l3gxxxx_irq_drdy`:
 *   Instead of polling the sensor regularly for data, the data ready interrupt
 *   on signal `INT2/DRDY` is used to read the data. If module `l3gxxxx_fifo`
 *   is used at the same time, the FIFO status interrupts are enabled instead
 *   of the data ready interrupt. The GPIO pin used for the interrupt signal
 *   can be overridden, for example:
 *
 *     ```
 *     DRIVER=l3gd20h USEMODULE=l3gxxxx_irq_drdy \
 *     CFLAGS='-DL3GXXXX_INT2_PIN=GPIO_PIN\(0,5\) BOARD=... make -C tests/drivers/l3gxxxx
 *     ```
 *
 * - Module `l3gxxxx_irq_event`:
 *   Event interrupt generation is configured according to default
 *   configuration parameters in the `l3gxxxx_params.h` file to generate
 *   interrupts on signal `INT1` when the absolute value of the angular rate
 *   of any axis exceeds the default threshold. The GPIO pin used for the
 *   interrupt signal can be overridden, for example:
 *
 *     ```
 *     DRIVER=l3gd20h USEMODULE=l3gxxxx_irq_event \
 *     CFLAGS='-DL3GXXXX_INT1_PIN=GPIO_PIN\(0,6\) BOARD=... make -C tests/drivers/l3gxxxx
 *     ```
 *
 * - Module `l3gxxxx_fifo`:
 *   Instead of fetching single data samples from the sensor, the FIFO of
 *   the sensor is enabled. Depending on the use of module `l3gxxxx_irq_drdy`,
 *   the FIFO state is either polled regularly or FIFO status interrupts on
 *   signal `INT2/DRDY` are used. All available data samples are read from
 *   the FIFO with a single burst access.
 *
 * - Module `l3gxxxx_config`:
 *   The configuration of the event interrupt generation is changed at
 *   runtime.
 */

#include <stdio.h>

#include "thread.h"
#include "ztimer.h"

#include "l3gxxxx.h"
#include "l3gxxxx_params.h"

#define TESTS_DRIVER_L3GXXXX_SLEEP (200)    /* 200 ms */

#if IS_USED(MODULE_L3GXXXX_IRQ)
int handle_event(const l3gxxxx_t *dev, l3gxxxx_int_event_src_t source)
{
    (void)dev;

    /* display the event */
    if (!source.active) {
        return -1;
    }
    if (source.x_low) {
        printf("|x| below ");
    }
    if (source.y_low) {
        printf("|y| below ");
    }
    if (source.z_low) {
        printf("|z| below ");
    }
    if (source.x_high) {
        printf("|x| above ");
    }
    if (source.y_high) {
        printf("|y| above ");
    }
    if (source.z_high) {
        printf("|z| above ");
    }
    printf("defined thresholds: ");

    return 0;
}
#endif /* IS_USED(MODULE_L3GXXXX_IRQ) */

int read_data(const l3gxxxx_t *dev)
{
#if IS_USED(MODULE_L3GXXXX_FIFO)

    l3gxxxx_data_fifo_t data;

    int num = l3gxxxx_read_fifo(dev, data);

    printf("FIFO samples num=%d\n", num);
    for (int i = 0; i < num; i++) {
        printf("gyro [dps] x: %+5" PRIi32 ", y: %+5" PRIi32 ", z: %+5" PRIi32 "\n",
               data[i].x / 1000, data[i].y / 1000, data[i].z / 1000);
    }

    return num;

#else /* IS_USED(MODULE_L3GXXXX_FIFO) */

    l3gxxxx_data_t data;
    l3gxxxx_read(dev, &data);

    printf("gyro [dps] x: %+5" PRIi32 ", y: %+5" PRIi32 ", z: %+5" PRIi32 "\n",
           data.x / 1000, data.y / 1000, data.z / 1000);

    return 1;

#endif /* IS_USED(MODULE_L3GXXXX_FIFO) */
}

int main(void)
{
    l3gxxxx_t dev;

    puts("L3GXXXX gyroscope driver test application\n");
    puts("Initializing L3GXXXX sensor");

    /* initialize the sensor with default configuration parameters */
    if (l3gxxxx_init(&dev, &l3gxxxx_params[0]) == L3GXXXX_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

#if IS_USED(MODULE_L3GXXXX_CONFIG)
    /* select LPF2 and HPF, configure HPF and reset REFERENCE by dummy read */
    int8_t ref;
    l3gxxxx_select_output_filter(&dev, L3GXXXX_HPF_ONLY);
    l3gxxxx_config_hpf(&dev, L3GXXXX_HPF_NORMAL, 0);
    l3gxxxx_get_hpf_ref(&dev, &ref);
#endif /* IS_USED(MODULE_L3GXXXX_CONFIG) */

#if IS_USED(MODULE_L3GXXXX_FIFO)  && IS_USED(MODULE_L3GXXXX_CONFIG)
    /* clear FIFO and activate FIFO Stream mode */
    l3gxxxx_set_fifo_mode(&dev, L3GXXXX_BYPASS, 0);
    l3gxxxx_set_fifo_mode(&dev, L3GXXXX_STREAM, 10);
#endif

#if IS_USED(MODULE_L3GXXXX_IRQ_EVENT) && IS_USED(MODULE_L3GXXXX_CONFIG)
    /* enable event interrupts (axis movement and wake-up) */
    l3gxxxx_int_event_cfg_t int_cfg;

    l3gxxxx_get_int_event_cfg(&dev, &int_cfg);

    int_cfg.x_high_enabled = true;
    int_cfg.y_high_enabled = true;
    int_cfg.z_high_enabled = true;
    int_cfg.x_low_enabled  = false;
    int_cfg.y_low_enabled  = false;
    int_cfg.z_low_enabled  = false;
    int_cfg.x_threshold = 1000;
    int_cfg.y_threshold = 1000;
    int_cfg.z_threshold = 1000;

    int_cfg.filter = L3GXXXX_HPF_ONLY;
    int_cfg.and_or = false;
    int_cfg.duration = 0;
    int_cfg.latch = true;

    l3gxxxx_set_int_event_cfg(&dev, &int_cfg);
#endif /* IS_USED(MODULE_L3GXXXX_CONFIG) && IS_USED(MODULE_L3GXXXX_IRQ_EVENT) */

    while (1) {

#if IS_USED(MODULE_L3GXXXX_IRQ)
        /* wait for next interrupt */
        l3gxxxx_int_src_t int_src = l3gxxxx_wait_int(&dev);
        (void)int_src;

        /* in case of event we determine the source */
        if (int_src.event.active) {
            handle_event(&dev, int_src.event);
        }
        /* read and print data in any case */
        read_data(&dev);
#else
        /* read and print data */
        read_data(&dev);
        ztimer_sleep(ZTIMER_MSEC, TESTS_DRIVER_L3GXXXX_SLEEP);
#endif /* IS_USED(MODULE_L3GXXXX_IRQ) */
    }

    return 0;
}
