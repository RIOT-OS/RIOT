/*
 * Copyright (C) 2014 Kaspar Schleiser
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
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "msg.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

int msg_queue_init(msg_queue_t *queue, char *buf, int size, int maxsize) {
    DEBUG("%s:l%d: initializing msg queue (buf=%x, size=%d, maxsize=%d)\n",
            __FILE__, __LINE__, (unsigned int) buf, size, maxsize);
    ringbuffer_init(&queue->rb, buf, size);
    queue->maxsize = maxsize;

    return 0;
}

int msg_queue_add(msg_queue_t *queue, msg_hdr_t *hdr) {
    DEBUG("%s:l%d: queueing msg. hdr->size=%d rb->avail=%d\n", __FILE__, __LINE__, hdr->size, ringbuffer_get_free(&queue->rb));

    int bytes_to_copy = hdr->size;
    if (queue->maxsize) {
        bytes_to_copy = (hdr->size < queue->maxsize) ? hdr->size : queue->maxsize;
    }

    if (ringbuffer_get_free(&queue->rb) < sizeof(msg_hdr_t) + bytes_to_copy) {
        return -1;
    }

    ringbuffer_add(&queue->rb, (char*)hdr, sizeof(msg_hdr_t));
    ringbuffer_add(&queue->rb, hdr->payload, bytes_to_copy);

    DEBUG("%s:l%d: Queued %db (from %x).\n", __FILE__, __LINE__, bytes_to_copy, (unsigned int) hdr->payload);

    return bytes_to_copy;
}

int msg_queue_get(msg_queue_t *queue, msg_hdr_t *dest) {
    msg_hdr_t hdr;
    int res = 0;
    if (ringbuffer_get(&queue->rb, (char*) &hdr, sizeof(msg_hdr_t))) {
    DEBUG("%s:l%d: unqueueing msg. dest->sender_pid=%d dest->payload=%x dest->size=%d\n",
            __FILE__, __LINE__, dest->sender_pid, (unsigned int)dest->payload, dest->size);
        res = (hdr.size < dest->size) ? hdr.size : dest->size;

        dest->sender_pid = hdr.sender_pid;
        dest->flags = hdr.flags;

        ringbuffer_get(&queue->rb, dest->payload, res);
        int leftover = hdr.size - res;
        if (leftover > 0) {
            ringbuffer_remove(&queue->rb, leftover);
        }
    }

    return res;
}

