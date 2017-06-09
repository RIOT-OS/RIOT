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
 * @brief       Test application for the TMP006 sensor driver.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "tmp006.h"
#include "tmp006_params.h"

int main(void)
{
    tmp006_t dev;
    int16_t rawtemp, rawvolt;
    float tamb, tobj;
    uint8_t drdy;

    puts("TMP006 infrared thermopile sensor driver test application\n");
    printf("Initializing TMP006 sensor at I2C_%i ... ", tmp006_params[0].i2c);
    if (tmp006_init(&dev,  &tmp006_params[0]) != TMP006_OK) {
        puts("init device [ERROR]");
        return -1;
    }

    if (tmp006_set_active(&dev)) {
        puts("start measurement [ERROR]");
        return -1;
    }
    xtimer_usleep(TMP006_CONVERSION_TIME);
    puts("[SUCCESS]\n");

    while (1) {
        tmp006_read(&dev, &rawvolt, &rawtemp, &drdy);

        if (drdy) {
            printf("Raw data T: %5d   V: %5d\n", rawtemp, rawvolt);
            tmp006_convert(rawvolt, rawtemp,  &tamb, &tobj);
            printf("Data Tabm: %d   Tobj: %d\n", (int)(tamb*100), (int)(tobj*100));
        }
        else {
            puts("conversion in progress ... ");
        }
        xtimer_usleep(TMP006_CONVERSION_TIME);
    }

    return 0;
}
