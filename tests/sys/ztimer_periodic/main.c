/*
 * SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "buildinfo/cpu.h"
#include "ztimer.h"
#include "ztimer/periodic.h"
#include "fmt.h"
#include "mutex.h"

static mutex_t _mutex = MUTEX_INIT_LOCKED;

#define REPEAT 5

static uint32_t _times[REPEAT];
static int _count;
#define ZTIMER_CLOCKS { ZTIMER_MSEC, ZTIMER_USEC }
static const char *_names[] = { "ZTIMER_MSEC", "ZTIMER_USEC" };
static uint32_t _intervals[] = { 100, 10000 };
static uint32_t _max_offsets[] = {
    2,
#ifdef CPU_NATIVE
    /* native is running on top of a multi-media OS, not a real-time OS */
    1000
#else
    100
#endif
};

static bool callback(void *arg)
{
    _times[_count] = ztimer_now(arg);

    _count += 1;

    /* enable this to test underflow behavior */
#if 0
    if (count == 2) {
        ztimer_spin(arg, INTERVAL * 2);
    }
#endif

    if (_count == REPEAT) {
        mutex_unlock(&_mutex);
    }

    return _count < REPEAT;
}

int main(void)
{
    ztimer_periodic_t t;
    ztimer_clock_t * const clocks[] = ZTIMER_CLOCKS;
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
            print_str("Test failed, timer not set\n");
            return 1;
        }

        /* wait for periodic to trigger REPEAT times */
        mutex_lock(&_mutex);

        for (unsigned i = 0; i < REPEAT; i++) {
            uint32_t offset = labs((int32_t)(_times[i] - _intervals[j] - last));
            printf("i: %u time: %" PRIu32 " offset: %" PRIu32 "\n",
                   i, _times[i], offset);
            if (offset > _max_offsets[j]) {
                print_str("Offset too large, failure\n");
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
