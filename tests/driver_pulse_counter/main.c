/*
 * Copyright (C) 2017 UC Berkeley
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
 * @brief       Test application for the PULSE_COUNTER driver
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include <stdio.h>

#include "timex.h"
#include "ztimer.h"
#include "pulse_counter_params.h"

#define SLEEP_MS  MS_PER_SEC

int main(void)
{
    pulse_counter_t dev;

    printf("PULSE_COUNTER driver test application\n");

    /* Initialization */
    if (pulse_counter_init(&dev, &pulse_counter_params[0])) {
        printf("[Failed]");
        return 1;
    }
    else {
        printf("[OK]\n");
    }

    while (1) {
        /* Pulse counter reading */
        int16_t count = pulse_counter_read_with_reset(&dev);
        printf("pulse counter: %d\n", count);

        ztimer_sleep(ZTIMER_MSEC, SLEEP_MS);
    }

    return 0;
}
