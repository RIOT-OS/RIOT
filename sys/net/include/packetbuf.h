/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     net
 * @{
 *
 * @file        packetbuf.h
 * @brief       A simple priority based packet buffer
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mutex.h"
#include "queue.h"


/**
 * @brief   Data structure to represent a packet in the packet buffer
 *
 * @implements queue_node_t
 */
typedef struct packetbuf_queue_t {
    /**
     * @brief   next packet in queue. Not to be set by the user.
     * @internal
     */
    struct packetbuf_queue_t *next;
    void *packet_data;              /**< pointer to the packet data */
    /**
     * @brief   priority of the packet. Not to be set by the user.
     * @internal
     */
    uint32_t priority;
    volatile uint8_t processing;    /**< internal processing state, if 0
                                         this packet is allowed to be removed */
    size_t packet_len;              /**< length of the packet in byte */
} packetbuf_queue_t;

/**
 * @brief   Data structure to represent the packet buffer
 *
 * @implements queue_node_t
 */
typedef struct {
    char *buffer;                       /**< buffer store packets and metadata
                                             in */
    size_t buflen;                      /**< available space in buffer */
    size_t allocated;                   /**< allocated space in buffer */
    queue_node_t queue;                 /**< packet queue */
    mutex_t mutex;                      /**< the packet buffer's mutex */
} packetbuf_t;

/**
 * @def     PACKETBUF_DEFAULT
 *
 * @brief   Flag for packetbuf_add() and packetbuf_add_high_priority():
 *          Use default options
 */
#define PACKETBUF_DEFAULT (0)

/**
 * @def     PACKETBUF_NO_COPY
 *
 * @brief   Flag for packetbuf_add() and packetbuf_add_high_priority():
 *          Do not copy data, just add given pointer to queue
 */
#define PACKETBUF_NO_COPY (1)

/**
 * @brief   Initializes a packet buffer
 *
 * @param[out] out      Pointer to store the new packet buffer in.
 * @param[in] buffer    Memory space to use with the packet buffer.
 * @param[in] buflen    Maximum length of *buffer*
 *
 * @note    A *buflen* of n * (protocol MTU + sizeof(packetbuf_queue_t)), n > 1
 *          is recommended but not expected.
 *
 * @return  *out* on success
 * @return  NULL on wrong input (errno is set to EINVAL).
 */
packetbuf_t *packetbuf_init(packetbuf_t *out, void *buffer, size_t buflen);

/**
 * @brief   Add packet with lowest priority to packet buffer.
 *
 * @param[in,out] buf       The packet buffer.
 * @param[in] packet_data   The packet to copy into the buffer, NULL to just
 *                          allocate space (but not with `flags & PACKETBUF_NO_COPY`).
 * @param[in] packet_len    The maximum length of the packet.
 * @param[in] flags         Flags
 *
 * @see PACKETBUF_DEFAULT
 * @see PACKETBUF_NO_COPY
 *
 * @return  The pointer to the in the packet buffer.
 * @return  NULL if not enough space available (errno == ENOBUFS) or on wrong
 *          input (errno == EINVAL).
 */
void *packetbuf_add(packetbuf_t *buf, void *packet_data, size_t packet_len,
                    uint8_t flags);

/**
 * @brief   Add packet with highest priority to packet buffer.
 *
 * @param[in,out] buf       The packet buffer.
 * @param[in] packet_data   The packet to copy into the buffer, NULL to just
 *                          allocate space.
 * @param[in] packet_len    The maximum length of the packet.
 * @param[in] flags         Flags
 *
 * @see PACKETBUF_DEFAULT
 * @see PACKETBUF_NO_COPY
 *
 * @return  The pointer to *packet_data* in the ringbuffer of the packet buffer.
 * @return  NULL if not enough space available (errno == ENOBUFS) or on wrong
 *          input (errno == EINVAL).
 */
void *packetbuf_add_high_priority(packetbuf_t *buf, void *packet_data,
                                  size_t packet_len, uint8_t flags);

/**
 * @brief   Read current packet (without remove) from packet buffer.
 *
 * @param[in] buf           The packet buffer
 * @param[out] packet_data  The current packet
 *
 * @return  On success, size of the current packet.
 * @return  0, if *buf* or *packet_data* is NULL (errno is set to EINVAL).
 * @return  0, if *buf* is empty (errno is set to ENODATA).
 */
size_t packetbuf_get(packetbuf_t *buf, void **packet_data);

/**
 * @brief   Read current packet (with remove) from packet buffer.
 *
 * @param[in] buf           The packet buffer
 * @param[out] packet_data  The current packet
 *
 * @return  On success, size of the current packet.
 * @return  0, if *buf* or *packet_data* is NULL (errno is set to EINVAL).
 * @return  0, if *buf* is empty (errno is set to ENODATA).
 */
size_t packetbuf_remove(packetbuf_t *buf, void *packet_data);

/**
 * @brief   Iterates through items in the packet queue of the buffer.
 *
 * @param[in] buf   The packet buffer
 * @param[in] prev  The previous packet before the desired in the queue
 *
 * @return  the next packet after *prev*
 */
static inline packetbuf_queue_t *packetbuf_iter(packetbuf_t *buf,
                                                packetbuf_queue_t *prev)
{
    return (packetbuf_queue_t *)((prev == NULL) ? buf->queue.next : prev->next);
}

/**
 * @brief   Removes all packets in *buf* where *prossecing* is 0
 *
 * @param[in] buf   The packet buffer
 */
void packetbuf_garbagecollect(packetbuf_t *buf);
