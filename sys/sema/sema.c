/*
 * Copyright (C) 2013-15 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  René Kijewski <kijewski@inf.fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <limits.h>

#include "irq.h"
#include "msg.h"
#include "vtimer.h"

#include "sema.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define MSG_SIGNAL         (0x0501)
#define MSG_TIMEOUT        (0x0502)
#define MSG_DESTROYED      (0x0503)

int sem_create(sem_t *sem, unsigned int value)
{
    if (sem == NULL) {
#ifdef MODULE_POSIX_SEMAPHORE
        errno = EINVAL;
#endif
        return -EINVAL;
    }
    sem->value = value;
    /* waiters for the mutex */
    sem->queue.first = NULL;
    return 0;
}

int sem_destroy(sem_t *sem)
{
    unsigned int old_state;
    priority_queue_node_t *next;
    if (sem == NULL) {
#ifdef MODULE_POSIX_SEMAPHORE
        errno = EINVAL;
#endif
        return -EINVAL;
    }
    old_state = disableIRQ();
    while ((next = priority_queue_remove_head(&sem->queue)) != NULL) {
        msg_t msg;
        kernel_pid_t pid = (kernel_pid_t)next->data;
        msg.type = MSG_DESTROYED;
        msg.content.ptr = (void *) sem;
        msg_send_int(&msg, pid);
    }
    restoreIRQ(old_state);
    return 0;
}

int sem_wait_timed_msg(sem_t *sem, timex_t *timeout, msg_t *msg)
{
    if (sem == NULL) {
        return -EINVAL;
    }
    while (1) {
        unsigned old_state = disableIRQ();
        priority_queue_node_t n;
        vtimer_t timeout_timer;

        unsigned value = sem->value;
        if (value != 0) {
            sem->value = value - 1;
            restoreIRQ(old_state);
            return 0;
        }

        /* I'm going blocked */
        n.priority = (uint32_t)sched_active_thread->priority;
        n.data = (unsigned int)sched_active_pid;
        n.next = NULL;
        priority_queue_add(&sem->queue, &n);

        DEBUG("sem_wait: %" PRIkernel_pid ": Adding node to semaphore queue: prio: %" PRIu32 "\n",
              sched_active_thread->pid, sched_active_thread->priority);

        if (timeout != NULL) {
            vtimer_set_msg(&timeout_timer, *timeout, sched_active_pid,
                           MSG_TIMEOUT, sem);
        }

        restoreIRQ(old_state);
        msg_receive(msg);

        if (timeout != NULL) {
            vtimer_remove(&timeout_timer);  /* remove timer just to be sure */
        }

        if (msg->content.ptr != (void *) sem) {
            return -EAGAIN;
        }

        switch (msg->type) {
            case MSG_SIGNAL:
                continue;
            case MSG_TIMEOUT:
                return -ETIMEDOUT;
            case MSG_DESTROYED:
                return -ECANCELED;
            default:
                return -EAGAIN;
        }
    }
}

int sem_wait_timed(sem_t *sem, timex_t *timeout)
{
    int result;
    do {
        msg_t msg;
        result = sem_wait_timed_msg(sem, timeout, &msg);
        DEBUG("sem_wait: %" PRIkernel_pid ": Discarding message from %" PRIkernel_pid "\n",
              sched_active_thread->pid, msg->sender_pid);
    } while (result == -EAGAIN);
    return result;
}

int sem_post(sem_t *sem)
{
    unsigned int old_state, value;
    priority_queue_node_t *next;
    if (sem == NULL) {
        return -EINVAL;
    }
    old_state = disableIRQ();
    value = sem->value;
    if (value == UINT_MAX) {
        restoreIRQ(old_state);
        return -EOVERFLOW;
    }
    ++sem->value;
    next = priority_queue_remove_head(&sem->queue);
    if (next) {
        uint16_t prio = (uint16_t)next->priority;
        kernel_pid_t pid = (kernel_pid_t) next->data;
        msg_t msg;
        DEBUG("sem_post: %" PRIkernel_pid ": waking up %" PRIkernel_pid "\n",
              sched_active_thread->pid, next_process->pid);
        msg.type = MSG_SIGNAL;
        msg.content.ptr = (void *) sem;
        msg_send_int(&msg, pid);
        restoreIRQ(old_state);
        sched_switch(prio);
    }
    else {
        restoreIRQ(old_state);
    }

    return 1;
}

/** @} */
