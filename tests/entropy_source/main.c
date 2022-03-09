/*
 * Copyright (C) 2020 HAW Hamburg
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
 * @brief       Test application for Entropy Sources
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "fmt.h"
#include "xtimer.h"
#include "entropy_source.h"
#include "entropy_source/zero_entropy.h"
#if MODULE_PERIPH_ADC
#include "entropy_source/adc_noise.h"
#endif

#ifndef NUM_BYTES
#define NUM_BYTES     (64)
#endif

int main(void)
{
    uint8_t buf1;
    uint8_t buf2[NUM_BYTES];
    int ret, prop_cutorr;

    /* Get Adaptive Proportion Test cutoff value and use as max. number of requests */
    prop_cutorr = entropy_source_test_prop_cutoff(
        CONFIG_ENTROPY_SOURCE_ZERO_HMIN);

    /* Initialize zero entropy source */
    entropy_source_zero_init();

    /* Request single bytes*/
    for (int i = 0; i < prop_cutorr + 1; i++) {
        ret = entropy_source_zero_get(&buf1, 1);
        printf("Zero entropy single request %i/%i returned: %i\n",  i,
               prop_cutorr, ret);
    }

    /* Request a buffer */
    ret = entropy_source_zero_get(buf2, NUM_BYTES);
    printf("Zero entropy request %i Bytes: %i\n", NUM_BYTES, ret);

#if MODULE_PERIPH_ADC
    uint32_t start, stop;
    uint32_t entropy;

    entropy = entropy_source_adc_entropy_per_sample();
    printf("ADC noise source entropy/sample: %"PRIu32" [2^16 * bit / sample]\n", \
            entropy);
    print_str("ADC noise source entropy/sample: ");
    print_float(ENTROPY_SOURCE_HMIN_SCALE_BACK(entropy), 6);
    print_str(" [bit / sample]\n");

    /* Initialize ADC noise source */
    entropy_source_adc_init();

    for (unsigned i = 0; i < NUM_BYTES; i++) {
        /*Request single bytes*/
        ret = entropy_source_adc_get(&buf1, 1);
        if (ret < 0) {
            printf("ADC noise single request %u/%i: %i\n", i, NUM_BYTES, ret);
        }
        else {
            printf("%02x\n", buf1);
        }
    }

    /* Request a buffer and measure time */
    start = xtimer_now_usec();
    ret = entropy_source_adc_get(buf2, NUM_BYTES);
    stop = xtimer_now_usec();

    for (unsigned i = 0; i < sizeof(buf2); i++) {
        printf("%02x\n", buf2[i]);
    }
    printf("ADC noise request %i Bytes returned: %i. Time: %" PRIu32 "us\n", \
           NUM_BYTES, ret, stop - start);
#endif

    return 0;
}
