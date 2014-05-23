/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Kaspar Schleiser
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
#include <string.h>
#include "kernel.h"
#include "sched.h"
#include "msg.h"
#include "priority_queue.h"
#include "tcb.h"
#include "irq.h"

#ifdef MODULE_MSG_QUEUE
#include "msg_queue.h"
#endif

#include "flags.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "thread.h"

static int _msg_send(unsigned int target_pid, char *buf, uint16_t size, uint16_t flags);
static int _msg_send_receive(unsigned int target_pid, char *sbuf, uint16_t ssize, char *rbuf, uint16_t rsize, uint16_t flags);
static int _msg_send_int(unsigned int target_pid, char *buf, uint16_t size, uint16_t flags);
static int _msg_receive(char *buf, uint16_t size, msg_hdr_t *hdr, uint16_t flags);
static int _msg_receive_pulse(msg_pulse_t *m, int block);

static int msg_direct_copy(msg_hdr_t *src, msg_hdr_t *dest)
{
    int bytes_to_copy = (src->size < dest->size) ? src->size : dest->size;

    DEBUG("msg_direct_copy(): copying %d bytes from %x to %x (src->size=%u dest->size=%u src->sender_pid=%d dest->sender_pid=%d)\n", bytes_to_copy, (unsigned int)src->payload, (unsigned int)dest->payload, src->size, dest->size, src->sender_pid, dest->sender_pid);

    dest->sender_pid = src->sender_pid;
    dest->flags = src->flags;

    memcpy(dest->payload, src->payload, bytes_to_copy);

    return bytes_to_copy;
}

int msg_send(unsigned int target_pid, char *buf, uint16_t size) {
    return _msg_send(target_pid, buf, size, MSG_BLOCK);
}

int msg_try_send(unsigned int target_pid, char *buf, uint16_t size) {
    return _msg_send(target_pid, buf, size, 0);
}


static int _msg_send(unsigned int target_pid, char *buf, uint16_t size, uint16_t flags) {
    return _msg_send_receive(target_pid, buf, size, NULL, 0, flags);
}

static int _msg_send_receive(unsigned int target_pid, char *sbuf, uint16_t ssize, char *rbuf, uint16_t rsize, uint16_t flags)
{
    int res = 0;
    int block = flags & MSG_BLOCK;

    if (inISR()) {
        return msg_send_int(target_pid, sbuf, ssize);
    }

    if (sched_active_pid == target_pid) {
        return msg_send_to_self(m);
    }

    dINT();

    tcb_t *target = (tcb_t*) sched_threads[target_pid];

    if (sched_active_thread->pid == target_pid) {
        return -1;
    }

    if (target == NULL) {
        DEBUG("msg_send(): target thread does not exist\n");
        eINT();
        return -1;
    }

    int state = disableIRQ();

    DEBUG("msg_send() %s:%i: (%d:\"%s\") Sending from %i to %i. block=%i src->state=%i target->state=%i\n", __FILE__, __LINE__, sched_active_pid, sched_active_thread->name, sched_active_pid, target_pid, block, sched_active_thread->status, target->status);
    DEBUG("msg_send() %s:%i: %s sbuf=%x ssize=%d rbuf=%x rsize=%d\n", __FILE__, __LINE__, sched_active_thread->name, (unsigned int)sbuf, ssize, (unsigned int)rbuf, rsize);

    msg_hdr_t m = { .sender_pid=sched_active_pid, .size=ssize, .payload=sbuf, .rbuf=rbuf, .rsize=rsize, .flags=flags };
    sched_active_thread->wait_data = (void*) &m;

    if (target->status != STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send() %s:%i: Target %i is not RECEIVE_BLOCKED.\n", __FILE__, __LINE__, target_pid);

#ifdef MODULE_MSG_QUEUE
        if (target->msg_queue) {
            if ((res = msg_queue_add(target->msg_queue, &m))) {
                if (flags & MSG_REPLY_EXPECTED) {
                    sched_set_status((tcb_t*) sched_active_thread, STATUS_REPLY_BLOCKED);
                }
                restoreIRQ(state);
                thread_yield();
                return res;
            }
        }
#endif

        if (!block) {
            DEBUG("msg_send: %s: Receiver not waiting, block=%u\n", sched_active_thread->name, block);
            restoreIRQ(state);
            return -1;
        }

        DEBUG("msg_send: %s: send_blocked.\n", sched_active_thread->name);
        priority_queue_node_t n;
        n.priority = sched_active_thread->priority;
        n.data = (unsigned int) sched_active_thread;
        n.next = NULL;
        DEBUG("msg_send: %s: Adding node to msg_waiters:\n", sched_active_thread->name);

        priority_queue_add(&(target->msg_waiters), &n);

        int newstatus;

        if (flags & MSG_REPLY_EXPECTED) {
            newstatus = STATUS_REPLY_BLOCKED;
        }
        else {
            newstatus = STATUS_SEND_BLOCKED;
        }

        sched_set_status((tcb_t*) sched_active_thread, newstatus);

        DEBUG("msg_send: %s: Back from %s block.\n", sched_active_thread->name,
                (newstatus == STATUS_SEND_BLOCKED ? "send" : "receive"));
    }
    else {
        DEBUG("msg_send: %s: Direct msg copy from %i to %i.\n", sched_active_thread->name, thread_getpid(), target_pid);

        res = msg_direct_copy(&m, (msg_hdr_t*) target->wait_data);

        sched_set_status(target, STATUS_PENDING);

        if (flags & MSG_REPLY_EXPECTED) {
            sched_set_status((tcb_t*) sched_active_thread, STATUS_REPLY_BLOCKED);
        }
    }

    restoreIRQ(state);
    thread_yield();

    return res;
}

