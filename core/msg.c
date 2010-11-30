/**
 * kernel messaging implementation
 *
 * Copyright (C) 2010 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
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

static int queue_msg(tcb *target, msg *m) {
        int n = cib_put(&(target->msg_queue));

        if (n != -1) {
            target->msg_array[n] = *m;
            return 1;
        }

        return 0;
}

int msg_send(msg* m, unsigned int target_pid, bool block) {
    if (inISR()) {
        return msg_send_int(m, target_pid);
    }

<<<<<<< HEAD
    int result = 1;

    tcb *target = (tcb*)sched_threads[target_pid];

    m->sender_pid = thread_pid;
    if (m->sender_pid == target_pid) return -1;
=======
    tcb *target = (tcb*)sched_threads[target_pid];
>>>>>>> master

    m->sender_pid = thread_pid;
    if (m->sender_pid == target_pid) {
        return -1;
    }

    if (target == NULL) {
        return -1;
    }

    dINT();
    if (target->status !=  STATUS_RECEIVE_BLOCKED) {
        if (target->msg_array && queue_msg(target, m)) {
            eINT();
            return 1;
        }

        if (! block ) {
            DEBUG("%s: receiver not waiting. block=%u\n", active_thread->name, block);
            eINT();
            return 0;
        }

        DEBUG("%s: send_blocked.\n", active_thread->name);
        queue_node_t n;
        n.priority = active_thread->priority;
        n.data = (unsigned int) active_thread;
<<<<<<< HEAD
        DEBUG("%s: Adding node to msg_queue:\n", active_thread->name);
=======
        DEBUG("%s: Adding node to msg_waiters:\n", active_thread->name);
>>>>>>> master

        queue_priority_add(&(target->msg_waiters), &n);

        active_thread->wait_data = (void*) m;

        int newstatus;
        if (active_thread->status == STATUS_REPLY_BLOCKED) {
            newstatus = STATUS_REPLY_BLOCKED;
        } else {
            newstatus = STATUS_SEND_BLOCKED;
        }

        sched_set_status((tcb*)active_thread,  newstatus);
        
        DEBUG("%s: back from send block.\n", active_thread->name);
    } else {
        DEBUG("%s: direct msg copy.\n", active_thread->name);
        /* copy msg to target */
        msg* target_message = (msg*)target->wait_data;
        *target_message = *m;
        sched_set_status(target,  STATUS_PENDING);
    }

    eINT();
    thread_yield();

    return 1;
}

int msg_send_int(msg* m, unsigned int target_pid) {
    tcb *target = (tcb*)sched_threads[target_pid];

    if (target->status ==  STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send_int: direct msg copy.\n");

        m->sender_pid = target_pid;

        /* copy msg to target */
        msg* target_message = (msg*)target->wait_data;
        *target_message = *m;
        sched_set_status(target,  STATUS_PENDING);

        sched_context_switch_request = 1;
        return 1;
    } else {
        DEBUG("msg_send_int: receiver not waiting.\n");
        return (queue_msg(target, m));
    }
}

int msg_send_receive(msg *m, msg *reply, unsigned int target_pid) {
    dINT();
    tcb *me = (tcb*) sched_threads[thread_pid];
    sched_set_status(me,  STATUS_REPLY_BLOCKED);
    me->wait_data = (void*) reply;
    msg_send(m, target_pid, true);

    /* msg_send blocks until reply received */
    
    return 1;
}

int msg_reply(msg *m, msg *reply) {
    int state = disableIRQ();

    tcb *target = (tcb*)sched_threads[m->sender_pid];
    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("%s: msg_reply(): target \"%s\" not waiting for reply.", active_thread->name, target->name);
        restoreIRQ(state);
        return -1;
    }
    
    DEBUG("%s: msg_reply(): direct msg copy.\n", active_thread->name);
    /* copy msg to target */
    msg* target_message = (msg*)target->wait_data;
    *target_message = *reply;
    sched_set_status(target,  STATUS_PENDING);
    restoreIRQ(state);
    thread_yield();

    return 1;
}

int msg_reply_int(msg *m, msg *reply) {
    tcb *target = (tcb*)sched_threads[m->sender_pid];
    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("%s: msg_reply_int(): target \"%s\" not waiting for reply.", active_thread->name, target->name);
        return -1;
    }
    msg* target_message = (msg*)target->wait_data;
    *target_message = *reply;
    sched_set_status(target,  STATUS_PENDING);
    sched_context_switch_request = 1;
    return 1;
}

int msg_receive(msg* m) {
    dINT();
    DEBUG("%s: msg_receive.\n", active_thread->name);

    tcb *me = (tcb*) sched_threads[thread_pid];

    int n = -1;
    if (me->msg_array) {
        n = cib_get(&(me->msg_queue));
    }

    if (n >= 0) {
        DEBUG("%s: msg_receive(): We've got a queued message.\n", active_thread->name);
        *m = me->msg_array[n];
    } else {
        me->wait_data = (void*) m;
    }

<<<<<<< HEAD
    if (n == NULL) {
        DEBUG("%s: msg_receive blocked\n", active_thread->name);
        sched_set_status(me,  STATUS_RECEIVE_BLOCKED);

        eINT();
        thread_yield();
=======
    queue_node_t *node = queue_remove_head(&(me->msg_waiters));

    if (node == NULL) {
        DEBUG("%s: msg_receive(): No thread in waiting list.\n", active_thread->name);
        if (n < 0) {
            DEBUG("%s: msg_receive(): No msg in queue. Going blocked.\n", active_thread->name);
            sched_set_status(me,  STATUS_RECEIVE_BLOCKED);
>>>>>>> master

            eINT();
            thread_yield();

            /* sender copied message */
        }
        return 1;
    } else {
<<<<<<< HEAD
        DEBUG("%s: msg_receive direct copy.\n", active_thread->name);
        tcb *sender = (tcb*)n->data;
=======
        DEBUG("%s: msg_receive(): Wakeing up waiting thread.\n", active_thread->name);
        tcb *sender = (tcb*)node->data;

        if (n >= 0) {
            /* we've already got a messgage from the queue.  as there is a
             * waiter, take it's message into the just freed queue space.
             */
            m = &(me->msg_array[cib_put(&(me->msg_queue))]);
        }
>>>>>>> master

        /* copy msg */
        msg* sender_msg = (msg*)sender->wait_data;
        *m = *sender_msg;

        /* remove sender from queue */
        sender->wait_data = NULL;
        sched_set_status(sender,  STATUS_PENDING);

        eINT();
        return 1;
    }
}

int msg_init_queue(msg* array, int num) {
    /* make sure brainfuck condition is met */
    if (num && (num & (num - 1)) == 0) {
        tcb *me = (tcb*)active_thread;
        me->msg_array = array;
        cib_init(&(me->msg_queue), num);
        return 0;
    } 
    
    return -1;
}
