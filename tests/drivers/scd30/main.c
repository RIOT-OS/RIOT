/*
 * SPDX-FileCopyrightText: 2020 Puhang Ding
 * SPDX-FileCopyrightText: 2020 Jan Schlichter
 * SPDX-FileCopyrightText: 2020 Nishchay Agrawal
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_scd30
 * @{
 * @file
 * @brief       Sensirion SCD30 sensor driver implementation
 *
 * @author      Puhang Ding      <czarsir@gmail.com>
 * @author      Jan Schlichter   <schlichter@ibr.cs.tu-bs.de>
 * @author      Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 * @}
 */

#include <stdio.h>
#include "periph/gpio.h"
#include "xtimer.h"
#include "scd30.h"
#include "scd30_params.h"
#include "scd30_internal.h"

#define MEASUREMENT_INTERVAL_SECS (2)

scd30_t scd30_dev;
scd30_params_t params = SCD30_PARAMS;
scd30_measurement_t result;

int main(void)
{
    printf("SCD30 Test:\n");
    int i = 0;

    scd30_init(&scd30_dev, &params);
    uint16_t pressure_compensation = SCD30_DEF_PRESSURE;
    uint16_t value = 0;
    uint16_t interval = MEASUREMENT_INTERVAL_SECS;

    scd30_set_param(&scd30_dev, SCD30_INTERVAL, MEASUREMENT_INTERVAL_SECS);
    scd30_set_param(&scd30_dev, SCD30_START, pressure_compensation);

    scd30_get_param(&scd30_dev, SCD30_INTERVAL, &value);
    printf("[test][dev-%d] Interval: %u s\n", i, value);
    scd30_get_param(&scd30_dev, SCD30_T_OFFSET, &value);
    printf("[test][dev-%d] Temperature Offset: %u.%02u C\n", i, value / 100u,
           value % 100u);
    scd30_get_param(&scd30_dev, SCD30_A_OFFSET, &value);
    printf("[test][dev-%d] Altitude Compensation: %u m\n", i, value);
    scd30_get_param(&scd30_dev, SCD30_ASC, &value);
    printf("[test][dev-%d] ASC: %u\n", i, value);
    scd30_get_param(&scd30_dev, SCD30_FRC, &value);
    printf("[test][dev-%d] FRC: %u ppm\n", i, value);

    while (i < TEST_ITERATIONS) {
        xtimer_sleep(1);
        scd30_read_triggered(&scd30_dev, &result);
        printf(
            "[scd30_test-%d] Triggered measurements co2: %.02fppm,"
            " temp: %.02f°C, hum: %.02f%%. \n", i, result.co2_concentration,
            result.temperature, result.relative_humidity);
        i++;
    }

    i = 0;
    scd30_start_periodic_measurement(&scd30_dev, &interval,
                                     &pressure_compensation);

    while (i < TEST_ITERATIONS) {
        xtimer_sleep(MEASUREMENT_INTERVAL_SECS);
        scd30_read_periodic(&scd30_dev, &result);
        printf(
            "[scd30_test-%d] Continuous measurements co2: %.02fppm,"
            " temp: %.02f°C, hum: %.02f%%. \n", i, result.co2_concentration,
            result.temperature, result.relative_humidity);
        i++;
    }

    scd30_stop_measurements(&scd30_dev);

    return 0;
}
