/*
 * Copyright (C) 2020 TUBA Freiberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Round Robin Scheduler implementation
 *
 * @author      Karl Fessel <karl.fessel@ovgu.de>
 *
 * @}
 */

#include "thread.h"
#include "xtimer.h"
#include "clist.h"
#include "sched_rr.h"

static xtimer_t rr_timer;


/*
 * will run in the current threads context (xtimer/ISR)
 * and force it to give way to implement a simple round robin
 * it may be started in any thread
 */
void schedule_rr(void * d)
{
    (void)d;
    /*
     * try to reorder the current priority
     * (put the current thread at the end of the run queue of its priority)
     * and setup the schedueler to schedule when returning from the IRQ
     */
    thread_t *active_thread = thread_get_active();
    if (active_thread != NULL) {
        if (!(SCHED_RR_MASK & 1 << active_thread->priority)) {
            clist_lpoprpush(&sched_runqueues[active_thread->priority]);
            thread_yield_higher();
        }
    }

    /* setup a timer to pull itself up agian */
    /* if time is shorter than XTIMER_BACKOFF we stack the stack */
    xtimer_set(&rr_timer, SCHED_RR_TIMER);
}

void stop_schedule_rr(void)
{
    xtimer_remove(&rr_timer);
}

void start_schedule_rr(void)
{
    rr_timer.callback = schedule_rr;
    xtimer_set(&rr_timer, SCHED_RR_TIMER);
}
