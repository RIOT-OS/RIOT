/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *               2020 Freie Universität Berlin
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       ztimer periodic test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "ztimer.h"
#include "ztimer/periodic.h"
#include "fmt.h"
#include "mutex.h"

static mutex_t _mutex = MUTEX_INIT_LOCKED;

#define N 5

static uint32_t _times[N];
static int _count;
#define CLOCKS { ZTIMER_MSEC, ZTIMER_USEC }
static const char *_names[] = { "ZTIMER_MSEC", "ZTIMER_USEC" };
static uint32_t _intervals[] = { 100, 10000 };
static uint32_t _max_offsets[] = { 2, 100 };

static int callback(void *arg)
{
    _times[_count] = ztimer_now(arg);

    _count += 1;

    /* enable this to test underflow behavior */
#if 0
    if (count == 2) {
        ztimer_spin(arg, INTERVAL * 2);
    }
#endif

    if (_count == N) {
        mutex_unlock(&_mutex);
    }

    return (_count == N);
}

int main(void)
{
    ztimer_periodic_t t;
    ztimer_clock_t * const clocks[] = CLOCKS;
    int failed = 0;

    for (size_t j = 0; j < ARRAY_SIZE(clocks); j++) {
        printf("Testing clock: %s\n", _names[j]);
        ztimer_clock_t *clock = clocks[j];
        ztimer_periodic_init(clock, &t, callback, clock, _intervals[j]);

        _count = 0;
        ztimer_periodic_start(&t);
        /* ztimer_periodic_start stores the value of ztimer_now() + period into t.last. We use that
         * value instead of calling ztimer_now() again, as on slow boards the introduced offset can
         * result in a failing test, despite the timeout actually being triggered close enough
         * to the target. */
        uint32_t last = t.last - _intervals[j];

        if (!ztimer_is_set(clock, &t.timer)) {
            print_str("Test failed\n");
            return 1;
        }

        /* wait for periodic to trigger N times */
        mutex_lock(&_mutex);

        for (unsigned i = 0; i < N; i++) {
            uint32_t offset = labs((int32_t)(_times[i] - _intervals[j] - last));
            printf("i: %u time: %" PRIu32 " offset: %" PRIu32 "\n",
                   i, _times[i], offset);
            if (offset > _max_offsets[j]) {
                failed = 1;
            }
            last = _times[i];
        }

        ztimer_periodic_stop(&t);
    }

    if (!failed) {
        print_str("Test successful.\n");
    }
    else {
        print_str("Test failed!\n");
    }
}