int msg_reply(unsigned int target_pid, char *buf, uint16_t size) {
    DEBUG("msg_reply: %s: Replying to=%d buf=%x size=%d.\n", sched_active_thread->name, target_pid, (unsigned int)buf, size);

    tcb_t *target = (tcb_t*) sched_threads[target_pid];

    int res = 0;

    if (sched_active_thread->pid == target_pid) {
        return -1;
    }

    int state = disableIRQ();
    if (target->status == STATUS_REPLY_BLOCKED) {
        msg_hdr_t *target_hdr = (msg_hdr_t*) target->wait_data;
        target_hdr->payload = target_hdr->rbuf;
        target_hdr->size = target_hdr->rsize;

        DEBUG("msg_reply: %s: target buf=%x size=%d.\n", sched_active_thread->name, (unsigned int)target_hdr->payload, target_hdr->size);
        msg_hdr_t m = { .payload=buf, .size=size, .sender_pid=sched_active_thread->pid };

        res = msg_direct_copy(&m, target_hdr);

        sched_set_status(target, STATUS_PENDING);
    }

    restoreIRQ(state);
    thread_yield();

    return res;
}

int msg_send_int(unsigned int target_pid, char *buf, uint16_t size) {
    return _msg_send_int(target_pid, buf, size, 0);
}

static int _msg_send_int(unsigned int target_pid, char *buf, uint16_t size, uint16_t flags)
{
    tcb_t *target = (tcb_t *) sched_threads[target_pid];

    int res = 0;
    msg_hdr_t src = { .sender_pid = 0, .size = size, .payload = buf, .flags = flags };

    if (target->status == STATUS_RECEIVE_BLOCKED) {
        DEBUG("msg_send_int_raw: Direct msg copy from %i to %i.\n", thread_getpid(), target_pid);

        msg_hdr_t *dest = target->wait_data;

        res = msg_direct_copy(&src, dest);

        sched_set_status(target, STATUS_PENDING);

        sched_context_switch_request = 1;
        return res;
    }
    else {
#ifdef MODULE_MSG_QUEUE
        if (target->msg_queue) {
            if ((res = msg_queue_add(target->msg_queue, &src))) {
                return res;
            }
        }
#endif
        DEBUG("msg_send_int_raw: Receiver not waiting.\n");
        return -1;
    }
}

int msg_receive(char* buf, uint16_t size, msg_hdr_t *hdr) {
    return _msg_receive(buf, size, hdr, MSG_BLOCK);
}

int msg_try_receive(char* buf, uint16_t size, msg_hdr_t *hdr) {
    return _msg_receive(buf, size, hdr, 0);
}

