/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013 Freie Universität Berlin
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Circular linked list
 *
 * This file contains a circularly and singly linked list implementation.
 *
 * Its operations are:
 *
 * operation            | runtime | description
 * ---------------------|---------|---------------
 * clist_lpush()        | O(1)    | insert as head (leftmost node)
 * clist_lpeek()        | O(1)    | get the head without removing it
 * clist_lpop()         | O(1)    | remove and return head (leftmost node)
 * clist_rpush()        | O(1)    | append as tail (rightmost node)
 * clist_rpeek()        | O(1)    | get the tail without removing it
 * clist_rpop()         | O(n)    | remove and return tail (rightmost node)
 * clist_lpoprpush()    | O(1)    | move first element to the end of the list
 * clist_find()         | O(n)    | find and return node
 * clist_find_before()  | O(n)    | find node return node pointing to node
 * clist_remove()       | O(n)    | remove and return node
 * clist_sort()         | O(n²)   | sort list (stable)
 * clist_count()        | O(n)    | count the number of elements in a list
 * clist_is_empty()     | O(1)    | returns true if the list contains no elements
 * clist_exactly_one()  | O(1)    | returns true if the list contains one element
 * clist_more_than_one()| O(1)    | returns true if the list contains more than one element
 *
 * clist can be used as a traditional list, a queue (FIFO) and a stack (LIFO) using
 * fast O(1) operations.
 *
 * When used as traditional list, in order to traverse, make sure every element
 * is only visited once.
 *
 * Example:
 *
 *     void clist_traverse(clist_node_t *list) {
 *         clist_node_t *node = list->next;
 *         if (! node) {
 *             puts("list empty");
 *             return;
 *         }
 *
 *         do {
 *             node = node->next;
 *             // do something with node
 *         } while (node != list->next);
 *     }
 *
 * Or use the clist_foreach() helper function, e.g.,:
 *
 *    static int _print_node(clist_node_t *node)
 *    {
 *        printf("0x%08x ", (unsigned)node);
 *        return 0;
 *    }
 *
 *    [...]
 *    clist_foreach(&list, _print_node);
 *
 * To use clist as a queue, use clist_rpush() for adding elements and clist_lpop()
 * for removal. Using clist_lpush() and clist_rpop() is inefficient due to
 * clist_rpop()'s O(n) runtime.
 *
 * To use clist as stack, use clist_lpush()/clist_lpop().
 *
 * Implementation details:
 *
 * Each list is represented as a "clist_node_t". Its only member, the "next"
 * pointer, points to the last entry in the list, whose "next" pointer points to
 * the first entry.
 * Actual list objects should have a @c clist_node_t as member and then use
 * the container_of() macro in list operations.
 * See @ref thread_add_to_list() as example.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef CLIST_H
#define CLIST_H

#include <stdbool.h>
#include <stddef.h>
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief List node structure
 *
 * Used as is as reference to a list.
 *
 */
typedef list_node_t clist_node_t;

/**
 * @brief Checks if *list is empty
 *
 * @note Complexity: O(1)
 *
 * @param[in]   list        Pointer to clist
 *
 * @returns     true if list contains no elements, false otherwise
 */
static inline bool clist_is_empty(const clist_node_t *list)
{
    return list->next == NULL;
}

/**
 * @brief Appends *new_node* at the end of *list
 *
 * @note Complexity: O(1)
 *
 * @param[in,out]   list        Pointer to clist
 * @param[in,out]   new_node    Node which gets inserted.
 *                              Must not be NULL.
 */
static inline void clist_rpush(clist_node_t *list, clist_node_t *new_node)
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
 * @brief Inserts *new_node* at the beginning of *list
 *
 * @note Complexity: O(1)
 *
 * @param[in,out]   list        Pointer to clist
 * @param[in,out]   new_node    Node which gets inserted.
 *                              Must not be NULL.
 */
static inline void clist_lpush(clist_node_t *list, clist_node_t *new_node)
{
    if (list->next) {
        new_node->next = list->next->next;
        list->next->next = new_node;
    }
    else {
        new_node->next = new_node;
        list->next = new_node;
    }
}

/**
 * @brief Removes and returns first element from list
 *
 * @note Complexity: O(1)
 *
 * @param[in,out]   list        Pointer to the *list* to remove first element
 *                              from.
 */
static inline clist_node_t *clist_lpop(clist_node_t *list)
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
 * [ A, B, C ] becomes [ B, C, A ]
 *
 * @note Complexity: O(1)
 *
 * @param[in,out]   list        The list to work upon.
 */
static inline void clist_lpoprpush(clist_node_t *list)
{
    if (list->next) {
        list->next = list->next->next;
    }
}

/**
 * @brief Returns first element in list
 *
 * @note: Complexity: O(1)
 *
 * @param[in]   list        The list to work upon.
 * @returns     first (leftmost) list element, or NULL if list is empty
 */
static inline clist_node_t *clist_lpeek(const clist_node_t *list)
{
    if (list->next) {
        return list->next->next;
    }
    return NULL;
}

/**
 * @brief Returns last element in list
 *
 * @note: Complexity: O(1)
 *
 * @param[in]   list        The list to work upon.
 * @returns     last (rightmost) list element, or NULL if list is empty
 */
static inline clist_node_t *clist_rpeek(const clist_node_t *list)
{
    return list->next;
}

/**
 * @brief Removes and returns last element from list
 *
 * @note Complexity: O(n) with n being the number of elements in the list.
 *
 * @param[in,out]   list        Pointer to the *list* to remove last element
 *                              from.
 */
