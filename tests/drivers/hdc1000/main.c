/*
 * SPDX-FileCopyrightText: 2014-2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2014 PHYTEC Messtechnik GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the HDC1000 sensor driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdio.h>

#include "fmt.h"
#include "xtimer.h"
#include "hdc1000.h"
#include "hdc1000_params.h"

#define SLEEP_USEC  (1000 * 1000U)

int main(void)
{
    hdc1000_t dev;
    int16_t temp, hum;
    char tstr[8];
    char hstr[8];

    puts("HDC1000 Temperature and Humidity Sensor driver test application\n");
    printf("Initializing HDC1000 sensor at I2C_DEV(%i)... ",
            (int)hdc1000_params[0].i2c);
    if (hdc1000_init(&dev, &hdc1000_params[0]) == HDC1000_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    while (1) {
        size_t len;
        hdc1000_read(&dev, &temp, &hum);

        len = fmt_s16_dfp(tstr, temp, -2);
        tstr[len] = '\0';
        len = fmt_s16_dfp(hstr, hum, -2);
        hstr[len] = '\0';
        printf("Reading: T: %s °C  RH: %s %%\n", tstr, hstr);

        xtimer_usleep(SLEEP_USEC);
    }

    return 0;
}
