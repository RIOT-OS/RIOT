/**
 * Semaphore implemenation
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup posix
 *
 * @{
 *
 * @file    semaphore.c
 * @brief   Implementation of semaphores with priority queues
 *
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  René Kijewski <kijewski@inf.fu-berlin.de>
 *
 * @}
 */

#include <inttypes.h>

#include "irq.h"
#include "sched.h"
#include "tcb.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "semaphore.h"

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    (void) pshared;  /* nothing to do */

    sem->value = value;

    /* waiters for the mutex */
    sem->queue.first = NULL;

    return 0;
}

int sem_destroy(sem_t *sem)
{
    if (sem->queue.first) {
        DEBUG("%s: tried to destroy active semaphore.\n", sched_active_thread->name);
        return -1;
    }
    return 0;
}

sem_t *sem_open(const char *name, int oflag, ...)
{
    (void) name; /* named semaphore currently not supported */
    (void) oflag;
    return SEM_FAILED;
}

int sem_close(sem_t *sem)
{
    (void) sem; /* named semaphore currently not supported */
    return -1;
}

int sem_unlink(const char *name)
{
    (void) name; /* named semaphore currently not supported */
    return -1;
}

static void sem_thread_blocked(sem_t *sem)
{
    /* I'm going blocked */
    sched_set_status((tcb_t*) sched_active_thread, STATUS_MUTEX_BLOCKED);

    priority_queue_node_t n;
    n.priority = (uint32_t) sched_active_thread->priority;
    n.data = (size_t) sched_active_thread;
    n.next = NULL;

    DEBUG("%s: Adding node to mutex queue: prio: %" PRIu32 "\n",
          sched_active_thread->name, n.priority);

    /* add myself to the waiters queue */
    priority_queue_add(&sem->queue, &n);

    /* scheduler should schedule an other thread, that unlocks the
     * mutex in the future, when this happens I get scheduled again
     */
    thread_yield_higher();
}

int sem_wait(sem_t *sem)
{
    unsigned old_state = disableIRQ();
    while (1) {
        unsigned value = sem->value;
        if (value == 0) {
            sem_thread_blocked(sem);
            continue;
        }
        else {
            sem->value = value - 1;
            break;
        }
    }
    restoreIRQ(old_state);
    return 1;
}

int sem_timedwait(sem_t *sem, const struct timespec *abstime)
{
    (void) sem; /* timedwait currently not supported */
    (void) abstime;
    return -1; /* signal problem */
}

int sem_trywait(sem_t *sem)
{
    unsigned old_state = disableIRQ();
    int result;

    unsigned value = sem->value;
    if (value == 0) {
        result = -1;
    }
    else {
        result = 0;
        sem->value = value - 1;
    }

    restoreIRQ(old_state);
    return result;
}

int sem_post(sem_t *sem)
{
    unsigned old_state = disableIRQ();
    ++sem->value;

    thread_priority_t prio = PRIORITY_IDLE;

    priority_queue_node_t *next = priority_queue_remove_head(&sem->queue);
    if (next) {
        tcb_t *next_process = (tcb_t*) next->data;
        DEBUG("%s: waking up %s\n", sched_active_thread->name, next_process->name);
        prio = next_process->priority;
        sched_set_status(next_process, STATUS_PENDING);
    }

    restoreIRQ(old_state);
    sched_switch(prio);

    return 1;
}

int sem_getvalue(sem_t *sem, int *sval)
{
    *sval = sem->value;
    return 0;
}
