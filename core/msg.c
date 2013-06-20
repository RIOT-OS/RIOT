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
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @}
 */

#include "kernel.h"
#include "sched.h"
#include "msg.h"
#include "queue.h"
#include "tcb.h"
#include <stddef.h>
#include <irq.h>
#include <cib.h>

#include "flags.h"

//#define ENABLE_DEBUG
#include "debug.h"

static int queue_msg(tcb_t *target, msg_t *m)
{
    int n = cib_put(&(target->msg_queue));

    if(n != -1) {
        target->msg_array[n] = *m;
        return 1;
    }

    return 0;
}

int msg_send(msg_t *m, unsigned int target_pid, bool block)
{
    if(inISR()) {
        return msg_send_int(m, target_pid);
    }

    tcb_t *target = (tcb_t*) sched_threads[target_pid];

    m->sender_pid = thread_pid;

    if(m->sender_pid == target_pid) {
        return -1;
    }

    if(target == NULL) {
        return -1;
    }

    dINT();

    if(target->status !=  STATUS_RECEIVE_BLOCKED) {
        if(target->msg_array && queue_msg(target, m)) {
            eINT();
            return 1;
        }

        if(!block) {
            DEBUG("%s: receiver not waiting. block=%u\n", active_thread->name, block);
            eINT();
            return 0;
        }

        DEBUG("%s: send_blocked.\n", active_thread->name);
        queue_node_t n;
        n.priority = active_thread->priority;
        n.data = (unsigned int) active_thread;
        DEBUG("%s: Adding node to msg_waiters:\n", active_thread->name);

        queue_priority_add(&(target->msg_waiters), &n);

        active_thread->wait_data = (void*) m;

        int newstatus;

        if(active_thread->status == STATUS_REPLY_BLOCKED) {
            newstatus = STATUS_REPLY_BLOCKED;
        }
        else {
            newstatus = STATUS_SEND_BLOCKED;
        }

        sched_set_status((tcb_t*) active_thread,  newstatus);

        DEBUG("%s: back from send block.\n", active_thread->name);
    }
    else {
        DEBUG("%s: direct msg copy.\n", active_thread->name);
        /* copy msg to target */
        msg_t *target_message = (msg_t*) target->wait_data;
        *target_message = *m;
        sched_set_status(target,  STATUS_PENDING);
    }

    eINT();
    thread_yield();

    return 1;
}

int msg_send_int(msg_t *m, unsigned int target_pid)
{
    tcb_t *target = (tcb_t *) sched_threads[target_pid];

    if(target->status ==  STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send_int: direct msg copy from %i to %i.\n", thread_getpid(), target_pid);

        m->sender_pid = target_pid;

        /* copy msg to target */
        msg_t *target_message = (msg_t*) target->wait_data;
        *target_message = *m;
        sched_set_status(target,  STATUS_PENDING);

        sched_context_switch_request = 1;
        return 1;
    }
    else {
        DEBUG("msg_send_int: receiver not waiting.\n");
        return (queue_msg(target, m));
    }
}

int msg_send_receive(msg_t *m, msg_t *reply, unsigned int target_pid)
{
    dINT();
    tcb_t *me = (tcb_t*) sched_threads[thread_pid];
    sched_set_status(me,  STATUS_REPLY_BLOCKED);
    me->wait_data = (void*) reply;
    msg_send(m, target_pid, true);

    /* msg_send blocks until reply received */

    return 1;
}

int msg_reply(msg_t *m, msg_t *reply)
{
    int state = disableIRQ();

    tcb_t *target = (tcb_t*) sched_threads[m->sender_pid];

    if(target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("%s: msg_reply(): target \"%s\" not waiting for reply.", active_thread->name, target->name);
        restoreIRQ(state);
        return -1;
    }

    DEBUG("%s: msg_reply(): direct msg copy.\n", active_thread->name);
    /* copy msg to target */
    msg_t *target_message = (msg_t*) target->wait_data;
    *target_message = *reply;
    sched_set_status(target,  STATUS_PENDING);
    restoreIRQ(state);
    thread_yield();

    return 1;
}

int msg_reply_int(msg_t *m, msg_t *reply)
{
    tcb_t *target = (tcb_t*) sched_threads[m->sender_pid];

    if(target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("%s: msg_reply_int(): target \"%s\" not waiting for reply.", active_thread->name, target->name);
        return -1;
    }

    msg_t *target_message = (msg_t*) target->wait_data;
    *target_message = *reply;
    sched_set_status(target,  STATUS_PENDING);
    sched_context_switch_request = 1;
    return 1;
}

int msg_receive(msg_t *m)
{
    dINT();
    DEBUG("%s: msg_receive.\n", active_thread->name);

    tcb_t *me = (tcb_t*) sched_threads[thread_pid];

    int n = -1;

    if(me->msg_array) {
        n = cib_get(&(me->msg_queue));
    }

    if(n >= 0) {
        DEBUG("%s: msg_receive(): We've got a queued message.\n", active_thread->name);
        *m = me->msg_array[n];
    }
    else {
        me->wait_data = (void *) m;
    }

    queue_node_t *node = queue_remove_head(&(me->msg_waiters));

    if(node == NULL) {
        DEBUG("%s: msg_receive(): No thread in waiting list.\n", active_thread->name);

        if(n < 0) {
            DEBUG("%s: msg_receive(): No msg in queue. Going blocked.\n", active_thread->name);
            sched_set_status(me,  STATUS_RECEIVE_BLOCKED);

            eINT();
            thread_yield();

            /* sender copied message */
        }

        return 1;
    }
    else {
        DEBUG("%s: msg_receive(): Wakeing up waiting thread.\n", active_thread->name);
        tcb_t *sender = (tcb_t*) node->data;

        if(n >= 0) {
            /* we've already got a messgage from the queue.  as there is a
             * waiter, take it's message into the just freed queue space.
             */
            m = &(me->msg_array[cib_put(&(me->msg_queue))]);
        }

        /* copy msg */
        msg_t *sender_msg = (msg_t*) sender->wait_data;
        *m = *sender_msg;

        /* remove sender from queue */
        sender->wait_data = NULL;
        sched_set_status(sender,  STATUS_PENDING);

        eINT();
        return 1;
    }
}

int msg_init_queue(msg_t *array, int num)
{
    /* make sure brainfuck condition is met */
    if(num && (num & (num - 1)) == 0) {
        tcb_t *me = (tcb_t*) active_thread;
        me->msg_array = array;
        cib_init(&(me->msg_queue), num);
        return 0;
    }

    return -1;
}
