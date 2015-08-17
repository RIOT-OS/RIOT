/*
 * Copyright (C) 2014, 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gnrc_pktqueue Packet Queue
 * @brief       Packet wrapper for the Priority Queue
 * @ingroup     net
 * @{
 *
 * @file
 * @brief       gnrc_pktsnip_t-centric wrapper for @ref priority_queue_t
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef GNRC_PKTQUEUE_H_
#define GNRC_PKTQUEUE_H_

#include <stdint.h>
#include <stdlib.h>

#include "net/gnrc/pkt.h"
#include "utlist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   data type for packet queue nodes
 *
 * @extends priority_queue_node_t
 */
typedef struct gnrc_pktqueue {
    struct gnrc_pktqueue *next; /**< next node in queue */
    gnrc_pktsnip_t *pkt;        /**< pointer to the packet */
} gnrc_pktqueue_t;

/**
 * @brief       add @p node into @p queue based on its priority
 *
 * @details     The new node will be appended after objects with the same
 *              priority.
 *
 * @param[in,out]   queue   the queue, may not be NULL
 * @param[in]       node    the node to add.
 */
static inline void gnrc_pktqueue_add(gnrc_pktqueue_t **queue, gnrc_pktqueue_t *node)
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
static inline gnrc_pktqueue_t *gnrc_pktqueue_remove(gnrc_pktqueue_t **queue, gnrc_pktqueue_t *node)
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
static inline gnrc_pktqueue_t *gnrc_pktqueue_remove_head(gnrc_pktqueue_t **queue)
{
    return gnrc_pktqueue_remove(queue, *queue);
}

#ifdef __cplusplus
}
#endif

#endif /* GNRC_PKTQUEUE_H_ */
/**
 * @}
 */
