/*
 * Copyright (C) 2016-2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the Si114x UV, IR, visible and proximity
 *              sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *              Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "si114x.h"
#include "si114x_params.h"
#include "xtimer.h"
#include "board.h"

static si114x_t dev;

int main(void)
{
    puts("Si1145 test application\n"
         "+------------Initializing------------+\n");

    int result = si114x_init(&dev, &si114x_params[0]);
    if (result == -SI114X_ERR_I2C) {
        puts("[Error] The given i2c is not enabled");
        return -1;
    }
    else if (result == -SI114X_ERR_NODEV) {
        puts("[Error] The sensor did not answer correctly on the "
             "given address");
        return 1;
    }

    puts("Initialization successful\n"
         "+--------Starting Measurements--------+\n");

    while (1) {
        printf("UV index: %" PRIu16 "\n"
               "IR light [lx]: %" PRIu16 "\n"
               "Visible light [lx]: %" PRIu16 "\n"
               "Distance [cnt]: %" PRIu16 "\n"
               "Response: 0x%02x\n"
               "\n+-------------------------------------+\n",
               si114x_read_uv(&dev),
               si114x_read_ir(&dev),
               si114x_read_visible(&dev),
               si114x_read_distance(&dev),
               si114x_read_response(&dev));

        /* 2 seconds delay between measures */
        xtimer_sleep(2);
    }

    return 0;
}
