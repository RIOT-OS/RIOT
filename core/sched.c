/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     core_shed
 * @{
 *
 * @file        shed.c
 * @brief       Scheduler implementation
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 *
 * TODO: setup dependency from SCHEDSTATISTICS to MODULE_HWTIMER
 */

#include <stdint.h>
#include "sched.h"
#include "kernel.h"
#include "kernel_internal.h"
#include "bitarithm.h"
#include "thread.h"
#include "irq.h"

#if SCHEDSTATISTICS
#include "hwtimer.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

volatile int num_tasks = 0;

volatile unsigned int sched_context_switch_request;

tcb_t *active_thread;

clist_node_t *runqueues[SCHED_PRIO_LEVELS + 1];
static uint32_t runqueue_bitcache = 0;

void sched_run(void)
{
    sched_context_switch_request = 0;

    tcb_t *my_active_thread = (tcb_t *)active_thread;

    if (my_active_thread) {
        if (my_active_thread->status == STATUS_RUNNING) {
            my_active_thread->status = STATUS_PENDING;
        }

#ifdef SCHED_TEST_STACK
        if (*((unsigned int *)my_active_thread->stack_start) != (unsigned int) my_active_thread->stack_start) {
            printf("scheduler(): stack overflow detected, task=%s pid=%u\n", my_active_thread->name, my_active_thread->pid);
        }
#endif

    }

#ifdef SCHEDSTATISTICS
    unsigned long time = hwtimer_now();

    if (my_active_thread && my_active_thread->schedstats.laststart) {
        my_active_thread->schedstats.runtime_ticks += time - my_active_thread->schedstats.laststart;
    }
#endif

    DEBUG("\nscheduler: previous task: %s\n", (my_active_thread == NULL) ? "none" : my_active_thread->name);

    if (num_tasks == 0) {
        DEBUG("scheduler: no tasks left.\n");

        while (!num_tasks) {
            /* loop until a new task arrives */
            ;
        }

        DEBUG("scheduler: new task created.\n");
    }

    my_active_thread = NULL;

    while (!my_active_thread) {
        int nextrq = number_of_lowest_bit(runqueue_bitcache);
        my_active_thread = container_of(runqueues[nextrq], tcb_t, rq_entry);
        DEBUG("scheduler: first in queue: %s\n", my_active_thread->stack_start);
        clist_advance(&(runqueues[nextrq]));

#if SCHEDSTATISTICS
        my_active_thread->schedstats.laststart = time;
        ++my_active_thread->schedstats.schedules;
#endif
    }

    DEBUG("scheduler: next task: %s\n", my_active_thread->name);

    if (my_active_thread != active_thread) {
        if (active_thread != NULL) {  /* TODO: necessary? */
            if (active_thread->status ==  STATUS_RUNNING) {
                active_thread->status =  STATUS_PENDING ;
            }
        }

        sched_set_status(my_active_thread,  STATUS_RUNNING);
    }

    active_thread = my_active_thread;

    DEBUG("scheduler: done.\n");
}

void sched_set_status(tcb_t *process, unsigned int status)
{
    if (status >= STATUS_ON_RUNQUEUE) {
        if (!(process->status >= STATUS_ON_RUNQUEUE)) {
            DEBUG("adding process %s to runqueue %u.\n", process->name, process->priority);
            clist_add(&runqueues[process->priority], &(process->rq_entry));
            clist_remove(&runqueues[SCHED_PRIO_LEVELS], &(process->rq_entry));

            runqueue_bitcache |= 1 << process->priority;
        }
    }
    else if (process->status >= STATUS_ON_RUNQUEUE) {
        DEBUG("removing process %s from runqueue %u.\n", process->name, process->priority);
        clist_remove(&runqueues[process->priority], &(process->rq_entry));

        if (status != STATUS_STOPPED) {
            clist_add(&runqueues[SCHED_PRIO_LEVELS], &(process->rq_entry));
        }

        if (!runqueues[process->priority]) {
            runqueue_bitcache &= ~(1 << process->priority);
        }
    }

    process->status = status;
}

void sched_switch(uint16_t current_prio, uint16_t other_prio)
{
    int in_isr = inISR();

    DEBUG("%s: %i %i %i\n", active_thread->name, (int)current_prio, (int)other_prio, in_isr);

    if (current_prio >= other_prio) {
        if (in_isr) {
            sched_context_switch_request = 1;
        }
        else {
            thread_yield();
        }
    }
}

void sched_task_exit(void)
{
    DEBUG("sched_task_exit(): ending task %s...\n", active_thread->name);

    dINT();
    num_tasks--;

    sched_set_status(active_thread,  STATUS_STOPPED);

    active_thread = NULL;
    cpu_switch_context_exit();
}
