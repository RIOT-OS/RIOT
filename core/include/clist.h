/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file        clist.h
 * @brief       Circular linkes list
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __CLIST_H
#define __CLIST_H

#include <stddef.h>

#ifndef container_of
#define container_of(PTR, TYPE, MEMBER) \
    ((TYPE *) ((char *) (const __typeof__ (((TYPE *) 0)->MEMBER) *) (PTR) - offsetof(TYPE, MEMBER)))
#endif

typedef struct clist_node_t {
    struct clist_node_t *next;
    struct clist_node_t *prev;
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

/** @} */
#endif // __CLIST_H
