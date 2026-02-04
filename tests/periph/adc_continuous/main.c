/*
 * SPDX-FileCopyrightText: 2014-2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for peripheral ADC drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "ztimer.h"
#include "periph/adc.h"

#define RES             ADC_RES_10BIT
#define DELAY_MS        100U

int main(void)
{
    int sample = 0;

    puts("\nRIOT ADC peripheral driver test\n");
    puts("This test will sample all available ADC lines once every 100ms with\n"
         "a 10-bit resolution and print the sampled results to STDIO\n\n");

    /* initialize all available ADC lines */
    for (unsigned i = 0; i < ADC_NUMOF; i++) {
        if (adc_init(ADC_LINE(i)) < 0) {
            printf("Initialization of ADC_LINE(%u) failed\n", i);
            return 1;
        } else {
            printf("Successfully initialized ADC_LINE(%u)\n", i);
        }
    }

    adc_continuous_begin(RES);
    while (1) {
        for (unsigned i = 0; i < ADC_NUMOF; i++) {
            sample = adc_continuous_sample(ADC_LINE(i));
            printf("ADC_LINE(%u): %i\n", i, sample);
        }
        ztimer_sleep(ZTIMER_MSEC, DELAY_MS);
    }

    adc_continuous_stop();
    return 0;
}
