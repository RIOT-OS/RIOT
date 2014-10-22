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

#include "flags.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "thread.h"

static int _bmsg_send(msg_node_t *target, msg_node_t *source, char *buf, uint16_t size, uint16_t flags);
static int _bmsg_send_receive(msg_node_t *target, msg_node_t *source, char *sbuf, uint16_t ssize, char *rbuf, uint16_t rsize, uint16_t flags);
static int _bmsg_send_int(msg_node_t *target, char *buf, uint16_t size, uint16_t flags);
static int _bmsg_receive(char *buf, uint16_t size, msg_hdr_t *hdr, uint16_t flags);
static int _msg_receive(msg_t *m, int block);

static int msg_direct_copy(msg_hdr_t *src, msg_hdr_t *dest)
{
    int bytes_to_copy = (src->size < dest->size) ? src->size : dest->size;

    DEBUG("msg_direct_copy(): copying %d bytes from %x to %x (src->size=%u dest->size=%u src->sender_pid=%d dest->sender_pid=%d)\n", bytes_to_copy, (unsigned int)src->payload, (unsigned int)dest->payload, src->size, dest->size, src->sender_node ? src->sender_node->endpoint->owner->pid:0, dest->target_node->endpoint->owner->pid);

    /* copy over payload */
    memcpy(dest->payload, src->payload, bytes_to_copy);

    /* now copy over header */
    *dest = *src;

    dest->size = bytes_to_copy;

    return bytes_to_copy;
}

int bmsg_send(msg_node_t *target, msg_node_t *source, char *buf, uint16_t size) {
    return _bmsg_send(target, source, buf, size, MSG_BLOCK);
}

int bmsg_try_send(msg_node_t *target, msg_node_t *source, char *buf, uint16_t size) {
    return _bmsg_send(target, source, buf, size, 0);
}

int bmsg_send_receive(msg_node_t *target, char *sbuf, uint16_t ssize, char *rbuf, uint16_t rsize) {
    return _bmsg_send_receive(target, NULL, sbuf, ssize, rbuf, rsize, MSG_REPLY_EXPECTED | MSG_BLOCK);
}

static int _bmsg_send(msg_node_t *target, msg_node_t *source, char *buf, uint16_t size, uint16_t flags) {
    return _bmsg_send_receive(target, source, buf, size, NULL, 0, flags);
}

