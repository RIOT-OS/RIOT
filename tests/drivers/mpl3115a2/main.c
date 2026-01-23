/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2014 PHYTEC Messtechnik GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the MPL3115A2 sensor driver.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "xtimer.h"
#include "mpl3115a2.h"
#include "mpl3115a2_params.h"

#define SLEEP_USEC  (1UL * US_PER_SEC)
static mpl3115a2_t dev;

int main(void)
{
    puts("MPL3115A2 pressure sensor driver test application\n");
    printf("Initializing MPL3115A2 sensor at I2C_%i... ", mpl3115a2_params[0].i2c);

    if (mpl3115a2_init(&dev, &mpl3115a2_params[0]) != MPL3115A2_OK) {
        puts("[FAILED] init device!");
        return 1;
    }

    if (mpl3115a2_set_active(&dev) != MPL3115A2_OK) {
        puts("[FAILED] activate measurement!");
        return 2;
    }
    puts("[SUCCESS]");

    while (1) {
        uint32_t pressure;
        int16_t temp;
        uint8_t status;
        xtimer_usleep(SLEEP_USEC);
        if ((mpl3115a2_read_pressure(&dev, &pressure, &status) |
             mpl3115a2_read_temp(&dev, &temp)) != MPL3115A2_OK) {
            puts("[FAILED] read values!");
        }
        else {
            printf("Pressure: %u Pa, Temperature: %3d.%d C, State: %#02x\n",
                   (unsigned int)pressure, temp/10, abs(temp%10), status);
        }
    }

    return 0;
}
