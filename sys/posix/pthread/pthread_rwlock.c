/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     pthread
 * @{
 *
 * @file
 * @brief       Implementation of a fair, POSIX conforming reader/writer lock.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "pthread.h"
#include "sched.h"
#include "vtimer.h"

#include <stdint.h>
#include <string.h>

#define ENABLE_DEBUG (0)
#include "debug.h"

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr)
{
    (void) attr;

    if (rwlock == NULL) {
        DEBUG("Thread %" PRIkernel_pid " pthread_rwlock_%s(): rwlock=NULL supplied\n", thread_pid, "init");
        return EINVAL;
    }

    memset(rwlock, 0, sizeof (*rwlock));
    return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
    if (rwlock == NULL) {
        DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): rwlock=NULL supplied\n", thread_pid, "destroy");
        return EINVAL;
    }

    /* do not unlock the mutex, no need */
    if ((mutex_trylock(&rwlock->mutex) == 0) || (rwlock->readers != 0)) {
        return EBUSY;
    }

    return 0;
}

bool __pthread_rwlock_blocked_readingly(const pthread_rwlock_t *rwlock)
{
    if (rwlock->readers < 0) {
        /* a writer holds the lock */
        return true;
    }

    /* Determine if there is a writer waiting to get this lock who has a higher or the same priority: */

    if (rwlock->queue.first == NULL) {
        /* no waiting thread */
        return false;
    }

    priority_queue_node_t *qnode = rwlock->queue.first;
    if (qnode->priority > sched_active_thread->priority) {
        /* the waiting thread has a lower priority */
        return false;
    }

    /* if the waiting node is a writer, then we cannot enter the critical section (to prevent starving the writer) */
    __pthread_rwlock_waiter_node_t *waiting_node = (__pthread_rwlock_waiter_node_t *) qnode->data;
    return waiting_node->is_writer;
}

bool __pthread_rwlock_blocked_writingly(const pthread_rwlock_t *rwlock)
{
    /* if any thread holds the lock, then no writer may enter the critical section */
    return rwlock->readers != 0;
}

static int pthread_rwlock_lock(pthread_rwlock_t *rwlock,
                               bool (*is_blocked)(const pthread_rwlock_t *rwlock),
                               bool is_writer,
                               int incr_when_held,
                               bool allow_spurious)
{
    if (rwlock == NULL) {
        DEBUG("Thread %" PRIkernel_pid": pthread_rwlock_%s(): is_writer=%u, allow_spurious=%u %s\n",
              thread_pid, "lock", is_writer, allow_spurious, "rwlock=NULL");
        return EINVAL;
    }

    mutex_lock(&rwlock->mutex);
    if (!is_blocked(rwlock)) {
        DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): is_writer=%u, allow_spurious=%u %s\n",
              thread_pid, "lock", is_writer, allow_spurious, "is open");
        rwlock->readers += incr_when_held;
    }
    else {
        DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): is_writer=%u, allow_spurious=%u %s\n",
              thread_pid, "lock", is_writer, allow_spurious, "is locked");

        /* queue for the lock */
        __pthread_rwlock_waiter_node_t waiting_node = {
            .is_writer = is_writer,
            .thread = (tcb_t *) sched_active_thread,
            .qnode = {
                .next = NULL,
                .data = (uintptr_t) &waiting_node,
                .priority = sched_active_thread->priority,
            },
            .continue_ = false,
        };
        priority_queue_add(&rwlock->queue, &waiting_node.qnode);

        while (1) {
            /* wait to be unlocked, so this thread can try to acquire the lock again */
            mutex_unlock_and_sleep(&rwlock->mutex);

            mutex_lock(&rwlock->mutex);
            if (waiting_node.continue_) {
                /* pthread_rwlock_unlock() already set rwlock->readers */
                DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): is_writer=%u, allow_spurious=%u %s\n",
                      thread_pid, "lock", is_writer, allow_spurious, "continued");
                break;
            }
            else if (allow_spurious) {
                DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): is_writer=%u, allow_spurious=%u %s\n",
                      thread_pid, "lock", is_writer, allow_spurious, "is timed out");
                priority_queue_remove(&rwlock->queue, &waiting_node.qnode);
                mutex_unlock(&rwlock->mutex);
                return ETIMEDOUT;
            }
        }
    }
    mutex_unlock(&rwlock->mutex);

    return 0;
}

static int pthread_rwlock_trylock(pthread_rwlock_t *rwlock,
                                  bool (*is_blocked)(const pthread_rwlock_t *rwlock),
                                  int incr_when_held)
{
    if (rwlock == NULL) {
        DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): rwlock=NULL supplied\n", thread_pid, "trylock");
        return EINVAL;
    }
    else if (mutex_trylock(&rwlock->mutex) == 0) {
        return EBUSY;
    }
    else if (is_blocked(rwlock)) {
        mutex_unlock(&rwlock->mutex);
        return EBUSY;
    }

    rwlock->readers += incr_when_held;

    mutex_unlock(&rwlock->mutex);
    return 0;
}