static int _bmsg_send_receive(msg_node_t *target, msg_node_t *source, char *sbuf, uint16_t ssize, char *rbuf, uint16_t rsize, uint16_t flags)
{
    DEBUG("_msg_send_receive()\n");
    int res = 0;
    int block = flags & MSG_BLOCK;

    if (inISR()) {
        return bmsg_send_int(target, sbuf, ssize);
    }

    if (target == NULL) {
        return -1;
    }

    tcb_t *target_tcb = (tcb_t*) target->endpoint->owner;

    if (sched_active_pid == (int)target_tcb->pid) {
        return -1;
    }

    unsigned state = disableIRQ();

    msg_node_t *msg_node = source ? source : sched_active_thread->msg_node;

    msg_hdr_t m = { .sender_node=msg_node, .target_node=target, .size=ssize, .payload=sbuf, .rbuf=rbuf, .rsize=rsize, .flags=flags };

    DEBUG("msg_send_receive():  msg_hdr_t m = { .sender_node=%x, .target_node=%x, .size=%u, .payload=%x, .rbuf=%x, .rsize=%u, .flags=%u }\n", (unsigned)msg_node, (unsigned)target, ssize, (unsigned)sbuf, (unsigned)rbuf, rsize, flags);
    msg_node->endpoint->wait_data = (void*) &m;

    if (target_tcb->status != STATUS_RECEIVE_BLOCKED) {
        DEBUG("_msg_send_receive() %s:%i: Target %i not receive_blocked.\n", __FILE__, __LINE__, target_tcb->pid);

#ifdef MODULE_MSG_QUEUE
        if (target->endpoint->msg_queue) {
            if ((res = msg_queue_add(target->endpoint->msg_queue, &m)) != -1) {
                if (flags & MSG_REPLY_EXPECTED) {
                    DEBUG("_bmsg_send_receive(): going REPLY_BLOCKED.\n");
                    sched_set_status((tcb_t*) sched_active_thread, STATUS_REPLY_BLOCKED);
                }
                restoreIRQ(state);
                thread_yield();
                if (flags & MSG_REPLY_EXPECTED) {
                    DEBUG("_bmsg_send_receive(): back from REPLY_BLOCKED.\n");
                }

                return res;
            }
        }
#endif

        if (!block) {
            DEBUG("bmsg_send: %"PRIkernel_pid": Receiver not waiting, block=%u\n", sched_active_pid, block);
            restoreIRQ(state);
            return -1;
        }

        DEBUG("bmsg_send: %"PRIkernel_pid": send_blocked.\n", sched_active_pid);

        priority_queue_node_t n;
        n.priority = sched_active_thread->priority;
        n.data = (unsigned int) &m;
        n.next = NULL;
        DEBUG("bmsg_send: %"PRIkernel_pid": Adding node %x to msg_waiters:\n", sched_active_pid, (unsigned int)msg_node);

        priority_queue_add(&(target->endpoint->msg_waiters), &n);

        int newstatus;

        if (flags & MSG_REPLY_EXPECTED) {
            newstatus = STATUS_REPLY_BLOCKED;
            DEBUG("_bmsg_send_receive(): going REPLY_BLOCKED.\n");
        }
        else {
            newstatus = STATUS_SEND_BLOCKED;
        }

        sched_set_status((tcb_t*) sched_active_thread, newstatus);
    }
    else {
        msg_hdr_t *target_hdr = (msg_hdr_t*) target_tcb->msg_node->endpoint->wait_data;
        DEBUG("bmsg_send: %"PRIkernel_pid": Direct msg copy from %x to %x.\n", sched_active_pid, (unsigned)&m, (unsigned)target_hdr);

        res = msg_direct_copy(&m, target_hdr);

        sched_set_status(target_tcb, STATUS_PENDING);

        if (flags & MSG_REPLY_EXPECTED) {
            sched_set_status((tcb_t*) sched_active_thread, STATUS_REPLY_BLOCKED);
            DEBUG("_bmsg_send_receive(): going REPLY_BLOCKED.\n");
        }
    }

    restoreIRQ(state);
    thread_yield();
    if (flags & MSG_REPLY_EXPECTED) {
        DEBUG("_bmsg_send_receive(): Back from REPLY_BLOCKED.\n");
    }

    return res;
}

static int _bmsg_reply(msg_node_t *target, char *buf, uint16_t size, uint16_t flags) {
    tcb_t *target_tcb = (tcb_t*) target->endpoint->owner;
    tcb_t *me = (tcb_t*) sched_active_thread;
    msg_node_t *msg_node = me->msg_node;

    DEBUG("bmsg_reply: %"PRIkernel_pid": Replying to=%d buf=%x size=%d.\n", sched_active_pid, target_tcb->pid, (unsigned int)buf, size);

    int res = 0;

    /* can't reply to self */
    if (msg_node == target) {
        return -1;
    }

    unsigned state = disableIRQ();
    if (target_tcb->status == STATUS_REPLY_BLOCKED) {
        msg_hdr_t *target_hdr = (msg_hdr_t*) target->endpoint->wait_data;
        target_hdr->payload = target_hdr->rbuf;
        target_hdr->size = target_hdr->rsize;

        DEBUG("bmsg_reply: %"PRIkernel_pid": target buf=%x size=%d.\n", sched_active_pid, (unsigned int)target_hdr->payload, target_hdr->size);
        msg_hdr_t m = { .payload=buf, .size=size, .sender_node=msg_node, .target_node=target, .flags=flags };

        res = msg_direct_copy(&m, target_hdr);

        sched_set_status(target_tcb, STATUS_PENDING);
    }

    restoreIRQ(state);
    thread_yield();

    return res;
}