static int _msg_receive(char* buf, uint16_t size, msg_hdr_t *hdr, uint16_t flags)
{
    DEBUG("_msg_receive(): %s: buf=%x size=%d\n", sched_active_thread->name, (unsigned int)buf, size);

    int res = 0;
    int state = disableIRQ();

    tcb_t *me = (tcb_t*) sched_threads[sched_active_pid];
    msg_hdr_t m = { .payload=buf, .size=size, .flags=flags };

#ifdef MODULE_MSG_QUEUE
    if (sched_active_thread->msg_queue) {
        res = msg_queue_get(sched_active_thread->msg_queue, &m);
        if (res) {
            restoreIRQ(state);
            return res;
        }
    }
#endif

    me->wait_data = (void *) &m;

    priority_queue_node_t *node = priority_queue_remove_head(&(me->msg_waiters));

    if (node == NULL) {
        if (flags & MSG_BLOCK) {
            DEBUG("_msg_receive(): %s: No thread in waiting list. Going Blocked.\n", sched_active_thread->name);

            sched_set_status(me, STATUS_RECEIVE_BLOCKED);

            restoreIRQ(state);
            thread_yield();

            /* Once we're back here, the sender copied a message,
             * and we're in STATUS_PENDING. */

            res = m.size;
        }
        else {
            DEBUG("_msg_receive: %s: No thread in waiting list. Called without flags & MSG_BLOCK.\n", sched_active_thread->name);
            restoreIRQ(state);
        }
    }
    else {
        DEBUG("_msg_receive: %s: Waking up waiting thread.\n", sched_active_thread->name);
        tcb_t *sender = (tcb_t*) node->data;

        /* copy msg */
        res = msg_direct_copy((msg_hdr_t*) sender->wait_data, &m);

        /* remove sender from queue */
        if (sender->status != STATUS_REPLY_BLOCKED) {
            sender->wait_data = NULL;
            sched_set_status(sender, STATUS_PENDING);
        }

        restoreIRQ(state);
    }

    /* save message info if caller supplied space for it */
    if (hdr) {
        *hdr = m;
    }

    return res;
}

int msg_send_pulse(msg_pulse_t *m, unsigned int target_pid)
{
    return _msg_send(target_pid, (char*) m, sizeof(msg_pulse_t), MSG_BLOCK | MSG_IS_PULSE);
}

int msg_try_send_pulse(msg_pulse_t *m, unsigned int target_pid)
{
    return _msg_send(target_pid, (char*) m, sizeof(msg_pulse_t), MSG_IS_PULSE);
}

int msg_send_pulse_int(msg_pulse_t *m, unsigned int target_pid)
{
    return _msg_send_int(target_pid, (char*) m, sizeof(msg_pulse_t), MSG_IS_PULSE);
}

int msg_send_receive_pulse(msg_pulse_t *m, msg_pulse_t *reply, unsigned int target_pid)
{
    return _msg_send_receive(target_pid, (char*) m, sizeof(msg_pulse_t), (char*) reply, sizeof(msg_pulse_t), MSG_IS_PULSE | MSG_REPLY_EXPECTED | MSG_BLOCK);
}

int msg_reply_pulse(msg_pulse_t *m, msg_pulse_t *reply)
{
    DEBUG("msg_reply_pulse: %s: Replying from %d to %d.\n", sched_active_thread->name, sched_active_thread->pid, m->sender_pid);
    return msg_reply(m->sender_pid, (char*)reply, sizeof(msg_pulse_t));
}

int msg_try_receive_pulse(msg_pulse_t *m)
{
    return _msg_receive_pulse(m, 0);
}

int msg_receive_pulse(msg_pulse_t *m)
{
    return _msg_receive_pulse(m, 1);
}

static int _msg_receive_pulse(msg_pulse_t *m, int block)
{
    int res = 0;

    msg_hdr_t hdr;

    res = _msg_receive((char*) m, sizeof(msg_pulse_t), &hdr, MSG_IS_PULSE | (block ? MSG_BLOCK : 0));
    m->sender_pid = hdr.sender_pid;

    if (! (hdr.flags & MSG_IS_PULSE)) {
        res = 0;
    }

    return res;
}
