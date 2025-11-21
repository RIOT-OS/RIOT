/*
 * SPDX-FileCopyrightText: 2013-2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       A simple priority queue
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <inttypes.h>
#include <assert.h>

#include "priority_queue.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void priority_queue_remove(priority_queue_t *root_, priority_queue_node_t *node)
{
    /* The strict aliasing rules allow this assignment. */
    priority_queue_node_t *root = (priority_queue_node_t *)root_;

    while (root->next != NULL) {
        if (root->next == node) {
            root->next = node->next;
            node->next = NULL;
            return;
        }

        root = root->next;
    }
}

priority_queue_node_t *priority_queue_remove_head(priority_queue_t *root)
{
    priority_queue_node_t *head = root->first;

    if (head) {
        root->first = head->next;
    }
    return head;
}

void priority_queue_add(priority_queue_t *root, priority_queue_node_t *new_obj)
{
    /* The strict aliasing rules allow this assignment. */
    priority_queue_node_t *node = (priority_queue_node_t *)root;

    while (node->next != NULL) {
        /* not trying to add the same node twice */
        assert(node->next != new_obj);
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

#if IS_ACTIVE(ENABLE_DEBUG)
void priority_queue_print(priority_queue_t *root)
{
    printf("queue:\n");

    for (priority_queue_node_t *node = root->first; node; node = node->next) {
        printf("Data: %" PRIuPTR " Priority: %" PRIu32 "\n", node->data,
               node->priority);
    }
}

void priority_queue_print_node(priority_queue_node_t *node)
{
    printf("Data: %" PRIuPTR " Priority: %" PRIu32 " Next: %p\n", node->data,
           node->priority, (void *)node->next);
}
#endif
