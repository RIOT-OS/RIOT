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
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "mutex.h"
#include "tcb.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

int _mutex_lock(struct mutex_t *mutex, int non_blocking)
{
    unsigned irqstate = disableIRQ();
    DEBUG("_mutex_wait: pid=%" PRIkernel_pid " holder=%" PRIkernel_pid
          " non-blocking=%u\n", sched_active_pid, mutex->val, non_blocking);

    if (mutex->val == KERNEL_PID_UNDEF) {
        /* mutex was not locked */
        kernel_pid_t pid = sched_active_pid;
        if (pid == KERNEL_PID_UNDEF) {
            /* if used by an interrupt handler the pid could be undefined. */
            /* this might hurt debugging, but won't hurt normal use. */
            pid = (kernel_pid_t) -1u;
        }
        DEBUG("_mutex_wait: locking for holder=%" PRIkernel_pid "\n", pid);

        mutex->val = pid;
        restoreIRQ(irqstate);
        return 1;
    }
    else if (non_blocking) {
        /* mutex was locked and we were asked not to block. */
        /* nothing to do. */
        DEBUG("_mutex_wait: already locked, aborting\n");
        restoreIRQ(irqstate);
        return 0;
    }
    else {
        sched_set_status((tcb_t*) sched_active_thread, STATUS_MUTEX_BLOCKED);

        priority_queue_node_t n;
        n.priority = (unsigned int) sched_active_thread->priority;
        n.data = sched_active_pid;
        n.next = NULL;

        DEBUG("_mutex_wait: add to wait queue, prio%" PRIu32 "\n", n.priority);

        priority_queue_add(&(mutex->queue), &n);
        restoreIRQ(irqstate);
        thread_yield_higher();

        /* we were woken up by scheduler. */
        /* waker removed us from queue. we have the mutex now. */
        return 0;
    }
}

void mutex_unlock(struct mutex_t *mutex)
{
    unsigned irqstate = disableIRQ();
    DEBUG("mutex_unlock: pid=%" PRIkernel_pid " holder=%" PRIkernel_pid "\n",
          sched_active_pid, mutex->val);

    if (mutex->val == KERNEL_PID_UNDEF) {
        /* the mutex was not locked */
        restoreIRQ(irqstate);
        return;
    }

    priority_queue_node_t *next = priority_queue_remove_head(&(mutex->queue));
    if (!next) {
        /* the mutex was locked and no thread was waiting for it */
        mutex->val = KERNEL_PID_UNDEF;
        restoreIRQ(irqstate);
        return;
    }

    /* the waked up thread holds the mutex now */
    mutex->val = next->data;

    tcb_t *process = (tcb_t *) sched_threads[next->data];
    DEBUG("mutex_unlock: waking up %" PRIkernel_pid "\n", next->data);
    sched_set_status(process, STATUS_PENDING);

    uint16_t process_priority = process->priority;
    restoreIRQ(irqstate);
    sched_switch(process_priority);
}

void mutex_unlock_and_sleep(struct mutex_t *mutex)
{
    DEBUG("mutex_unlock_and_sleep: pid=%" PRIkernel_pid " holder=%"
          PRIkernel_pid "\n", sched_active_pid, mutex->val);
    unsigned irqstate = disableIRQ();

    if (mutex->val != KERNEL_PID_UNDEF) {
        priority_queue_node_t *next = priority_queue_remove_head(&(mutex->queue));
        if (next) {
            /* the waked up thread holds the mutex now */
            mutex->val = next->data;

            tcb_t *process = (tcb_t *) sched_threads[next->data];
            DEBUG("mutex_unlock_and_sleep: waking up %" PRIkernel_pid "\n",
                  next->data);
            sched_set_status(process, STATUS_PENDING);
        }
        else {
            mutex->val = KERNEL_PID_UNDEF;
        }
    }

    DEBUG("mutex_unlock_and_sleep: going to sleep.\n");
    sched_set_status((tcb_t*) sched_active_thread, STATUS_SLEEPING);
    restoreIRQ(irqstate);
    thread_yield_higher();
}
