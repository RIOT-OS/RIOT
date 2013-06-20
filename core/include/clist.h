/**
 * Circular linked list implementation
 *
 * Used by the scheduler
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup	lib
 * @{
 * @file
 * @author Freie Universität Berlin, Computer Systems & Telematics
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 */

#ifndef __CLIST_H
#define __CLIST_H

typedef struct clist_node_t {
    struct clist_node_t *next;
    struct clist_node_t *prev;
    unsigned int data;
} clist_node_t;

/* inserts new_node after node */
void clist_add(clist_node_t **node, clist_node_t *new_node);

/* removes node. */
void clist_remove(clist_node_t **list, clist_node_t *node);

/* advances the circle list. second list entry will be first, first is last. */
/*void clist_advance(clist_node_t** list);*/

static inline void clist_advance(clist_node_t **list)
{
    *list = (*list)->next;
}


void clist_print(clist_node_t *clist);

/**
 * @}
 */
#endif // __CLIST_H

