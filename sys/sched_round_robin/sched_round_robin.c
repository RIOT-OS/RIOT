/*
 * Copyright (C) 2021 TUBA Freiberg
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

#include "sched.h"
#include "thread.h"
#include "ztimer.h"
#include "sched_round_robin.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _sched_round_robin_cb(void *d);

static ztimer_t _rr_timer = { .callback = _sched_round_robin_cb };

/*
 * Assuming simple reads from and writes to a byte to be atomic on every board
 * Value 0 is assumed to show this system is uninitialised.
 * The timer will not be started for prio = 0;
 */
static uint8_t _current_rr_priority = 0;

void sched_runq_callback(uint8_t prio);

void _sched_round_robin_cb(void *d)
{
    (void)d;
    /*
     * reorder current Round Robin priority
     * (put the current thread at the end of the run queue of its priority)
     * and setup the scheduler to schedule when returning from the IRQ
     */
    uint8_t prio = _current_rr_priority;
    if (prio != 0xff) {
        DEBUG_PUTS("Round_Robin");
        sched_runq_advance(prio);
        _current_rr_priority = 0xff;
    }
    thread_t *active_thread = thread_get_active();
    if (active_thread) {
        uint8_t active_priority = active_thread->priority;
        if (active_priority == prio) {
            thread_yield_higher();
            /* thread change will call the runqueue_change_cb */
        }
        else {
            sched_runq_callback(active_priority);
        }
    }
}

static inline void _sched_round_robin_remove(void)
{
    _current_rr_priority = 0xff;
    ztimer_remove(SCHED_RR_TIMERBASE, &_rr_timer);
}

static inline void _sched_round_robin_set(uint8_t prio)
{
    if (prio == 0) {
        return;
    }
    _current_rr_priority = prio;
    ztimer_set(SCHED_RR_TIMERBASE, &_rr_timer, SCHED_RR_TIMEOUT);
}

void sched_runq_callback(uint8_t prio)
{
    if (SCHED_RR_MASK & (1 << prio) || prio == 0) {
        return;
    }

    if (_current_rr_priority == prio) {
        if (sched_runq_is_empty(prio)) {
            _sched_round_robin_remove();
            thread_t *active_thread = thread_get_active();
            if (active_thread) {
                prio = active_thread->priority;
            }
            else {
                return;
            }
        }
    }

    if (_current_rr_priority == 0xff  &&
        !(SCHED_RR_MASK & (1 << prio)) &&
        sched_runq_more_than_one(prio)) {
        _sched_round_robin_set(prio);
    }
}

void sched_round_robin_init(void)
{
    /* init _current_rr_priority */
    _current_rr_priority = 0xff;
    /* check if applicable to active priority */
    thread_t *active_thread = thread_get_active();
    if (active_thread) {
        sched_runq_callback(active_thread->priority);
    }
}
