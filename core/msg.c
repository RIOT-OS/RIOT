/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_msg
 * @{
 *
 * @file
 * @brief       Kernel messaging implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stddef.h>
#include <inttypes.h>
#include "kernel.h"
#include "sched.h"
#include "msg.h"
#include "priority_queue.h"
#include "tcb.h"
#include "irq.h"
#include "cib.h"

#include "flags.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "thread.h"

static int _msg_receive(msg_t *m, int block);
static int _msg_send(const msg_t *m, kernel_pid_t target_pid, bool block, unsigned state);

static inline void _msg_copy_into(msg_t *dest, kernel_pid_t sender_pid, const msg_t *src)
{
    dest->sender_pid = sender_pid;
    dest->type = src->type;
    dest->content = src->content;
}

static int queue_msg(tcb_t *target, kernel_pid_t sender_pid, const msg_t *m)
{
    int n = cib_put(&(target->msg_queue));
    if (n < 0) {
        DEBUG("queue_msg(): message queue is full (or there is none)\n");
        return 0;
    }

    DEBUG("queue_msg(): queuing message\n");
    _msg_copy_into(&target->msg_array[n], sender_pid, m);
    return 1;
}

int msg_send(const msg_t *m, kernel_pid_t target_pid)
{
    if (inISR()) {
        return msg_send_int(m, target_pid);
    }
    if (sched_active_pid == target_pid) {
        return msg_send_to_self(m);
    }
    return _msg_send(m, target_pid, true, disableIRQ());
}

int msg_try_send(const msg_t *m, kernel_pid_t target_pid)
{
    if (inISR()) {
        return msg_send_int(m, target_pid);
    }
    if (sched_active_pid == target_pid) {
        return msg_send_to_self(m);
    }
    return _msg_send(m, target_pid, false, disableIRQ());
}

static int _msg_send(const msg_t *m, kernel_pid_t target_pid, bool block, unsigned state)
{
#if DEVELHELP
    if (!pid_is_valid(target_pid)) {
        DEBUG("msg_send(): target_pid is invalid, continuing anyways\n");
    }
#endif /* DEVELHELP */

    tcb_t *target = (tcb_t*) sched_threads[target_pid];
    tcb_t *me = (tcb_t*) sched_active_thread;

    if (target == NULL) {
        DEBUG("msg_send(): target thread does not exist\n");
        restoreIRQ(state);
        return -1;
    }

    DEBUG("msg_send() %s:%i: Sending from %" PRIkernel_pid " to %" PRIkernel_pid ". "
          "block=%i src->state=%i target->state=%i\n", __FILE__, __LINE__,
          sched_active_pid, target_pid, block, me->status, target->status);

    if (target->status != STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send() %s:%i: Target %" PRIkernel_pid " is not RECEIVE_BLOCKED.\n",
              __FILE__, __LINE__, target_pid);
        if (queue_msg(target, sched_active_pid, m)) {
            DEBUG("msg_send() %s:%i: Target %" PRIkernel_pid " has a msg_queue. "
                  "Queueing message.\n", __FILE__, __LINE__, target_pid);
            restoreIRQ(state);
            if (me->status == STATUS_REPLY_BLOCKED) {
                thread_yield_higher();
            }
            return 1;
        }

        if (!block) {
            DEBUG("msg_send: %s: Receiver not waiting, block=%u\n", me->name, block);
            restoreIRQ(state);
            return 0;
        }

        DEBUG("msg_send: %s: send_blocked.\n", me->name);
        priority_queue_node_t n;
        n.priority = me->priority;
        n.data = (unsigned int) me;
        n.next = NULL;
        DEBUG("msg_send: %s: Adding node to msg_waiters:\n", me->name);

        priority_queue_add(&(target->msg_waiters), &n);

        me->wait_data = (msg_t *) m; /* discard const-ness */

        int newstatus;
        if (me->status == STATUS_REPLY_BLOCKED) {
            newstatus = STATUS_REPLY_BLOCKED;
        }
        else {
            newstatus = STATUS_SEND_BLOCKED;
        }

        sched_set_status(me, newstatus);

        DEBUG("msg_send: %s: Back from send block.\n", me->name);

        restoreIRQ(state);
        thread_yield_higher();
    }
    else {
        DEBUG("msg_send: %s: Direct msg copy from %" PRIkernel_pid " to %" PRIkernel_pid ".\n",
              me->name, thread_getpid(), target_pid);
        /* copy msg to target */
        _msg_copy_into(target->wait_data, sched_active_pid, m);
        sched_set_status(target, STATUS_PENDING);

        restoreIRQ(state);
        thread_yield_higher();
    }

    return 1;
}

int msg_send_to_self(const msg_t *m)
{
    unsigned state = disableIRQ();

    int res = queue_msg((tcb_t *) sched_active_thread, sched_active_pid, m);

    restoreIRQ(state);
    return res;
}

