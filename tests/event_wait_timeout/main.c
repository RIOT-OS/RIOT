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

#include <stdio.h>

#include "event.h"
#include "thread.h"
#include "xtimer.h"
#include "thread_flags.h"

#define TIMEOUT         (50U * US_PER_MS)       /* 50ms */
#define PRIO            (THREAD_PRIORITY_MAIN - 5)
#define STACKSIZE       (THREAD_STACKSIZE_DEFAULT)

#define FLAG_SYNC       (0x0040)

static void _on_evt(event_t *evt);

static event_queue_t _evtq;
static event_t _evt = { .handler = _on_evt };

static char _stack[STACKSIZE];
static thread_t *_thread_main;

static unsigned _wakeup_evt = 0;
static unsigned _wakeup_timeout = 0;

static void _on_evt(event_t *evt)
{
    (void)evt;
    ++_wakeup_evt;
}

static void *_cnt_thread(void *arg)
{
    (void)arg;
    event_queue_init(&_evtq);

    while (1) {
        event_t *evt = event_wait_timeout(&_evtq, TIMEOUT);
        if (evt) {
            evt->handler(evt);
        }
        else {
            ++_wakeup_timeout;
        }
    }

    return NULL;
}

int main(void)
{
    /* setup */
    _thread_main = (thread_t *)thread_get(thread_getpid());

    puts("[START] event_wait_timeout test application.\n");

    /* test 'instant' return */
    event_queue_t tmp_eq = EVENT_QUEUE_INIT;
    event_t *tmp_evt = event_wait_timeout(&tmp_eq, 0);
    if (tmp_evt != NULL) {
        puts("[FAILED]");
        return 1;
    }

    thread_create(_stack, sizeof(_stack), PRIO, 0, _cnt_thread, NULL, "cnt");
    /* first, wait 155ms -> should lead to 3 timeout wakeups */
    xtimer_usleep(155U * US_PER_MS);
    /* post event 3 times -> should lead to 3 event wakeups */
    for (unsigned i = 0; i < 3; i++) {
        event_post(&_evtq, &_evt);
        xtimer_usleep(5U * US_PER_MS);
    }
    /* wait for 35ms and post another event -> +1 event wakeup */
    xtimer_usleep(35U * US_PER_MS);
    event_post(&_evtq, &_evt);
    /* finally, wait 60ms and collect results -> +1 timeout wakeup */
    xtimer_usleep(60U * US_PER_MS);

    unsigned events = _wakeup_evt;
    unsigned timeouts = _wakeup_timeout;

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
