/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @brief       Test application for the HMC5883L 3-axis digital compass
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 *
 * The test application demonstrates the use of the HMC5883L. It uses the
 * default configuration parameters.
 *
 * - Continuous measurement at a Data Output Rate (DOR) of 15 Hz
 * - Normal mode, no biasing
 * - Gain 1090 LSB/Gs
 * - No averaging of data samples
 *
 * The application can use different approaches to get new data:
 *
 * - using the #hmc5883l_read function at a lower rate than the DOR
 * - using the data-ready interrupt (**DRDY**), see #hmc5883l_init_int.
 *
 * To use the data-ready interrupt (**DRDY), the application has to enable
 * module `hmc5883l_int` and has to configure the GPIO to which the
 * interrupt signal is connected. This is done by overrding the default
 * configuration parameter `HMC5883L_PARAM_INT_PIN` if necessary, for example:
 *
 * ```
 * USEMODULE=hmc5883l_int CFLAGS='-DHMC5883L_PARAM_INT_PIN=GPIO_PIN\(0,12\)' \
 * make flash -C tests/drivers/hmc5883l BOARD=...
 * ```
 *
 * The heading angle is calculated for a magnetic declination in radians defined
 * by `HMC5883L_MAG_DECL`. This depends on the current location. The value for
 * the current location can be determined at http://www.magnetic-declination.com/.
 */

#include <math.h>
#include <stdio.h>

#include "thread.h"
#include "xtimer.h"

#include "hmc5883l.h"
#include "hmc5883l_params.h"

/*
 * Magnetic declination in radians according to the real geo location, see:
 * http://www.magnetic-declination.com/
 */
#ifndef HMC5883L_MAG_DECL
#define HMC5883L_MAG_DECL   (0.0573F)
#endif

#ifndef M_PI
#define M_PI    (3.14159265358979323846)
#endif

#define HMC5883L_SLEEP  (100 * US_PER_MS)

kernel_pid_t p_main;

#if MODULE_HMC5883L_INT
static void hmc5883l_isr_data_ready (void *arg)
{
    (void)arg;
    /* send a message to trigger main thread to handle the interrupt */
    msg_t msg;
    msg_send(&msg, p_main);
}
#endif

int main(void)
{
    hmc5883l_t dev;

    p_main = thread_getpid();

    puts("HMC5883L magnetometer driver test application\n");
    puts("Initializing HMC5883L sensor");

    /* initialize the sensor with default configuration parameters */
    if (hmc5883l_init(&dev, &hmc5883l_params[0]) == HMC5883L_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    #if MODULE_HMC5883L_INT
    /* init INT2/DRDY signal pin and enable the interrupt */
    hmc5883l_init_int(&dev, hmc5883l_isr_data_ready, 0);
    #endif /* MODULE_HMC5883L_INT */

    while (1) {
        #if MODULE_HMC5883L_INT
        /* wait for data ready interrupt */
        msg_t msg;
        msg_receive(&msg);
        #else
        /* wait longer than period of HMC5883L DOR */
        xtimer_usleep(HMC5883L_SLEEP);
        #endif

        /* read data in any case */
        hmc5883l_data_t data;
        if (hmc5883l_read(&dev, &data) == HMC5883L_OK) {
            /* print xyz data */
            printf("magn [mGs] x = %" PRIi16 ", y = %" PRIi16 ", z = %" PRIi16 "\n",
                    data.x, data.y, data.z);

            /* compute and print heading for the given magnetic declination in rad */
            float head = atan2(data.y, data.x) + HMC5883L_MAG_DECL;
            if (head < 0) {
                head += 2 * M_PI;
            }
            if (head > 2 * M_PI) {
                head -= 2 * M_PI;
            }
            unsigned deg = head * 180 / M_PI;
            printf("head [deg] %u\n", deg);
         }
    }

    return 0;
}
