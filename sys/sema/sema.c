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
#include "xtimer.h"

#include "sema.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define MSG_SIGNAL         (0x0501)
#define MSG_TIMEOUT        (0x0502)
#define MSG_DESTROYED      (0x0503)

int sema_create(sema_t *sema, unsigned int value)
{
    if (sema == NULL) {
        return -EINVAL;
    }
    sema->value = value;
    /* waiters for the mutex */
    sema->queue.first = NULL;
    return 0;
}

int sema_destroy(sema_t *sema)
{
    unsigned int old_state;
    priority_queue_node_t *next;
    if (sema == NULL) {
        return -EINVAL;
    }
    old_state = disableIRQ();
    while ((next = priority_queue_remove_head(&sema->queue)) != NULL) {
        msg_t msg;
        kernel_pid_t pid = (kernel_pid_t)next->data;
        msg.type = MSG_DESTROYED;
        msg.content.ptr = (void *) sema;
        msg_send_int(&msg, pid);
    }
    restoreIRQ(old_state);
    return 0;
}

int sema_wait_timed_msg(sema_t *sema, uint64_t timeout, msg_t *msg)
{
    if (sema == NULL) {
        return -EINVAL;
    }
    while (1) {
        unsigned old_state = disableIRQ();
        priority_queue_node_t n;
        xtimer_t timeout_timer;
        msg_t timeout_msg;

        unsigned value = sema->value;
        if (value != 0) {
            sema->value = value - 1;
            restoreIRQ(old_state);
            return 0;
        }

        /* I'm going blocked */
        n.priority = (uint32_t)sched_active_thread->priority;
        n.data = (unsigned int)sched_active_pid;
        n.next = NULL;
        priority_queue_add(&sema->queue, &n);

        DEBUG("sema_wait: %" PRIkernel_pid ": Adding node to semaphore queue: prio: %" PRIu32 "\n",
              sched_active_thread->pid, sched_active_thread->priority);

        if (timeout != 0) {
            timeout_msg.type = MSG_TIMEOUT;
            timeout_msg.content.ptr = (char *)sema;
            /* we will stay in the same stack context so we can use timeout_msg */
            xtimer_set_msg64(&timeout_timer, timeout, &timeout_msg, sched_active_pid);
        }

        restoreIRQ(old_state);
        msg_receive(msg);

        if (timeout != 0) {
            xtimer_remove(&timeout_timer);
        }
        priority_queue_remove(&sema->queue, &n);
        if (msg->content.ptr != (void *)sema) {
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

int sema_wait_timed(sema_t *sema, uint64_t timeout)
{
    int result;
    do {
        msg_t msg;
        result = sema_wait_timed_msg(sema, timeout, &msg);
        DEBUG("sema_wait: %" PRIkernel_pid ": Discarding message from %" PRIkernel_pid "\n",
              sched_active_thread->pid, msg->sender_pid);
    }
    while (result == -EAGAIN);
    return result;
}

int sema_post(sema_t *sema)
{
    unsigned int old_state, value;
    priority_queue_node_t *next;
    if (sema == NULL) {
        return -EINVAL;
    }
    old_state = disableIRQ();
    value = sema->value;
    if (value == UINT_MAX) {
        restoreIRQ(old_state);
        return -EOVERFLOW;
    }
    ++sema->value;
    next = priority_queue_remove_head(&sema->queue);
    if (next) {
        uint16_t prio = (uint16_t)next->priority;
        kernel_pid_t pid = (kernel_pid_t) next->data;
        msg_t msg;
        DEBUG("sema_post: %" PRIkernel_pid ": waking up %" PRIkernel_pid "\n",
              sched_active_thread->pid, next_process->pid);
        msg.type = MSG_SIGNAL;
        msg.content.ptr = (void *) sema;
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