int bmsg_reply(msg_node_t *target, char *buf, uint16_t size) {
    return _bmsg_reply(target, buf, size, 0);
}

int bmsg_send_int(msg_node_t *target, char *buf, uint16_t size) {
    return _bmsg_send_int(target, buf, size, 0);
}

static int _bmsg_send_int(msg_node_t *target, char *buf, uint16_t size, uint16_t flags)
{
    int res = 0;
    msg_hdr_t src = { .sender_node=NULL, .target_node=target, .size=size, .payload=buf, .flags=flags };

    tcb_t *target_tcb = (tcb_t*) target->endpoint->owner;

    if (target_tcb->status == STATUS_RECEIVE_BLOCKED) {
        DEBUG("bmsg_send_int_raw: Direct msg copy from isr to %i.\n", target->endpoint->owner->pid);

        msg_hdr_t *dest = target->endpoint->wait_data;

        res = msg_direct_copy(&src, dest);

        sched_set_status(target_tcb, STATUS_PENDING);

        sched_context_switch_request = 1;
        return res;
    }
    else {
#ifdef MODULE_MSG_QUEUE
        if (target->endpoint->msg_queue) {
            if ((res = msg_queue_add(target->endpoint->msg_queue, &src))) {
                return res;
            }
        }
#endif
        DEBUG("bmsg_send_int_raw: Receiver not waiting.\n");
        return -1;
    }
}

int bmsg_receive(char* buf, uint16_t size, msg_hdr_t *hdr) {
    return _bmsg_receive(buf, size, hdr, MSG_BLOCK);
}

int bmsg_try_receive(char* buf, uint16_t size, msg_hdr_t *hdr) {
    return _bmsg_receive(buf, size, hdr, 0);
}

static int _bmsg_receive(char* buf, uint16_t size, msg_hdr_t *hdr, uint16_t flags)
{
    DEBUG("_bmsg_receive(): %"PRIkernel_pid": buf=%x size=%d\n", sched_active_pid, (unsigned int)buf, size);

    int res = 0;
    unsigned state = disableIRQ();

    tcb_t *me = (tcb_t*) sched_active_thread;

    msg_node_t *msg_node = me->msg_node;

    msg_hdr_t m = { .target_node=msg_node, .payload=buf, .size=size, .flags=flags };

#ifdef MODULE_MSG_QUEUE
    if (msg_node->endpoint->msg_queue) {
        DEBUG("_bmsg_receive(): %"PRIkernel_pid": trying msg queue %x.\n", sched_active_pid, (unsigned int) msg_node->endpoint->msg_queue);
        res = msg_queue_get(msg_node->endpoint->msg_queue, &m);
        if (res != -1) {
            DEBUG("_bmsg_receive(): got msg from queue. res=%d\n", res);
            goto out;
        }
    }
#endif

    msg_node->endpoint->wait_data = (void *) &m;

    priority_queue_node_t *node = priority_queue_remove_head(&(msg_node->endpoint->msg_waiters));

    if (node == NULL) {
        if (flags & MSG_BLOCK) {
            DEBUG("_bmsg_receive(): %"PRIkernel_pid": No thread in waiting list. Going Blocked.\n", me->name);
            sched_set_status(me, STATUS_RECEIVE_BLOCKED);

            restoreIRQ(state);
            thread_yield();

            /* Once we're back here, the sender copied a message,
             * and we're in STATUS_PENDING. */

            DEBUG("_bmsg_receive(): %"PRIkernel_pid": Back from receive block.\n", me->name);
            res = m.size;
            goto safe_out;
        }
        else {
            DEBUG("_bmsg_receive: %"PRIkernel_pid": No thread in waiting list. Called without flags & MSG_BLOCK.\n", me->name);
        }
    }
    else {
        DEBUG("_bmsg_receive: %"PRIkernel_pid": Waking up waiting thread.\n", me->name);
        msg_hdr_t *hdr = (msg_hdr_t*) node->data;
        msg_node_t *sender_node = hdr->sender_node;
        tcb_t *sender = (tcb_t*) sender_node->endpoint->owner;

        /* copy msg */
        res = msg_direct_copy(hdr, &m);

        /* remove sender from queue */
        if (sender->status != STATUS_REPLY_BLOCKED) {
            sender_node->endpoint->wait_data = NULL;
            sched_set_status(sender, STATUS_PENDING);
        }
    }

#ifdef MODULE_MSG_QUEUE
out:
#endif
    restoreIRQ(state);
safe_out:
    /* save message info if caller supplied space for it */
    if (hdr) {
        *hdr = m;
    }

    return res;
}

