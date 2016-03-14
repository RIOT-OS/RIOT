/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test case for the low-level DAC driver
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "periph/dac.h"


#define RES             DAC_RES_10BIT
#define ADC_RES         ADC_RES_10BIT
#define DELAY           (1000 * 1000U)
#define MAX_VALUE_STEPS 1000


int main(void)
{
    uint32_t last = xtimer_now();
    uint16_t val = 0;
    uint16_t step = 0xffff / STEPS;

    puts("\nRIOT DAC peripheral driver test\n");
    puts("This test application produces a saw tooth signal on each available\n"
         "DAC line. The period of the signal should be around 100ms\n");

    /* initialize all DAC lines */
    for (int i = 0; i < DAC_NUMOF; i++) {
        if (dac_init(DAC_LINE(i)) < 0) {
            printf("Error initializing DAC_LINE(%i)\n", i);
            return 1;
        }

#ifdef ADC_NUMOF
        printf("Initializing ADC_LINE(0)");
        if (adc_init(ADC_LINE(0)) == 0) {
            puts("    ...[ok]");
        }
        else {
            printf("Successfully initialized DAC_LINE(%i)\n", i);
        }
    }
    puts("");

    /* create saw tooth signal */
    while (1) {
        for (int i = 0; i < DAC_NUMOF; i++) {
            for (int j = 0; j < DAC_MAX_CHANNELS; j++) {
                /* Write values to DAC */
                status_dac_write = dac_write(i, j, write_value);
                if (status_dac_write < 0) {
                    printf("%i: Something went wrong writing DAC\n", status_dac_write);
                    return -1;
                }
#ifdef ADC_NUMOF
                /* Read values from ADC */
                int sample = adc_sample(ADC_LINE(0), ADC_RES);
                if (sample < 0) {
                    printf("%i: Something went wrong sampling ADC\n", sample);
                    return -1;
                }
                printf("Wrote %i Read %i using DAC %i Channel %i and ADC_0 Channel 0\n", write_value, sample, i, j);
#else
                printf("Wrote %i to DAC %i Channel %i\n", write_value, i, j);
#endif
            }
        }
        puts("\n");
        write_value+=100;
        if (write_value >= MAX_VALUE_STEPS) {
            write_value = 0;
        }
        val += step;
        xtimer_usleep_until(&last, DELAY);
    }

    return 0;
}
