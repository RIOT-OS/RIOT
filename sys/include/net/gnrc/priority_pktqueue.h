/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_GNRC_PRIORITY_PKTQUEUE_H
#define NET_GNRC_PRIORITY_PKTQUEUE_H

/**
 * @defgroup    net_gnrc_priority_pktqueue Priority packet queue for GNRC
 * @ingroup     net_gnrc
 * @brief       Wrapper for priority_queue that holds gnrc_pktsnip_t*
 * @{
 *
 * @file
 * @brief       gnrc priority packet queue API
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo <shuguo.zhuo@inria.fr>
 */

#include <stdint.h>

#include "priority_queue.h"
#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief data type for gnrc priority packet queue nodes
 */
typedef struct gnrc_priority_pktqueue_node {
    struct gnrc_priority_pktqueue_node *next;   /**< next queue node */
    uint32_t priority;                          /**< queue node priority */
    gnrc_pktsnip_t *pkt;                        /**< queue node data */
} gnrc_priority_pktqueue_node_t;

/**
 * @brief data type for gnrc priority packet queues
 */
typedef priority_queue_t gnrc_priority_pktqueue_t;

/**
 * @brief Static initializer for gnrc_priority_pktqueue_node_t.
 */
#define PRIORITY_PKTQUEUE_NODE_INIT(priority, pkt) { NULL, priority, pkt }

/**
 * @brief Static initializer for gnrc_priority_pktqueue_t.
 */
#define PRIORITY_PKTQUEUE_INIT { NULL }

/**
 * @brief   Initialize a gnrc priority packet queue node object.
 *
 * @param[out] node
 *          pre-allocated gnrc_priority_pktqueue_node_t object. Must not be NULL.
 * @param[in]  priority
 *          the priority of the gnrc packet snip
 * @param[in]  pkt
 *          the gnrc packet snip
 */
static inline void gnrc_priority_pktqueue_node_init(gnrc_priority_pktqueue_node_t *node,
                                                    uint32_t priority,
                                                    gnrc_pktsnip_t *pkt)
{
    node->next = NULL;
    node->priority = priority;
    node->pkt = pkt;
}

/**
 * @brief   Initialize a gnrc priority packet queue object.
 *
 * @param[out] queue
 *          pre-allocated gnrc_priority_pktqueue_t object. Must not be NULL.
 */
static inline void gnrc_priority_pktqueue_init(gnrc_priority_pktqueue_t *queue)
{
    gnrc_priority_pktqueue_t qn = PRIORITY_PKTQUEUE_INIT;

    *queue = qn;
}

/**
 * @brief   Get the length information of a gnrc priority packet queue object.
 *
 * @param[in] queue
 *          pre-allocated gnrc_priority_pktqueue_t object. Must not be NULL.
 * @return  the length of @p queue
 */
uint32_t gnrc_priority_pktqueue_length(gnrc_priority_pktqueue_t *queue);

/**
 * @brief flush the gnrc priority packet queue
 *
 * @param[out]  queue    the gnrc priority packet queue. Must not be NULL
 */
void gnrc_priority_pktqueue_flush(gnrc_priority_pktqueue_t *queue);

/**
 * @brief Get first element and remove it from @p queue
 *
 * @param[out]  queue   the gnrc priority packet queue. Must not be NULL
 *
 * @return              the old head
 */
gnrc_pktsnip_t *gnrc_priority_pktqueue_pop(gnrc_priority_pktqueue_t *queue);

/**
 * @brief Get first element from @p queue without removing
 *
 * @param[in]  queue    the gnrc priority packet queue. Must not be NULL
 *
 * @return              the head of @p queue
 */
gnrc_pktsnip_t *gnrc_priority_pktqueue_head(gnrc_priority_pktqueue_t *queue);

/**
 * @brief       add @p node into @p queue based on its priority
 *
 * @param[in,out]   queue   the gnrc priority packet queue. Must not be NULL
 * @param[in]       node    the node to add.
 */
void gnrc_priority_pktqueue_push(gnrc_priority_pktqueue_t *queue,
                                 gnrc_priority_pktqueue_node_t *node);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_PRIORITY_PKTQUEUE_H */
