/**
 * kernel mutex implementation
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

#include <stdio.h>
#include "mutex.h"
#include "atomic.h"
#include "queue.h"
#include "tcb.h"
#include "kernel.h"
#include "sched.h"
#include <irq.h>

//#define ENABLE_DEBUG
#include <debug.h>

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
    return (atomic_set_return(&mutex->val, thread_pid) == 0);
}

int prio(void)
{
    return active_thread->priority;
}

int mutex_lock(struct mutex_t *mutex)
{
    DEBUG("%s: trying to get mutex. val: %u\n", active_thread->name, mutex->val);

    if(atomic_set_return(&mutex->val, 1) != 0) {
        /* mutex was locked. */
        mutex_wait(mutex);
    }

    return 1;
}

void mutex_wait(struct mutex_t *mutex)
{
    int irqstate = disableIRQ();
    DEBUG("%s: Mutex in use. %u\n", active_thread->name, mutex->val);

    if(mutex->val == 0) {
        /* somebody released the mutex. return. */
        mutex->val = thread_pid;
        DEBUG("%s: mutex_wait early out. %u\n", active_thread->name, mutex->val);
        restoreIRQ(irqstate);
        return;
    }

    sched_set_status((tcb_t*) active_thread, STATUS_MUTEX_BLOCKED);

    queue_node_t n;
    n.priority = (unsigned int) active_thread->priority;
    n.data = (unsigned int) active_thread;
    n.next = NULL;

    DEBUG("%s: Adding node to mutex queue: prio: %u\n", active_thread->name, n.priority);

    queue_priority_add(&(mutex->queue), &n);

    restoreIRQ(irqstate);

    thread_yield();

    /* we were woken up by scheduler. waker removed us from queue. we have the mutex now. */
}

void mutex_unlock(struct mutex_t *mutex, int yield)
{
    DEBUG("%s: unlocking mutex. val: %u pid: %u\n", active_thread->name, mutex->val, thread_pid);
    int irqstate = disableIRQ();

    if(mutex->val != 0) {
        if(mutex->queue.next) {
            queue_node_t *next = queue_remove_head(&(mutex->queue));
            tcb_t *process = (tcb_t*) next->data;
            DEBUG("%s: waking up waiter %s.\n", process->name);
            sched_set_status(process, STATUS_PENDING);

            sched_switch(active_thread->priority, process->priority, inISR());
        }
        else {
            mutex->val = 0;
        }
    }

    restoreIRQ(irqstate);
}

