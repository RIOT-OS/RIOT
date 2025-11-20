/*
 * SPDX-FileCopyrightText: 2023 Technische Universität Braunschweig
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "ztimer.h"

#define MEASUREMENT_INTERVAL_USECS (2000000)

sen5x_t device;
sen5x_params_t para = SEN5X_PARAMS;
sen5x_measurement_t values;

int main(void)
{
    int ret = sen5x_init(&device, &para);
    if (ret > 0) {
        printf("[ERROR] Initialization of SEN5X sensor failed!\n");
        while (1) {};
    }
    ztimer_sleep(ZTIMER_USEC, 500000);
    sen5x_wake(&device);
    ztimer_sleep(ZTIMER_USEC, 500000);

    while (1) {
        sen5x_read_values(&device, &values);
        ztimer_sleep(ZTIMER_USEC, MEASUREMENT_INTERVAL_USECS);

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
