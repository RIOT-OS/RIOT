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
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>s
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
static int _msg_send(const msg_t *m, kernel_pid_t target_pid, bool block);

static int queue_msg(tcb_t *target, const msg_t *m, kernel_pid_t sender_pid)
{
    int n = cib_put(&(target->msg_queue));

    if (n != -1) {
        msg_t *dest = &target->msg_array[n];
        *dest = *m;
        dest->sender_pid = sender_pid;
        return 1;
    }

    return 0;
}

int msg_send(const msg_t *m, kernel_pid_t target_pid) {
    return _msg_send(m, target_pid, true);
}

int msg_try_send(const msg_t *m, kernel_pid_t target_pid) {
    return _msg_send(m, target_pid, false);
}

static int _msg_send(const msg_t *m, kernel_pid_t target_pid, bool block)
{
    if (inISR()) {
        return msg_send_int(m, target_pid);
    }

    if (sched_active_pid == target_pid) {
        return msg_send_to_self(m);
    }

#if DEVELHELP
    if (!pid_is_valid(target_pid)) {
        DEBUG("msg_send(): target_pid is invalid, continuing anyways\n");
    }
#endif /* DEVELHELP */

    dINT();

    tcb_t *target = (tcb_t*) sched_threads[target_pid];

    if (target == NULL) {
        DEBUG("msg_send(): target thread %" PRIkernel_pid " does not exist\n", target_pid);
        eINT();
        return -1;
    }

    DEBUG("msg_send() Sending from %" PRIkernel_pid " to %" PRIkernel_pid ". block=%i src->state=%i target->state=%i\n",
          sched_active_pid, target_pid, block, sched_active_thread->status, target->status);

    if (target->status != STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send(): target is not RECEIVE_BLOCKED.\n");

        if (target->msg_array && queue_msg(target, m, sched_active_pid)) {
            DEBUG("msg_send(): target has a msg_queue. Queueing message.\n");
            eINT();
            if (sched_active_thread->status == STATUS_REPLY_BLOCKED) {
                thread_yield_higher();
            }
            return 1;
        }

        if (!block) {
            DEBUG("msg_send: Receiver not waiting, not blocking.\n");
            eINT();
            return 0;
        }

        priority_queue_node_t n;
        n.priority = sched_active_thread->priority;
        n.data = (unsigned int) sched_active_thread;
        n.next = NULL;

        priority_queue_add(&(target->msg_waiters), &n);

        msg_t wait_data = *m;
        wait_data.sender_pid = sched_active_pid;
        sched_active_thread->wait_data = (void *) &wait_data;

        int newstatus;
        if (sched_active_thread->status == STATUS_REPLY_BLOCKED) {
            newstatus = STATUS_REPLY_BLOCKED;
        }
        else {
            newstatus = STATUS_SEND_BLOCKED;
        }

        DEBUG("msg_send(): Receiver not waiting, blocking\n");
        sched_set_status((tcb_t*) sched_active_thread, newstatus);
        eINT();
        thread_yield_higher();
        DEBUG("msg_send(): %" PRIkernel_pid " is back from send block.\n", sched_active_pid);
    }
    else {
        DEBUG("msg_send(): target is RECEIVE_BLOCKED. Using direct copying.\n");

        /* copy msg to target */
        msg_t *target_message = (msg_t *) target->wait_data;
        *target_message = *m;
        target_message->sender_pid = sched_active_pid;

        sched_set_status(target, STATUS_PENDING);
        thread_priority_t other_prio = target->priority;
        eINT();
        sched_switch(other_prio);
    }

    return 1;
}

int msg_send_to_self(const msg_t *m)
{
    unsigned state = disableIRQ();

    int res = queue_msg((tcb_t *) sched_active_thread, m, sched_active_pid);

    restoreIRQ(state);
    return res;
}

int msg_send_int(const msg_t *m, kernel_pid_t target_pid)
{
#if DEVELHELP
    if (!pid_is_valid(target_pid)) {
        DEBUG("msg_send_int(): target thread %" PRIkernel_pid " does not exist\n", target_pid);
    }
#endif /* DEVELHELP */

    tcb_t *target = (tcb_t *) sched_threads[target_pid];

    if (target == NULL) {
        DEBUG("msg_send_int(): target thread %" PRIkernel_pid " does not exist\n", target_pid);
        return -1;
    }

    if (target->status == STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send_int(): target is RECEIVE_BLOCKED. Using direct copying from"
              " %" PRIkernel_pid " to %" PRIkernel_pid ".\n",
              sched_active_pid, target_pid);

        /* copy msg to target */
        msg_t *target_message = (msg_t*) target->wait_data;
        *target_message = *m;
        target_message->sender_pid = target_pid;

        sched_set_status(target, STATUS_PENDING);
        sched_context_switch_request = 1;
        return 1;
    }
    else {
        DEBUG("msg_send_int(): Receiver %" PRIkernel_pid " not waiting, queuing message.\n", target_pid);
        return queue_msg(target, m, target_pid);
    }
}

