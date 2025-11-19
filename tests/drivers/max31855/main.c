/*
 * SPDX-FileCopyrightText: 2024 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the max31855 thermocouple-to-digital converter driver
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "max31855.h"
#include "max31855_params.h"
#include "ztimer.h"

#define DELAY   (2)

int main(void)
{
    max31855_t dev;
    max31855_data_t data;

    puts("MAX31855 Thermocouple-to-Digital converter test application\n");

    /* initialize first configured sensor */
    printf("Initializing MAX31855 converter...\t");
    if (max31855_init(&dev, &max31855_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    /* periodically convert temperature values */
    while (1) {
        ztimer_sleep(ZTIMER_SEC, DELAY);
        max31855_read(&dev, &data);

        if (data.fault != MAX31855_FAULT_NO_FAULT) {
            switch (data.fault) {
                case MAX31855_FAULT_GND_SHORT:
                    puts("Fault: Short to GND");
                    break;
                case MAX31855_FAULT_VCC_SHORT:
                    puts("Fault: Short to VCC");
                    break;
                case MAX31855_FAULT_OPEN_CIRCUIT:
                    puts("Fault: Open circuit");
                    break;
                default:
                    puts("Fault: Unknown");
                    break;
            }

            continue;
        }

        printf("Thermocouple temperature: %" PRIi32 ".%"PRId32 "°C\n",
               data.thermocouple_temperature/100, data.thermocouple_temperature%100);

        printf("Internal temperature: %" PRIi32 ".%" PRId32 "°C\n",
               data.internal_temperature/100, data.internal_temperature%100);
    }

    return 0;
}
