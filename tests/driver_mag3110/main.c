/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *               2017 HAW Hamburg
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
 * @brief       Test application for the MPL3115A2 magnetometer driver.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>

#include "timex.h"
#include "xtimer.h"

#include "mag3110.h"
#include "mag3110_params.h"

#define SLEEP_USEC  (1U * US_PER_SEC)

static mag3110_t dev;

int main(void)
{
    mag3110_data_t data;
    int8_t temp;

    puts("MAG3110 magnetometer driver test application\n");
    printf("Initializing MAG3110 magnetometer at I2C_%i... ",
           mag3110_params[0].i2c);
    if (mag3110_init(&dev, &mag3110_params[0]) != MAG3110_OK) {
        puts("[FAILED]");
        return -1;
    }
    puts("[SUCCESS]");

    while (1) {
        xtimer_usleep(SLEEP_USEC);
        mag3110_read(&dev, &data);
        printf("Field strength: X: %d Y: %d Z: %d\n", data.x, data.y, data.z);
        mag3110_read_dtemp(&dev, &temp);
        printf("Die Temperature T: %d\n", temp);
    }

    return 0;
}
