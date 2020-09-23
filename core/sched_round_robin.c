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
#include "sched_round_robin.h"
#define ENABLE_DEBUG 0
#include "debug.h"

static void sched_round_robin_cb(void * d);

static xtimer_t rr_timer = {.callback = sched_round_robin_cb};

static uint8_t current_rr_priority = 0xff;

static void sched_round_robin_cb(void * d)
{
    (void)d;
    /*
     * reorder current Round Robin priority
     * (put the current thread at the end of the run queue of its priority)
     * and setup the schedueler to schedule when returning from the IRQ
     */
    if (current_rr_priority != 0xff) {
        DEBUG_PUTS("Round_Robin_cb");
        clist_lpoprpush(&sched_runqueues[current_rr_priority]);
        thread_t * active_thread = thread_get_active();
        if(active_thread){
            uint8_t active_priority = active_thread->priority;
            if(active_priority == current_rr_priority){
                current_rr_priority = 0xff;
                thread_yield_higher();
            } else {
                current_rr_priority = 0xff;
                sched_round_robin_check_set(active_priority);
            }
        }else{
           current_rr_priority = 0xff;
        }
    }
}

static inline void sched_round_robin_remove(void)
{
    current_rr_priority = 0xff;
    xtimer_remove(&rr_timer);
}

static inline void sched_round_robin_set(uint8_t prio)
{
    current_rr_priority = prio;
    xtimer_set(&rr_timer, SCHED_RR_TIMER);
}

void sched_round_robin_check_set(uint8_t prio)
{
    /*return if Round Robin is in use, prio is masked or list lenght is 0 or 1*/
    if ( current_rr_priority != 0xff ||
         SCHED_RR_MASK & 1 << prio ||
         !sched_runqueues[prio].next ||
         sched_runqueues[prio].next->next ==
         sched_runqueues[prio].next->next->next) {
            DEBUG_PUTS("RR_not_set");
            return;
    }
    /*setup timer an Round Robin*/
    DEBUG_PUTS("RR_set_timer");
    sched_round_robin_set(prio);
}

void sched_round_robin_check_remove_set(uint8_t prio)
{
    /*may be called when a runqueue is emptied*/
    if(current_rr_priority == prio){
        sched_round_robin_remove();
        thread_t * active_thread = thread_get_active();
        if(active_thread){
            uint8_t active_priority = active_thread->priority;
            sched_round_robin_check_set(active_priority);
        }
    }
}
