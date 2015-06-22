/*
 * Copyright (C) 2014, 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ng_pktqueue Packet Queue
 * @brief       Packet wrapper for the Priority Queue
 * @ingroup     net
 * @{
 *
 * @file
 * @brief       ng_pktsnip_t-centric wrapper for @ref priority_queue_t
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef NG_PKTQUEUE_H_
#define NG_PKTQUEUE_H_

#include <stdint.h>
#include <stdlib.h>

#include "net/ng_pkt.h"
#include "utlist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   data type for packet queue nodes
 *
 * @extends priority_queue_node_t
 */
typedef struct ng_pktqueue {
    struct ng_pktqueue *next;   /**< next node in queue */
    ng_pktsnip_t *pkt;          /**< pointer to the packet */
} ng_pktqueue_t;

/**
 * @brief       add @p node into @p queue based on its priority
 *
 * @details     The new node will be appended after objects with the same
 *              priority.
 *
 * @param[in,out]   queue   the queue, may not be NULL
 * @param[in]       node    the node to add.
 */
static inline void ng_pktqueue_add(ng_pktqueue_t **queue, ng_pktqueue_t *node)
{
    LL_APPEND(*queue, node);
}

/**
 * @brief       remove @p node from @p queue
 *
 * @param[in]   queue   the queue, may not be NULL
 * @param[in]   node    the node to remove
 *
 * @return  @p node.
 */
static inline ng_pktqueue_t *ng_pktqueue_remove(ng_pktqueue_t **queue, ng_pktqueue_t *node)
{
    if (node) {
        LL_DELETE(*queue, node);
        node->next = NULL;
    }

    return node;
}

/**
 * @brief remove the packet queue's head
 *
 * @param[in]  queue    the queue, may not be NULL
 *
 * @return              the old head
 */
static inline ng_pktqueue_t *ng_pktqueue_remove_head(ng_pktqueue_t **queue)
{
    return ng_pktqueue_remove(queue, *queue);
}

#ifdef __cplusplus
}
#endif

#endif /* NG_PKTQUEUE_H_ */
/**
 * @}
 */
