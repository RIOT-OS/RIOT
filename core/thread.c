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

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "kernel_internal.h"
#include "bitarithm.h"
#include "hwtimer.h"
#include "sched.h"

volatile tcb_t *thread_get(kernel_pid_t pid)
{
    if ((pid != KERNEL_PID_UNDEF) && (KERNEL_PID_FIRST <= pid) && (pid <= KERNEL_PID_LAST)) {
        return sched_threads[pid];
    }
    return NULL;
}

int thread_getstatus(kernel_pid_t pid)
{
    volatile tcb_t *t = thread_get(pid);
    return t ? t->status : STATUS_NOT_FOUND;
}

const char *thread_getname(kernel_pid_t pid)
{
    volatile tcb_t *t = thread_get(pid);
    return t ? t->name : NULL;
}

void thread_sleep(void)
{
    if (inISR()) {
        return;
    }

    dINT();
    sched_set_status((tcb_t *)sched_active_thread, STATUS_SLEEPING);
    eINT();
    thread_yield();
}

int thread_wakeup(kernel_pid_t pid)
{
    DEBUG("thread_wakeup: Trying to wakeup PID %" PRIkernel_pid "...\n", pid);

    int old_state = disableIRQ();

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

#ifdef DEVELHELP
int thread_measure_stack_free(char *stack)
{
    unsigned int *stackp = (unsigned int *)stack;

    /* assume that the comparison fails before or after end of stack */
    /* assume that the stack grows "downwards" */
    while (*stackp == (unsigned int)stackp) {
        stackp++;
    }

    int space_free = (unsigned int)stackp - (unsigned int)stack;
    return space_free;
}
#endif

kernel_pid_t thread_create(char *stack, int stacksize, char priority, int flags, void *(*function)(void *arg), void *arg, const char *name)
{
    /* allocate our thread control block at the top of our stackspace */
#ifdef DEVELHELP
    int total_stacksize = stacksize;
#endif
    stacksize -= sizeof(tcb_t);

    /* align tcb address on 32bit boundary */
    unsigned int tcb_address = (unsigned int) stack + stacksize;

    if (tcb_address & 1) {
        tcb_address--;
        stacksize--;
    }

    if (tcb_address & 2) {
        tcb_address -= 2;
        stacksize -= 2;
    }

    tcb_t *cb = (tcb_t *) tcb_address;

    if (priority >= SCHED_PRIO_LEVELS) {
        return -EINVAL;
    }

#ifdef DEVELHELP
    if (flags & CREATE_STACKTEST) {
        /* assign each int of the stack the value of it's address */
        unsigned int *stackmax = (unsigned int *)((char *)stack + stacksize);
        unsigned int *stackp = (unsigned int *)stack;

        while (stackp < stackmax) {
            *stackp = (unsigned int)stackp;
            stackp++;
        }
    }
    else {
        /* create stack guard */
        *stack = (unsigned int)stack;
    }
#endif

    if (!inISR()) {
        dINT();
    }

    kernel_pid_t pid = KERNEL_PID_UNDEF;
    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; ++i) {
        if (sched_threads[i] == NULL) {
            pid = i;
            break;
        }
    }
    if (pid == KERNEL_PID_UNDEF) {
        DEBUG("thread_create(): too many threads!\n");

        if (!inISR()) {
            eINT();
        }

        return -EOVERFLOW;
    }

    sched_threads[pid] = cb;

    cb->pid = pid;
    cb->sp = thread_stack_init(function, arg, stack, stacksize);
    cb->stack_start = stack;

#ifdef DEVELHELP
    cb->stack_size = total_stacksize;
#endif

    cb->priority = priority;
    cb->status = 0;

    cb->rq_entry.next = NULL;
    cb->rq_entry.prev = NULL;

    cb->name = name;

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
            if (!inISR()) {
                eINT();
                thread_yield();
            }
            else {
                sched_context_switch_request = 1;
            }
        }
    }

    if (!inISR() && sched_active_thread != NULL) {
        eINT();
    }

    return pid;
}
