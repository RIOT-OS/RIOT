/*
 * Copyright (C) 2021 Inria
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
 * @brief       event_timeout application using ztimer
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "atomic_utils.h"
#include "test_utils/expect.h"
#include "timex.h"
#include "thread.h"
#include "mutex.h"
#include "event.h"
#include "event/timeout.h"
#include "event/periodic.h"
#include "event/thread.h"
#include "event/callback.h"
#include "ztimer.h"
#include "ztimer/periodic.h"

#define EVENT_TIMEOUT_TIME       (1 * US_PER_SEC)

static void callback_never(void *arg);
static void callback_timed(void *arg);
static void callback_4times(void *arg);

static uint8_t iter;
static event_timeout_t event_timeout;
static event_timeout_t event_timeout_cleared;
static event_periodic_t event_periodic;
static event_callback_t event_4times = EVENT_CALLBACK_INIT(callback_4times, &iter);
static event_callback_t event_timed = EVENT_CALLBACK_INIT(callback_timed, &iter);
static event_callback_t event_never = EVENT_CALLBACK_INIT(callback_never, 0);
static uint32_t before;
static mutex_t lock = MUTEX_INIT_LOCKED;

static void callback_timed(void *arg)
{
    expect(arg == event_timed.arg);
    uint32_t now = ztimer_now(ZTIMER_USEC);
    expect((now - before) >= 1 * US_PER_SEC);
    printf("triggered timed callback after %" PRIu32 "us\n", now - before);
    mutex_unlock(&lock);
}

static void callback_4times(void *arg)
{
    uint8_t *count = (uint8_t *)arg;

    *count = *count + 1;
    uint32_t now = event_periodic.timer.last;
    uint32_t elapsed = now - before;
    before = now;
    expect((elapsed) >= 1 * US_PER_SEC);
    if (*count <= 4) {
        printf("trigger %d of periodic timeout, elapsed time: %" PRIu32 " us\n",
               *count, elapsed);
    }
    if (*count == 2) {
        puts("stop periodic event");
        event_periodic_stop(&event_periodic);
        puts("resume periodic event, 2 triggers remaining");
        event_periodic_start(&event_periodic, EVENT_TIMEOUT_TIME);
        before = event_periodic.timer.last;
    }
    if (*count == 4) {
        mutex_unlock(&lock);
    }
    else if (*count > 4) {
        /* this callback should never be called */
        puts("this should only be called 4 times");
        puts("[FAILED]");
        while (1) {
            expect(false);
        }
    }
}

static void callback_never(void *arg)
{
    (void)arg;
    /* this callback should never be called */
    puts("this should never happen");
    puts("[FAILED]");
    while (1) {
        expect(false);
    }
}

int main(void)
{
    iter = 0;
    puts("posting periodic timed callback with timeout 1sec");
    event_periodic_init(&event_periodic, ZTIMER_USEC, EVENT_PRIO_MEDIUM,
                        &event_4times.super);
    event_periodic_set_count(&event_periodic, 4);
    event_periodic_start(&event_periodic, EVENT_TIMEOUT_TIME);
    before = event_periodic.timer.last;
    puts("waiting for periodic callback to be triggered 4 times");
    mutex_lock(&lock);
    puts("posting timed callback with timeout 0.5sec, clear right after");
    event_timeout_clear(&event_timeout_cleared);
    event_timeout_ztimer_init(&event_timeout_cleared, ZTIMER_USEC,
                              EVENT_PRIO_MEDIUM, &event_never.super);
    event_timeout_set(&event_timeout_cleared, EVENT_TIMEOUT_TIME / 2);
    event_timeout_clear(&event_timeout_cleared);
    puts("posting timed callback with timeout 1sec");
    event_timeout_ztimer_init(&event_timeout, ZTIMER_USEC, EVENT_PRIO_MEDIUM,
                              &event_timed.super);
    before = ztimer_now(ZTIMER_USEC);
    event_timeout_set(&event_timeout, EVENT_TIMEOUT_TIME);
    puts("waiting for timed callback to trigger");
    mutex_lock(&lock);
    puts("[SUCCESS]");
    return 0;
}
