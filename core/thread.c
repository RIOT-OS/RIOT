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
 *
 * @}
 */

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

void thread_sleep(void)
{
    if (inISR()) {
        return;
    }

    dINT();
    sched_set_status((tcb_t *) active_thread, STATUS_SLEEPING);
    eINT();
    thread_yield();
}

int thread_wakeup(tcb_t *other_thread)
{
    DEBUG("thread_wakeup: Trying to wakeup PID %i...\n", pid);

    int old_state = disableIRQ();

    if (other_thread->status == STATUS_SLEEPING) {
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

tcb_t *thread_create(char *stack, int stacksize, unsigned priority, int flags, void (*function)(void))
{
    /* allocate our thread control block at the top of our stackspace */
    int total_stacksize = stacksize;
    stacksize -= sizeof(tcb_t);

    /* align tcb address on 32bit boundary */
    unsigned int tcb_address = (unsigned int) stack + stacksize;

    while (tcb_address & 3) {
        tcb_address--;
        stacksize--;
    }

    tcb_t *cb = (tcb_t *) tcb_address;

    if (priority >= SCHED_PRIO_LEVELS) {
        return NULL;
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

    if (!inISR()) {
        dINT();
    }

    memset(cb, 0, sizeof (*cb));
    cb->sp = thread_stack_init(function, stack, stacksize);
    cb->stack_start = stack;
    cb->stack_size = total_stacksize;
    cb->priority = priority;

    num_tasks++;

    DEBUG("Created thread %s. PID: %u. Priority: %u.\n", name, cb->pid, priority);

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

    if (!inISR() && active_thread != NULL) {
        eINT();
    }

    return cb;
}