int msg_send_int(const msg_t *m, kernel_pid_t target_pid)
{
#if DEVELHELP
    if (!pid_is_valid(target_pid)) {
        DEBUG("msg_send(): target_pid is invalid, continuing anyways\n");
    }
#endif /* DEVELHELP */

    tcb_t *target = (tcb_t *) sched_threads[target_pid];

    if (target == NULL) {
        DEBUG("msg_send_int(): target thread does not exist\n");
        return -1;
    }

    if (target->status == STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send_int: Direct msg copy from %" PRIkernel_pid " to %" PRIkernel_pid ".\n",
              thread_getpid(), target_pid);

        /* copy msg to target */
        _msg_copy_into(target->wait_data, KERNEL_PID_ISR, m);
        sched_set_status(target, STATUS_PENDING);

        sched_context_switch_request = 1;
        return 1;
    }
    else {
        DEBUG("msg_send_int: Receiver not waiting.\n");
        return (queue_msg(target, KERNEL_PID_ISR, m));
    }
}

int msg_send_receive(const msg_t *m, msg_t *reply, kernel_pid_t target_pid)
{
    unsigned state = disableIRQ();

    tcb_t *me = (tcb_t*) sched_active_thread;
    sched_set_status(me, STATUS_REPLY_BLOCKED);

    msg_t in_out = *m;
    me->wait_data = &in_out;
    /* msg_send blocks until reply received */
    int result = _msg_send(&in_out, target_pid, true, state);
    *reply = in_out;

    return result;
}

int msg_reply(const msg_t *m, const msg_t *reply)
{
    unsigned state = disableIRQ();

    tcb_t *target = (tcb_t*) sched_threads[m->sender_pid];

    if (!target) {
        DEBUG("msg_reply(): %s: Target \"%" PRIkernel_pid "\" not existing...dropping msg!\n",
              sched_active_thread->name, m->sender_pid);
        return -1;
    }

    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("msg_reply(): %s: Target \"%s\" not waiting for reply.", sched_active_thread->name, target->name);
        restoreIRQ(state);
        return -1;
    }

    DEBUG("msg_reply(): %s: Direct msg copy.\n", sched_active_thread->name);
    /* copy msg to target */
    _msg_copy_into(target->wait_data, sched_active_pid, reply);

    sched_set_status(target, STATUS_PENDING);
    uint16_t target_prio = target->priority;
    restoreIRQ(state);
    sched_switch(target_prio);

    return 1;
}

int msg_try_receive(msg_t *m)
{
    return _msg_receive(m, 0);
}

int msg_receive(msg_t *m)
{
    return _msg_receive(m, 1);
}

static int _msg_receive(msg_t *m, int block)
{
    unsigned state = disableIRQ();
    DEBUG("_msg_receive: %s: _msg_receive.\n", sched_active_thread->name);

    tcb_t *me = (tcb_t*) sched_active_thread;

    int queue_index = cib_get(&(me->msg_queue));

    /* no message, fail */
    if ((!block) && (queue_index == -1)) {
        restoreIRQ(state);
        return -1;
    }

    if (queue_index >= 0) {
        DEBUG("_msg_receive: %s: _msg_receive(): We've got a queued message.\n", sched_active_thread->name);
        *m = me->msg_array[queue_index];
    }
    else {
        me->wait_data = m;
    }

    priority_queue_node_t *node = priority_queue_remove_head(&(me->msg_waiters));

    if (node == NULL) {
        DEBUG("_msg_receive: %s: _msg_receive(): No thread in waiting list.\n", sched_active_thread->name);

        if (queue_index < 0) {
            DEBUG("_msg_receive(): %s: No msg in queue. Going blocked.\n", sched_active_thread->name);
            sched_set_status(me, STATUS_RECEIVE_BLOCKED);

            restoreIRQ(state);
            thread_yield_higher();

            /* sender copied message */
        }
        else {
            restoreIRQ(state);
        }

        return 1;
    }
    else {
        DEBUG("_msg_receive: %s: _msg_receive(): Waking up waiting thread.\n", sched_active_thread->name);
        tcb_t *sender = (tcb_t*) node->data;

        if (queue_index >= 0) {
            /* We've already got a message from the queue. As there is a
             * waiter, take it's message into the just freed queue space.
             */
            m = &(me->msg_array[cib_put(&(me->msg_queue))]);
        }

        /* copy msg */
        _msg_copy_into(m, sender->pid, sender->wait_data);

        /* remove sender from queue */
        uint16_t sender_prio = PRIORITY_IDLE;
        if (sender->status != STATUS_REPLY_BLOCKED) {
            sender->wait_data = NULL;
            sched_set_status(sender, STATUS_PENDING);
            sender_prio = sender->priority;
        }

        restoreIRQ(state);
        if (sender_prio < PRIORITY_IDLE) {
            sched_switch(sender_prio);
        }
        return 1;
    }

    DEBUG("This should have never been reached!\n");
}

int msg_init_queue(msg_t *array, int num)
{
    /* check if num is a power of two by comparing to its complement */
    if (num && (num & (num - 1)) == 0) {
        tcb_t *me = (tcb_t*) sched_active_thread;
        me->msg_array = array;
        cib_init(&(me->msg_queue), num);
        return 0;
    }

    return -1;
}
