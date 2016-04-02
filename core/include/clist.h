/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file
 * @brief       Circular linked list
 *
 * This file contains a circularly linked list implementation.
 *
 * clist_insert(), clist_remove_head() and clist_advance() take constant time.
 *
 * Each list is represented as a "clist_node_t". It's only member, the "next"
 * pointer, points to the last entry in the list, whose "next" pointer points to
 * the first entry.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef CLIST_H
#define CLIST_H

#include "kernel_defines.h"
#include "list.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief List node structure
 *
 * Used as is as reference to a list.
 *
 * clist stores a pointer to the last element of a list.  That way, both
 * appending to end of list and removing head can be done in constant time.
 *
 * Actual list objects should have a @c clist_node_t as member and then use
 * the container_of() macro in list operations.
 * See @ref thread_add_to_list() as example.
 */
typedef list_node_t clist_node_t;

/**
 * @brief inserts *new_node* into *list*
 *
 * @param[in,out]   list        Ptr to clist
 * @param[in,out]   new_node    Node which gets inserted.
 *                              Must not be NULL.
 */
static inline void clist_insert(clist_node_t *list, clist_node_t *new_node)
{
    if (list->next) {
        new_node->next = list->next->next;
        list->next->next = new_node;
    }
    else {
        new_node->next = new_node;
    }
    list->next = new_node;
}

/**
 * @brief Removes and returns first element from list
 *
 * @param[in,out]   list        Pointer to the *list* to remove first element
 *                              from.
 */
static inline clist_node_t *clist_remove_head(clist_node_t *list)
{
    if (list->next) {
        clist_node_t *first = list->next->next;
        if (list->next == first) {
            list->next = NULL;
        }
        else {
            list->next->next = first->next;
        }
        return first;
    }
    else {
        return NULL;
    }
}

/**
 * @brief Advances the circle list.
 *
 * The result of this function is will be a list with
 * nodes shifted by one. So second list entry will be
 * first, first is last.
 *
 * @param[in,out]   list        The list to work upon.
 */
static inline void clist_advance(clist_node_t *list)
{
    if (list->next) {
        list->next = list->next->next;
    }
}

#if ENABLE_DEBUG
/**
 * @brief Prints list to stdout.
 *
 * @param[in]       clist       The list to get printed out.
 */
void clist_print(clist_node_t *clist);
#endif

#ifdef __cplusplus
}
#endif

#endif /* CLIST_H */
/** @} */
