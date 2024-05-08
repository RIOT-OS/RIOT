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
#include "ztimer.h"

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

    while (1) {
        uint16_t samples[2][NUM_SAMPLES];

        const adc_t lines[] = { ADC_LINE(0), ADC_LINE(1) };

        adc_sample_multi(2, lines, NUM_SAMPLES, samples, RES, SAMPLE_FREQ);

        for (unsigned i = 0; i < NUM_SAMPLES; ++i) {
            printf("%u\t%u\n", samples[0][i], samples[1][i]);
        }

        ztimer_sleep(ZTIMER_MSEC, DELAY_MS);
    }

    return 0;
}
