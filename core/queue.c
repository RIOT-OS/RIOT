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
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stddef.h>
#include <inttypes.h>
#include <stdio.h>

#include "queue.h"
#include "tcb.h"

void queue_remove(queue_node_t *root, queue_node_t *node)
{
    while (root->next != NULL) {
        if (root->next == node) {
            root->next = node->next;
            node->next = NULL;
            return;
        }

        root = root->next;
    }
}

queue_node_t *queue_remove_head(queue_node_t *root)
{
    queue_node_t *head = root->next;

    if (head != NULL) {
        root->next = head->next;
    }

    return head;
}

void queue_add_tail(queue_node_t *node, queue_node_t *new_obj)
{
    while (node->next != NULL) {
        node = node->next;
    }

    node->next = new_obj;
    new_obj->next = NULL;
}

void queue_add_head(queue_node_t *root, queue_node_t *new_obj)
{
    new_obj->next = root->next;
    root->next = new_obj;
}

void queue_priority_add(queue_node_t *root, queue_node_t *new_obj)
{
    queue_node_t *node = root;

    uint16_t new_obj_priority = new_obj->thread->priority;

    while (node->next != NULL) {
        if (node->next->thread->priority > new_obj_priority) {
            new_obj->next = node->next;
            node->next = new_obj;
            return;
        }

        node = node->next;
    }

    node->next = new_obj;
    new_obj->next = NULL;
}

void queue_priority_add_generic(queue_node_t *root, queue_node_t *new_obj, int (*cmp)(queue_node_t *, queue_node_t *))
{
    queue_node_t *node = root;

    while (node->next != NULL) {
        if (cmp(node->next, new_obj) < 0) {
            new_obj->next = node->next;
            node->next = new_obj;
            return;
        }

        node = node->next;
    }

    node->next = new_obj;
    new_obj->next = NULL;
}
