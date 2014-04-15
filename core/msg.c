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


static int queue_msg(tcb_t *target, msg_t *m)
{
    int n = cib_put(&(target->msg_queue));

    if (n != -1) {
        target->msg_array[n] = *m;
        return 1;
    }

    return 0;
}

int msg_send(msg_t *m, tcb_t *target, bool block)
{
    if (inISR()) {
        return msg_send_int(m, target);
    }

    m->sender = active_thread;

    if (active_thread == target) {
        return -1;
    }

    if (target == NULL) {
        return -1;
    }

    dINT();

    DEBUG("msg_send() %s:%i: Sending from %i to %i. block=%i src->state=%i target->state=%i\n", __FILE__, __LINE__, thread_pid, target_pid, block, active_thread->status, target->status);

    if (target->status != STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send() %s:%i: Target %i is not RECEIVE_BLOCKED.\n", __FILE__, __LINE__, target_pid);
        if (target->msg_array && queue_msg(target, m)) {
            DEBUG("msg_send() %s:%i: Target %i has a msg_queue. Queueing message.\n", __FILE__, __LINE__, target_pid);
            eINT();
            if (active_thread->status == STATUS_REPLY_BLOCKED) {
                thread_yield();
            }
            return 1;
        }

        if (!block) {
            DEBUG("msg_send: %s: Receiver not waiting, block=%u\n", active_thread->name, block);
            eINT();
            return 0;
        }

        DEBUG("msg_send: %s: send_blocked.\n", active_thread->name);
        queue_node_t n;
        n.thread = active_thread;
        n.next = NULL;
        DEBUG("msg_send: %p: Adding node to msg_waiters:\n", active_thread->stack_start);

        queue_priority_add(&(target->msg_waiters), &n);

        active_thread->wait_data = (void*) m;

        int newstatus;

        if (active_thread->status == STATUS_REPLY_BLOCKED) {
            newstatus = STATUS_REPLY_BLOCKED;
        }
        else {
            newstatus = STATUS_SEND_BLOCKED;
        }

        sched_set_status((tcb_t*) active_thread, newstatus);

        DEBUG("msg_send: %p: Back from send block.\n", active_thread->stack_start);
    }
    else {
        DEBUG("msg_send: %p: Direct msg copy from %i to %i.\n", active_thread->stack_start, thread_getpid(), target_pid);
        /* copy msg to target */
        msg_t *target_message = (msg_t*) target->wait_data;
        *target_message = *m;
        sched_set_status(target, STATUS_PENDING);
    }

    eINT();
    thread_yield();

    return 1;
}

int msg_send_int(msg_t *m, tcb_t *target)
{
    if (target->status == STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send_int: Direct msg copy from %i to %i.\n", thread_getpid(), target_pid);

        m->sender = active_thread;

        /* copy msg to target */
        msg_t *target_message = (msg_t*) target->wait_data;
        *target_message = *m;
        sched_set_status(target, STATUS_PENDING);

        sched_context_switch_request = 1;
        return 1;
    }
    else {
        DEBUG("msg_send_int: Receiver not waiting.\n");
        return (queue_msg(target, m));
    }
}

int msg_send_receive(msg_t *m, msg_t *reply, tcb_t *target)
{
    dINT();

    sched_set_status(active_thread, STATUS_REPLY_BLOCKED);
    active_thread->wait_data = (void*) reply;

    /* msg_send blocks until reply received */
    return msg_send(m, target, true);
}

int msg_reply(msg_t *m, msg_t *reply)
{
    int state = disableIRQ();

    tcb_t *target = m->sender;

    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("msg_reply(): %p: Target \"%p\" not waiting for reply.", active_thread->stack_start, target->stack_start);
        restoreIRQ(state);
        return -1;
    }

    DEBUG("msg_reply(): %p: Direct msg copy.\n", active_thread->stack_start);
    /* copy msg to target */
    msg_t *target_message = (msg_t*) target->wait_data;
    *target_message = *reply;
    sched_set_status(target, STATUS_PENDING);
    restoreIRQ(state);
    thread_yield();

    return 1;
}

int msg_reply_int(msg_t *m, msg_t *reply)
{
    tcb_t *target = m->sender;

    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("msg_reply_int(): %s: Target \"%s\" not waiting for reply.", active_thread->stack_start, target->stack_start);
        return -1;
    }

    msg_t *target_message = (msg_t*) target->wait_data;
    *target_message = *reply;
    sched_set_status(target, STATUS_PENDING);
    sched_context_switch_request = 1;
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
    dINT();
    DEBUG("_msg_receive: %s: _msg_receive.\n", active_thread->name);

    int queue_index = -1;

    if (active_thread->msg_array) {
        queue_index = cib_get(&(active_thread->msg_queue));
    }

    /* no message, fail */
    if ((!block) && (queue_index == -1)) {
        return -1;
    }

    if (queue_index >= 0) {
        DEBUG("_msg_receive: %p: _msg_receive(): We've got a queued message.\n", active_thread->stack_start);
        *m = active_thread->msg_array[queue_index];
    }
    else {
        active_thread->wait_data = (void *) m;
    }

    queue_node_t *node = queue_remove_head(&(active_thread->msg_waiters));

    if (node == NULL) {
        DEBUG("_msg_receive: %p: _msg_receive(): No thread in waiting list.\n", active_thread->stack_start);

        if (queue_index < 0) {
            DEBUG("_msg_receive(): %s: No msg in queue. Going blocked.\n", active_thread->stack_start);
            sched_set_status(active_thread, STATUS_RECEIVE_BLOCKED);

            eINT();
            thread_yield();

            /* sender copied message */
        }

        return 1;
    }
    else {
        DEBUG("_msg_receive: %p: _msg_receive(): Waking up waiting thread.\n", active_thread->stack_start);
        tcb_t *sender = container_of(node, tcb_t, rq_entry);

        if (queue_index >= 0) {
            /* We've already got a message from the queue. As there is a
             * waiter, take it's message into the just freed queue space.
             */
            m = &(active_thread->msg_array[cib_put(&(active_thread->msg_queue))]);
        }

        /* copy msg */
        msg_t *sender_msg = (msg_t*) sender->wait_data;
        *m = *sender_msg;

        /* remove sender from queue */
        if (sender->status != STATUS_REPLY_BLOCKED) {
            sender->wait_data = NULL;
            sched_set_status(sender, STATUS_PENDING);
        }

        eINT();
        return 1;
    }
}

int msg_init_queue(msg_t *array, int num)
{
    /* check if num is a power of two by comparing to its complement */
    if (num && (num & (num - 1)) == 0) {
        active_thread->msg_array = array;
        cib_init(&(active_thread->msg_queue), num);
        return 0;
    }

    return -1;
}
