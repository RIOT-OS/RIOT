/*
 * Copyright (C) Technische Universität Braunschweig 2023
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sen5x
 * @{
 *
 * @file
 * @brief       sen5x example application
 *
 * @author      Daniel Prigoshij <prigoshi@ibr.cs.tu-bs.de>
 *
 * @}
 */

#include <stdio.h>
#include "sen5x.h"
#include "sen5x_params.h"
#include "sen5x_constants.h"
#include "xtimer.h"

#define TEST_ITERATIONS (100)
#define MEASUREMENT_INTERVAL_USECS (2000000)

sen5x_t device;
sen5x_params_t para = SEN5X_PARAMS;
sen5x_measurement_t values;

int main(void)
{
    sen5x_init(&device, &para);
    xtimer_usleep(500000);
    sen5x_wake(&device);
    xtimer_usleep(500000);

    for(int i = 0; i < TEST_ITERATIONS; i++) {
        sen5x_read_values(&device, &values);
        xtimer_usleep(MEASUREMENT_INTERVAL_USECS);

        printf("Mass concentration pm1p0: %.1f µg/m³\n", values.mass_concentration_pm1p0 / 10.0f);
        printf("Mass concentration pm2p5: %.1f µg/m³\n", values.mass_concentration_pm2p5 / 10.0f);
        printf("Mass concentration pm4p0: %.1f µg/m³\n", values.mass_concentration_pm4p0 / 10.0f);
        printf("Mass concentration pm10p0: %.1f µg/m³\n", values.mass_concentration_pm10p0 / 10.0f);

        if (values.ambient_humidity == 0x7fff) {
            printf("Ambient humidity: n/a\n");
        } else {
            printf("Ambient humidity: %.1f %%RH\n",
            values.ambient_humidity / 100.0f);
        }
        if (values.ambient_temperature == 0x7fff) {
            printf("Ambient temperature: n/a\n");
        } else {
            printf("Ambient temperature: %.1f °C\n",
            values.ambient_temperature / 200.0f);
        }
        if (values.voc_index == 0x7fff) {
            printf("Voc index: n/a\n");
        } else {
            printf("Voc index: %.1f\n", values.voc_index / 10.0f);
        }
        if (values.nox_index == 0x7fff) {
            printf("Nox index: n/a\n");
        } else {
            printf("Nox index: %.1f\n", values.nox_index / 10.0f);
        }
    }
    return 0;
}
