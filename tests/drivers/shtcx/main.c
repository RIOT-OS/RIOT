/*
 * SPDX-FileCopyrightText: 2017 RWTH Aachen, Steffen Robertz, Josua Arndt
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the SHTCX temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 *
 * @}
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timex.h"
#include "ztimer.h"
#include "fmt.h"
#include "shtcx.h"
#include "shtcx_params.h"

int main(void)
{
    shtcx_t dev;
    int16_t temp;
    uint16_t hum;

    char str_temp[8];
    char str_hum[8];
    size_t len;

    puts("SHTCX test application\n");

    if ((shtcx_init(&dev, &shtcx_params[0])) != SHTCX_OK) {
        puts("can't initialize the sensor");
        return -1;
    }
    puts("SHTCX initialized\n");
    while (1) {
        if (shtcx_read(&dev, &hum, &temp) == SHTCX_OK) {
            len = fmt_s16_dfp(str_temp, temp, -2);
            str_temp[len] = '\0';

            len = fmt_s16_dfp(str_hum, hum, -2);
            str_hum[len] = '\0';
        } else {
            strcpy(str_temp, "ERROR");
            strcpy(str_hum, "ERROR");
        }
        /* print values to STDIO */
        printf("Temperature [°C]: %s\n", str_temp);
        printf("  Humidity [%%rH]: %s\n", str_hum);

        ztimer_sleep(ZTIMER_MSEC, 2 * MS_PER_SEC);
    }
    return 0;
}
