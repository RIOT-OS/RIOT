/*
 * SPDX-FileCopyrightText: 2019 Jan Schlichter
 * SPDX-FileCopyrightText: 2020 Nishchay Agrawal
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for sdp3x temperature and differential
 *              pressure sensor driver
 *
 * @author      Jan Schlichter   <schlichter@ibr.cs.tu-bs.de>
 * @author      Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 * @}
 */
#include <stdio.h>

#include "xtimer.h"
#include "sdp3x_params.h"
#include "sdp3x.h"

#define MEASUREMENT_INTERVAL_US (1 * US_PER_SEC)

static sdp3x_t sdp3x_dev;
static sdp3x_params_t params = SDP3X_PARAMS;
sdp3x_measurement_t result;
xtimer_t continuous_timer;

void continuous_measurement_callback(void *arg)
{
    uint32_t *interval = (uint32_t *)arg;

    sdp3x_read_continuous(&result, &sdp3x_dev);

    xtimer_set(&continuous_timer, *interval);
}

int main(void)
{

    sdp3x_init(&sdp3x_dev, &params);

    xtimer_sleep(1);

    uint32_t interval = MEASUREMENT_INTERVAL_US;

    uint32_t i = 0;

    sdp3x_start_continuous(&sdp3x_dev,
                           SDP3X_FLAG_DIFF_PRESS|SDP3X_FLAG_AVERAGE);

    continuous_timer.callback = (void *)continuous_measurement_callback;
    continuous_timer.arg = &interval;

    xtimer_set(&continuous_timer, interval);

    /*
     * Get measurements using continuous method for TEST_ITERATIONS
     * number of iterations
     */
    while (i < (uint32_t)TEST_ITERATIONS) {
        printf(
            "Continuous values for temp: %.02f°C pressure: %.02fPa \n",
            result.temperature/100.f, result.differential_pressure/100.f);
        xtimer_sleep(1);
        i++;
    }

    sdp3x_stop_continuous(&sdp3x_dev, &continuous_timer);
    i = 0;

    /*
     * Get measurements using triggered method for TEST_ITERATIONS
     * number of iterations
     */
    while (i < (uint32_t)TEST_ITERATIONS) {
        sdp3x_read_single_measurement(&sdp3x_dev, SDP3X_FLAG_DIFF_PRESS,
                                      &result);
        printf(
            "Triggered values for temp: %.02f°C pressure: %.02fPa \n",
            result.temperature/100.f, result.differential_pressure/100.f);
        xtimer_sleep(1);
        i++;
    }

    return 0;
}
