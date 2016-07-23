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

#include <stdio.h>
#include <inttypes.h>

#include "mutex.h"
#include "thread.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"
#include "thread.h"
#include "list.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

int _mutex_lock(mutex_t *mutex, int blocking)
{
    unsigned irqstate = irq_disable();

    DEBUG("PID[%" PRIkernel_pid "]: Mutex in use.\n", sched_active_pid);

    if (mutex->queue.next == NULL) {
        /* mutex is unlocked. */
        mutex->queue.next = MUTEX_LOCKED;
        DEBUG("PID[%" PRIkernel_pid "]: mutex_wait early out.\n",
              sched_active_pid);
        irq_restore(irqstate);
        return 1;
    }
    else if (blocking) {
        thread_t *me = (thread_t*)sched_active_thread;
        DEBUG("PID[%" PRIkernel_pid "]: Adding node to mutex queue: prio: %"
              PRIu32 "\n", sched_active_pid, (uint32_t)me->priority);
        sched_set_status(me, STATUS_MUTEX_BLOCKED);
        if (mutex->queue.next == MUTEX_LOCKED) {
            mutex->queue.next = (list_node_t*)&me->rq_entry;
            mutex->queue.next->next = NULL;
        }
        else {
            thread_add_to_list(&mutex->queue, me);
        }
        irq_restore(irqstate);
        thread_yield_higher();
        /* We were woken up by scheduler. Waker removed us from queue.
         * We have the mutex now. */
        return 1;
    }
    else {
        irq_restore(irqstate);
        return 0;
    }
}

void mutex_unlock(mutex_t *mutex)
{
    unsigned irqstate = irq_disable();

    DEBUG("mutex_unlock(): queue.next: 0x%08x pid: %" PRIkernel_pid "\n",
          (unsigned)mutex->queue.next, sched_active_pid);

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

    thread_t *process = container_of((clist_node_t*)next, thread_t, rq_entry);

    DEBUG("mutex_unlock: waking up waiting thread %" PRIkernel_pid "\n",
          process->pid);
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
    DEBUG("PID[%" PRIkernel_pid "]: unlocking mutex. queue.next: 0x%08x, and "
          "taking a nap\n", sched_active_pid, (unsigned)mutex->queue.next);
    unsigned irqstate = irq_disable();

    if (mutex->queue.next) {
        if (mutex->queue.next == MUTEX_LOCKED) {
            mutex->queue.next = NULL;
        }
        else {
            list_node_t *next = list_remove_head(&mutex->queue);
            thread_t *process = container_of((clist_node_t*)next, thread_t,
                                             rq_entry);
            DEBUG("PID[%" PRIkernel_pid "]: waking up waiter.\n", process->pid);
            sched_set_status(process, STATUS_PENDING);
            if (!mutex->queue.next) {
                mutex->queue.next = MUTEX_LOCKED;
            }
        }
    }

    DEBUG("PID[%" PRIkernel_pid "]: going to sleep.\n", sched_active_pid);
    sched_set_status((thread_t*)sched_active_thread, STATUS_SLEEPING);
    irq_restore(irqstate);
    thread_yield_higher();
}
