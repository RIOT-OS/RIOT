/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_priority_pktqueue
 * @{
 *
 * @file
 * @brief       gnrc priority packet queue implementation
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo <shuguo.zhuo@inria.fr>
 * @}
 */

#include <assert.h>

#include "net/gnrc/pktbuf.h"
#include "net/gnrc/priority_pktqueue.h"

/******************************************************************************/

static inline void _free_node(gnrc_priority_pktqueue_node_t *node)
{
    assert(node != NULL);

    priority_queue_node_init((priority_queue_node_t *)node);
}

/******************************************************************************/

gnrc_pktsnip_t *gnrc_priority_pktqueue_pop(gnrc_priority_pktqueue_t *queue)
{
    if (!queue || (gnrc_priority_pktqueue_length(queue) == 0)) {
        return NULL;
    }
    priority_queue_node_t *head = priority_queue_remove_head(queue);
    gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *) head->data;
    _free_node((gnrc_priority_pktqueue_node_t *)head);
    return pkt;
}

/******************************************************************************/

gnrc_pktsnip_t *gnrc_priority_pktqueue_head(gnrc_priority_pktqueue_t *queue)
{
    if (!queue || (gnrc_priority_pktqueue_length(queue) == 0)) {
        return NULL;
    }
    return (gnrc_pktsnip_t *)queue->first->data;
}
/******************************************************************************/

void gnrc_priority_pktqueue_push(gnrc_priority_pktqueue_t *queue,
                                 gnrc_priority_pktqueue_node_t *node)
{
    assert(queue != NULL);
    assert(node != NULL);
    assert(node->pkt != NULL);
    assert(sizeof(unsigned int) == sizeof(gnrc_pktsnip_t *));

    priority_queue_add(queue, (priority_queue_node_t *)node);
}

/******************************************************************************/

void gnrc_priority_pktqueue_flush(gnrc_priority_pktqueue_t *queue)
{
    assert(queue != NULL);

    if (gnrc_priority_pktqueue_length(queue) == 0) {
        return;
    }
    gnrc_priority_pktqueue_node_t *node;
    while ((node = (gnrc_priority_pktqueue_node_t *)priority_queue_remove_head(queue))) {
        gnrc_pktbuf_release(node->pkt);
        _free_node(node);
    }
}

/******************************************************************************/
uint32_t gnrc_priority_pktqueue_length(gnrc_priority_pktqueue_t *queue)
{
    assert(queue != NULL);

    uint32_t length = 0;
    priority_queue_node_t *node = queue->first;
    if (!node) {
        return length;
    }

    length++;
    while (node->next != NULL) {
        length++;
        node = node->next;
    }
    return length;
}