int msg_send(msg_t *m, unsigned int target_pid)
{
    return _bmsg_send(thread_get_msg_node(target_pid), NULL, (char*) m, sizeof(msg_t), MSG_BLOCK | MSG_IS_SIMPLE);
}

int msg_try_send(msg_t *m, unsigned int target_pid)
{
    return _bmsg_send(thread_get_msg_node(target_pid), NULL, (char*) m, sizeof(msg_t), MSG_IS_SIMPLE);
}

int msg_send_int(msg_t *m, unsigned int target_pid)
{
    return _bmsg_send_int(thread_get_msg_node(target_pid), (char*) m, sizeof(msg_t), MSG_IS_SIMPLE);
}

int msg_send_receive(msg_t *m, msg_t *reply, unsigned int target_pid)
{
    return _bmsg_send_receive(thread_get_msg_node(target_pid), NULL, (char*) m, sizeof(msg_t), (char*) reply, sizeof(msg_t), MSG_IS_SIMPLE | MSG_REPLY_EXPECTED | MSG_BLOCK);
}

int msg_reply(msg_t *m, msg_t *reply)
{
    DEBUG("msg_reply: %"PRIkernel_pid": Replying from %d to %d. &reply=%x\n", sched_active_pid, sched_active_thread->pid, m->sender_pid, (unsigned)reply);
    return _bmsg_reply(thread_get_msg_node(m->sender_pid), (char*)reply, sizeof(msg_t), MSG_IS_SIMPLE);
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
    int res = 0;

    msg_hdr_t hdr;

    res = _bmsg_receive((char*) m, sizeof(msg_t), &hdr, MSG_IS_SIMPLE | (block ? MSG_BLOCK : 0));

    /* fetch sender PID. Set to current PID if sent by ISR. */
    m->sender_pid = hdr.sender_node ? hdr.sender_node->endpoint->owner->pid : sched_active_pid;

    if (! (hdr.flags & MSG_IS_SIMPLE)) {
        res = 0;
    }

    DEBUG("_msg_receive(): received \"%i\" from %i to %i. res=%i\n", m->content.value, m->sender_pid, sched_active_pid, res);
    return res;
}

void msg_node_init(msg_node_t *msg_node) {
    msg_node->endpoint = sched_active_thread->msg_node->endpoint;
}

#ifdef MODULE_MSG_QUEUE
static int msg_queue_init(msg_queue_t *queue, char *buf, int size, int maxsize) {
    DEBUG("%s:l%d: initializing msg queue (buf=%x, size=%d, maxsize=%d)\n",
            __FILE__, __LINE__, (unsigned int) buf, size, maxsize);
    ringbuffer_init(&queue->rb, buf, size);
    queue->maxsize = maxsize;

    return 0;
}

int thread_msg_queue_init(char *buf, int size, int maxsize) {
    msg_queue_t *queue = (msg_queue_t *) buf;
    buf += sizeof(msg_queue_t);
    size -= sizeof(msg_queue_t);

    msg_queue_init(queue, buf, size, maxsize);
    thread_set_msg_queue(thread_getpid(), queue);

    return 0;
}

