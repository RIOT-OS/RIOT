/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
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
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "tmp006.h"

#ifdef TMP006_I2C
#define TEST_TMP006_I2C TMP006_I2C
#else
#define TEST_TMP006_I2C I2C_0
#endif
#ifdef TMP006_ADDR
#define TEST_TMP006_ADDR TMP006_ADDR
#else
#define TEST_TMP006_ADDR 0x40
#endif

#define TEST_TMP006_CONFIG_CR       TMP006_CONFIG_CR_DEF /**< Default for Testing */

int main(void)
{
    tmp006_t dev;
    int16_t rawtemp, rawvolt;
    float tamb, tobj;
    uint8_t drdy;

    puts("TMP006 infrared thermopile sensor driver test application\n");
    printf("Initializing TMP006 sensor at I2C_%i... ", TEST_TMP006_I2C);
    if (tmp006_init(&dev, TEST_TMP006_I2C, TEST_TMP006_ADDR, TEST_TMP006_CONFIG_CR) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    if (tmp006_set_active(&dev)) {
        puts("Measurement start failed.");
        return -1;
    }

    while (1) {
        tmp006_read(&dev, &rawvolt, &rawtemp, &drdy);

        if (drdy) {
            printf("Raw data T: %5d   V: %5d\n", rawtemp, rawvolt);
        }
        else {
            printf("conversion in progress\n");
        }

        tmp006_convert(rawvolt, rawtemp,  &tamb, &tobj);
        printf("Data Tabm: %d   Tobj: %d\n", (int)(tamb*100), (int)(tobj*100));

        xtimer_usleep(TMP006_CONVERSION_TIME);
    }

    return 0;
}
