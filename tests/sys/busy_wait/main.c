/*
 * SPDX-FileCopyrightText: 2024 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Busy Wait loop Test Application
 *
 *          This can be used to determine `CPU_CYCLES_PER_LOOP` by
 *          comparing the time the busy wait loop took with the
 *          actual µsec timer.
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <stdio.h>
#include "busy_wait.h"
#include "ztimer/stopwatch.h"

static inline void _measure_interval(ztimer_stopwatch_t *clock, unsigned usec)
{
    unsigned usec_real;

    printf("waiting for %u µs…\n", usec);
    ztimer_stopwatch_start(clock);
    busy_wait_us(usec);
    usec_real = ztimer_stopwatch_measure(clock);
    printf("took %u µs (diff: %d µs)\n", usec_real, (int)usec_real - usec);
}

int main(void)
{
    ztimer_stopwatch_t clock;
    ztimer_stopwatch_init(ZTIMER_USEC, &clock);

    _measure_interval(&clock, 10);
    _measure_interval(&clock, 100);
    _measure_interval(&clock, 1000);
    _measure_interval(&clock, 10000);

    return 0;
}
