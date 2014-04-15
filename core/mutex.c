/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     core_sync
 * @{
 *
 * @file        mutex.c
 * @brief       Kernel mutex implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "mutex.h"
#include "tcb.h"
#include "atomic.h"
#include "kernel.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

int mutex_init(struct mutex_t *mutex)
{
    mutex->val = 0;

    mutex->queue.thread = NULL;
    mutex->queue.next = NULL;

    return 1;
}

int mutex_trylock(struct mutex_t *mutex)
{
    DEBUG("%p: trylocking to get mutex. val: %u\n", active_thread->stack_start, mutex->val);
    return (atomic_set_return(&mutex->val, 1) == 0);
}

int mutex_lock(struct mutex_t *mutex)
{
    DEBUG("%p: trying to get mutex. val: %u\n", active_thread->stack_start, mutex->val);

    if (atomic_set_return(&mutex->val, 1) != 0) {
        /* mutex was locked. */
        mutex_wait(mutex);
    }

    return 1;
}

void mutex_wait(struct mutex_t *mutex)
{
    int irqstate = disableIRQ();
    DEBUG("%p: Mutex in use. %u\n", active_thread->stack_start, mutex->val);

    if (mutex->val == 0) {
        /* somebody released the mutex. return. */
        mutex->val = 1;
        DEBUG("%p: mutex_wait early out. %u\n", active_thread->stack_start, mutex->val);
        restoreIRQ(irqstate);
        return;
    }

    sched_set_status((tcb_t*) active_thread, STATUS_MUTEX_BLOCKED);

    queue_node_t n;
    n.thread = active_thread;
    n.next = NULL;

    DEBUG("%p: Adding node to mutex queue: prio: %" PRIu16 "\n", active_thread->stack_start, active_thread->priority);

    queue_priority_add(&(mutex->queue), &n);

    restoreIRQ(irqstate);

    thread_yield();

    /* we were woken up by scheduler. waker removed us from queue. we have the mutex now. */
}

void mutex_unlock(struct mutex_t *mutex)
{
    DEBUG("%p: unlocking mutex. val: %u pid: %u\n", active_thread->stack_start, mutex->val, thread_pid);
    int irqstate = disableIRQ();

    if (mutex->val != 0) {
        if (mutex->queue.next) {
            queue_node_t *next = queue_remove_head(&(mutex->queue));
            tcb_t *process = next->thread;
            DEBUG("%p: waking up waiter.\n", process->stack_start);
            sched_set_status(process, STATUS_PENDING);

            sched_switch(active_thread->priority, process->priority);
        }
        else {
            mutex->val = 0;
        }
    }

    restoreIRQ(irqstate);
}

void mutex_unlock_and_sleep(struct mutex_t *mutex)
{
    DEBUG("%p: unlocking mutex. val: %u pid: %u, and taking a nap\n", active_thread->stack_start, mutex->val, thread_pid);
    int irqstate = disableIRQ();

    if (mutex->val != 0) {
        if (mutex->queue.next) {
            queue_node_t *next = queue_remove_head(&(mutex->queue));
            tcb_t *process = next->thread;
            DEBUG("%p: waking up waiter.\n", process->stack_start);
            sched_set_status(process, STATUS_PENDING);
        }
        else {
            mutex->val = 0;
        }
    }
    DEBUG("%p: going to sleep.\n", active_thread->stack_start);
    sched_set_status((tcb_t*) active_thread, STATUS_SLEEPING);
    restoreIRQ(irqstate);
    thread_yield();
}
