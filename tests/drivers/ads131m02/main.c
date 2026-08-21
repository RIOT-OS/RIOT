/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief      Test application for the ADS131M02 ADC driver
 *
 * @author     Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <inttypes.h>
#include <stdio.h>
#include "ads131m02.h"
#include "ads131m02_params.h"
#include "periph_cpu.h"
#include "ztimer.h"

#define NUM_SAMPLES     10

int main(void)
{
    ads131m02_t dev;
    uint32_t chan0[NUM_SAMPLES];
    uint32_t chan1[NUM_SAMPLES];

    int res = ads131m02_init(&dev, &ads131m02_params[0]);

    printf("ads131m02_init: %d\n", res);
    if (res != 0) {
        return res;
    }

    res = ads131m02_sample(&dev, ADS131M02_CHANNEL_0 | ADS131M02_CHANNEL_1,
                           chan0, chan1, NUM_SAMPLES);

    printf("ads131m02_sample: %d\n", res);

    for (int i = 0; i < res; ++i) {
        printf("[%2d] chan0: %10" PRIu32 "  chan1: %10" PRIu32 "\n",
               i, chan0[i], chan1[i]);
    }

    return 0;
}
