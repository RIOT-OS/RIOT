/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the Atmel IO1 Xplained extension
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "io1_xplained.h"
#include "io1_xplained_params.h"
#include "xtimer.h"
#include "board.h"

#define SLEEP_1S   (1 * 1000 * 1000u) /* 1 seconds delay between each test */

int main(void)
{
    io1_xplained_t dev;
    float temperature;

    puts("IO1 Xplained extention test application\n");
    puts("+-------------Initializing------------+\n");

    if (io1_xplained_init(&dev, &io1_xplained_params[0]) != IO1_XPLAINED_OK) {
        puts("[Error] Cannot initialize the IO1 Xplained extension\n");
        return 1;
    }

    puts("Initialization successful");
    puts("\n+--------Starting tests --------+");
    while (1) {
        /* Get temperature in degrees celsius */
        io1_xplained_read_temperature(&dev, &temperature);
        printf("Temperature [°C]: %.2f\n"
               "\n+-------------------------------------+\n",
               temperature);
        xtimer_usleep(SLEEP_1S);

        /* set led */
        io1_xplained_set_led();
        xtimer_usleep(SLEEP_1S);

        /* clear led */
        io1_xplained_clear_led();
        xtimer_usleep(SLEEP_1S);

        /* toggle led */
        io1_xplained_toggle_led();
        xtimer_usleep(SLEEP_1S);

        /* toggle led again */
        io1_xplained_toggle_led();
        xtimer_usleep(SLEEP_1S);
    }

    return 0;
}
