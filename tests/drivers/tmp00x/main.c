/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2014 PHYTEC Messtechnik GmbH
 * SPDX-FileCopyrightText: 2017-2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the TMP00X (TMP006 and TMP007) sensor driver.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "tmp00x.h"
#include "tmp00x_params.h"

int main(void)
{
    tmp00x_t dev;
    int16_t rawtemp, rawvolt;
    float tamb, tobj;
    uint16_t drdy;

    puts("TMP00X infrared thermopile sensor driver test application\n");
    printf("Initializing TMP00X sensor at I2C_%i ... ", tmp00x_params[0].i2c);
    if (tmp00x_init(&dev,  &tmp00x_params[0]) != TMP00X_OK) {
        puts("init device [ERROR]");
        return -1;
    }

    if (tmp00x_set_active(&dev)) {
        puts("start measurement [ERROR]");
        return -1;
    }
    xtimer_usleep(CONFIG_TMP00X_CONVERSION_TIME);
    puts("[SUCCESS]\n");

    while (1) {
        tmp00x_read(&dev, &rawvolt, &rawtemp, &drdy);

        if (drdy) {
            printf("Raw data T: %5d   V: %5d\n", rawtemp, rawvolt);
            tmp00x_convert(rawvolt, rawtemp,  &tamb, &tobj);
            printf("Data Tabm: %d   Tobj: %d\n", (int)(tamb*100), (int)(tobj*100));
        }
        else {
            puts("conversion in progress ... ");
        }
        xtimer_usleep(CONFIG_TMP00X_CONVERSION_TIME);
    }

    return 0;
}
