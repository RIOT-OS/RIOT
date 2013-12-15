/**
 * kernel messaging implementation
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup kernel_msg
 * @{
 * @file
 * @author Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @author Oliver Hahm <oliver.hahm@inria.fr>
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
#include <inttypes.h>
#include <string.h>

#include "flags.h"

#include "thread.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static int _msg_receive(msg_t *m, int block);
static int _msg_copy_data(msg_t *src, msg_t *target);

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
        if (m->size > 0 && target->msg_array) {
            DEBUG("msg_send: %s: Receiver has a queue and msg size > 0."
                  " This is unsupported.\n", active_thread->name);
            eINT();
            return 2;
        }

        if (target->msg_array && queue_msg(target, m)) {
            eINT();
            return 1;
        }

        if (!block) {
            DEBUG("msg_send: %s: Receiver not waiting, block=%u\n", active_thread->name, block);
            eINT();
            return 0;
        }

        DEBUG("msg_send: %s: preparing send_blocked.\n", active_thread->name);
        queue_node_t n;
        n.priority = active_thread->priority;
        n.data = (unsigned int) active_thread;
        n.next = NULL;
        DEBUG("msg_send: %s: Adding node to msg_waiters.\n", active_thread->name);

        queue_priority_add(&(target->msg_waiters), &n);

        active_thread->wait_data = (void*) m;

        int newstatus;

        if (active_thread->status == STATUS_REPLY_BLOCKED) {
            newstatus = STATUS_REPLY_BLOCKED;
        }
        else {
            newstatus = STATUS_SEND_BLOCKED;
        }

        /* save content. might be overwritten by msg_receive. */
        char *saved_ptr = m->content.ptr;

        DEBUG("msg_send: %s: Going blocked.\n", active_thread->name);
        sched_set_status((tcb_t*) active_thread, newstatus);

        eINT();
        thread_yield();

        DEBUG("msg_send: %s: Back from send block.\n", active_thread->name);

        if ( m->size > 0 ) {
            int ret = m->content.value;
            DEBUG("msg_send: %s: Returning error %i.\n", active_thread->name, ret);
            m->content.ptr = saved_ptr;
            return ret;
        }
    }
    else {
        DEBUG("msg_send: %s: Direct msg copy from %i to %i.\n", active_thread->name, thread_getpid(), target_pid);
        msg_t *target_message = (msg_t*) target->wait_data;

        /* copy any extra msg data */
        if (_msg_copy_data(m, target_message)) {
            DEBUG("%s: other thread's buffer too small. Returning Error.\n", active_thread->name);
            return -2;
        }

        /* copy msg to target */
        *target_message = *m;

        sched_set_status(target, STATUS_PENDING);
        eINT();
        thread_yield();
    }

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

        if (_msg_copy_data(m, target_message)) {
            DEBUG("msg_send_int: Couldn't copy msg data.\n");
            return -2;
        }

        *target_message = *m;
        sched_set_status(target, STATUS_PENDING);

        sched_context_switch_request = 1;
        return 1;
    }
    else {
        DEBUG("msg_send_int: Receiver not waiting.\n");
        if (m->size) {
            DEBUG("msg_send_int: Queuing of msg with size>0 not supported.\n");
            return -2;
        }
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

    if (!target) {
        DEBUG("msg_reply(): %s: Target \"%" PRIu16 "\" not existing...dropping msg!\n", active_thread->name, m->sender_pid);
        return -1;
    }

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

    tcb_t *me = (tcb_t*) sched_threads[thread_pid];

    while(1) {
        int queue_index = -1;

        if (me->msg_array) {
            queue_index = cib_get(&(me->msg_queue));
        }

        /* no message, fail */
        if ((!block) && (queue_index == -1)) {
            return -1;
        }

        if (queue_index >= 0) {
            DEBUG("_msg_receive: %s: _msg_receive(): We've got a queued message.\n", active_thread->name);
            /* no need to check return. checked for buffer space above. */
            _msg_copy_data (&(me->msg_array[queue_index]), m);
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
                sched_set_status(me, STATUS_RECEIVE_BLOCKED);

                eINT();
                thread_yield();

                /* sender copied message */
            }

            return 1;
        }
        else {
            int try_again = 0;
            DEBUG("_msg_receive: %s: _msg_receive(): Checking waiting thread.\n", active_thread->name);
            tcb_t *sender = (tcb_t*) node->data;

            if (queue_index >= 0) {
                /* We've already got a message from the queue.  As there is a
                 * waiter, take it's message into the just freed queue space.
                 */
                m = &(me->msg_array[cib_put(&(me->msg_queue))]);
            }

            /* copy msg */
            msg_t *sender_msg = (msg_t*) sender->wait_data;

            if ( _msg_copy_data (sender_msg, m)) {
                /* we don't have enough space for this msg.
                 * Inform sender through his msg_t structure. */
                DEBUG("_msg_receive: %s: Informing sender.\n", active_thread->name);
                sender_msg->content.value = -2;
                try_again = 1;
            } else {
                *m = *sender_msg;
                sender_msg->content.value = 1;
            }

            /* remove sender from queue */
            sender->wait_data = NULL;

            /* wakeup sender */
            sched_set_status(sender, STATUS_PENDING);

            eINT();

            /* if we couldn't receive a message beacuse of missing/too little
             * buffer space, try again */
            if (try_again)
                continue;
            else
                return 1;
        }
    } // while(1)
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

/*
 * helper function to copy the data portion of messages
 * with extra data.
 *
 * Tries to copy src->size bytes from src->content.ptr to
 * target->content.ptr.
 *
 * returns 0 on success, -1 if src->size > target->size
 */
static int _msg_copy_data(msg_t *src, msg_t *target) {
    /* if src->size is >0, we're also sending data
     * with size=src->size and located at src->content.ptr
     */
    if (src->size > 0) {
        /* make sure receiver has enough reserved space */
        if (src->size <= target->size) {
            DEBUG("_msg_copy_data: %s: Copying %i bytes of data. (target space:%ib).\n",
                    active_thread->name, src->size, target->size);

            /* copy over data */
            memcpy(target->content.ptr, src->content.ptr, src->size);

            /* save target's buffer pointer in our structure.
             * This way, the receiver can still access it after
             * it's msg_t is overwritten by ours.
             */
            src->content.ptr = target->content.ptr;
        } else {
            DEBUG("_msg_copy_data: %s: Target buffer to small (msg size: %ib target space: %ib)\n",
                    active_thread->name, src->size, target->size);
            return -1;
        }
    }
    return 0;
}
