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
 * @file        clist.c
 * @brief       Circular linked list implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stddef.h>
#include "clist.h"
#include <stdio.h>

/* inserts new_node after node */
void clist_add(clist_node_t **node, clist_node_t *new_node)
{
    if (*node != NULL) {
        new_node->next = (*node);
        new_node->prev = (*node)->prev;
        (*node)->prev->next = new_node;
        (*node)->prev = new_node;

        if ((*node)->prev == *node) {
            (*node)->prev = new_node;
        }
    }
    else {
        *node = new_node;
        new_node->next = new_node;
        new_node->prev = new_node;
    }
}

/* removes node. */
void clist_remove(clist_node_t **list, clist_node_t *node)
{
    if (node->next != node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if (node == *list) {
            *list = node->next;
        }
    }
    else {
        *list = NULL;
    }
}
