/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for taking multiple samples from two ADCs in parallel
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>

#include "macros/units.h"
#include "periph/adc.h"
#include "time_units.h"
#include "ztimer.h"
#include "ztimer/stopwatch.h"

#define RES             ADC_RES_10BIT
#define DELAY_MS        100U
#define NUM_SAMPLES     128U
#define SAMPLE_FREQ     KHZ(100)

int main(void)
{
    if (ADC_NUMOF < 2) {
        printf("This test needs at least 2 ADC lines\n");
        return -1;
    }

    /* initialize two ADC lines */
    for (unsigned i = 0; i < 2; i++) {
        if (adc_init(ADC_LINE(i)) < 0) {
            printf("Initialization of ADC_LINE(%u) failed\n", i);
            return 1;
        } else {
            printf("Successfully initialized ADC_LINE(%u)\n", i);
        }
    }

    ztimer_stopwatch_t clock = {
        .clock = ZTIMER_USEC,
    };

    while (1) {
        uint16_t samples[2][NUM_SAMPLES];

        const adc_t lines[] = { ADC_LINE(0), ADC_LINE(1) };

        ztimer_stopwatch_start(&clock);
        adc_sample_multi(2, lines, NUM_SAMPLES, samples, RES, SAMPLE_FREQ);

        uint32_t usecs = ztimer_stopwatch_measure(&clock);
        ztimer_stopwatch_stop(&clock);
        printf("%u samples in %"PRIu32" µs (%lu Hz)\n", NUM_SAMPLES, usecs,
               (US_PER_SEC * NUM_SAMPLES) / usecs);

        for (unsigned i = 0; i < NUM_SAMPLES; ++i) {
            printf("%u\t%u\n", samples[0][i], samples[1][i]);
        }

        ztimer_sleep(ZTIMER_MSEC, DELAY_MS);
    }

    return 0;
}
