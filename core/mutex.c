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
 * @author      René Kijewski <kijewski@inf.fu-berlin.de>
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

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Locking helper function
 * @param irqstate Result from disableIRQ().
 * @returns 1 if thread_yield() was called, 0 otherwise
 *
 * Precondition: interrupts are disabled.
 * Postcondition: interrupts have their old state.
 */
static int mutex_lock_restore(struct mutex_t *mutex, int irqstate)
{
    DEBUG("%s: trying to get mutex. val: %u\n", active_thread->name, mutex->val);

    if (mutex->val == 0) {
        /* Mutex was unlocked, we hold it now. */
        mutex->val = 1;
        restoreIRQ(irqstate);
        return 0;
    }
    else {
        /* Mutex was locked, we need to wait for it to get unlocked. */
        DEBUG("%s: Mutex in use. %u\n", active_thread->name, mutex->val);

        sched_set_status((tcb_t*) active_thread, STATUS_MUTEX_BLOCKED);

        queue_node_t n;
        n.priority = (unsigned int) active_thread->priority;
        n.data = (unsigned int) active_thread;
        n.next = NULL;

        DEBUG("%s: Adding node to mutex queue: prio: %" PRIu32 "\n", active_thread->name, n.priority);

        queue_priority_add(&(mutex->queue), &n);

        restoreIRQ(irqstate);

        thread_yield();
        /* We were woken up by the scheduler. Waker removed us from queue. We hold the mutex now. */
        return 1;
    }
}

/**
* @brief Unlock helper function.
* @returns priority of waiting thread that was woken up, -1 otherwise
*
* Precondition and postcondition: interrupts are disabled.
*/
static int mutex_unlock_noyield(struct mutex_t *mutex)
{
    if (mutex->val == 0) {
        /* the mutex was not locked */
        return -1;
    }
    else if (!mutex->queue.next) {
        /* The mutex was locked and nobody wants to hold the mutex, so it's unlocked now. */
        mutex->val = 0;
        return -1;
    }

    /* Wake up waiting process. It holds the lock, now. */
    queue_node_t *next = queue_remove_head(&(mutex->queue));
    tcb_t *process = (tcb_t*) next->data;

    DEBUG("%s: waking up waiter.\n", process->name);
    sched_set_status(process, STATUS_PENDING);

    return process->priority;
}

int mutex_init(struct mutex_t *mutex)
{
    mutex->val = 0;

    mutex->queue.priority = 0;
    mutex->queue.data = 0;
    mutex->queue.next = NULL;

    return 1;
}

int mutex_trylock(struct mutex_t *mutex)
{
    DEBUG("%s: trylocking to get mutex. val: %u\n", active_thread->name, mutex->val);
    return (atomic_set_return(&mutex->val, 1) == 0);
}

int mutex_lock(struct mutex_t *mutex)
{
    int irqstate = disableIRQ();
    mutex_lock_restore(mutex, irqstate);
    return 1;
}

void mutex_unlock(struct mutex_t *mutex)
{
    int irqstate = disableIRQ();

    DEBUG("%s: unlocking mutex. val: %u pid: %u\n", active_thread->name, mutex->val, thread_pid);

    int other_priority = mutex_unlock_noyield(mutex);
    restoreIRQ(irqstate);

    if (other_priority >= 0) {
        /* If there was a thread woken up, then test if we should yield. */
        sched_switch(active_thread->priority, other_priority);
    }
}

void mutex_unlock_and_sleep(struct mutex_t *mutex)
{
    int irqstate = disableIRQ();

    DEBUG("%s: unlocking mutex. val: %u pid: %u, and take a nap\n",
          active_thread->name, mutex->val, thread_pid);

    mutex_unlock_noyield(mutex);

    sched_set_status((tcb_t*) active_thread, STATUS_SLEEPING);
    restoreIRQ(irqstate);
    thread_yield();
}

void mutex_unlock_and_lock(struct mutex_t *old_mutex, struct mutex_t *new_mutex)
{
    int irqstate = disableIRQ();

    DEBUG("%s: unlocking mutex. val(old): %u , val(new): %u, pid: %u\n",
          active_thread->name, old_mutex->val, new_mutex->val, thread_pid);

    int other_priority = mutex_unlock_noyield(old_mutex);
    int has_yielded = mutex_lock_restore(new_mutex, irqstate);

    if (other_priority >= 0 && has_yielded == 0) {
        sched_switch(active_thread->priority, other_priority);
    }
}
