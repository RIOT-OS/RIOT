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

struct tcb_t;

typedef struct queue_node_t {
    struct queue_node_t *next;
    struct tcb_t *thread;
} queue_node_t;

void queue_add_tail(queue_node_t *root, queue_node_t *new_obj);
void queue_add_head(queue_node_t *root, queue_node_t *new_obj);
queue_node_t *queue_remove_head(queue_node_t *root);
void queue_priority_add(queue_node_t *root, queue_node_t *new_obj);
void queue_remove(queue_node_t *root, queue_node_t *node);

/** @} */
#endif /* __QUEUE_H */
