/**
 * thread functions
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup kernel
 * @{
 * @file
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "thread.h"
#include "kernel.h"

//#define ENABLE_DEBUG
#include "debug.h"
#include "kernel_intern.h"
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

unsigned int thread_getstatus(int pid)
{
    if(sched_threads[pid] == NULL) {
        return STATUS_NOT_FOUND;
    }

    return sched_threads[pid]->status;
}

void thread_sleep()
{
    if(inISR()) {
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
    int isr = inISR();

    if(! isr) {
        DEBUG("thread_wakeup: Not in interrupt.\n");
        dINT();
    }

    int result = sched_threads[pid]->status;

    if(result == STATUS_SLEEPING) {
        DEBUG("thread_wakeup: Thread is sleeping.\n");
        sched_set_status((tcb_t *)sched_threads[pid], STATUS_RUNNING);

        if(!isr) {
            eINT();
            thread_yield();
        }
        else {
            sched_context_switch_request = 1;
        }

        return 1;
    }
    else {
        DEBUG("thread_wakeup: Thread is not sleeping!\n");

        if(!isr) {
            eINT();
        }

        return STATUS_NOT_FOUND;
    }
}

int thread_measure_stack_usage(char *stack)
{
    unsigned int *stackp = (unsigned int *)stack;

    /* assumption that the comparison fails before or after end of stack */
    while(*stackp == (unsigned int)stackp) {
        stackp++;
    }

    int space = (unsigned int)stackp - (unsigned int)stack;
    return space;
}

int thread_create(char *stack, int stacksize, char priority, int flags, void (*function)(void), const char *name)
{
    /* allocate our thread control block at the top of our stackspace */
    int total_stacksize = stacksize;
    stacksize -= sizeof(tcb_t);

    /* align tcb address on 32bit boundary */
    unsigned int tcb_address = (unsigned int) stack + stacksize;

    if(tcb_address & 1) {
        tcb_address--;
        stacksize--;
    }

    if(tcb_address & 2) {
        tcb_address -= 2;
        stacksize -= 2;
    }

    tcb_t *cb = (tcb_t *) tcb_address;

    if(priority >= SCHED_PRIO_LEVELS) {
        return -EINVAL;
    }

    if(flags & CREATE_STACKTEST) {
        /* assign each int of the stack the value of it's address */
        unsigned int *stackmax = (unsigned int *)((char *)stack + stacksize);
        unsigned int *stackp = (unsigned int *)stack;

        while(stackp < stackmax) {
            *stackp = (unsigned int)stackp;
            stackp++;
        }
    }
    else {
        /* create stack guard */
        *stack = (unsigned int)stack;
    }

    if(! inISR()) {
        dINT();
    }

    int pid = 0;

    while(pid < MAXTHREADS) {
        if(sched_threads[pid] == NULL) {
            sched_threads[pid] = cb;
            cb->pid = pid;
            break;
        }

        pid++;
    }

    if(pid == MAXTHREADS) {
        DEBUG("thread_create(): too many threads!\n");

        if(! inISR()) {
            eINT();
        }

        return -EOVERFLOW;
    }

    cb->sp = thread_stack_init(function, stack, stacksize);
    cb->stack_start = stack;
    cb->stack_size = total_stacksize;

    cb->priority = priority;
    cb->status = 0;

    cb->rq_entry.data = (unsigned int) cb;
    cb->rq_entry.next = NULL;
    cb->rq_entry.prev = NULL;

    cb->name = name;

    cb->wait_data = NULL;

    cb->msg_waiters.data = 0;
    cb->msg_waiters.priority = 0;
    cb->msg_waiters.next = NULL;

    cib_init(&(cb->msg_queue), 0);
    cb->msg_array = NULL;

    num_tasks++;

    DEBUG("Created thread %s. PID: %u. Priority: %u.\n", name, cb->pid, priority);

    if(flags & CREATE_SLEEPING) {
        sched_set_status(cb, STATUS_SLEEPING);
    }
    else {
        sched_set_status(cb, STATUS_PENDING);

        if(!(flags & CREATE_WOUT_YIELD)) {
            if(! inISR()) {
                eINT();
                thread_yield();
            }
            else {
                sched_context_switch_request = 1;
            }
        }
    }

    if(!inISR() && active_thread != NULL) {
        eINT();
    }

    return pid;
}

