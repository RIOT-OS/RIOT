/*
 * Copyright (C) 2025 Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_util
 *
 * @file
 * @brief       clist helper implementations
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 */

#include "clist.h"
#include "container.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CONFIG_CLIST_SORT_BUF_SIZE
/**
 * @brief   size of the two static buffer used to speed up the sort
 *
 * Bumping this will increase the sorting speed of larger list at the cost of
 * stack size. The stack size required by the two buffers amounts to:
 * `2 * CONFIG_CLIST_SORT_BUF_SIZE * sizeof(void *)`
 */
#  define CONFIG_CLIST_SORT_BUF_SIZE 8
#endif

#if (CONFIG_CLIST_SORT_BUF_SIZE == 0) || (CONFIG_CLIST_SORT_BUF_SIZE & (CONFIG_CLIST_SORT_BUF_SIZE - 1) != 0)
#  error "CONFIG_CLIST_SORT_BUF_SIZE needs to be a power of 2"
#endif

struct lists_buf {
    list_node_t *lists[CONFIG_CLIST_SORT_BUF_SIZE];
};

/**
 * @brief   print the given lists if `ENABLE_DEBUG` is set to `1`
 * @param   head    List to print
 * @note    this function will be fully optimized out with `ENABLE_DEBUG == 0`
 */
static void _debug_print_list(list_node_t *head)
{
    if (ENABLE_DEBUG) {
        while (head) {
            DEBUG("-->[%p]", (void *)head);
            head = head->next;
        }
    }
}

/**
 * @brief   Merge two sorted lists into one sorted lists
 * @param   lhs     left list to sort
 * @param   rhs     right list to sort
 * @param   cmp     compare function to use to determine the list order
 * @return  head of the newly merged list
 *
 * @pre    @p lhs is not `NULL`
 * @pre    @p rhs is not `NULL`
 */
static list_node_t *_merge(list_node_t *lhs, list_node_t *rhs, clist_cmp_func_t cmp)
{
    DEBUG("\nmerging:\nlhs: ");
    _debug_print_list(lhs);
    DEBUG("\nrhs: ");
    _debug_print_list(rhs);

    /* handle first element special */
    list_node_t *result;
    if (cmp(lhs, rhs) > 0) {
        result = rhs;
        rhs = rhs->next;
    }
    else {
        result = lhs;
        lhs = lhs->next;
    }

    /* while both lists still contain elements, merge them */
    list_node_t *iter = result;
    while (lhs && rhs) {
        if (cmp(lhs, rhs) > 0) {
            iter->next = rhs;
            rhs = rhs->next;
        }
        else {
            iter->next = lhs;
            lhs = lhs->next;
        }
        iter = iter->next;
    }

    /* fill the rest with the list that is not fully drained */
    if (lhs) {
        iter->next = lhs;
    }
    else {
        iter->next = rhs;
    }

    DEBUG("\nmerged: ");
    _debug_print_list(result);
    DEBUG_PUTS("");

    return result;
}

/**
 * @brief   Break circular list into a linear list
 *
 * @return  Head of the list
 * @retval  NULL    List was not broken and already is sorted
 */
static list_node_t *linearlize_list(clist_node_t *list)
{
    list_node_t *last, *first;

    last = list->next;

    /* Special case: Empty list */
    if (!last) {
        return NULL;
    }

    first = last->next;

    /* Special case: One item list */
    if (first == last) {
        return NULL;
    }

    last->next = NULL;
    return first;
}

/**
 * @brief   take a buf of sorted list and merge them down into a single
 *          sorted list
 * @param   lists   bunch of lists to merge
 * @param   cmp     compare function to use to determine the list order
 * @return  head of the merged lists
 *
 * @note    @p lists may have `NULL` elements
 */
static list_node_t *_binary_merge_bufs(struct lists_buf *lists,
                                       clist_cmp_func_t cmp)
{
    while (lists->lists[1]) {
        unsigned dst_idx = 0;
        unsigned src_idx = 0;
        while (src_idx < ARRAY_SIZE(lists->lists)) {
            if (!lists->lists[src_idx + 1]) {
                list_node_t *tmp = lists->lists[src_idx];
                lists->lists[src_idx] = NULL;
                lists->lists[dst_idx] = tmp;
                break;
            }
            list_node_t *merged = _merge(lists->lists[src_idx],
                                         lists->lists[src_idx + 1], cmp);
            lists->lists[src_idx++] = NULL;
            lists->lists[src_idx++] = NULL;
            lists->lists[dst_idx++] = merged;
        }
    }

    return lists->lists[0];
}

/**
 * @brief   Consume up to `2 * CONFIG_CLIST_SORT_BUF_SIZE` elements from the
 *          given list and sort them
 * @param   iter_in_out     pointer to the iterator
 * @param   cmp             compare function to use to determine the list order
 * @return  Head of the sorted sub-list bitten of from the input
 *
 * @p iter_in_out points to the iterator used to track which part of the input
 * of @ref clist_sort has not been sorted yet. At return, it will be updated
 * to the first item after the chunk that has been bitten off from the input
 * and sorted. If the end of the input was reached, it is upstead to point to
 * `NULL` instead.
 */
static list_node_t *_sort_chunk(list_node_t **iter_in_out, clist_cmp_func_t cmp)
{
    struct lists_buf lists = { .lists = { NULL } };
    list_node_t *iter = *iter_in_out;

    for (unsigned i = 0; i < ARRAY_SIZE(lists.lists); i++) {
        if (!iter) {
            break;
        }
        list_node_t *lhs = iter;
        iter = iter->next;
        lhs->next = NULL;
        list_node_t *rhs = iter;
        if (!rhs) {
            lists.lists[i] = lhs;
            break;
        }
        iter = iter->next;
        rhs->next = NULL;
        lists.lists[i] = _merge(lhs, rhs, cmp);
    }

    *iter_in_out = iter;

    return _binary_merge_bufs(&lists, cmp);
}

/**
 * @brief   Get the last element of the given linear list
 */
list_node_t *_get_last(list_node_t *head) {
    while (head->next) {
        head = head->next;
    }
    return head;
}

void clist_sort(clist_node_t *list, clist_cmp_func_t cmp)
{
    list_node_t *head = linearlize_list(list);
    if (!head) {
        return;
    }

    list_node_t *iter = head;
    unsigned idx = 0;
    struct lists_buf lists = { .lists = {NULL } };

    while (iter) {
        list_node_t *tmp = _sort_chunk(&iter, cmp);
        if (lists.lists[idx]) {
            lists.lists[idx] = _merge(lists.lists[idx], tmp, cmp);
        }
        else {
            lists.lists[idx] = tmp;
        }

        idx++;
        idx &= (ARRAY_SIZE(lists.lists) - 1);
    }

    list_node_t *result = _binary_merge_bufs(&lists, cmp);

    /* turn the linear list back into a circular one */
    list->next = _get_last(result);
    list->next->next = result;
}
