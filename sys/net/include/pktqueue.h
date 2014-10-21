/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    pktqueue Packet Queue
 * @brief       Packet wrapper for the Priority Queue
 * @ingroup     net
 * @{
 *
 * @file        pktqueue.h
 * @brief       Pointer-centric wrapper for @ref priority_queue
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __PKTQUEUE_H_
#define __PKTQUEUE_H_

#include <stdint.h>
#include <stdlib.h>

#include "priority_queue.h"

/**
 * @brief   data type for packet queue nodes
 *
 * @extends priority_queue_node_t
 */
typedef struct pktqueue_node_t {
    struct pktqueue_node_t *next;   /**< next node in queue */
    void *data;                     /**< pointer to the data */
    uint32_t priority;              /**< priority of the node */
} pktqueue_node_t;

/**
 * @brief   data type for packet queues
 *
 * @extends priority_queue_t
 */
typedef struct {
    pktqueue_node_t *first;     /**< first node in the queue */
} pktqueue_t;

/**
 * @brief   Static initializer for pktqueue_node_t
 */
#define PKTQUEUE_NODE_INIT { NULL, NULL, 0 }

/**
 * @brief   Initializes a packet queue node.
 * @details For initialization of variables use PKTQUEUE_NODE_INIT instead.
 *          Only use this function for dynamically allocated packet queue nodes.
 *
 * @param[out] node pre-allocated pktqueue_node_t object. must not be NULL.
 */
static inline void pktqueue_node_init(pktqueue_node_t *node)
{
    priority_queue_node_init((priority_queue_node_t *)node);
}

/**
 * @brief Static initializer for pktqueue_t.
 */
#define PKTQUEUE_INIT PRIORITY_QUEUE_INIT

/**
 * @brief   Initialize a packet queue object.
 * @details For initialization of variables use PKTQUEUE_INIT
 *          instead. Only use this function for dynamically allocated
 *          packet queues.
 * @param[out] queue    pre-allocated pktqueue_t object, must not be NULL.
 */
static inline void pktqueue_init(pktqueue_t *queue)
{
    priority_queue_init((priority_queue_t *)queue);
}

/**
 * @brief get the packet queue's head without removing it
 *
 * @param[out]  root    the queue
 *
 * @return              the head
 */
static inline pktqueue_node_t *pktqueue_get_head(pktqueue_t *queue)
{
    return queue->first;
}

/**
 * @brief remove the packet queue's head
 *
 * @param[in]  root     the queue
 *
 * @return              the old head
 */
static inline pktqueue_node_t *pktqueue_remove_head(pktqueue_t *queue)
{
    return (pktqueue_node_t *)priority_queue_remove_head((priority_queue_t *)queue);
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
static inline void pktqueue_add(pktqueue_t *queue, pktqueue_node_t *node)
{
    priority_queue_add((priority_queue_t *)queue, (priority_queue_node_t *) node);
}

/**
 * @brief       remove *node* from *queue*
 *
 * @param[in]   queue   the queue
 * @param[in]   node    the node to remove
 */
static inline void pktqueue_remove(pktqueue_t *queue, pktqueue_node_t *node)
{
    priority_queue_remove((priority_queue_t *)queue, (priority_queue_node_t *) node);
}

#endif /* __PKTQUEUE_H_ */
/**
 * @}
 */
