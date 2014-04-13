/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     core_thread
 * @{
 *
 * @file        thread.c
 * @brief       Threading implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "kernel.h"
#include "irq.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "kernel_internal.h"
#include "bitarithm.h"
#include "hwtimer.h"
#include "sched.h"

inline int thread_getpid()
{
    return active_thread->pid;
}

int thread_getlastpid()
{
    extern int last_pid;
    return last_pid;
}

int thread_getstatus(int pid)
{
    if (sched_threads[pid] == NULL) {
        return STATUS_NOT_FOUND;
    }

    return sched_threads[pid]->status;
}

const char *thread_getname(int pid)
{
    if (sched_threads[pid] == NULL) {
        return NULL;
    }

    return sched_threads[pid]->name;
}

void thread_sleep()
{
    if (inISR()) {
        return;
    }

    dINT();
    sched_set_status((tcb_t *)active_thread, STATUS_SLEEPING);
    eINT();
    thread_yield();
}

int thread_wakeup(int pid)
{
    DEBUG("thread_wakeup: Trying to wakeup PID %i...\n", pid);

    int old_state = disableIRQ();

    tcb_t *other_thread = (tcb_t *) sched_threads[pid];
    if (other_thread && other_thread->status == STATUS_SLEEPING) {
        DEBUG("thread_wakeup: Thread is sleeping.\n");

        sched_set_status(other_thread, STATUS_RUNNING);

        restoreIRQ(old_state);
        sched_switch(active_thread->priority, other_thread->priority);

        return 1;
    }
    else {
        DEBUG("thread_wakeup: Thread is not sleeping!\n");

        restoreIRQ(old_state);
        return STATUS_NOT_FOUND;
    }
}

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

int thread_create(char *stack, int stacksize, char priority, int flags, void (*function)(void), const char *name)
{
    unsigned tcb_size = (flags & CREATE_NOMSG) == 0 ? sizeof (msg_tcb_t) : sizeof (tcb_t);

    /* allocate our thread control block at the top of our stackspace */
    int total_stacksize = stacksize;
    stacksize -= tcb_size;

    /* align tcb address on 32bit boundary */
    unsigned int tcb_address = (unsigned int) stack + stacksize;
    while (tcb_address & 3) {
        tcb_address--;
        stacksize--;
    }

    tcb_t *cb = (tcb_t *) tcb_address;

    if (priority >= SCHED_PRIO_LEVELS) {
        return -EINVAL;
    }

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

    unsigned old_state = disableIRQ();

    int pid = 0;

    while (pid < MAXTHREADS) {
        if (sched_threads[pid] == NULL) {
            break;
        }
        pid++;
    }

    if (pid == MAXTHREADS) {
        DEBUG("thread_create(): too many threads!\n");

        restoreIRQ(old_state);
        return -EOVERFLOW;
    }

    memset(cb, 0, tcb_size);
    cb->sp = thread_stack_init(function, stack, stacksize);
    cb->stack_start = stack;
    cb->stack_size = total_stacksize;
    cb->pid = pid;
    cb->priority = priority;
    cb->flags = flags;
    cb->rq_entry.data = (unsigned int) cb;
    cb->name = name;

    sched_threads[pid] = cb;

    num_tasks++;

    DEBUG("Created thread %s. PID: %u. Priority: %u.\n", name, cb->pid, priority);

    if (flags & CREATE_SLEEPING) {
        sched_set_status(cb, STATUS_SLEEPING);
        restoreIRQ(old_state);
    }
    else {
        sched_set_status(cb, STATUS_PENDING);
        restoreIRQ(old_state);

        if (!(flags & CREATE_WOUT_YIELD)) {
            sched_switch(active_thread ? active_thread->priority : SCHED_PRIO_LEVELS, priority);
        }
    }

    return pid;
}
