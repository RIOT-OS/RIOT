/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     core_msg
 * @{
 *
 * @file
 * @brief       Kernel messaging implementation
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stddef.h>
#include <inttypes.h>
#include "kernel.h"
#include "sched.h"
#include "msg.h"
#include "queue.h"
#include "tcb.h"
#include "irq.h"
#include "cib.h"

#include "flags.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "thread.h"

static int _msg_receive(msg_t *m, int block);


static int queue_msg(msg_tcb_t *target, msg_t *m)
{
    int n = cib_put(&(target->msg_queue));

    if (n != -1) {
        target->msg_array[n] = *m;
        return 1;
    }

    return 0;
}

int msg_send(msg_t *m, unsigned int target_pid, bool block)
{
    if (inISR()) {
        return msg_send_int(m, target_pid);
    }

    if ((unsigned int) thread_pid == target_pid) {
        return -1;
    }

    tcb_t *target_ = (tcb_t *) sched_threads[target_pid];
    if ((target_ == NULL) || ((target_->flags & CREATE_NOMSG) != 0)) {
        return -1;
    }
    msg_tcb_t *msg_target = (msg_tcb_t *) target_;

    m->sender_pid = thread_pid;

    unsigned old_state = disableIRQ();

    DEBUG("msg_send() Sending from %i to %i. block=%i src->state=%i target->state=%i\n",
          thread_pid, target_pid, block, active_thread->status, msg_target->tcb.status);

    if (msg_target->tcb.status != STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send() Target %i is not RECEIVE_BLOCKED.\n", target_pid);

        if (msg_target->msg_array && queue_msg(msg_target, m)) {
            DEBUG("msg_send() Target %i has a msg_queue. Queueing message.\n", target_pid);
            restoreIRQ(old_state);
            if (active_thread->status == STATUS_REPLY_BLOCKED) {
                thread_yield();
            }
            return 1;
        }

        if (!block || ((active_thread->flags & CREATE_NOMSG) != 0)) {
            DEBUG("msg_send: %s: Receiver not waiting, block=%u\n", active_thread->name, block);
            restoreIRQ(old_state);
            return 0;
        }

        msg_tcb_t *msg_source = (msg_tcb_t *) active_thread;

        DEBUG("msg_send: %s: send_blocked.\n", msg_source->tcb.name);
        queue_node_t n;
        n.priority = msg_source->tcb.priority;
        n.data = (unsigned int) msg_source;
        n.next = NULL;
        DEBUG("msg_send: %s: Adding node to msg_waiters:\n", msg_source->tcb.name);

        queue_priority_add(&(msg_target->msg_waiters), &n);

        msg_source->wait_data = (void*) m;

        int newstatus;

        if (msg_source->tcb.status == STATUS_REPLY_BLOCKED) {
            newstatus = STATUS_REPLY_BLOCKED;
        }
        else {
            newstatus = STATUS_SEND_BLOCKED;
        }

        /* get blocked */
        sched_set_status(&msg_source->tcb, newstatus);
        restoreIRQ(old_state);
        thread_yield();

        DEBUG("msg_send: %s: Back from send block.\n", msg_source->tcb.name);
    }
    else {
        DEBUG("msg_send: %s: Direct msg copy from %i to %i.\n",
              active_thread->name, thread_pid, target_pid);

        /* copy msg to target */
        msg_t *target_message = (msg_t*) msg_target->wait_data;
        *target_message = *m;

        /* wake up other thread */
        int target_prio = msg_target->tcb.priority;
        sched_set_status(&msg_target->tcb, STATUS_PENDING);
        restoreIRQ(old_state);
        sched_switch(active_thread->priority, target_prio);
    }

    return 1;
}

int msg_send_int(msg_t *m, unsigned int target_pid)
{
    tcb_t *target_ = (tcb_t *) sched_threads[target_pid];
    if ((target_ == NULL) && ((target_->flags & CREATE_NOMSG) != 0)) {
        return -1;
    }
    msg_tcb_t *msg_target = (msg_tcb_t *) target_;

    if (msg_target->tcb.status == STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send_int: Direct msg copy from %i to %i.\n", thread_pid, target_pid);

        m->sender_pid = target_pid;

        /* copy msg to target */
        msg_t *target_message = (msg_t *) msg_target->wait_data;
        *target_message = *m;
        sched_set_status(&msg_target->tcb, STATUS_PENDING);

        sched_context_switch_request = 1;
        return 1;
    }
    else {
        DEBUG("msg_send_int: Receiver not waiting.\n");
        return queue_msg(msg_target, m);
    }
}

int msg_send_receive(msg_t *m, msg_t *reply, unsigned int target_pid)
{
    if ((active_thread->flags & CREATE_NOMSG) != 0) {
        return -1;
    }

    unsigned old_state = disableIRQ();

    msg_tcb_t *me = (msg_tcb_t *) active_thread;
    sched_set_status(&me->tcb, STATUS_REPLY_BLOCKED);
    me->wait_data = (void*) reply;

    /* msg_send blocks until reply received */
    int result = msg_send(m, target_pid, true);

    restoreIRQ(old_state);
    return result;
}

int msg_reply(msg_t *m, msg_t *reply)
{
    unsigned old_state = disableIRQ();

    tcb_t *target_ = (tcb_t *) sched_threads[m->sender_pid];

    if (!target_) {
        DEBUG("msg_reply(): %s: Target \"%" PRIu16 "\" not existing...dropping msg!\n",
              active_thread->name, m->sender_pid);
        restoreIRQ(old_state);
        return -1;
    }

    if (target_->status != STATUS_REPLY_BLOCKED) {
        DEBUG("msg_reply(): %s: Target \"%s\" not waiting for reply.",
              active_thread->name, target_->name);
        restoreIRQ(old_state);
        return -1;
    }

    /* No need to test target_'s flags. Only if it accepts messages it can be STATUS_REPLY_BLOCKED. */
    msg_tcb_t *msg_target = (msg_tcb_t *) target_;
    int target_prio = msg_target->tcb.priority;

    /* copy msg to target */
    msg_t *target_message = (msg_t *) msg_target->wait_data;
    *target_message = *reply;
    sched_set_status(&msg_target->tcb, STATUS_PENDING);

    restoreIRQ(old_state);
    sched_switch(active_thread->priority, target_prio);

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
    unsigned old_state = disableIRQ();
    DEBUG("_msg_receive: %s: _msg_receive.\n", active_thread->name);

    if ((active_thread->flags & CREATE_NOMSG) != 0) {
        restoreIRQ(old_state);
        return -1;
    }

    msg_tcb_t *me = (msg_tcb_t *) active_thread;

    int queue_index = -1;

    if (me->msg_array) {
        queue_index = cib_get(&(me->msg_queue));
    }

    /* no message, fail */
    if ((!block) && (queue_index == -1)) {
        restoreIRQ(old_state);
        return -1;
    }

    if (queue_index >= 0) {
        DEBUG("_msg_receive: %s: _msg_receive(): We've got a queued message.\n", active_thread->name);
        *m = me->msg_array[queue_index];
    }
    else {
        me->wait_data = (void *) m;
    }

    queue_node_t *node = queue_remove_head(&(me->msg_waiters));

    if (node == NULL) {
        DEBUG("_msg_receive: %s: _msg_receive(): No thread in waiting list.\n", active_thread->name);

        if (queue_index < 0) {
            DEBUG("_msg_receive(): %s: No msg in queue. Going blocked.\n", active_thread->name);
            sched_set_status(&me->tcb, STATUS_RECEIVE_BLOCKED);

            restoreIRQ(old_state);
            thread_yield();

            /* sender copied message */
        }
        else {
            restoreIRQ(old_state);
        }
    }
    else {
        DEBUG("_msg_receive: %s: _msg_receive(): Waking up waiting thread.\n", active_thread->name);
        msg_tcb_t *sender = (msg_tcb_t *) node->data;

        if (queue_index >= 0) {
            /* We've already got a message from the queue. As there is a
             * waiter, take it's message into the just freed queue space.
             */
            m = &(me->msg_array[cib_put(&(me->msg_queue))]);
        }

        /* copy msg */
        msg_t *sender_msg = (msg_t *) sender->wait_data;
        *m = *sender_msg;

        int sender_prio = -1;

        /* remove sender from queue */
        if (sender->tcb.status != STATUS_REPLY_BLOCKED) {
            sender->wait_data = NULL;
            sched_set_status(&sender->tcb, STATUS_PENDING);
            sender_prio = sender->tcb.priority;
        }

        restoreIRQ(old_state);
        if (sender_prio >= 0) {
            sched_switch(active_thread->priority, sender_prio);
        }
    }
    return 1;
}

int msg_init_queue(msg_t *array, int num)
{
    /* check if num is a power of two by comparing to its complement */
    if (num && (num & (num - 1)) != 0) {
        return -1;
    }

    /* check if the active_thread allows messaging */
    if ((active_thread->flags & CREATE_NOMSG) != 0) {
        return -1;
    }

    msg_tcb_t *me = (msg_tcb_t *) active_thread;
    me->msg_array = array;
    cib_init(&(me->msg_queue), num);
    return 0;
}
