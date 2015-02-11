/*
 * Copyright (C) 2014, 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    ng_pktqueue Packet Queue
 * @brief       Packet wrapper for the Priority Queue
 * @ingroup     net
 * @{
 *
 * @file
 * @brief       `ng_pktsnip_t`-centric wrapper for @ref priority_queue_t
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef NG_PKTQUEUE_H_
#define NG_PKTQUEUE_H_

#include <stdint.h>
#include <stdlib.h>

#include "net/ng_pkt.h"
#include "priority_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   data type for packet queue nodes
 *
 * @extends priority_queue_node_t
 */
typedef struct ng_pktqueue_node {
    struct ng_pktqueue_node *next;  /**< next node in queue */
    uint32_t priority;              /**< priority of the node */
    ng_pktsnip_t *data;             /**< pointer to the data */
} ng_pktqueue_node_t;

/**
 * @brief   data type for packet queues
 *
 * @extends priority_queue_t
 */
typedef struct {
    ng_pktqueue_node_t *first;  /**< first node in the queue */
} ng_pktqueue_t;

/**
 * @brief   Static initializer for ng_pktqueue_node_t
 */
#define NG_PKTQUEUE_NODE_INIT { NULL, NULL, 0 }

/**
 * @brief   Initializes a packet queue node.
 * @details For initialization of variables use NG_PKTQUEUE_NODE_INIT instead.
 *          Only use this function for dynamically allocated packet queue nodes.
 *
 * @param[out] node pre-allocated ng_pktqueue_node_t object. must not be NULL.
 */
static inline void ng_pktqueue_node_init(ng_pktqueue_node_t *node)
{
    priority_queue_node_init((priority_queue_node_t *)node);
}

/**
 * @brief Static initializer for ng_pktqueue_t.
 */
#define NG_PKTQUEUE_INIT PRIORITY_QUEUE_INIT

/**
 * @brief   Initialize a packet queue object.
 * @details For initialization of variables use NG_PKTQUEUE_INIT
 *          instead. Only use this function for dynamically allocated
 *          packet queues.
 * @param[out] queue    pre-allocated ng_pktqueue_t object, must not be NULL.
 */
static inline void ng_pktqueue_init(ng_pktqueue_t *queue)
{
    priority_queue_init((priority_queue_t *)queue);
}

/**
 * @brief get the packet queue's head without removing it
 *
 * @param[out] queue    the queue
 *
 * @return              the head
 */
static inline ng_pktqueue_node_t *ng_pktqueue_get_head(ng_pktqueue_t *queue)
{
    return queue->first;
}

/**
 * @brief remove the packet queue's head
 *
 * @param[in]  queue    the queue
 *
 * @return              the old head
 */
static inline ng_pktqueue_node_t *ng_pktqueue_remove_head(ng_pktqueue_t *queue)
{
    return (ng_pktqueue_node_t *)priority_queue_remove_head((priority_queue_t *)queue);
}

/**
 * @brief       add *node* into *queue* based on its priority
 *
 * @details     The new node will be appended after objects with the same
 *              priority.
 *
 * @param[in]   queue   the queue
 * @param[in]   node    the node to add
 */
static inline void ng_pktqueue_add(ng_pktqueue_t *queue, ng_pktqueue_node_t *node)
{
    priority_queue_add((priority_queue_t *)queue, (priority_queue_node_t *) node);
}

/**
 * @brief       remove *node* from *queue*
 *
 * @param[in]   queue   the queue
 * @param[in]   node    the node to remove
 */
static inline void ng_pktqueue_remove(ng_pktqueue_t *queue, ng_pktqueue_node_t *node)
{
    priority_queue_remove((priority_queue_t *)queue, (priority_queue_node_t *) node);
}

#ifdef __cplusplus
}
#endif

#endif /* NG_PKTQUEUE_H_ */
/**
 * @}
 */
