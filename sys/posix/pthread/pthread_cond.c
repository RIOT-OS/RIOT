/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences (HAW)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file        pthread_cond.c
 * @brief       Condition variable implementation
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "pthread_cond.h"
#include "thread.h"
#include "vtimer.h"
#include "sched.h"
#include "irq.h"
#include "debug.h"

struct vtimer_t timer;

int pthread_cond_condattr_destroy(struct pthread_condattr_t *attr)
{
    if (attr != NULL) {
        DEBUG("pthread_cond_condattr_destroy: currently attributes are not supported.\n");
    }

    return 0;
}

int pthread_cond_condattr_init(struct pthread_condattr_t *attr)
{
    if (attr != NULL) {
        DEBUG("pthread_cond_condattr_init: currently attributes are not supported.\n");
    }

    return 0;
}

int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared)
{
    (void)attr;
    (void)pshared;

    return 0;
}

int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared)
{
    (void)attr;
    (void)pshared;

    return 0;
}

int pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t *clock_id)
{
    (void)attr;
    (void)clock_id;

    return 0;
}

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id)
{
    (void)attr;
    (void)clock_id;

    return 0;
}

int pthread_cond_init(struct pthread_cond_t *cond, struct pthread_condattr_t *attr)
{
    if (attr != NULL) {
        DEBUG("pthread_cond_init: currently attributes are not supported.\n");
    }

    cond->queue.first = NULL;
    return 0;
}

int pthread_cond_destroy(struct pthread_cond_t *cond)
{
    pthread_cond_init(cond, NULL);
    return 0;
}

int pthread_cond_wait(struct pthread_cond_t *cond, struct mutex_t *mutex)
{
    priority_queue_node_t n;
    n.priority = sched_active_thread->priority;
    n.data = sched_active_thread->pid;
    n.next = NULL;

    /* the signaling thread may not hold the mutex, the queue is not thread safe */
    unsigned old_state = disableIRQ();
    priority_queue_add(&(cond->queue), &n);
    restoreIRQ(old_state);

    mutex_unlock_and_sleep(mutex);

    if (n.data != -1u) {
        /* on signaling n.data is set to -1u */
        /* if it isn't set, then the wakeup is either spurious or a timer wakeup */
        old_state = disableIRQ();
        priority_queue_remove(&(cond->queue), &n);
        restoreIRQ(old_state);
    }

    mutex_lock(mutex);
    return 0;
}

int pthread_cond_timedwait(struct pthread_cond_t *cond, struct mutex_t *mutex, const struct timespec *abstime)
{
    timex_t now, then, reltime;

    vtimer_now(&now);
    then.seconds = abstime->tv_sec;
    then.microseconds = abstime->tv_nsec / 1000u;
    reltime = timex_sub(then, now);

    vtimer_t timer;
    vtimer_set_wakeup(&timer, reltime, sched_active_thread->pid);
    int result = pthread_cond_wait(cond, mutex);
    vtimer_remove(&timer);

    return result;
}

int pthread_cond_signal(struct pthread_cond_t *cond)
{
    unsigned old_state = disableIRQ();

    priority_queue_node_t *head = priority_queue_remove_head(&(cond->queue));
    int other_prio = -1;
    if (head != NULL) {
        tcb_t *other_thread = (tcb_t *) sched_threads[head->data];
        if (other_thread) {
            other_prio = other_thread->priority;
            sched_set_status(other_thread, STATUS_PENDING);
        }
        head->data = -1u;
    }

    restoreIRQ(old_state);

    if (other_prio >= 0) {
        sched_switch(other_prio);
    }

    return 0;
}

static int max_prio(int a, int b)
{
    return (a < 0) ? b : ((a < b) ? a : b);
}

int pthread_cond_broadcast(struct pthread_cond_t *cond)
{
    unsigned old_state = disableIRQ();

    int other_prio = -1;

    while (1) {
        priority_queue_node_t *head = priority_queue_remove_head(&(cond->queue));
        if (head == NULL) {
            break;
        }

        tcb_t *other_thread = (tcb_t *) sched_threads[head->data];
        if (other_thread) {
            other_prio = max_prio(other_prio, other_thread->priority);
            sched_set_status(other_thread, STATUS_PENDING);
        }
        head->data = -1u;
    }

    restoreIRQ(old_state);

    if (other_prio >= 0) {
        sched_switch(other_prio);
    }

    return 0;
}
