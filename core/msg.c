/**
 * kernel messaging implementation
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup kernel_msg
 * @{
 * @file
 * @author Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @author Oliver Hahm <oliver.hahm@inria.fr>
 * @author Christian Mehlis <christian.mehlis@fu-berlin.de>
 * @author René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include <stddef.h>

#include "cib.h"
#include "flags.h"
#include "irq.h"
#include "kernel.h"
#include "sched.h"
#include "msg.h"
#include "queue.h"
#include "sched.h"
#include "tcb.h"
#include "thread.h"
#include "hwtimer.h"
#include "swtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define MICROS_IN_S (1000*1000)

static int queue_msg(tcb_t *target, msg_t *m)
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

    tcb_t *target = (tcb_t*) sched_threads[target_pid];

    m->sender_pid = thread_pid;

    if (m->sender_pid == target_pid) {
        return -1;
    }

    if (target == NULL) {
        return -1;
    }

    dINT();

    if (target->status != STATUS_RECEIVE_BLOCKED) {
        if (target->msg_array && queue_msg(target, m)) {
            eINT();
            return 1;
        }

        if (!block) {
            DEBUG("msg_send: %s: Receiver not waiting, block=%u\n", active_thread->name, block);
            eINT();
            return 0;
        }

        DEBUG("msg_send: %s: send_blocked.\n", active_thread->name);
        queue_node_t n;
        n.priority = active_thread->priority;
        n.data = (unsigned int) active_thread;
        n.next = NULL;
        DEBUG("msg_send: %s: Adding node to msg_waiters:\n", active_thread->name);

        queue_priority_add(&(target->msg_waiters), &n);

        active_thread->wait_data = (void*) m;

        int newstatus;

        if (active_thread->status == STATUS_REPLY_BLOCKED) {
            newstatus = STATUS_REPLY_BLOCKED;
        } else {
            newstatus = STATUS_SEND_BLOCKED;
        }

        sched_set_status((tcb_t*) active_thread, newstatus);

        DEBUG("msg_send: %s: Back from send block.\n", active_thread->name);
    } else {
        DEBUG("msg_send: %s: Direct msg copy from %i to %i.\n", active_thread->name, thread_getpid(), target_pid);
        /* copy msg to target */
        msg_t *target_message = (msg_t*) target->wait_data;
        *target_message = *m;
        sched_set_status(target, STATUS_PENDING);
    }

    eINT();
    thread_yield();

    return 1;
}

int msg_send_int(msg_t *m, unsigned int target_pid)
{
    tcb_t *target = (tcb_t *) sched_threads[target_pid];

    if (target->status == STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send_int: Direct msg copy from %i to %i.\n", thread_getpid(), target_pid);

        m->sender_pid = target_pid;

        /* copy msg to target */
        msg_t *target_message = (msg_t*) target->wait_data;
        *target_message = *m;
        sched_set_status(target, STATUS_PENDING);

        sched_context_switch_request = 1;
        return 1;
    } else {
        DEBUG("msg_send_int: Receiver not waiting.\n");
        return (queue_msg(target, m));
    }
}

int msg_send_receive(msg_t *m, msg_t *reply, unsigned int target_pid)
{
    dINT();
    tcb_t *me = (tcb_t*) sched_threads[thread_pid];
    sched_set_status(me, STATUS_REPLY_BLOCKED);
    me->wait_data = (void*) reply;

    /* msg_send blocks until reply received */

    return msg_send(m, target_pid, true);
}

int msg_reply(msg_t *m, msg_t *reply)
{
    int state = disableIRQ();

    tcb_t *target = (tcb_t*) sched_threads[m->sender_pid];

    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("msg_reply(): %s: Target \"%s\" not waiting for reply.", active_thread->name, target->name);
        restoreIRQ(state);
        return -1;
    }

    DEBUG("msg_reply(): %s: Direct msg copy.\n", active_thread->name);
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
    tcb_t *target = (tcb_t*) sched_threads[m->sender_pid];

    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("msg_reply_int(): %s: Target \"%s\" not waiting for reply.", active_thread->name, target->name);
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
    return msg_receive_timeout(m, 0);
}

int msg_receive(msg_t *m)
{
    return msg_receive_timeout(m, -1);
}

int msg_receive_timeout(msg_t *m, int timeout_us)
{
    dINT();
    DEBUG("_msg_receive: %s: _msg_receive.\n", active_thread->name);

    tcb_t *me = (tcb_t*) sched_threads[thread_pid];

    /* -1 means: we have no msg in our inbox
     * >=0 we have a new msg
     */
    int msg_index = -1;
    if (me->msg_array) {
        msg_index = cib_get(&(me->msg_queue));
    }

    /* there could be a thread waiting to send us a message */
    queue_node_t *waiter_node = queue_remove_head(&(me->msg_waiters));

    if ((waiter_node == NULL) && (msg_index >= 0)) {
        /* we got a message in our inbox AND no one is waiting for sending us a message */
        DEBUG("%s: msg_receive(): We've got a queued message.\n", active_thread->name);
        *m = me->msg_array[msg_index];
        return 1;
    } else if ((waiter_node == NULL) && (msg_index < 0)) {
        /* we don't have a message AND no one is waiting to send us a message, so sleep */

        if (timeout_us == 0) {
            /* we are non-blocking */
            return -1;
        }

        swtimer_t timer;
        if (timeout_us > 0) {
            /* user wants a timeout, we start a timer for waking us up */
            swtimer_set_msg(&timer, HWTIMER_TICKS(timeout_us), thread_getpid(),
                    &timer);
        }

        /* when a sender wants to send us a message, write it into m */
        me->wait_data = (void *) m;

        DEBUG("%s: msg_receive(): No msg in queue. Going blocked.\n", active_thread->name);
        sched_set_status(me, STATUS_RECEIVE_BLOCKED);

        eINT();
        thread_yield();

        if ((timeout_us > 0) && (m->type == MSG_TIMER)
                && (m->content.ptr == (char *) &timer)) {
            DEBUG("%s: the timer sent us the wake-up signal.\n", active_thread->name);
            return -1;
        }

        /* sender copied message */
        return 1;
    } else /*if (waiter_node != NULL)*/{
        /* we got a message AND someone is blocked because he wants to send us a message */
        DEBUG("%s: msg_receive(): Wakeing up waiting thread.\n", active_thread->name);
        tcb_t *sender = (tcb_t*) waiter_node->data;

        if (msg_index >= 0) {
            /* copy inbox message into m*/
            DEBUG("%s: msg_receive(): We've got a queued message.\n", active_thread->name);
            *m = me->msg_array[msg_index];

            /* copy the message of the blocked thread into our inbox, b/c there is a free slot now */
            m = &(me->msg_array[cib_put(&(me->msg_queue))]);
        } else {
            /* we had no message in our inbox, but sender has a message ready for us */
        }

        /* copy msg into our inbox */
        msg_t *sender_msg = (msg_t*) sender->wait_data;
        *m = *sender_msg;

        if (sender->status & STATUS_SEND_BLOCKED) {
            /* remove sender from queue */
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
        tcb_t *me = (tcb_t*) active_thread;
        me->msg_array = array;
        cib_init(&(me->msg_queue), num);
        return 0;
    }

    return -1;
}
