/*
 * Copyright (C) 2015 Daniel Krebs
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_lwmac
 * @file
 * @brief       Wrapper for priority_queue that holds gnrc_pktsnip_t* and is
 *              aware of it's length.
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @}
 */

#ifndef LWMAC_PACKET_QUEUE_H
#define LWMAC_PACKET_QUEUE_H

#include <stdint.h>
#include <priority_queue.h>
#include <net/gnrc/pkt.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef priority_queue_node_t packet_queue_node_t;

/* TODO: Description */
typedef struct {
    priority_queue_t queue;
    uint32_t length;
    packet_queue_node_t* buffer;
    size_t buffer_size;
} packet_queue_t;


static inline uint32_t packet_queue_length(packet_queue_t *q)
{
    return (q ? q->length : 0);
}

void packet_queue_flush(packet_queue_t* q);

/* Get first element and remove it from queue */
gnrc_pktsnip_t* packet_queue_pop(packet_queue_t* q);

/* Get first element without removing */
gnrc_pktsnip_t* packet_queue_head(packet_queue_t* q);

packet_queue_node_t* packet_queue_push(packet_queue_t* q,
                                       gnrc_pktsnip_t* snip,
                                       uint32_t priority);

void packet_queue_init(packet_queue_t* q,
                       packet_queue_node_t buffer[],
                       size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif /* LWMAC_PACKET_QUEUE_H */