int msg_send_receive(const msg_t *m, msg_t *reply, kernel_pid_t target_pid)
{
    dINT();
    tcb_t *me = (tcb_t*) sched_threads[sched_active_pid];
    sched_set_status(me, STATUS_REPLY_BLOCKED);
    me->wait_data = (void*) reply;

    /* msg_send blocks until reply received */

    return msg_send(m, target_pid);
}

int msg_reply(const msg_t *m, msg_t *reply)
{
    unsigned state = disableIRQ();

    tcb_t *target = (tcb_t*) sched_threads[m->sender_pid];

    if (!target) {
        DEBUG("msg_reply(): %" PRIkernel_pid ": Target %" PRIkernel_pid " not existing...dropping msg!\n",
              sched_active_pid, m->sender_pid);
        return -1;
    }

    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("msg_reply(): %" PRIkernel_pid ": Target %" PRIkernel_pid " not waiting for reply.",
              sched_active_pid, m->sender_pid);
        restoreIRQ(state);
        return -1;
    }

    DEBUG("msg_reply(): %" PRIkernel_pid ": Direct msg copy to %" PRIkernel_pid "\n",
          sched_active_pid, m->sender_pid);
    /* copy msg to target */
    msg_t *target_message = (msg_t*) target->wait_data;
    *target_message = *reply;
    target_message->sender_pid = sched_active_pid;

    sched_set_status(target, STATUS_PENDING);
    thread_priority_t target_prio = target->priority;
    restoreIRQ(state);
    sched_switch(target_prio);

    return 1;
}

int msg_reply_int(const msg_t *m, msg_t *reply)
{
    tcb_t *target = (tcb_t*) sched_threads[m->sender_pid];

    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("msg_reply_int(): %" PRIkernel_pid ": Target %" PRIkernel_pid " not waiting for reply.",
              sched_active_pid, m->sender_pid);
        return -1;
    }

    msg_t *target_message = (msg_t*) target->wait_data;
    *target_message = *reply;
    target_message->sender_pid = m->sender_pid;

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

    tcb_t *me = (tcb_t*) sched_active_thread;
    DEBUG("_msg_receive(): Thread %" PRIkernel_pid " wants a message.\n", me->pid);

    int queue_index = -1;

    if (me->msg_array) {
        queue_index = cib_get(&(me->msg_queue));
    }

    /* no message, fail */
    if ((!block) && (queue_index == -1)) {
        DEBUG("_msg_receive(): no message, not blocking. Done.\n");
        eINT();
        return -1;
    }

    if (queue_index >= 0) {
        DEBUG("_msg_receive(): Got queued message.\n");
        *m = me->msg_array[queue_index];
    }
    else {
        me->wait_data = (void *) m;
    }

    priority_queue_node_t *node = priority_queue_remove_head(&(me->msg_waiters));

    if (node == NULL) {
        if (queue_index < 0) {
            DEBUG("_msg_receive(): No queued message. No thread in waiting list. Going blocked.\n");
            sched_set_status(me, STATUS_RECEIVE_BLOCKED);

            eINT();
            thread_yield_higher();

            /* sender copied message */
            DEBUG("_msg_receive(): %" PRIkernel_pid " was unblocked by receiving a message. Done.\n", sched_active_pid);
        }
        else {
            DEBUG("_msg_receive(): No thread in waiting list. Done.\n");
            eINT();
        }

        return 1;
    }
    else {
        tcb_t *sender = (tcb_t*) node->data;

        if (queue_index >= 0) {
            /* We've already got a message from the queue. As there is a
             * waiter, take it's message into the just freed queue space.
             */
            DEBUG("_msg_receive(): storing message from %" PRIkernel_pid " in our message queue\n",
                  sender->pid);
            m = &(me->msg_array[cib_put(&(me->msg_queue))]);
        }

        /* copy msg */
        msg_t *sender_msg = (msg_t*) sender->wait_data;
        *m = *sender_msg;

        /* remove sender from queue */
        if (sender->status != STATUS_REPLY_BLOCKED) {
            DEBUG("_msg_receive(): Got message. Waking up sender %" PRIkernel_pid ". Done.\n",
                  sender->pid);
            sender->wait_data = NULL;
            sched_set_status(sender, STATUS_PENDING);
        }
        else {
            DEBUG("_msg_receive(): Got message. Waiting sender %" PRIkernel_pid " is reply blocked. Done.\n",
                  sender->pid);
        }

        eINT();
        return 1;
    }
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