static int pthread_rwlock_timedlock(pthread_rwlock_t *rwlock,
                                    bool (*is_blocked)(const pthread_rwlock_t *rwlock),
                                    bool is_writer,
                                    int incr_when_held,
                                    const struct timespec *abstime)
{
    timex_t now, then;

    then.seconds = abstime->tv_sec;
    then.microseconds = abstime->tv_nsec / 1000u;
    timex_normalize(&then);

    vtimer_now(&now);

    if (timex_cmp(then, now) <= 0) {
        return ETIMEDOUT;
    }
    else {
        timex_t reltime = timex_sub(then, now);

        vtimer_t timer;
        vtimer_set_wakeup(&timer, reltime, sched_active_pid);
        int result = pthread_rwlock_lock(rwlock, is_blocked, is_writer, incr_when_held, true);
        if (result != ETIMEDOUT) {
            vtimer_remove(&timer);
        }

        return result;
    }
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
    return pthread_rwlock_lock(rwlock, __pthread_rwlock_blocked_readingly, false, +1, false);
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
    return pthread_rwlock_lock(rwlock, __pthread_rwlock_blocked_writingly, true, -1, false);
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
    return pthread_rwlock_trylock(rwlock, __pthread_rwlock_blocked_readingly, +1);
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{
    return pthread_rwlock_trylock(rwlock, __pthread_rwlock_blocked_writingly, -1);
}

int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime)
{
    return pthread_rwlock_timedlock(rwlock, __pthread_rwlock_blocked_readingly, false, +1, abstime);
}

int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime)
{
    return pthread_rwlock_timedlock(rwlock, __pthread_rwlock_blocked_writingly, true, -1, abstime);
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
    if (rwlock == NULL) {
        DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): rwlock=NULL supplied\n", thread_pid, "unlock");
        return EINVAL;
    }

    mutex_lock(&rwlock->mutex);
    if (rwlock->readers == 0) {
        /* the lock is open */
        DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): lock is open\n", thread_pid, "unlock");
        mutex_unlock(&rwlock->mutex);
        return EPERM;
    }

    if (rwlock->readers > 0) {
        DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): release %s lock\n", thread_pid, "unlock", "read");
        --rwlock->readers;
    }
    else {
        DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): release %s lock\n", thread_pid, "unlock", "write");
        rwlock->readers = 0;
    }

    if (rwlock->readers != 0 || rwlock->queue.first == NULL) {
        /* this thread was not the last reader, or no one is waiting to aquire the lock */
        DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): no one is waiting\n", thread_pid, "unlock");
        mutex_unlock(&rwlock->mutex);
        return 0;
    }

    /* wake up the next thread */
    priority_queue_node_t *qnode = priority_queue_remove_head(&rwlock->queue);
    __pthread_rwlock_waiter_node_t *waiting_node = (__pthread_rwlock_waiter_node_t *) qnode->data;
    waiting_node->continue_ = true;
    uint16_t prio = qnode->priority;
    sched_set_status(waiting_node->thread, STATUS_PENDING);

    if (waiting_node->is_writer) {
        DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): continue %s %" PRIkernel_pid "\n",
              thread_pid, "unlock", "writer", waiting_node->thread->pid);
        --rwlock->readers;
    }
    else {
        DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): continue %s %" PRIkernel_pid "\n",
              thread_pid, "unlock", "reader", waiting_node->thread->pid);
        ++rwlock->readers;

        /* wake up further readers */
        while (rwlock->queue.first) {
            waiting_node = (__pthread_rwlock_waiter_node_t *) rwlock->queue.first->data;
            if (waiting_node->is_writer) {
                /* Not to be unfair to writers, we don't try to wake up readers that came after the first writer. */
                DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): continuing readers blocked by writer %" PRIkernel_pid "\n",
                      thread_pid, "unlock", waiting_node->thread->pid);
                break;
            }
            waiting_node->continue_ = true;
            DEBUG("Thread %" PRIkernel_pid ": pthread_rwlock_%s(): continue %s %" PRIkernel_pid "\n",
                  thread_pid, "unlock", "reader", waiting_node->thread->pid);

            /* wake up this reader */
            qnode = priority_queue_remove_head(&rwlock->queue);
            if (qnode->priority < prio) {
                prio = qnode->priority;
            }
            sched_set_status(waiting_node->thread, STATUS_PENDING);

            ++rwlock->readers;
        }
    }

    mutex_unlock(&rwlock->mutex);

    /* yield if a woken up thread had a higher priority */
    sched_switch(prio);
    return 0;
}
