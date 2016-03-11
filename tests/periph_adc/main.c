/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
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
 * @brief       Test application for peripheral ADC drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "periph/adc.h"


#define RES             ADC_RES_10BIT
#define DELAY           (100 * 1000U)


int main(void)
{
    uint32_t last = xtimer_now();
    int sample = 0;

    puts("\nRIOT ADC peripheral driver test\n");
    puts("This test will sample all available ADC lines once every 100ms with\n"
         "a 10-bit resolution and print the sampled results to STDIO\n\n");

    /* initialize all available ADC lines */
    for (int i = 0; i < ADC_NUMOF; i++) {
        if (adc_init(ADC_LINE(i)) < 0) {
            printf("Initialization of ADC_LINE(%i) failed\n", i);
            return 1;
        } else {
            printf("Successfully initialized ADC_LINE(%i)\n", i);
        }
    }

    while (1) {
        for (int i = 0; i < ADC_NUMOF; i++) {
            sample = adc_sample(ADC_LINE(i), RES);
            if (sample < 0) {
                printf("ADC_LINE(%i): 10-bit resolution not applicable\n", i);
            } else {
                printf("ADC_LINE(%i): %i\n", i, sample);
            }
        }
        xtimer_usleep_until(&last, DELAY);
    }

    return 0;
}
