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
 *
 * @}
 */

#include <stddef.h>
#include <inttypes.h>
#include <assert.h>
#include "sched.h"
#include "msg.h"
#include "list.h"
#include "thread.h"
#include "irq.h"
#include "cib.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "thread.h"

#ifdef MODULE_CORE_MSG

static int _msg_receive(msg_t *m, int block);
static int _msg_send(msg_t *m, kernel_pid_t target_pid, bool block, unsigned state);

static int queue_msg(thread_t *target, const msg_t *m)
{
    int n = cib_put(&(target->msg_queue));
    if (n < 0) {
        DEBUG("queue_msg(): message queue is full (or there is none)\n");
        return 0;
    }

    DEBUG("queue_msg(): queuing message\n");
    msg_t *dest = &target->msg_array[n];
    *dest = *m;
    return 1;
}

int msg_send(msg_t *m, kernel_pid_t target_pid)
{
    if (irq_is_in()) {
        return msg_send_int(m, target_pid);
    }
    if (sched_active_pid == target_pid) {
        return msg_send_to_self(m);
    }
    return _msg_send(m, target_pid, true, irq_disable());
}

int msg_try_send(msg_t *m, kernel_pid_t target_pid)
{
    if (irq_is_in()) {
        return msg_send_int(m, target_pid);
    }
    if (sched_active_pid == target_pid) {
        return msg_send_to_self(m);
    }
    return _msg_send(m, target_pid, false, irq_disable());
}

static int _msg_send(msg_t *m, kernel_pid_t target_pid, bool block, unsigned state)
{
#ifdef DEVELHELP
    if (!pid_is_valid(target_pid)) {
        DEBUG("msg_send(): target_pid is invalid, continuing anyways\n");
    }
#endif /* DEVELHELP */

    thread_t *target = (thread_t*) sched_threads[target_pid];

    m->sender_pid = sched_active_pid;

    if (target == NULL) {
        DEBUG("msg_send(): target thread does not exist\n");
        irq_restore(state);
        return -1;
    }

    thread_t *me = (thread_t *) sched_active_thread;

    DEBUG("msg_send() %s:%i: Sending from %" PRIkernel_pid " to %" PRIkernel_pid
          ". block=%i src->state=%i target->state=%i\n", RIOT_FILE_RELATIVE,
          __LINE__, sched_active_pid, target_pid,
          block, me->status, target->status);

    if (target->status != STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send() %s:%i: Target %" PRIkernel_pid " is not RECEIVE_BLOCKED.\n",
              RIOT_FILE_RELATIVE, __LINE__, target_pid);

        if (queue_msg(target, m)) {
            DEBUG("msg_send() %s:%i: Target %" PRIkernel_pid
                  " has a msg_queue. Queueing message.\n", RIOT_FILE_RELATIVE,
                  __LINE__, target_pid);
            irq_restore(state);
            if (me->status == STATUS_REPLY_BLOCKED) {
                thread_yield_higher();
            }
            return 1;
        }

        if (!block) {
            DEBUG("msg_send: %" PRIkernel_pid ": Receiver not waiting, block=%u\n",
                  me->pid, block);
            irq_restore(state);
            return 0;
        }

        DEBUG("msg_send: %" PRIkernel_pid ": going send blocked.\n",
              me->pid);

        me->wait_data = (void*) m;

        int newstatus;

        if (me->status == STATUS_REPLY_BLOCKED) {
            newstatus = STATUS_REPLY_BLOCKED;
        }
        else {
            newstatus = STATUS_SEND_BLOCKED;
        }

        sched_set_status((thread_t*) me, newstatus);

        thread_add_to_list(&(target->msg_waiters), me);

        irq_restore(state);
        thread_yield_higher();

        DEBUG("msg_send: %" PRIkernel_pid ": Back from send block.\n",
              me->pid);
    }
    else {
        DEBUG("msg_send: %" PRIkernel_pid ": Direct msg copy from %"
              PRIkernel_pid " to %" PRIkernel_pid ".\n",
              me->pid, thread_getpid(), target_pid);
        /* copy msg to target */
        msg_t *target_message = (msg_t*) target->wait_data;
        *target_message = *m;
        sched_set_status(target, STATUS_PENDING);

        irq_restore(state);
        thread_yield_higher();
    }

    return 1;
}

int msg_send_to_self(msg_t *m)
{
    unsigned state = irq_disable();

    m->sender_pid = sched_active_pid;
    int res = queue_msg((thread_t *) sched_active_thread, m);

    irq_restore(state);
    return res;
}

int msg_send_int(msg_t *m, kernel_pid_t target_pid)
{
#ifdef DEVELHELP
    if (!pid_is_valid(target_pid)) {
        DEBUG("msg_send(): target_pid is invalid, continuing anyways\n");
    }
#endif /* DEVELHELP */

    thread_t *target = (thread_t *) sched_threads[target_pid];

    if (target == NULL) {
        DEBUG("msg_send_int(): target thread does not exist\n");
        return -1;
    }

    m->sender_pid = KERNEL_PID_ISR;
    if (target->status == STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send_int: Direct msg copy from %" PRIkernel_pid " to %"
              PRIkernel_pid ".\n", thread_getpid(), target_pid);


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

int msg_send_receive(msg_t *m, msg_t *reply, kernel_pid_t target_pid)
{
    assert(sched_active_pid != target_pid);
    unsigned state = irq_disable();
    thread_t *me = (thread_t*) sched_threads[sched_active_pid];
    sched_set_status(me, STATUS_REPLY_BLOCKED);
    me->wait_data = (void*) reply;

    /* msg_send blocks until reply received */
    return _msg_send(m, target_pid, true, state);
}

int msg_reply(msg_t *m, msg_t *reply)
{
    unsigned state = irq_disable();

    thread_t *target = (thread_t*) sched_threads[m->sender_pid];
    assert(target != NULL);

    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("msg_reply(): %" PRIkernel_pid ": Target \"%" PRIkernel_pid
              "\" not waiting for reply.", sched_active_thread->pid, target->pid);
        irq_restore(state);
        return -1;
    }

    DEBUG("msg_reply(): %" PRIkernel_pid ": Direct msg copy.\n",
          sched_active_thread->pid);
    /* copy msg to target */
    msg_t *target_message = (msg_t*) target->wait_data;
    *target_message = *reply;
    sched_set_status(target, STATUS_PENDING);
    uint16_t target_prio = target->priority;
    irq_restore(state);
    sched_switch(target_prio);

    return 1;
}

