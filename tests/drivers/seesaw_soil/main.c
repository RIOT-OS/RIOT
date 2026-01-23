/*
 * SPDX-FileCopyrightText: 2020 Viktor Gal
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the Adafruit Seesaw Soil sensor driver
 *
 * @author      Viktor Gal <viktor.gal@maeth.com>
 *
 * @}
 */

#include <stdio.h>

#include "fmt.h"
#include "xtimer.h"
#include "seesaw_soil.h"
#include "seesaw_soil_params.h"

#define SLEEP_USEC  (1000 * 1000U)

int main(void)
{
    seesaw_soil_t dev;
    int16_t temp;
    uint16_t moist;
    char tstr[8];
    char mstr[8];

    puts("Seesaw Soil Temperature and Moisture Sensor driver test application\n");
    printf("Initializing Seesaw Soil sensor at I2C_DEV(%i)... ",
            (int)seesaw_soil_params[0].i2c);
    if (seesaw_soil_init(&dev, &seesaw_soil_params[0]) == SEESAW_SOIL_OK) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    while (1) {
        size_t len;
        seesaw_soil_temperature(&dev, &temp);
        seesaw_soil_moisture(&dev, &moist);

        len = fmt_s16_dfp(tstr, temp, -2);
        tstr[len] = '\0';
        len = fmt_u16_dec(mstr, moist);
        mstr[len] = '\0';
        printf("Reading: T: %s °C  Moist: %s\n", tstr, mstr);

        xtimer_usleep(SLEEP_USEC);
    }

    return 0;
}
