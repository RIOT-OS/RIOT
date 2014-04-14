/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <string.h>
#include <errno.h>

#include "packetbuf.h"
#include "ringbuffer.h"
#include "queue.h"

packetbuf_t *packetbuf_init(packetbuf_t *out, void *buffer, size_t buflen)
{
    if (!out || !buffer || !buflen) {
        errno = EINVAL;
        return NULL;
    }

    out->queue.next = NULL;
    out->buffer = buffer;
    out->buflen = buflen;
    out->allocated = 0;
    mutex_init(&(out->mutex));

    return out;
}


void *_packetbuf_add(void (*add_to_queue)(queue_node_t *, queue_node_t *),
                     packetbuf_t *buf, void *packet_data, size_t packet_len,
                     uint8_t flags)
{
    packetbuf_queue_t *node;

    if (!buf || !packet_len || (!packet_data && (flags & PACKETBUF_NO_COPY))) {
        errno = EINVAL;
        return NULL;
    }

    if (sizeof(packetbuf_queue_t) + packet_len > buf->buflen) {
        errno = ENOBUFS;
        return NULL;
    }

    mutex_lock(&(buf->mutex));
    if (sizeof(packetbuf_queue_t) + packet_len > buf->buflen - buf->allocated) {
        /* just overide old data if it fits, but not enough space left */
        buf->allocated = 0;
    }

    node = (packetbuf_queue_t *)&(buf->buffer[buf->allocated]);

    buf->allocated += sizeof(packetbuf_queue_t);

    node->packet_len = packet_len;
    node->next = NULL;
    if (flags & PACKETBUF_NO_COPY) {
        node->packet_data = packet_data;
    }
    else {
        node->packet_data = &(buf->buffer[buf->allocated]);
        buf->allocated += packet_len;
    }
    node->priority = 0;

    add_to_queue((queue_node_t *)&buf->queue, (queue_node_t *)node);

    if (packet_data && !(flags & PACKETBUF_NO_COPY)) {
        memcpy(node->packet_data, packet_data, node->packet_len);
    }
    mutex_unlock(&(buf->mutex));

    return (void *)node->packet_data;
}

void *packetbuf_add_high_priority(packetbuf_t *buf, void *packet_data,
                                  size_t packet_len, uint8_t flags)
{
    return _packetbuf_add(queue_add_head, buf, packet_data, packet_len, flags);
}

void *packetbuf_add(packetbuf_t *buf, void *packet_data, size_t packet_len,
                    uint8_t flags)
{
    return _packetbuf_add(queue_add_tail, buf, packet_data, packet_len, flags);
}

size_t packetbuf_get(packetbuf_t *buf, void **packet_data)
{
    size_t packet_len;

    if (!buf || !packet_data) {
        errno = EINVAL;
        return 0;
    }

    mutex_lock(&(buf->mutex));

    if (!(buf->queue.next)) {
        errno = ENODATA;
        mutex_unlock(&(buf->mutex));
        return 0;
    }
    *packet_data = buf->queue.next->packet_data;
    packet_len = buf->queue.next->packet_len;
    mutex_unlock(&(buf->mutex));

    return packet_len;
}

size_t packetbuf_remove(packetbuf_t *buf, void *packet_data)
{
    size_t packet_len;
    packetbuf_queue_t *packet;

    if (!buf || !packet_data) {
        errno = EINVAL;
        return 0;
    }

    mutex_lock(&(buf->mutex));
    if (!(buf->queue.next)) {
        errno = ENODATA;
        mutex_unlock(&(buf->mutex));
        return 0;
    }

    packet = (packetbuf_queue_t *)queue_remove_head((queue_node_t *)&(buf->queue));

    packet_len = packet->packet_len;
    memcpy(packet_data, packet->packet_data, packet_len);

    mutex_unlock(&(buf->mutex));

    return packet_len;
}

void *packetbuf_move_right(packetbuf_t *buf, void *packet, size_t offset)
{
    packetbuf_queue_t *ptr = &buf->queue;
    int found = 0;

    if (!offset) {
        return packet;
    }

    if (!buf || !packet) {
        errno = EINVAL;
        return 0;
    }

    mutex_lock(&(buf->mutex));
    while ((ptr = ptr->next)) {
        if (ptr->packet_data == packet) {
            found = 1;
            break;
        }
    }

    if (!found) {
        errno = EADDRNOTAVAIL;
        mutex_unlock(&(buf->mutex));
        return NULL;
    }

    if (ptr->packet_len < offset) {
        errno = EINVAL;
        mutex_unlock(&(buf->mutex));
        return NULL;
    }

    ptr->packet_data = (void *)(((char *)ptr->packet_data) + offset);
    ptr->packet_len -= offset;
    mutex_unlock(&(buf->mutex));

    return ptr->packet_data;
}
