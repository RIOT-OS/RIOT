/*
 * Copyright (C) 2017 HAW Hamburg
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
 * @brief       HTS221 humidity and temperature sensor driver test application
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "hts221.h"
#include "hts221_params.h"
#include "xtimer.h"

#define SLEEP_S     (2U)
static hts221_t dev;

int main(void)
{
    printf("Init HTS221 on I2C_DEV(%i)\n", (int)hts221_params[0].i2c);
    if (hts221_init(&dev, &hts221_params[0]) != HTS221_OK) {
        puts("[FAILED]");
        return 1;
    }
    if (hts221_power_on(&dev) != HTS221_OK) {
        puts("[FAILED] to set power on!");
        return 2;
    }
    if (hts221_set_rate(&dev, dev.p.rate) != HTS221_OK) {
        puts("[FAILED] to set continuous mode!");
        return 3;
    }

    while(1) {
        uint16_t hum = 0;
        int16_t temp = 0;
        if (hts221_read_humidity(&dev, &hum) != HTS221_OK) {
            puts(" -- failed to read humidity!");
        }
        if (hts221_read_temperature(&dev, &temp) != HTS221_OK) {
            puts(" -- failed to read temperature!");
        }
        printf("H: %d.%d%%, T: %d.%d°C\n", (hum / 10), (hum % 10),
               (temp / 10), abs(temp % 10));
        xtimer_sleep(SLEEP_S);
    }
    return 0;
}
