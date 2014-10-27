/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_sched
 * @{
 *
 * @file        sched.c
 * @brief       Scheduler implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>

#include "sched.h"
#include "kernel.h"
#include "kernel_internal.h"
#include "clist.h"
#include "bitarithm.h"
#include "irq.h"
#include "thread.h"
#include "irq.h"

#if SCHEDSTATISTICS
#include "hwtimer.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

volatile int sched_num_threads = 0;

volatile unsigned int sched_context_switch_request;

volatile tcb_t *sched_threads[KERNEL_PID_LAST + 1];
volatile tcb_t *sched_active_thread;

volatile kernel_pid_t sched_active_pid = KERNEL_PID_UNDEF;

clist_node_t *sched_runqueues[SCHED_PRIO_LEVELS];
static uint32_t runqueue_bitcache = 0;

#if SCHEDSTATISTICS
static void (*sched_cb) (uint32_t timestamp, uint32_t value) = NULL;
schedstat sched_pidlist[KERNEL_PID_LAST + 1];
#endif

void sched_run(void)
{
    sched_context_switch_request = 0;

#ifdef SCHEDSTATISTICS
    unsigned long time = hwtimer_now();
#endif

    tcb_t *my_active_thread = (tcb_t *)sched_active_thread;

    if (my_active_thread) {
        if (my_active_thread->status == STATUS_RUNNING) {
            my_active_thread->status = STATUS_PENDING;
        }

#ifdef SCHED_TEST_STACK
        if (*((unsigned int *)my_active_thread->stack_start) != (unsigned int) my_active_thread->stack_start) {
            printf("scheduler(): stack overflow detected, task=%s pid=%" PRIkernel_pid "\n", my_active_thread->name, my_active_thread->pid);
        }
#endif

#ifdef SCHEDSTATISTICS
        if (sched_pidlist[my_active_thread->pid].laststart) {
            sched_pidlist[my_active_thread->pid].runtime_ticks += time - sched_pidlist[my_active_thread->pid].laststart;
        }
#endif
    }

    DEBUG("\nscheduler: previous task: %s\n", (my_active_thread == NULL) ? "none" : my_active_thread->name);

    /* The bitmask in runqueue_bitcache is never empty,
     * since the threading should not be started before at least the idle thread was started.
     */
    int nextrq = bitarithm_lsb(runqueue_bitcache);
    my_active_thread = clist_get_container(sched_runqueues[nextrq], tcb_t, rq_entry);
    DEBUG("scheduler: first in queue: %s\n", my_active_thread->name);

    kernel_pid_t my_next_pid = my_active_thread->pid;

#if SCHEDSTATISTICS
    sched_pidlist[my_next_pid].laststart = time;
    sched_pidlist[my_next_pid].schedules++;
    if ((sched_cb) && (my_next_pid != sched_active_pid)) {
        sched_cb(time, my_next_pid);
    }
#endif

    sched_active_pid = my_next_pid;

    DEBUG("scheduler: next task: %s\n", my_active_thread->name);

    if (my_active_thread != sched_active_thread) {
        if (sched_active_thread != NULL) {
            if (sched_active_thread->status == STATUS_RUNNING) {
                sched_active_thread->status = STATUS_PENDING;
            }
        }

        sched_set_status((tcb_t *)my_active_thread, STATUS_RUNNING);
    }

    sched_active_thread = (volatile tcb_t *) my_active_thread;

    DEBUG("scheduler: done.\n");
}

#if SCHEDSTATISTICS
void sched_register_cb(void (*callback)(uint32_t, uint32_t))
{
    sched_cb = callback;
}
#endif

void sched_set_status(tcb_t *process, unsigned int status)
{
    if (status >= STATUS_ON_RUNQUEUE) {
        if (!(process->status >= STATUS_ON_RUNQUEUE)) {
            DEBUG("adding process %s to runqueue %u.\n", process->name, process->priority);
            clist_add(&sched_runqueues[process->priority], &(process->rq_entry));
            runqueue_bitcache |= 1 << process->priority;
        }
    }
    else {
        if (process->status >= STATUS_ON_RUNQUEUE) {
            DEBUG("removing process %s from runqueue %u.\n", process->name, process->priority);
            clist_remove(&sched_runqueues[process->priority], &(process->rq_entry));

            if (!sched_runqueues[process->priority]) {
                runqueue_bitcache &= ~(1 << process->priority);
            }
        }
    }

    process->status = status;
}

void sched_switch(uint16_t other_prio)
{
    int in_isr = inISR();
    uint16_t current_prio = sched_active_thread->priority;

    DEBUG("%s: %" PRIu16 " %" PRIu16 " %i\n", sched_active_thread->name, current_prio, other_prio, in_isr);

    if (current_prio > other_prio) {
        if (in_isr) {
            sched_context_switch_request = 1;
        }
        else {
            thread_yield_higher();
        }
    }
}

NORETURN void sched_task_exit(void)
{
    DEBUG("sched_task_exit(): ending task %s...\n", sched_active_thread->name);

    dINT();
    sched_threads[sched_active_pid] = NULL;
    sched_num_threads--;

    sched_set_status((tcb_t *)sched_active_thread, STATUS_STOPPED);

    sched_active_thread = NULL;
    cpu_switch_context_exit();
}
