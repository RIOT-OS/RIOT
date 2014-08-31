/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#if ENABLE_DEBUG

void clist_print(clist_node_t *clist)
{
    clist_node_t *start = clist, *node = start;
    if (!start) {
        return;
    }

    do {
        printf("list entry: %p: prev=%p next=%p\n", clist, clist->prev, clist->next);
        clist = clist->next;

        if (clist == start) {
            break;
        }
    } while (node != start);
}

#endif

/*
int main (int argc, char* argv[]) {
    clist_node_t a;
    clist_node_t b;
    clist_node_t c;

    clist_node_t* clist = NULL;

    a.data = 0;
    a.next = NULL;
    a.prev = NULL;

    b.data = 1;
    b.next = NULL;
    b.prev = NULL;

    c.data = 2;
    c.next = NULL;
    c.prev = NULL;

    printf("adding a,b,c\n");
    clist_add(&clist,&a);
    clist_add(&clist,&b);
    clist_add(&clist,&c);

    clist_print(clist);
    printf("\n");

    printf("removing b...\n");
    clist_remove(&clist, &b);

    clist_print(clist);
    printf("\n");

    printf("removing a...\n");

    clist_remove(&clist, &a);
    clist_print(clist);

    printf("removing c...\n");

    clist_remove(&clist, &c);
    clist_print(clist);
}
*/
