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

#include "cpu.h"
#include "mutex.h"
#include "thread.h"
#include "sched.h"
#include "irq.h"
#include "list.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if MAXTHREADS > 1

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
static inline __attribute__((always_inline))
void _block(mutex_t *mutex,
            unsigned irq_state,
            uinttxtptr_t pc)
{
    /* pc is only used when MODULE_CORE_MUTEX_DEBUG */
    (void)pc;
#if IS_USED(MODULE_CORE_MUTEX_DEBUG)
    printf("[mutex] waiting for thread %" PRIkernel_pid " (pc = 0x%" PRIxTXTPTR
           ")\n",
           mutex->owner, mutex->owner_calling_pc);
#endif
    thread_t *me = thread_get_active();

    /* Fail visibly even if a blocking action is called from somewhere where
     * it's subtly not allowed, eg. board_init */
    assert(me != NULL);
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

#ifdef MODULE_CORE_MUTEX_PRIORITY_INHERITANCE
    thread_t *owner = thread_get(mutex->owner);
    if ((owner) && (owner->priority > me->priority)) {
        DEBUG("PID[%" PRIkernel_pid "] prio of %" PRIkernel_pid
              ": %u --> %u\n",
              thread_getpid(), mutex->owner,
              (unsigned)owner->priority, (unsigned)me->priority);
        sched_change_priority(owner, me->priority);
    }
#endif

    irq_restore(irq_state);
    thread_yield_higher();
    /* We were woken up by scheduler. Waker removed us from queue. */
#if IS_USED(MODULE_CORE_MUTEX_DEBUG)
    mutex->owner_calling_pc = pc;
#endif
}

bool mutex_lock_internal(mutex_t *mutex, bool block)
{
    uinttxtptr_t pc = 0;
#if IS_USED(MODULE_CORE_MUTEX_DEBUG)
    pc = cpu_get_caller_pc();
#endif
    unsigned irq_state = irq_disable();

    DEBUG("PID[%" PRIkernel_pid "] mutex_lock_internal(block=%u).\n",
          thread_getpid(), (unsigned)block);

    if (mutex->queue.next == NULL) {
        /* mutex is unlocked. */
        mutex->queue.next = MUTEX_LOCKED;
#if IS_USED(MODULE_CORE_MUTEX_PRIORITY_INHERITANCE) \
        || IS_USED(MODULE_CORE_MUTEX_DEBUG)
        thread_t *me = thread_get_active();
        mutex->owner = me->pid;
#endif
#if IS_USED(MODULE_CORE_MUTEX_DEBUG)
        mutex->owner_calling_pc = pc;
#endif
#if IS_USED(MODULE_CORE_MUTEX_PRIORITY_INHERITANCE)
        mutex->owner_original_priority = me->priority;
#endif
        DEBUG("PID[%" PRIkernel_pid "] mutex_lock(): early out.\n",
              thread_getpid());
        irq_restore(irq_state);
    }
    else {
        if (!block) {
            irq_restore(irq_state);
            return false;
        }
        _block(mutex, irq_state, pc);
    }

    return true;
}

int mutex_lock_cancelable(mutex_cancel_t *mc)
{
    uinttxtptr_t pc = 0;
#if IS_USED(MODULE_CORE_MUTEX_DEBUG)
    pc = cpu_get_caller_pc();
#endif
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
#if IS_USED(MODULE_CORE_MUTEX_PRIORITY_INHERITANCE) \
        || IS_USED(MODULE_CORE_MUTEX_DEBUG)
        thread_t *me = thread_get_active();
        mutex->owner = me->pid;
#endif
#if IS_USED(MODULE_CORE_MUTEX_DEBUG)
        mutex->owner_calling_pc = pc;
#endif
#if IS_USED(MODULE_CORE_MUTEX_PRIORITY_INHERITANCE)
        mutex->owner_original_priority = me->priority;
#endif
        DEBUG("PID[%" PRIkernel_pid "] mutex_lock_cancelable() early out.\n",
              thread_getpid());
        irq_restore(irq_state);
        return 0;
    }
    else {
        _block(mutex, irq_state, pc);
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

#if IS_USED(MODULE_CORE_MUTEX_PRIORITY_INHERITANCE)
    thread_t *owner = thread_get(mutex->owner);
    if ((owner) && (owner->priority != mutex->owner_original_priority)) {
        DEBUG("PID[%" PRIkernel_pid "] prio %u --> %u\n",
              owner->pid, (unsigned)owner->priority,
              (unsigned)mutex->owner_original_priority);
        sched_change_priority(owner, mutex->owner_original_priority);
    }
#endif
#if IS_USED(MODULE_CORE_MUTEX_DEBUG)
    mutex->owner_calling_pc = 0;
#endif

    irq_restore(irqstate);
    thread_yield_higher();
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

#else /* MAXTHREADS < 2 */
typedef int dont_be_pedantic;
#endif
