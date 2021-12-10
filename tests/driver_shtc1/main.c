/*
 * Copyright (C) 2017 RWTH Aachen, Steffen Robertz, Josua Arndt
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
 * @brief       Test application for the SHTC1 temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 *
 * @}
 */
#include <stdio.h>
#include <stdlib.h>

#include "timex.h"
#include "ztimer.h"
#include "fmt.h"
#include "shtc1.h"
#include "shtc1_params.h"

int main(void)
{
    shtc1_t dev;
    int16_t temp;
    uint16_t hum;

    char str_temp[8];
    char str_hum[8];
    size_t len;

    puts("SHTC1 test application\n");

    if ((shtc1_init(&dev, &shtc1_params[0])) != SHTC1_OK) {
        puts("can't initialize the sensor");
        return -1;
    }
    puts("SHTC1 initialized\n");
    while (1) {
        if (shtc1_read(&dev, &hum, &temp) == SHTC1_OK) {
            len = fmt_s16_dfp(str_temp, temp, -2);
            str_temp[len] = '\0';

            len = fmt_s16_dfp(str_hum, hum, -2);
            str_hum[len] = '\0';
        }
        /* print values to STDIO */
        printf("Temperature [°C]: %s\n", str_temp);
        printf("  Humidity [%%rH]: %s\n", str_hum);

        ztimer_sleep(ZTIMER_MSEC, 2 * MS_PER_SEC);
    }
    return 0;
}
