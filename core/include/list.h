/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       Intrusive linked list
 *
 * Lists are represented as element pointing to the first actual list element.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef LIST_H
#define LIST_H

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
static inline void list_add(list_node_t *node, list_node_t *new_node) {
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
static inline list_node_t* list_remove_head(list_node_t *list) {
    list_node_t* head = list->next;
    if (head) {
        list->next = head->next;
    }
    return head;
}

#ifdef __cplusplus
}
#endif

#endif /* LIST_H */
/** @} */