int msg_reply_int(msg_t *m, msg_t *reply)
{
    thread_t *target = (thread_t*) sched_threads[m->sender_pid];

    if (target->status != STATUS_REPLY_BLOCKED) {
        DEBUG("msg_reply_int(): %" PRIkernel_pid ": Target \"%" PRIkernel_pid
              "\" not waiting for reply.", sched_active_thread->pid, target->pid);
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
    unsigned state = irq_disable();
    DEBUG("_msg_receive: %" PRIkernel_pid ": _msg_receive.\n",
          sched_active_thread->pid);

    thread_t *me = (thread_t*) sched_threads[sched_active_pid];

    int queue_index = -1;

    if (me->msg_array) {
        queue_index = cib_get(&(me->msg_queue));
    }

    /* no message, fail */
    if ((!block) && ((!me->msg_waiters.next) && (queue_index == -1))) {
        irq_restore(state);
        return -1;
    }

    if (queue_index >= 0) {
        DEBUG("_msg_receive: %" PRIkernel_pid ": _msg_receive(): We've got a queued message.\n",
              sched_active_thread->pid);
        *m = me->msg_array[queue_index];
    }
    else {
        me->wait_data = (void *) m;
    }

    list_node_t *next = list_remove_head(&me->msg_waiters);

    if (next == NULL) {
        DEBUG("_msg_receive: %" PRIkernel_pid ": _msg_receive(): No thread in waiting list.\n",
              sched_active_thread->pid);

        if (queue_index < 0) {
            DEBUG("_msg_receive(): %" PRIkernel_pid ": No msg in queue. Going blocked.\n",
                  sched_active_thread->pid);
            sched_set_status(me, STATUS_RECEIVE_BLOCKED);

            irq_restore(state);
            thread_yield_higher();

            /* sender copied message */
        }
        else {
            irq_restore(state);
        }

        return 1;
    }
    else {
        DEBUG("_msg_receive: %" PRIkernel_pid ": _msg_receive(): Waking up waiting thread.\n",
              sched_active_thread->pid);

        thread_t *sender = container_of((clist_node_t*)next, thread_t, rq_entry);

        if (queue_index >= 0) {
            /* We've already got a message from the queue. As there is a
             * waiter, take it's message into the just freed queue space.
             */
            m = &(me->msg_array[cib_put(&(me->msg_queue))]);
        }

        /* copy msg */
        msg_t *sender_msg = (msg_t*) sender->wait_data;
        *m = *sender_msg;

        /* remove sender from queue */
        uint16_t sender_prio = THREAD_PRIORITY_IDLE;
        if (sender->status != STATUS_REPLY_BLOCKED) {
            sender->wait_data = NULL;
            sched_set_status(sender, STATUS_PENDING);
            sender_prio = sender->priority;
        }

        irq_restore(state);
        if (sender_prio < THREAD_PRIORITY_IDLE) {
            sched_switch(sender_prio);
        }
        return 1;
    }

    DEBUG("This should have never been reached!\n");
}

int msg_avail(void)
{
    DEBUG("msg_available: %" PRIkernel_pid ": msg_available.\n",
          sched_active_thread->pid);

    thread_t *me = (thread_t*) sched_active_thread;

    int queue_index = -1;

    if (me->msg_array) {
        queue_index = cib_avail(&(me->msg_queue));
    }

    return queue_index;
}

void msg_init_queue(msg_t *array, int num)
{
    thread_t *me = (thread_t*) sched_active_thread;
    me->msg_array = array;
    cib_init(&(me->msg_queue), num);
}

void msg_queue_print(void)
{
    unsigned state = irq_disable();

    thread_t *thread =(thread_t *)sched_active_thread;
    cib_t *msg_queue = &thread->msg_queue;
    msg_t *msg_array = thread->msg_array;
    unsigned int i = msg_queue->read_count & msg_queue->mask;

    printf("Message queue of thread %" PRIkernel_pid "\n", thread->pid);
    printf("    size: %u (avail: %d)\n", msg_queue->mask + 1,
           cib_avail((cib_t *)msg_queue));

    for (; i != (msg_queue->write_count & msg_queue->mask);
         i = (i + 1) & msg_queue->mask) {
        msg_t *m = &msg_array[i];
        printf("    * %u: sender: %" PRIkernel_pid ", type: 0x%04" PRIu16
               ", content: %" PRIu32 " (%p)\n", i, m->sender_pid, m->type,
               m->content.value, (void *)m->content.ptr);
    }

    irq_restore(state);
}

#endif /* MODULE_CORE_MSG */
