/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#ifdef __cplusplus
 extern "C" {
#endif

#include "kernel_macros.h"

/**
 * @def         clist_get_container(NODE, TYPE, MEMBER)
 * @brief       Returns the container of the circular list
 * @details     For a struct `TYPE` with a member `MEMBER`, which is a `clist_node_t`,
 *              given a pointer `NODE` to `TYPE::MEMBER` this function returns a pointer
 *              to the instance of `TYPE`.
 * @details     E.g. for `struct my_struct_t { ...; clist_node_t n; ... } my_struct;`,
 *              `&my_struct == clist_get_container(&my_struct.n, struct my_struct_t, n)`.
 * @param[in]   NODE     pointer to a member
 * @param[in]   TYPE     a type name (a struct or union), container of NODE
 * @param[in]   MEMBER   name of the member of TYPE which NODE points to
 * @return      Pointer to the container of NODE.
 */
#define clist_get_container(NODE, TYPE, MEMBER) container_of(NODE, TYPE, MEMBER)

/**
 * @brief Structure representing a node in the clist.
 */
typedef struct clist_node_t {
    struct clist_node_t *next;  /**< pointer to next node         */
    struct clist_node_t *prev;  /**< pointer to the previous node */
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

#ifdef __cplusplus
}
#endif

#endif /* __CLIST_H */
/** @} */
