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

// mspgcc bug : PRIxxx macros not defined before mid-2011 versions
#ifndef PRIu32
#define PRIu32 "lu"
#endif

typedef struct queue_node_t {
    struct queue_node_t *next;
    unsigned int data;
    uint32_t priority;
} queue_node_t;

queue_node_t *queue_remove_head(queue_node_t *root);
void queue_add_tail(queue_node_t *root, queue_node_t *new_obj);
void queue_add_head(queue_node_t *root, queue_node_t *new_obj);
queue_node_t *queue_remove_head(queue_node_t *root);
void queue_priority_add(queue_node_t *root, queue_node_t *new_obj);
void queue_priority_add_generic(queue_node_t *root, queue_node_t *new_obj, int(*cmp)(queue_node_t *, queue_node_t *)) ;
void queue_remove(queue_node_t *root, queue_node_t *node);

#if ENABLE_DEBUG
void queue_print(queue_node_t *node);
void queue_print_node(queue_node_t *node);
#endif

/** @} */
#endif /* __QUEUE_H */
