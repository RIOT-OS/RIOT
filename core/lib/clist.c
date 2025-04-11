/*
 * Copyright (C) 2025 Marian Buschsieweke <marian.buschsieweke@posteo.net>
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
 * @brief       clist helper implementations
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include "clist.h"

clist_node_t *_clist_sort(clist_node_t *list, clist_cmp_func_t cmp)
{
    clist_node_t *sorted, *unsorted;

    /* Special case: Empty list */
    if (!list) {
        return NULL;
    }

    /* Special case: Only one item */
    if (!list->next) {
        return list;
    }

    /* We break the circular list into two linear lists. The list sorted
     * gets one element, all others go into unsorted. We then sort in one
     * element from unsorted into sorted at a time using insertion sort.
     * Note: Since sorted is one element long at start, it is naturally sorted,
     *       as every one item list is inherently sorted.
      */
    sorted = list->next;
    unsorted = sorted->next;
    sorted->next = NULL;
    list->next = NULL;

sort_next_cycle:
    while (unsorted) {
        clist_node_t *iter = sorted;
        clist_node_t **prev = &sorted;
        while (iter) {
            if (cmp(unsorted, iter) < 0) {
                /* head of unsorted needs to be inserted before iter */
                clist_node_t *tmp = unsorted->next;
                *prev = unsorted;
                unsorted->next = iter;
                unsorted = tmp;
                goto sort_next_cycle;
            }
            prev = &iter->next;
            iter = iter->next;
        }
        /* head of unsorted needs to be inserted after iter */
        clist_node_t *tmp = unsorted->next;
        *prev = unsorted;
        unsorted->next = NULL;
        unsorted = tmp;
    }

    /* We now have in sorted a sorted linear list. Let's make that circular
     * again and return the pointer to the last item */

    clist_node_t *last = sorted;
    while (last->next) {
        last = last->next;
    }
    last->next = sorted;

    return last;
}
