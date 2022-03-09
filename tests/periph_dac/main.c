/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
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

#define DELAY           (100U)
#define STEPS           (1000U)

int main(void)
{
    xtimer_ticks32_t last = xtimer_now();
    uint16_t val = 0;
    uint16_t step = 0xffff / STEPS;

    puts("\nRIOT DAC peripheral driver test\n");
    puts("This test application produces a saw tooth signal on each available\n"
         "DAC line. The period of the signal should be around 100ms\n");

    /* initialize all DAC lines */
    for (unsigned i = 0; i < DAC_NUMOF; i++) {
        if (dac_init(DAC_LINE(i)) < 0) {
            printf("Error initializing DAC_LINE(%u)\n", i);
            return 1;
        }
        else {
            printf("Successfully initialized DAC_LINE(%u)\n", i);
        }
    }
    puts("");

    /* create saw tooth signal */
    while (1) {
        for (unsigned i = 0; i < DAC_NUMOF; i++) {
            dac_set(DAC_LINE(i), val);
        }
        val += step;
        xtimer_periodic_wakeup(&last, DELAY);
    }

    return 0;
}
