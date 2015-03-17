/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_thread
 * @{
 *
 * @file        thread.c
 * @brief       Threading implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "thread.h"
#include "kernel.h"
#include "irq.h"

#include "debug.h"
#include "kernel_internal.h"
#include "bitarithm.h"
#include "hwtimer.h"
#include "sched.h"

volatile tcb_t *thread_get(kernel_pid_t pid)
{
    if (pid_is_valid(pid)) {
        return sched_threads[pid];
    }
    return NULL;
}

int thread_getstatus(kernel_pid_t pid)
{
    volatile tcb_t *t = thread_get(pid);
    return t ? t->status : STATUS_NOT_FOUND;
}

#ifdef DEVELHELP
const char *thread_getname(kernel_pid_t pid)
{
    volatile tcb_t *t = thread_get(pid);
    return t ? t->name : NULL;
}
#endif

void thread_sleep(void)
{
    if (inISR()) {
        return;
    }

    unsigned state = disableIRQ();
    sched_set_status((tcb_t *)sched_active_thread, STATUS_SLEEPING);
    restoreIRQ(state);
    thread_yield_higher();
}

int thread_wakeup(kernel_pid_t pid)
{
    DEBUG("thread_wakeup: Trying to wakeup PID %" PRIkernel_pid "...\n", pid);

    unsigned old_state = disableIRQ();

    tcb_t *other_thread = (tcb_t *) sched_threads[pid];
    if (other_thread && other_thread->status == STATUS_SLEEPING) {
        DEBUG("thread_wakeup: Thread is sleeping.\n");

        sched_set_status(other_thread, STATUS_RUNNING);

        restoreIRQ(old_state);
        sched_switch(other_thread->priority);

        return 1;
    }
    else {
        DEBUG("thread_wakeup: Thread is not sleeping!\n");

        restoreIRQ(old_state);
        return STATUS_NOT_FOUND;
    }
}

void thread_yield(void)
{
    unsigned old_state = disableIRQ();
    tcb_t *me = (tcb_t *)sched_active_thread;
    if (me->status >= STATUS_ON_RUNQUEUE) {
        clist_advance(&sched_runqueues[me->priority]);
    }
    restoreIRQ(old_state);

    thread_yield_higher();
}

#ifdef DEVELHELP
uintptr_t thread_measure_stack_free(char *stack)
{
    uintptr_t *stackp = (uintptr_t *)stack;

    /* assume that the comparison fails before or after end of stack */
    /* assume that the stack grows "downwards" */
    while (*stackp == (uintptr_t) stackp) {
        stackp++;
    }

    uintptr_t space_free = (uintptr_t) stackp - (uintptr_t) stack;
    return space_free;
}
#endif

kernel_pid_t thread_create(char *stack, int stacksize, char priority, int flags, thread_task_func_t function, void *arg, const char *name)
{
    if (priority >= SCHED_PRIO_LEVELS) {
        return -EINVAL;
    }

#ifdef DEVELHELP
    int total_stacksize = stacksize;
#else
    (void) name;
#endif

    /* align the stack on a 16/32bit boundary */
    uintptr_t misalignment = (uintptr_t) stack % ALIGN_OF(void *);
    if (misalignment) {
        misalignment = ALIGN_OF(void *) - misalignment;
        stack += misalignment;
        stacksize -= misalignment;
    }

    /* make room for the thread control block */
    stacksize -= sizeof(tcb_t);

    /* round down the stacksize to a multiple of tcb_t alignments (usually 16/32bit) */
    stacksize -= stacksize % ALIGN_OF(tcb_t);

    /* allocate our thread control block at the top of our stackspace */
    tcb_t *cb = (tcb_t *) (stack + stacksize);

#ifdef DEVELHELP
    if (flags & CREATE_STACKTEST) {
        /* assign each int of the stack the value of it's address */
        uintptr_t *stackmax = (uintptr_t *) (stack + stacksize);
        uintptr_t *stackp = (uintptr_t *) stack;

        while (stackp < stackmax) {
            *stackp = (uintptr_t) stackp;
            stackp++;
        }
    }
    else {
        /* create stack guard */
        *stack = (uintptr_t) stack;
    }
#endif

    unsigned state = disableIRQ();

    kernel_pid_t pid = KERNEL_PID_UNDEF;
    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; ++i) {
        if (sched_threads[i] == NULL) {
            pid = i;
            break;
        }
    }
    if (pid == KERNEL_PID_UNDEF) {
        DEBUG("thread_create(): too many threads!\n");

        restoreIRQ(state);

        return -EOVERFLOW;
    }

    sched_threads[pid] = cb;

    cb->pid = pid;
    cb->sp = thread_stack_init(function, arg, stack, stacksize);

#ifdef DEVELHELP
    cb->stack_start = stack;
    cb->stack_size = total_stacksize;
    cb->name = name;
#endif

    cb->priority = priority;
    cb->status = 0;

    cb->rq_entry.next = NULL;
    cb->rq_entry.prev = NULL;

    cb->wait_data = NULL;

    cb->msg_waiters.first = NULL;

    cib_init(&(cb->msg_queue), 0);
    cb->msg_array = NULL;

    sched_num_threads++;

    DEBUG("Created thread %s. PID: %" PRIkernel_pid ". Priority: %u.\n", name, cb->pid, priority);

    if (flags & CREATE_SLEEPING) {
        sched_set_status(cb, STATUS_SLEEPING);
    }
    else {
        sched_set_status(cb, STATUS_PENDING);

        if (!(flags & CREATE_WOUT_YIELD)) {
            restoreIRQ(state);
            sched_switch(priority);
            return pid;
        }
    }

    restoreIRQ(state);

    return pid;
}
