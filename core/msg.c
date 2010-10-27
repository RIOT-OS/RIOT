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
#include "scheduler.h"
#include "msg.h"
#include "queue.h"
#include "tcb.h"
#include <stddef.h>
#include <irq.h>

#include "flags.h"

//#define ENABLE_DEBUG
#include "debug.h"

int msg_send(msg* m, unsigned int target_pid, bool block) {
    if (inISR()) {
        return msg_send_int(m, target_pid);
    }

    int result = 1;

    tcb *target = (tcb*)fk_threads[target_pid];

    m->sender_pid = fk_pid;
    if (m->sender_pid == target_pid) return -1;

    dINT();

    if (target == NULL) {
        eINT();
        return -1;
    }

    if (target->status !=  STATUS_RECEIVE_BLOCKED) {
        if (! block ) {
            DEBUG("%s: receiver not waiting. block=%u\n", fk_thread->name, block);
            eINT();
            return 0;
        }

        DEBUG("%s: send_blocked.\n", fk_thread->name);
        queue_node_t n;
        n.priority = fk_thread->priority;
        n.data = (unsigned int) fk_thread;
        DEBUG("%s: Adding node to msg_queue:\n", fk_thread->name);

        queue_priority_add(&(target->msg_queue), &n);

        fk_thread->wait_data = (void*) m;

        int newstatus;
        if (fk_thread->status == STATUS_REPLY_BLOCKED) {
            newstatus = STATUS_REPLY_BLOCKED;
        } else {
            newstatus = STATUS_SEND_BLOCKED;
        }

        sched_set_status((tcb*)fk_thread,  newstatus);
        
        DEBUG("%s: back from send block.\n", fk_thread->name);
    } else {
        DEBUG("%s: direct msg copy.\n", fk_thread->name);
        /* copy msg to target */
        msg* target_message = (msg*)target->wait_data;
        *target_message = *m;
        sched_set_status(target,  STATUS_PENDING);
    }

    eINT();
    fk_yield();

    return result;
}

int msg_send_int(msg* m, unsigned int target_pid) {
    tcb *target = (tcb*)fk_threads[target_pid];

    if (target->status ==  STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send_int: direct msg copy.\n");

        m->sender_pid = target_pid;

        /* copy msg to target */
        msg* target_message = (msg*)target->wait_data;
        *target_message = *m;
        sched_set_status(target,  STATUS_PENDING);

        fk_context_switch_request = 1;
        return 1;
    } else {
        DEBUG("msg_send_int: receiver not waiting.\n");
        return 0;
    }

}

int msg_send_receive(msg *m, msg *reply, unsigned int target_pid) {
    dINT();
    tcb *me = (tcb*) fk_threads[fk_pid];
    sched_set_status(me,  STATUS_REPLY_BLOCKED);
    me->wait_data = (void*) reply;
    msg_send(m, target_pid, true);

    /* msg_send blocks until reply received */
    
    return 1;
}

int msg_reply(msg *m, msg *reply) {
    int state = disableIRQ();

    tcb *target = (tcb*)fk_threads[m->sender_pid];
    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("%s: msg_reply(): target \"%s\" not waiting for reply.", fk_thread->name, target->name);
        restoreIRQ(state);
        return -1;
    }
    
    DEBUG("%s: msg_reply(): direct msg copy.\n", fk_thread->name);
    /* copy msg to target */
    msg* target_message = (msg*)target->wait_data;
    *target_message = *reply;
    sched_set_status(target,  STATUS_PENDING);
    restoreIRQ(state);
    fk_yield();

    return 1;
}

int msg_reply_int(msg *m, msg *reply) {
    tcb *target = (tcb*)fk_threads[m->sender_pid];
    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("%s: msg_reply_int(): target \"%s\" not waiting for reply.", fk_thread->name, target->name);
        return -1;
    }
    msg* target_message = (msg*)target->wait_data;
    *target_message = *m;
    sched_set_status(target,  STATUS_PENDING);
    fk_context_switch_request = 1;
    return 1;
}


int msg_receive(msg* m) {
    dINT();
    DEBUG("%s: msg_receive.\n", fk_thread->name);

    tcb *me = (tcb*) fk_threads[fk_pid];

    me->wait_data = (void*) m;

    queue_node_t *n = queue_remove_head(&(me->msg_queue));

    if (n == NULL) {
        DEBUG("%s: msg_receive blocked\n", fk_thread->name);
        sched_set_status(me,  STATUS_RECEIVE_BLOCKED);

        eINT();
        fk_yield();

        /* sender copied message */
        return 1;
    } else {
        DEBUG("%s: msg_receive direct copy.\n", fk_thread->name);
        tcb *sender = (tcb*)n->data;

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
