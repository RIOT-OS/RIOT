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
 * @brief       Test application for the HDC1000 sensor driver.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#ifndef TEST_HDC1000_I2C
#error "TEST_HDC1000_I2C not defined"
#endif
#ifndef TEST_HDC1000_ADDR
#error "TEST_HDC1000_ADDR not defined"
#endif

#include <stdio.h>

#include "xtimer.h"
#include "hdc1000.h"

#define SLEEP       (1000 * 1000U)

int main(void)
{
    hdc1000_t dev;
    uint16_t rawtemp, rawhum;
    int temp, hum;

    puts("HDC1000 Temperature and Humidity Sensor driver test application\n");
    printf("Initializing HDC1000 sensor at I2C_%i... ", TEST_HDC1000_I2C);
    if (hdc1000_init(&dev, TEST_HDC1000_I2C, TEST_HDC1000_ADDR) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    while (1) {
        if (hdc1000_startmeasure(&dev)) {
            puts("Start measure failed.");
            return -1;
        }
        xtimer_usleep(HDC1000_CONVERSION_TIME);

        hdc1000_read(&dev, &rawtemp, &rawhum);
        printf("Raw data T: %5i   RH: %5i\n", rawtemp, rawhum);

        hdc1000_convert(rawtemp, rawhum,  &temp, &hum);
        printf("Data T: %d   RH: %d\n", temp, hum);

        xtimer_usleep(SLEEP);

    }

    return 0;
}
