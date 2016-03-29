/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_sync
 * @{
 *
 * @file
 * @brief       Kernel mutex implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "mutex.h"
#include "thread.h"
#include "atomic.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"
#include "thread.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static void mutex_wait(struct mutex_t *mutex);

int mutex_trylock(struct mutex_t *mutex)
{
    DEBUG("%s: trylocking to get mutex. val: %u\n", sched_active_thread->name, ATOMIC_VALUE(mutex->val));
    return atomic_set_to_one(&mutex->val);
}

void mutex_lock(struct mutex_t *mutex)
{
    DEBUG("%s: trying to get mutex. val: %u\n", sched_active_thread->name, ATOMIC_VALUE(mutex->val));

    if (atomic_set_to_one(&mutex->val) == 0) {
        /* mutex was locked. */
        mutex_wait(mutex);
    }
}

static void mutex_wait(struct mutex_t *mutex)
{
    unsigned irqstate = irq_disable();
    DEBUG("%s: Mutex in use. %u\n", sched_active_thread->name, ATOMIC_VALUE(mutex->val));

    if (atomic_set_to_one(&mutex->val)) {
        /* somebody released the mutex. return. */
        DEBUG("%s: mutex_wait early out. %u\n", sched_active_thread->name, ATOMIC_VALUE(mutex->val));
        irq_restore(irqstate);
        return;
    }

    sched_set_status((thread_t*) sched_active_thread, STATUS_MUTEX_BLOCKED);

    priority_queue_node_t n;
    n.priority = (unsigned int) sched_active_thread->priority;
    n.data = (unsigned int) sched_active_thread;
    n.next = NULL;

    DEBUG("%s: Adding node to mutex queue: prio: %" PRIu32 "\n", sched_active_thread->name, n.priority);

    priority_queue_add(&(mutex->queue), &n);

    irq_restore(irqstate);

    thread_yield_higher();

    /* we were woken up by scheduler. waker removed us from queue. we have the mutex now. */
}

void mutex_unlock(struct mutex_t *mutex)
{
    unsigned irqstate = irq_disable();
    DEBUG("mutex_unlock(): val: %u pid: %" PRIkernel_pid "\n", ATOMIC_VALUE(mutex->val), sched_active_pid);

    if (ATOMIC_VALUE(mutex->val) == 0) {
        /* the mutex was not locked */
        irq_restore(irqstate);
        return;
    }

    priority_queue_node_t *next = priority_queue_remove_head(&(mutex->queue));
    if (!next) {
        /* the mutex was locked and no thread was waiting for it */
        ATOMIC_VALUE(mutex->val) = 0;
        irq_restore(irqstate);
        return;
    }

    thread_t *process = (thread_t *) next->data;
    DEBUG("mutex_unlock: waking up waiting thread %" PRIkernel_pid "\n", process->pid);
    sched_set_status(process, STATUS_PENDING);

    uint16_t process_priority = process->priority;
    irq_restore(irqstate);
    sched_switch(process_priority);
}

void mutex_unlock_and_sleep(struct mutex_t *mutex)
{
    DEBUG("%s: unlocking mutex. val: %u pid: %" PRIkernel_pid ", and taking a nap\n", sched_active_thread->name, ATOMIC_VALUE(mutex->val), sched_active_pid);
    unsigned irqstate = irq_disable();

    if (ATOMIC_VALUE(mutex->val) != 0) {
        priority_queue_node_t *next = priority_queue_remove_head(&(mutex->queue));
        if (next) {
            thread_t *process = (thread_t *) next->data;
            DEBUG("%s: waking up waiter.\n", process->name);
            sched_set_status(process, STATUS_PENDING);
        }
        else {
            ATOMIC_VALUE(mutex->val) = 0; /* This is safe, interrupts are disabled */
        }
    }
    DEBUG("%s: going to sleep.\n", sched_active_thread->name);
    sched_set_status((thread_t*) sched_active_thread, STATUS_SLEEPING);
    irq_restore(irqstate);
    thread_yield_higher();
}
