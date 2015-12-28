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
#include "tcb.h"
#include "atomic.h"
#include "kernel.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"
#include "thread.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static void mutex_wait(struct mutex_t *mutex);

int mutex_trylock(struct mutex_t *mutex)
{
    DEBUG("mutex_trylock: pid=%" PRIkernel_pid " val=%u\n",
          sched_active_pid, ATOMIC_VALUE(mutex->val));
    return atomic_set_to_one(&mutex->val);
}

void mutex_lock(struct mutex_t *mutex)
{
    DEBUG("mutex_lock: pid=%" PRIkernel_pid " val=%u\n",
          sched_active_pid, ATOMIC_VALUE(mutex->val));
    if (atomic_set_to_one(&mutex->val) == 0) {
        /* mutex was locked. */
        mutex_wait(mutex);
    }
}

static void mutex_wait(struct mutex_t *mutex)
{
    unsigned irqstate = disableIRQ();

    if (atomic_set_to_one(&mutex->val)) {
        /* somebody released the mutex. return. */
        DEBUG("mutex_wait: early out, pid=%" PRIkernel_pid " val=%u\n",
              sched_active_pid, ATOMIC_VALUE(mutex->val));
        restoreIRQ(irqstate);
        return;
    }

    sched_set_status((tcb_t*) sched_active_thread, STATUS_MUTEX_BLOCKED);

    priority_queue_node_t n;
    n.priority = (unsigned int) sched_active_thread->priority;
    n.data = (unsigned int) sched_active_thread;
    n.next = NULL;

    DEBUG("mutex_wait: add to wait queue, pid=%" PRIkernel_pid " val=%u "
          "prio%" PRIu32 "\n", sched_active_pid, ATOMIC_VALUE(mutex->val),
          n.priority);

    priority_queue_add(&(mutex->queue), &n);

    restoreIRQ(irqstate);

    thread_yield_higher();

    /* we were woken up by scheduler. waker removed us from queue. we have the mutex now. */
}

void mutex_unlock(struct mutex_t *mutex)
{
    unsigned irqstate = disableIRQ();
    DEBUG("mutex_unlock: pid=%" PRIkernel_pid " val=%u\n",
          sched_active_pid, ATOMIC_VALUE(mutex->val));

    if (ATOMIC_VALUE(mutex->val) == 0) {
        /* the mutex was not locked */
        restoreIRQ(irqstate);
        return;
    }

    priority_queue_node_t *next = priority_queue_remove_head(&(mutex->queue));
    if (!next) {
        /* the mutex was locked and no thread was waiting for it */
        ATOMIC_VALUE(mutex->val) = 0;
        restoreIRQ(irqstate);
        return;
    }

    tcb_t *process = (tcb_t *) next->data;
    DEBUG("mutex_unlock: waking up %" PRIkernel_pid "\n", process->pid);
    sched_set_status(process, STATUS_PENDING);

    uint16_t process_priority = process->priority;
    restoreIRQ(irqstate);
    sched_switch(process_priority);
}

void mutex_unlock_and_sleep(struct mutex_t *mutex)
{
    DEBUG("mutex_unlock_and_sleep: pid=%" PRIkernel_pid " val=%u\n",
          sched_active_pid, ATOMIC_VALUE(mutex->val));
    unsigned irqstate = disableIRQ();

    if (ATOMIC_VALUE(mutex->val) != 0) {
        priority_queue_node_t *next = priority_queue_remove_head(&(mutex->queue));
        if (next) {
            tcb_t *process = (tcb_t *) next->data;
            DEBUG("mutex_unlock_and_sleep: waking up %" PRIkernel_pid "\n",
                  process->pid);
            sched_set_status(process, STATUS_PENDING);
        }
        else {
            ATOMIC_VALUE(mutex->val) = 0; /* This is safe, interrupts are disabled */
        }
    }

    DEBUG("mutex_unlock_and_sleep: going to sleep.\n");
    sched_set_status((tcb_t*) sched_active_thread, STATUS_SLEEPING);
    restoreIRQ(irqstate);
    thread_yield_higher();
}
