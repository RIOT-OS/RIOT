/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file        queue.h
 * @brief       A simple queue implementation
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __QUEUE_H
#define __QUEUE_H

#include <stdint.h>
#include <inttypes.h>

/* mspgcc bug : PRIxxx macros not defined before mid-2011 versions */
/**
 * @brief   Macro for printing 32 bit format specifier
 * @def     PRIu32
 */
#ifndef PRIu32
#define PRIu32 "lu"
#endif

/**
 * data type for queue nodes
 */
typedef struct queue_node_t {
    struct queue_node_t *next;  /**< next queue node */
    unsigned int data;          /**< queue node data */
    uint32_t priority;          /**< queue node priority */
} queue_node_t;

/**
 * @brief attach `new_obj` to the tail of the queue (identified
 * `root`)
 *
 * @param[in,out]   root    the queue's root
 * @param[in]       new_obj the object to append
 */
void queue_add_tail(queue_node_t *root, queue_node_t *new_obj);

/**
 * @brief attach `new_obj` to `root` at the beginning
 *
 * @param[in,out]   root    the queue's root
 * @param[in]       new_obj the object to prepend
 */
void queue_add_head(queue_node_t *root, queue_node_t *new_obj);

/**
 * @brief remove the queue's head
 *
 * @param[out]  root    the queue's root
 *
 * @return              the old head
 */
queue_node_t *queue_remove_head(queue_node_t *root);

/**
 * @brief insert `new_obj` into `root` based on its priority
 *
 * @details
 * The new object will be appended after objects with the same priority.
 *
 * @param[in,out]   root    the queue's root
 * @param[in]       new_obj the object to prepend
 */
void queue_priority_add(queue_node_t *root, queue_node_t *new_obj);

/**
 * @brief insert `new_obj` into `root` based on an arbitrary priority
 *
 * @details
 * The new object will be appended after objects with the same priority.
 *
 * @param[in,out]   root    the queue's root
 * @param[in]       new_obj the object to prepend
 * @param[in]       cmp     a comparator function used to determine the priority
 */
void queue_priority_add_generic(queue_node_t *root, queue_node_t *new_obj, int(*cmp)(queue_node_t *, queue_node_t *)) ;

/**
 * @brief remove `node` from `root`
 *
 * @param[in,out]   root    the queue's root
 * @param[in]       node    the node to remove
 */
void queue_remove(queue_node_t *root, queue_node_t *node);

#if ENABLE_DEBUG
void queue_print(queue_node_t *node);
void queue_print_node(queue_node_t *node);
#endif

/** @} */
#endif /* __QUEUE_H */
