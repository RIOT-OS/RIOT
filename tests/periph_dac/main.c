/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
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

#include "cpu.h"
#include "board.h"
#include "vtimer.h"
#include "periph/dac.h"
#include "periph/adc.h"

#if DAC_NUMOF < 1
#error "Please enable at least 1 DAC device to run this test"
#endif

#define RES             DAC_RES_10BIT
#define DELAY           (1000 * 1000U)
#define MAX_VALUE_STEPS 1000

static int values[DAC_NUMOF][DAC_MAX_CHANNELS];

int main(void)
{
    uint16_t write_value = 0;
    int8_t status_dac_write;

    puts("\nRIOT DAC peripheral driver test\n");
    puts("This test simply sets each available DAC channel about every 100ms\n\n");

    for (int i = 0; i < DAC_NUMOF; i++) {
        /* initialize result vector */
        for (int j = 0; j < DAC_MAX_CHANNELS; j++) {
            values[i][j] = -1;
        }
        /* initialize DAC device */
        printf("Initializing DAC_%i @ %i bit resolution", i, (6 + (2* RES)));
        if (dac_init(i, RES) == 0) {
            puts("    ...[ok]");
        }
        else {
            puts("    ...[failed]");
            return 1;
        }

#if ADC_NUMOF > 0
        printf("Initializing ADC_0 @ %i bit resolution", (6 + (2* RES)));
        if (adc_init(0, RES) == 0) {
            puts("    ...[ok]");
        }
        else {
            puts("    ...[failed]");
            return 1;
        }
#endif
    }
    puts("\n");

    while (1) {
        for (int i = 0; i < DAC_NUMOF; i++) {
            for (int j = 0; j < DAC_MAX_CHANNELS; j++) {
                /* Write values to DAC */
                status_dac_write = dac_write(i, j, write_value);
                if (status_dac_write < 0) {
                    printf("%i: Something went wrong writing DAC\n", status_dac_write);
                    return -1;
                }
#if ADC_NUMOF > 0
                /* Read values from ADC */
                int sample = adc_sample(ADC_0, 0);
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
        /* sleep a little while */
        vtimer_usleep(DELAY);
    }

    return 0;
}
