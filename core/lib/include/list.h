/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2016 TriaGnoSys GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Intrusive linked list
 *
 * Lists are represented as element pointing to the first actual list element.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Víctor Ariño <victor.arino@zii.aero>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief List node structure
 *
 * Used as is as reference to a list, or as member of any data structure that
 * should be member of a list.
 *
 * Actual list objects should have a @c list_node_t as member and then use
 * the container_of() macro in list operations.
 * See @ref thread_add_to_list() as example.
 */
typedef struct list_node {
    struct list_node *next;     /**< pointer to next list entry */
} list_node_t;

/**
 * @brief Insert object into list
 *
 * If called with a list reference as node, the new node will become the new
 * list head.
 *
 * @param[in] node      list node before new entry
 * @param[in] new_node  list node to insert
 */
static inline void list_add(list_node_t *node, list_node_t *new_node)
{
    new_node->next = node->next;
    node->next = new_node;
}

/**
 * @brief Removes the head of the list and returns it
 *
 * @param[in] list  Pointer to the list itself, where list->next points
 *                  to the root node
 *
 * @return  removed old list head, or NULL if empty
 */
static inline list_node_t *list_remove_head(list_node_t *list)
{
    list_node_t *head = list->next;

    if (head) {
        list->next = head->next;
    }
    return head;
}

/**
 * @brief Removes the node from the list
 *
 * @param[in] list  Pointer to the list itself, where list->next points
 *                  to the root node
 * @param[in] node  List node to remove from the list
 *
 * @return  removed node, or NULL if empty or not found
 */
static inline list_node_t *list_remove(list_node_t *list, list_node_t *node)
{
    while (list->next) {
        if (list->next == node) {
            list->next = node->next;
            return node;
        }
        list = list->next;
    }
    return list->next;
}

#ifdef __cplusplus
}
#endif

/** @} */
