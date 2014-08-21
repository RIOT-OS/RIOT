/*
 * POSIX compatible implementation of barriers.
 *
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief Synchronization barriers.
 * @author René Kijewski <kijewski@inf.fu-berlin.de>
 * @}
 */

#include "sched.h"
#include "pthread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int pthread_barrier_init(pthread_barrier_t *barrier,
                         const pthread_barrierattr_t *attr,
                         unsigned int count)
{
    (void) attr;
    barrier->next = NULL;
    mutex_init(&barrier->mutex);
    barrier->count = count;
    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    barrier->count = -1;
    return 0;
}

static inline int priority_min(int a, int b)
{
    if (a == -1) {
        return b;
    }
    return a < b ? a : b;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    /* Idea: the count is decreased by every thread that waits on the barrier.
     * If the value is bigger than zero afterwards, then the thread has to wait
     * to be woken up. Once the value reaches zero, everyone gets woken up. */

    mutex_lock(&barrier->mutex);
    DEBUG("%s: hit a synchronization barrier. pid=%" PRIkernel_pid"\n",
          sched_active_thread->name, sched_active_thread->pid);

    int switch_prio = -1;

    if (--barrier->count > 0) {
        /* need to wait for further threads */

        DEBUG("%s: waiting for %u threads. pid=%" PRIkernel_pid "\n",
              sched_active_thread->name, barrier->count, sched_active_thread->pid);

        pthread_barrier_waiting_node_t node;
        node.pid = sched_active_pid;
        node.next = barrier->next;
        node.cont = 0;

        barrier->next = &node;
        mutex_unlock(&barrier->mutex);

        while (1) {
            /* The mutex is reacquired before checking if we should continue,
             * so that the waiting thread don't accidentially run before the
             * wake up loop has ended. Otherwise the thread could run into the
             * the barrier again before `barrier->count` was reset. */
            mutex_lock(&barrier->mutex);
            if (node.cont) {
                break;
            }
            mutex_unlock_and_sleep(&barrier->mutex);
        }
    }
    else {
        /* all threads have arrived, wake everybody up */

        DEBUG("%s: waking every other thread up. pid=%" PRIkernel_pid "\n",
              sched_active_thread->name, sched_active_thread->pid);

        int count = 1; /* Count number of woken up threads.
                        * The first thread is the current thread. */
        pthread_barrier_waiting_node_t *next;
        for (next = barrier->next; next; next = next->next) {
            ++count;
            next->cont = 1;

            tcb_t *other = (tcb_t *) sched_threads[next->pid];
            switch_prio = priority_min(switch_prio, other->priority);
            sched_set_status(other, STATUS_PENDING);
        }
        barrier->next = NULL;
        barrier->count = count;
    }

    mutex_unlock(&barrier->mutex);

    if (switch_prio != -1) {
        sched_switch(switch_prio);
    }

    return 0;
}
