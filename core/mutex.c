/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013 Freie Universität Berlin
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

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>

#include "mutex.h"
#include "thread.h"
#include "sched.h"
#include "irq.h"
#include "list.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Block waiting for a locked mutex
 * @pre     IRQs are disabled
 * @post    IRQs are restored to @p irq_state
 * @post    The calling thread is no longer waiting for the mutex, either
 *          because it got the mutex, or because the operation was cancelled
 *          (only possible for @ref mutex_lock_cancelable)
 *
 * Most applications don't use @ref mutex_lock_cancelable. Inlining this
 * function into both @ref mutex_lock and @ref mutex_lock_cancelable is,
 * therefore, beneficial for the majority of applications.
 */
static inline __attribute__((always_inline)) void _block(mutex_t *mutex,
                                                         unsigned irq_state)
{
    thread_t *me = thread_get_active();

    DEBUG("PID[%" PRIkernel_pid "] mutex_lock() Adding node to mutex queue: "
          "prio: %" PRIu32 "\n", thread_getpid(), (uint32_t)me->priority);
    sched_set_status(me, STATUS_MUTEX_BLOCKED);
    if (mutex->queue.next == MUTEX_LOCKED) {
        mutex->queue.next = (list_node_t *)&me->rq_entry;
        mutex->queue.next->next = NULL;
    }
    else {
        thread_add_to_list(&mutex->queue, me);
    }

    irq_restore(irq_state);
    thread_yield_higher();
    /* We were woken up by scheduler. Waker removed us from queue. */
}

void mutex_lock(mutex_t *mutex)
{
    unsigned irq_state = irq_disable();

    DEBUG("PID[%" PRIkernel_pid "] mutex_lock().\n", thread_getpid());

    if (mutex->queue.next == NULL) {
        /* mutex is unlocked. */
        mutex->queue.next = MUTEX_LOCKED;
        DEBUG("PID[%" PRIkernel_pid "] mutex_lock(): early out.\n",
              thread_getpid());
        irq_restore(irq_state);
    }
    else {
        _block(mutex, irq_state);
    }
}

int mutex_lock_cancelable(mutex_cancel_t *mc)
{
    unsigned irq_state = irq_disable();

    DEBUG("PID[%" PRIkernel_pid "] mutex_lock_cancelable()\n",
          thread_getpid());

    if (mc->cancelled) {
        DEBUG("PID[%" PRIkernel_pid "] mutex_lock_cancelable cancelled "
              "early.\n", thread_getpid());
        irq_restore(irq_state);
        return -ECANCELED;
    }

    mutex_t *mutex = mc->mutex;

    if (mutex->queue.next == NULL) {
        /* mutex is unlocked. */
        mutex->queue.next = MUTEX_LOCKED;
        DEBUG("PID[%" PRIkernel_pid "] mutex_lock_cancelable() early out.\n",
              thread_getpid());
        irq_restore(irq_state);
        return 0;
    }
    else {
        _block(mutex, irq_state);
        if (mc->cancelled) {
            DEBUG("PID[%" PRIkernel_pid "] mutex_lock_cancelable() "
                  "cancelled.\n", thread_getpid());
        }
        return (mc->cancelled) ? -ECANCELED : 0;
    }
}

void mutex_unlock(mutex_t *mutex)
{
    unsigned irqstate = irq_disable();

    DEBUG("PID[%" PRIkernel_pid "] mutex_unlock(): queue.next: %p\n",
          thread_getpid(), (void *)mutex->queue.next);

    if (mutex->queue.next == NULL) {
        /* the mutex was not locked */
        irq_restore(irqstate);
        return;
    }

    if (mutex->queue.next == MUTEX_LOCKED) {
        mutex->queue.next = NULL;
        /* the mutex was locked and no thread was waiting for it */
        irq_restore(irqstate);
        return;
    }

    list_node_t *next = list_remove_head(&mutex->queue);

    thread_t *process = container_of((clist_node_t *)next, thread_t, rq_entry);

    DEBUG("PID[%" PRIkernel_pid "] mutex_unlock(): waking up waiting thread %"
          PRIkernel_pid "\n", thread_getpid(),  process->pid);
    sched_set_status(process, STATUS_PENDING);

    if (!mutex->queue.next) {
        mutex->queue.next = MUTEX_LOCKED;
    }

    uint16_t process_priority = process->priority;

    irq_restore(irqstate);
    sched_switch(process_priority);
}

void mutex_unlock_and_sleep(mutex_t *mutex)
{
    DEBUG("PID[%" PRIkernel_pid "] mutex_unlock_and_sleep(): queue.next: %p\n",
          thread_getpid(), (void *)mutex->queue.next);
    unsigned irqstate = irq_disable();

    if (mutex->queue.next) {
        if (mutex->queue.next == MUTEX_LOCKED) {
            mutex->queue.next = NULL;
        }
        else {
            list_node_t *next = list_remove_head(&mutex->queue);
            thread_t *process = container_of((clist_node_t *)next, thread_t,
                                             rq_entry);
            DEBUG("PID[%" PRIkernel_pid "] mutex_unlock_and_sleep(): waking up "
                  "waiter.\n", process->pid);
            sched_set_status(process, STATUS_PENDING);
            if (!mutex->queue.next) {
                mutex->queue.next = MUTEX_LOCKED;
            }
        }
    }

    DEBUG("PID[%" PRIkernel_pid "] mutex_unlock_and_sleep(): going to sleep.\n",
          thread_getpid());
    sched_set_status(thread_get_active(), STATUS_SLEEPING);
    irq_restore(irqstate);
    thread_yield_higher();
}

void mutex_cancel(mutex_cancel_t *mc)
{
    unsigned irq_state = irq_disable();

    mc->cancelled = 1;

    mutex_t *mutex = mc->mutex;
    thread_t *thread = mc->thread;

    if (thread_is_active(thread)) {
        /* thread is still running or about to run, so it will check
         * `mc-cancelled` in time */
        irq_restore(irq_state);
        return;
    }

    if ((mutex->queue.next != MUTEX_LOCKED)
        && (mutex->queue.next != NULL)
        && list_remove(&mutex->queue, (list_node_t *)&thread->rq_entry)) {
        /* Thread was queued and removed from list, wake it up */
        if (mutex->queue.next == NULL) {
            mutex->queue.next = MUTEX_LOCKED;
        }
        sched_set_status(thread, STATUS_PENDING);
        irq_restore(irq_state);
        sched_switch(thread->priority);
        return;
    }

    irq_restore(irq_state);
}

/* Helper for compatibility with C++ or other non-C languages */
int mutex_trylock_ffi(mutex_t *mutex)
{
    return mutex_trylock(mutex);
}
