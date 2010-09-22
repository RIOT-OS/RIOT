/**
 * kernel mutex implementation
 *
 * Copyright (C) 2010 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
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
#include "scheduler.h"

//#define ENABLE_DEBUG
#include <debug.h>

int mutex_init(struct mutex_t* mutex) {
    mutex->val = 0;

    mutex->queue.priority = 0;
    mutex->queue.data = 0;
    mutex->queue.next = NULL;

    return 1;
}

int mutex_trylock(struct mutex_t* mutex) {
    return (atomic_set_return(&mutex->val, fk_pid ) == 0);
}

int prio() {
    return fk_thread->priority;
}

int mutex_lock(struct mutex_t* mutex) {
   DEBUG("%s: trying to get mutex. val: %u\n", fk_thread->name, mutex->val);

    if (atomic_set_return(&mutex->val,fk_pid) != 0) {
        // mutex was locked.
        mutex_wait(mutex);
    }
    return 1;
}

void mutex_unlock(struct mutex_t* mutex, int yield) {
    DEBUG("%s: unlocking mutex. val: %u pid: %u\n", fk_thread->name, mutex->val, fk_pid);
    int me_value;

    if (inISR()) {
        me_value = 0;
        yield = MUTEX_INISR;
    } else {
        me_value = fk_pid;
    }
                
    if (atomic_set_return(&mutex->val,0) != me_value ) {
        // there were waiters.
        mutex_wake_waiters(mutex, yield);
    }
}

void mutex_wait(struct mutex_t *mutex) {
    dINT();
    DEBUG("%s: Mutex in use. %u\n", fk_thread->name, mutex->val);
    if (mutex->val == 0) {
        // somebody released the mutex. return.
        mutex->val = fk_pid;
        DEBUG("%s: mutex_wait early out. %u\n", fk_thread->name, mutex->val);
        eINT();
        return;
    }

    sched_set_status((tcb*)fk_thread, STATUS_MUTEX_BLOCKED);

    queue_node_t n;
    n.priority = (unsigned int) fk_thread->priority;
    n.data = (unsigned int) fk_thread;
    n.next = NULL;

    DEBUG("%s: Adding node to mutex queue: prio: %u data: %u\n", fk_thread->name, n.priority, n.data);

    queue_priority_add(&(mutex->queue), &n);

    eINT();

    fk_yield();

    /* we were woken up by scheduler. waker removed us from queue. we have the mutex now. */
}

void mutex_wake_waiters(struct mutex_t *mutex, int flags) {
    if ( ! (flags & MUTEX_INISR)) dINT();
    DEBUG("%s: waking up waiters.\n", fk_thread->name);

    queue_node_t *next = queue_remove_head(&(mutex->queue));
    tcb* process = (tcb*)next->data;

    sched_set_status(process, STATUS_PENDING);

    if ( mutex->queue.next != NULL) {
        mutex->val = -1;
    } else {
        mutex->val = process->pid;
    }

    DEBUG("%s: waiters woken up.\n", fk_thread->name);

    /* If called from process, reenable interrupts, yield if requested */
    if (! (flags & MUTEX_INISR)) {
        eINT();
        if (flags & MUTEX_YIELD) fk_yield();
    } else {
       fk_context_switch_request = 1; 
    }
}

