/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file        queue.c
 * @brief       A simple queue implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "queue.h"

void queue_remove(queue_t *root_, queue_node_t *node)
{
    /* The strict aliasing rules allow this assignment. */
    queue_node_t *root = (queue_node_t *) root_;

    while (root->next != NULL) {
        if (root->next == node) {
            root->next = node->next;
            node->next = NULL;
            return;
        }

        root = root->next;
    }
}

queue_node_t *queue_remove_head(queue_t *root)
{
    queue_node_t *head = root->first;
    if (head) {
        root->first = head->next;
    }
    return head;
}

void queue_priority_add(queue_t *root, queue_node_t *new_obj)
{
    /* The strict aliasing rules allow this assignment. */
    queue_node_t *node = (queue_node_t *) root;

    while (node->next != NULL) {
        if (node->next->priority > new_obj->priority) {
            new_obj->next = node->next;
            node->next = new_obj;
            return;
        }

        node = node->next;
    }

    node->next = new_obj;
    new_obj->next = NULL;
}

#if ENABLE_DEBUG
void queue_print(queue_t *node)
{
    printf("queue:\n");

    for (queue_node_t *node = node->first; node; node = node->next) {
        printf("Data: %u Priority: %lu\n", node->data, (unsigned long) node->priority);
    }
}

void queue_print_node(queue_node_t *node)
{
    printf("Data: %u Priority: %lu Next: %u\n", (unsigned int) node->data, (unsigned long) node->priority, (unsigned int)node->next);
}
#endif
