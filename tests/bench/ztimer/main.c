/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       ztimer set / remove / now benchmark application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "test_utils/expect.h"

#include "msg.h"
#include "thread.h"
#include "ztimer.h"

#ifndef ZTIMER
#define ZTIMER ZTIMER_MSEC
#endif

#ifndef NUMOF_TIMERS
#define NUMOF_TIMERS   (1000U)
#endif

#ifndef REPEAT
#define REPEAT   (1000U)
#endif

#ifndef BASE
#define BASE    (10000000LU)
#endif

#ifndef SPREAD
#define SPREAD  (10LU)
#endif

static ztimer_t _timers[NUMOF_TIMERS];

/* This variable is set by any timer that actually triggers.  As the test is
 * only testing set/remove/now operations, timers are not supposed to trigger.
 * Thus, after every test there's an 'expect(!_triggers)'
 */
static unsigned _triggers;

/*
 * The test assumes that first, middle and last will always end up in at the
 * same index within the timer queue.  In order to compensate for the time that
 * previous operations take themselves, the interval is corrected. The
 * variables "start" and "_base" are used for that.
 */
uint32_t _base;

static void _callback(void *arg) {
    unsigned *triggers = arg;
    *triggers += 1;
}

/* returns the interval for timer 'n' that has to be set in order to insert it
 * into position n */
static uint32_t _timer_val(unsigned n)
{
    return _base + (SPREAD * n);
}

/* set timer 'n' to its intended position 'n' */
static void _timer_set(unsigned n)
{
    ztimer_set(ZTIMER, &_timers[n], _timer_val(n));
}

/* remove timer 'n' */
static void _timer_remove(unsigned n)
{
    ztimer_remove(ZTIMER, &_timers[n]);
}

static void _print_result(const char *desc, unsigned n, uint32_t total)
{
    printf("%30s %8"PRIu32" / %u = %"PRIu32"\n", desc, total, n, total/n);
}

int main(void)
{
    puts("ztimer benchmark application.\n");

    unsigned n;
    uint32_t before, diff, start;

    /* initializing timer structs */
    for (unsigned int n = 0; n < NUMOF_TIMERS; n++) {
        _timers[n].callback = _callback;
        _timers[n].arg = &_triggers;
    }

    start = ztimer_now(ZTIMER_USEC);

    /*
     * test setting one set timer REPEAT times
     *
     */
    _base = BASE;
    before = ztimer_now(ZTIMER_USEC);
    for (n = 0; n < REPEAT; n++) {
        _timer_set(0);
    }

    diff = ztimer_now(ZTIMER_USEC) - before;

    _print_result("set() one", REPEAT, diff);
    expect(!_triggers);

    /*
     * test removing one unset timer REPEAT times
     *
     */
    before = ztimer_now(ZTIMER_USEC);
    for (n = 0; n < REPEAT; n++) {
        _timer_remove(0);
    }

    diff = ztimer_now(ZTIMER_USEC) - before;

    _print_result("remove() one", REPEAT, diff);
    expect(!_triggers);

    /*
     * test setting / removing one timer REPEAT times
     *
     */
    before = ztimer_now(ZTIMER_USEC);
    _base = BASE  - (before - start);
    for (n = 0; n < REPEAT; n++) {
        _timer_set(0);
        _timer_remove(0);
    }

    diff = ztimer_now(ZTIMER_USEC) - before;

    _print_result("set() + remove() one", REPEAT, diff);
    expect(!_triggers);

    /*
     * test setting NUMOF_TIMERS timers with increasing targets
     *
     */
    before = ztimer_now(ZTIMER_USEC);
    _base = BASE  - (before - start);
    for (unsigned int n = 0; n < NUMOF_TIMERS; n++) {
        _timer_set(n);
    }

    diff = ztimer_now(ZTIMER_USEC) - before;

    _print_result("set() many increasing target", NUMOF_TIMERS, diff);
    expect(!_triggers);

    /*
     * test re-setting first timer REPEAT times
     *
     */
    before = ztimer_now(ZTIMER_USEC);
    _base = BASE  - (before - start);
    for (n = 0; n < REPEAT; n++) {
        _timer_set(0);
    }

    diff = ztimer_now(ZTIMER_USEC) - before;

    _print_result("re-set()  first", REPEAT, diff);
    expect(!_triggers);

    /*
     * test setting middle timer REPEAT times
     *
     */
    before = ztimer_now(ZTIMER_USEC);
    _base = BASE  - (before - start);
    for (n = 0; n < REPEAT; n++) {
        _timer_set(NUMOF_TIMERS/2);
    }

    diff = ztimer_now(ZTIMER_USEC) - before;

    _print_result("re-set() middle", REPEAT, diff);
    expect(!_triggers);

    /*
     * test setting last timer REPEAT times
     *
     */
    before = ztimer_now(ZTIMER_USEC);
    _base = BASE  - (before - start);
    for (n = 0; n < REPEAT; n++) {
        _timer_set(NUMOF_TIMERS - 1);
    }

    diff = ztimer_now(ZTIMER_USEC) - before;

    _print_result("re-set()   last", REPEAT, diff);
    expect(!_triggers);

    /*
     * test removing / setting first timer REPEAT times
     *
     */
    before = ztimer_now(ZTIMER_USEC);
    _base = BASE  - (before - start);
    for (n = 0; n < REPEAT; n++) {
        _timer_remove(0);
        _timer_set(0);
    }

    diff = ztimer_now(ZTIMER_USEC) - before;

    _print_result("remove() + set()  first", REPEAT, diff);
    expect(!_triggers);

    /*
     * test removing / setting middle timer REPEAT times
     *
     */
    before = ztimer_now(ZTIMER_USEC);
    _base = BASE  - (before - start);
    for (n = 0; n < REPEAT; n++) {
        _timer_remove(NUMOF_TIMERS/2);
        _timer_set(NUMOF_TIMERS/2);
    }

    diff = ztimer_now(ZTIMER_USEC) - before;

    _print_result("remove() + set() middle", REPEAT, diff);
    expect(!_triggers);

    /*
     * test removing / setting last timer REPEAT times
     *
     */
    before = ztimer_now(ZTIMER_USEC);
    _base = BASE  - (before - start);
    for (n = 0; n < REPEAT; n++) {
        _timer_remove(NUMOF_TIMERS - 1);
        _timer_set(NUMOF_TIMERS - 1);
    }

    diff = ztimer_now(ZTIMER_USEC) - before;

    _print_result("remove() + set()   last", REPEAT, diff);
    expect(!_triggers);

    /*
     * test removing NUMOF_TIMERS timers (latest first)
     *
     */
    before = ztimer_now(ZTIMER_USEC);
    for (n = 0; n < NUMOF_TIMERS; n++) {
        _timer_remove(NUMOF_TIMERS - n - 1);
    }

    diff = ztimer_now(ZTIMER_USEC) - before;

    _print_result("remove() many decreasing", NUMOF_TIMERS, diff);
    expect(!_triggers);

    /*
     * test ztimer_now()
     *
     */
    before = ztimer_now(ZTIMER_USEC);
    n = REPEAT;
    while (n--) {
        ztimer_now(ZTIMER);
    }

    diff = ztimer_now(ZTIMER_USEC) - before;

    _print_result("ztimer_now()", REPEAT, diff);
    expect(!_triggers);

    _print_result("sizeof(ztimer_t)", NUMOF_TIMERS, sizeof(_timers));

    puts("done.");

    return 0;
}