static inline clist_node_t *clist_rpop(clist_node_t *list)
{
    if (list->next) {
        list_node_t *last = list->next;
        while (list->next->next != last) {
            clist_lpoprpush(list);
        }
        return clist_lpop(list);
    }
    else {
        return NULL;
    }
}

/**
 * @brief Finds node and returns its predecessor
 *
 * @note Complexity: O(n)
 *
 * @param[in]       list    pointer to clist
 * @param[in,out]   node    Node to look for
 *                          Must not be NULL.
 *
 * @returns         predecessor of node if found
 * @returns         NULL if node is not a list member
 */
static inline clist_node_t *clist_find_before(const clist_node_t *list,
                                              const clist_node_t *node)
{
    clist_node_t *pos = list->next;

    if (!pos) {
        return NULL;
    }
    do {
        pos = pos->next;
        if (pos->next == node) {
            return pos;
        }
    } while (pos != list->next);

    return NULL;
}

/**
 * @brief Finds and returns node
 *
 * @note Complexity: O(n)
 *
 * @param[in]       list    pointer to clist
 * @param[in,out]   node    Node to look for
 *                          Must not be NULL.
 *
 * @returns         node if found
 * @returns         NULL if node is not a list member
 */
static inline clist_node_t *clist_find(const clist_node_t *list,
                                       const clist_node_t *node)
{
    clist_node_t *tmp = clist_find_before(list, node);

    if (tmp) {
        return tmp->next;
    }
    else {
        return NULL;
    }
}

/**
 * @brief Finds and removes node
 *
 * @note Complexity: O(n)
 *
 * @param[in]       list    pointer to clist
 * @param[in,out]   node    Node to remove for
 *                          Must not be NULL.
 *
 * @returns         node if found and removed
 * @returns         NULL if node is not a list member
 */
static inline clist_node_t *clist_remove(clist_node_t *list, clist_node_t *node)
{
    if (list->next) {
        if (list->next->next == node) {
            return clist_lpop(list);
        }
        else {
            clist_node_t *tmp = clist_find_before(list, node);
            if (tmp) {
                tmp->next = tmp->next->next;
                if (node == list->next) {
                    list->next = tmp;
                }
                return node;
            }
        }
    }

    return NULL;
}

/**
 * @brief Traverse clist, call function for each member
 *
 * The pointer supplied by @p arg will be passed to every call to @p func.
 *
 * If @p func returns non-zero, traversal will be aborted like when calling
 * break within a for loop, returning the corresponding node.
 *
 * @param[in]       list        List to traverse.
 * @param[in]       func        Function to call for each member.
 * @param[in]       arg         Pointer to pass to every call to @p func
 *
 * @returns         NULL on empty list or full traversal
 * @returns         node that caused @p func(node, arg) to exit non-zero
 */
static inline clist_node_t *clist_foreach(clist_node_t *list, int (*func)(
                                              clist_node_t *,
                                              void *), void *arg)
{
    clist_node_t *node = list->next;

    if (node) {
        do {
            node = node->next;
            if (func(node, arg)) {
                return node;
            }
        } while (node != list->next);
    }

    return NULL;
}

/**
 * @brief Typedef for comparison function used by @ref clist_sort()
 *
 */
typedef int (*clist_cmp_func_t)(clist_node_t *a, clist_node_t *b);

/**
 * @brief   Sort a list
 *
 * This function will sort @p list using merge sort.
 * The sorting algorithm runs in O(N log N) time. It is also stable.
 *
 * Apart from the to-be-sorted list, the function needs a comparison function.
 * That function will be called by the sorting implementation for every
 * comparison.  It gets two pointers a, b of type "clist_node_t" as parameters
 * and must return
 * <0, 0 or >0 if a is lesser, equal or larger than b.
 *
 * Example:
 *
 *     typedef struct {
 *         clist_node_t next;
 *         uint32_t value;
 *     } mylist_node_t;
 *
 *     int _cmp(clist_node_t *a, clist_node_t *b)
 *     {
 *         uint32_t a_val = ((mylist_node_t *)a)->value;
 *         uint32_t b_val = ((mylist_node_t *)b)->value;
 *
 *         if (a_val < b_val) {
 *             return -1;
 *         }
 *         else if (a_val > b_val) {
 *             return 1;
 *         }
 *         else {
 *             return 0;
 *         }
 *     }
 *
 *     ...
 *
 *     clist_sort(list, _cmp);
 *
 * @param[in,out]   list    List to sort
 * @param[in]       cmp     Comparison function
 */
void clist_sort(clist_node_t *list, clist_cmp_func_t cmp);

/**
 * @brief   Count the number of items in the given list
 *
 * @param[in]   list    ptr to the clist
 *
 * @return  the number of elements in the given list
 */
static inline size_t clist_count(clist_node_t *list)
{
    clist_node_t *node = list->next;
    size_t cnt = 0;

    if (node) {
        do {
            node = node->next;
            ++cnt;
        } while (node != list->next);
    }

    return cnt;
}

/**
 * @brief   Tells if a list has exactly one element
 *
 * @note    Complexity: O(1)
 *
 * @param[in]   list    Pointer to the clist
 *
 * @retval      true    If list has exactly one element
 */
static inline bool clist_exactly_one(clist_node_t *list)
{
    return !clist_is_empty(list) && (list->next == list->next->next);
}

/**
 * @brief   Tells if a list has more than one element
 *
 * @note    Complexity: O(1)
 *
 * @param[in]   list    Pointer to the clist
 *
 * @retval      true    If list has more than one element
 */
static inline bool clist_more_than_one(clist_node_t *list)
{
    return !clist_is_empty(list) && (list->next != list->next->next);
}

#ifdef __cplusplus
}
#endif

#endif /* CLIST_H */
/** @} */
