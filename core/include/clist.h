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
 * @brief       Circular linked list
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __CLIST_H
#define __CLIST_H

/**
 * @brief Structure representing a node in the clist.
 */
typedef struct clist_node_t {
    struct clist_node_t *next;  /**< pointer to next node         */
    struct clist_node_t *prev;  /**< pointer to the previous node */
    unsigned int data;          /**< holding data for this node   */
} clist_node_t;

/**
 * @brief Inserts *new_node* after *node* into list
 *
 * @param[in,out]   node        Node after which *new_node* gets inserted
 * @param[in,out]   new_node    Node which gets inserted after *node*.
 *                              Must not be NULL.
 */
void clist_add(clist_node_t **node, clist_node_t *new_node);

/**
 * @brief Removes *node* from list
 *
 * @param[in,out]   list        Pointer to the *list* to remove *node* from.
 * @param[in]       node        Node to remove from *list*
 *                              Must not be NULL.
 */
void clist_remove(clist_node_t **list, clist_node_t *node);

/**
 * @brief Advances the circle list.
 *
 * The result of this function is will be a list with
 * nodes shifted by one. So second list entry will be
 * first, first is last.
 *
 * @param[in,out]   list        The list to work upon.
 */
static inline void clist_advance(clist_node_t **list)
{
    *list = (*list)->next;
}

#if ENABLE_DEBUG
/**
 * @brief Prints list to stdout.
 *
 * @param[in]       clist       The list to get printed out.
 */
void clist_print(clist_node_t *clist);
#endif

#endif /* __CLIST_H */
/** @} */
