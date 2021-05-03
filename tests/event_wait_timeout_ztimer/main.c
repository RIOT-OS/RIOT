/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief       Test application to test event wait timeout
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>

#include "event.h"
#include "thread.h"
#include "timex.h"
#include "thread_flags.h"

#define STATIC_TIMEOUT_MS   (10U)                               /* 10ms */
#define STATIC_TIMEOUT_US   (STATIC_TIMEOUT_MS * US_PER_MS)     /* 10ms */
#define TIMEOUT_US          (50U * US_PER_MS)                   /* 50ms */
#define PRIO                (THREAD_PRIORITY_MAIN - 5)
#define STACKSIZE           (THREAD_STACKSIZE_DEFAULT)

#define FLAG_SYNC           (0x0040)

static void _on_evt(event_t *evt);

static event_queue_t _evtq;
static event_t _evt = { .handler = _on_evt };

static char _stack[STACKSIZE];
static thread_t *_thread_main;

static atomic_uint _wakeup_evt = ATOMIC_VAR_INIT(0);
static atomic_uint _wakeup_timeout = ATOMIC_VAR_INIT(0);

static void _on_evt(event_t *evt)
{
    (void)evt;
    atomic_fetch_add(&_wakeup_evt, 1);
}

static void *_cnt_thread(void *arg)
{
    (void)arg;
    event_queue_init(&_evtq);

    while (1) {
        event_t *evt = event_wait_timeout_ztimer(&_evtq, ZTIMER_USEC, TIMEOUT_US);
        if (evt) {
            evt->handler(evt);
        }
        else {
            atomic_fetch_add(&_wakeup_timeout, 1);
        }
    }

    return NULL;
}

int main(void)
{
    /* setup */
    _thread_main = thread_get_active();

    puts("[START] event_wait_timeout test application.\n");

    /* test 'instant' return */
    event_queue_t tmp_eq = EVENT_QUEUE_INIT;
    event_t *tmp_evt = event_wait_timeout_ztimer(&tmp_eq, ZTIMER_USEC, 0);
    if (tmp_evt != NULL) {
        puts("[FAILED");
        return 1;
    }

    /* test return in a predefined amount of time */
    puts("waiting for event with 10ms timeout @ ZTIMER_USEC...");
    ztimer_now_t before = ztimer_now(ZTIMER_USEC);
    tmp_evt = event_wait_timeout_ztimer(&tmp_eq, ZTIMER_USEC, STATIC_TIMEOUT_US);
    if (tmp_evt != NULL) {
        puts("[FAILED]");
        return 1;
    }
    ztimer_now_t diff = ztimer_now(ZTIMER_USEC) - before;
    printf("event_wait time out after %"PRIu32"us\n", diff);

    puts("waiting for event with 10ms timeout @ ZTIMER_MSEC...");
    before = ztimer_now(ZTIMER_USEC);
    tmp_evt = event_wait_timeout_ztimer(&tmp_eq, ZTIMER_MSEC, STATIC_TIMEOUT_MS);
    if (tmp_evt != NULL) {
        puts("[FAILED]");
        return 1;
    }
    diff = ztimer_now(ZTIMER_USEC) - before;
    printf("event_wait time out after %"PRIu32"us\n", diff);

    thread_create(_stack, sizeof(_stack), PRIO, 0, _cnt_thread, NULL, "cnt");
    /* first, wait 155ms -> should lead to 3 timeout wakeups */
    ztimer_sleep(ZTIMER_MSEC, 155U);
    /* post event 3 times -> should lead to 3 event wakeups */
    for (unsigned i = 0; i < 3; i++) {
        event_post(&_evtq, &_evt);
        ztimer_sleep(ZTIMER_MSEC, 5U);
    }
    /* wait for 35ms and post another event -> +1 event wakeup */
    ztimer_sleep(ZTIMER_MSEC, 35U);
    event_post(&_evtq, &_evt);
    /* finally, wait 60ms and collect results -> +1 timeout wakeup */
    ztimer_sleep(ZTIMER_MSEC, 60U);

    unsigned events = atomic_load(&_wakeup_evt);
    unsigned timeouts = atomic_load(&_wakeup_timeout);

    /* rate results */
    printf("finished: %u/4 events and %u/4 timeouts recorded\n",
           events, timeouts);
    if ((events == 4) && (timeouts == 4)) {
        puts("[SUCCESS]");
    }
    else {
        puts("[FAILED]");
    }

    return 0;
}
