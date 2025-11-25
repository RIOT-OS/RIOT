/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
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
 * @brief       Application to test functionality of the frequency meter
 *              peripheral
 *
 * @author      Urs Gompper <urs.gompper@ml-pa.com>
 * @}
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "periph/freqm.h"

int main(void)
{
    puts("FREQM peripheral driver test");

    /* Initialize frequency meter peripheral */
    freqm_init(0);

    uint32_t period_us = UINT32_MAX;
    uint32_t freq_hz = 0;

    /* Measure in blocking mode */
    if (!freqm_frequency_get(0, &freq_hz, period_us)) {
        printf("Measured Clock Frequency: %ld Hz\n", freq_hz);
    }
    else {
        puts("Overflow occurred to the FREQM value counter!");
        return EXIT_FAILURE;
    }

    puts("Test run finished.");

    /* main thread exits */
    return 0;
}
