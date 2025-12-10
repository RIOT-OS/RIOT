/*
 * SPDX-FileCopyrightText: 2017 UC Berkeley
 * SPDX-License-Identifier: LGPL-2.1-only
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