int msg_queue_add(msg_queue_t *queue, msg_hdr_t *hdr) {
    DEBUG("%s:l%d: queueing msg. hdr->size=%d rb->avail=%d\n", __FILE__, __LINE__, hdr->size, ringbuffer_get_free(&queue->rb));

    msg_queue_hdr_t qhdr;
    int payload_size = 0;

    qhdr.flags = (char) hdr->flags;
    qhdr.sender_node = hdr->sender_node;

    if (qhdr.flags & MSG_IS_SIMPLE) {
        msg_t *msg = (msg_t*) hdr->payload;
        qhdr.fields.msg.type = msg->type;
        qhdr.fields.msg.content = msg->content;
        
        DEBUG("%s:l%d: Queueing message. Flags=%u sender_node=%x type=%d value=%d\n",
                __FILE__, __LINE__, qhdr.flags, (unsigned)qhdr.sender_node, qhdr.fields.msg.type, qhdr.fields.msg.content.value);
    }
    else {
        qhdr.fields.bmsg.target_node = hdr->target_node;

        if (qhdr.flags & MSG_REPLY_EXPECTED) {
            /* No need to copy payload into queue if the * sending thread is
             * reply blocked. */
            payload_size = 0;
        }
        else {
            payload_size = hdr->size;

            /* calculate min(payload_size, queue->maxsize) */
            if (queue->maxsize) {
                payload_size = (payload_size < queue->maxsize) ? payload_size : queue->maxsize;
            }
        }

        qhdr.fields.bmsg.size = payload_size;
    }

    /* bail out if message doesn't fit */
    if (ringbuffer_get_free(&queue->rb) < sizeof(msg_queue_hdr_t) + payload_size) {
        return -1;
    }

    ringbuffer_add(&queue->rb, (char*)&qhdr, sizeof(msg_queue_hdr_t));
    if (payload_size) {
        ringbuffer_add(&queue->rb, hdr->payload, payload_size);
    }

    DEBUG("%s:l%d: msg_queue_add() Queued %db header and %db from %x.\n", __FILE__, __LINE__, sizeof(msg_queue_hdr_t), payload_size, (unsigned int) hdr->payload);

    return payload_size;
}

int msg_queue_get(msg_queue_t *queue, msg_hdr_t *dest) {
    msg_queue_hdr_t qhdr;
    int res = 0;
    res = ringbuffer_get(&queue->rb, (char*) &qhdr, sizeof(msg_queue_hdr_t));

    if (res) {
        DEBUG("%s:l%d: Unqueued message. Flags=%u sender_node=%x\n",
                __FILE__, __LINE__, qhdr.flags, (unsigned)qhdr.sender_node);
        dest->sender_node = qhdr.sender_node;

        if (qhdr.flags & MSG_IS_SIMPLE) {
            msg_t *msg = (msg_t*) dest->payload;
            dest->size = sizeof(*msg);
            msg->type = qhdr.fields.msg.type;
            msg->content = qhdr.fields.msg.content;
            dest->target_node = sched_active_thread->msg_node;
        }
        else {
            dest->target_node = qhdr.fields.bmsg.target_node;
            dest->size = qhdr.fields.bmsg.size;

            if (qhdr.fields.bmsg.size) {
                res = ringbuffer_get(&queue->rb, dest->payload, qhdr.fields.bmsg.size);
            } 
        }

        if (qhdr.flags & MSG_REPLY_EXPECTED) {
            /* if the message was queued and the sender is reply-blocked,
             * the message payload wasn't copied into the queue. So copy it
             * over now. Also read reply data from waiting sender thread.
             */
            msg_hdr_t *sender_hdr = (msg_hdr_t*) qhdr.sender_node->endpoint->wait_data;
            if (qhdr.flags & MSG_IS_SIMPLE) {
                dest->rbuf = sender_hdr->rbuf;
                dest->rsize = sender_hdr->rsize;
            }
            else {
                msg_direct_copy(sender_hdr, dest);
            }
        }
    } 
    else {
        res = -1;
    }

    return res;
}
#endif
