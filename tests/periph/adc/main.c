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

#define DELAY_MS        100U

int main(void)
{
    puts("\nRIOT ADC peripheral driver test\n");
    puts("This test will sample all available ADC lines once every 100ms with\n"
         "6 to 16-bit resolution and print the sampled results to STDOUT.\n"
         "Not all MCUs support all resolutions, unsupported resolutions\n"
         "are printed as -1.\n");

    /* initialize all available ADC lines */
    for (unsigned i = 0; i < ADC_NUMOF; i++) {
        if (adc_init(ADC_LINE(i)) < 0) {
            printf("Initialization of ADC_LINE(%u) failed\n", i);
            return 1;
        } else {
            printf("Successfully initialized ADC_LINE(%u)\n", i);
        }
    }

    while (1) {
        for (unsigned i = 0; i < ADC_NUMOF; i++) {
            int sample6  = adc_sample(ADC_LINE(i), ADC_RES_6BIT);
            int sample8  = adc_sample(ADC_LINE(i), ADC_RES_8BIT);
            int sample10 = adc_sample(ADC_LINE(i), ADC_RES_10BIT);
            int sample12 = adc_sample(ADC_LINE(i), ADC_RES_12BIT);
            int sample14 = adc_sample(ADC_LINE(i), ADC_RES_14BIT);
            int sample16 = adc_sample(ADC_LINE(i), ADC_RES_16BIT);

            printf("ADC_LINE(%u): %2i %3i %4i %4i %5i %5i\n", i, sample6, sample8, sample10,
                   sample12, sample14, sample16);
        }
        putchar('\n');
        ztimer_sleep(ZTIMER_MSEC, DELAY_MS);
    }

    return 0;
}
