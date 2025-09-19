/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>

#include "board.h"
#include "saul_reg.h"
#include "ztimer.h"

int main(void)
{
    /* We sleep for 5 seconds to allow the system to initialize */
    ztimer_sleep(ZTIMER_MSEC, 5000);
    puts("Welcome to SAUL magic!");

    /* Define our temperature sensor */
    saul_reg_t *temperature_sensor = saul_reg_find_type(SAUL_SENSE_TEMP);

    /* Exit if we can't find a temperature sensor */
    if (!temperature_sensor) {
        puts("No temperature sensor found");
        return 1;
    }
    else {
        /*
         * Otherwise print the name of the temperature sensor
         * and continue the program
         */
        printf("Temperature sensor found: %s\n", temperature_sensor->name);
    }

    /* We start an infinite loop to continuously read the temperature */
    while (1) {
        /* Define a variable to store the temperature */
        phydat_t temperature;

        /*
         * Read the temperature sensor
         * and store the result in the temperature variable
         * saul_reg_read returns the dimension of the data read (1 in this case)
         */
        int dimension = saul_reg_read(temperature_sensor, &temperature);

        /* If the read was successful (1+ Dimensions), print the temperature */
        if (dimension <= 0) {
            puts("Error reading temperature sensor");
            return 1;
        }

        /* Dump the temperature to the console */
        phydat_dump(&temperature, dimension);

        /* Sleep for 1 seconds */
        ztimer_sleep(ZTIMER_MSEC, 1000);
    }
}
