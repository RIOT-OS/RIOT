/*
 * Copyright (C) 2013, 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file        priority_queue.h
 * @brief       A simple priority queue
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief data type for priority queue nodes
 */
typedef struct priority_queue_node_t {
    struct priority_queue_node_t *next; /**< next queue node */
    uint32_t priority;                  /**< queue node priority */
    unsigned int data;                  /**< queue node data */
} priority_queue_node_t;

/**
 * @brief data type for priority queues
 */
typedef struct queue {
    priority_queue_node_t *first;        /**< first queue node */
} priority_queue_t;

/**
 * @brief Static initializer for priority_queue_node_t.
 */
#define PRIORITY_QUEUE_NODE_INIT { NULL, 0, 0 }

/**
 * @brief   Initialize a priority queue node object.
 * @details For initialization of variables use PRIORITY_QUEUE_NODE_INIT
 *          instead. Only use this function for dynamically allocated
 *          priority queue nodes.
 * @param[out] priority_queue_node
 *          pre-allocated priority_queue_node_t object, must not be NULL.
 */
static inline void priority_queue_node_init(
        priority_queue_node_t *priority_queue_node)
{
    priority_queue_node_t qn = PRIORITY_QUEUE_NODE_INIT;
    *priority_queue_node = qn;
}

/**
 * @brief Static initializer for priority_queue_t.
 */
#define PRIORITY_QUEUE_INIT { NULL }

/**
 * @brief   Initialize a priority queue object.
 * @details For initialization of variables use PRIORITY_QUEUE_INIT
 *          instead. Only use this function for dynamically allocated
 *          priority queues.
 * @param[out] priority_queue
 *          pre-allocated priority_queue_t object, must not be NULL.
 */
static inline void priority_queue_init(priority_queue_t *priority_queue)
{
    priority_queue_t q = PRIORITY_QUEUE_INIT;
    *priority_queue = q;
}

/**
 * @brief remove the priority queue's head
 *
 * @param[out]  root    the queue's root
 *
 * @return              the old head
 */
priority_queue_node_t *priority_queue_remove_head(priority_queue_t *root);

/**
 * @brief insert `new_obj` into `root` based on its priority
 *
 * @details
 * The new object will be appended after objects with the same priority.
 *
 * @param[in,out]   root    the queue's root
 * @param[in]       new_obj the object to prepend
 */
void priority_queue_add(priority_queue_t *root, priority_queue_node_t *new_obj);

/**
 * @brief remove `node` from `root`
 *
 * @param[in,out]   root    the priority queue's root
 * @param[in]       node    the node to remove
 */
void priority_queue_remove(priority_queue_t *root, priority_queue_node_t *node);

#if ENABLE_DEBUG
void priority_queue_print(priority_queue_t *root);
void priority_queue_print_node(priority_queue_t *root);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* QUEUE_H */
