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

void clist_sort(clist_node_t *list, clist_cmp_func_t cmp)
{
    clist_node_t *sorted, *unsorted, *last, *first;

    last = list->next;

    /* Special case: Empty list */
    if (!last) {
        return;
    }

    first = last->next;

    /* Special case: One item list */
    if (first == last) {
        return;
    }

    /* We break the circular list into two linear lists. The list sorted
     * gets one element, all others go into unsorted. We then sort in one
     * element from unsorted into sorted at a time using insertion sort.
     * Note: Since sorted is one element long at start, it is naturally sorted,
     *       as every one item list is inherently sorted.
      */
    sorted = first;
    unsorted = sorted->next;
    first->next = NULL;
    last->next = NULL;

sort_next_cycle:
    while (unsorted) {
        clist_node_t *iter = sorted;
        clist_node_t **prev = &sorted;
        while (iter) {
            if (cmp(unsorted, iter) < 0) {
                /* Head of unsorted needs to be inserted before iter. We will
                 * insert as many elements from unsorted before iter as
                 * posbile: So all elements that are already sorted and
                 * smaller than iter. (Note: Not smaller or equal, as we claim
                 * to use a stable sort here) */

                /* safe away current iter */
                clist_node_t *end = iter;
                *prev = unsorted;
                iter = unsorted;
                unsorted = unsorted->next;
                while (unsorted) {
                    if ((cmp(unsorted, end) >= 0) || (cmp(iter, unsorted) > 0)) {
                        iter->next = end;
                        goto sort_next_cycle;
                    }
                    iter = unsorted;
                    unsorted = unsorted->next;
                }
                iter->next = end;
                goto sort_next_cycle;
            }
            prev = &iter->next;
            iter = iter->next;
        }
        /* head of unsorted needs to be inserted after iter */
        clist_node_t *tmp = unsorted->next;
        *prev = unsorted;

        /* keep inserting elements from unsorted while elements are already
         * in order */
        iter = unsorted;
        unsorted = tmp;
        while (unsorted) {
            if (cmp(iter, unsorted) > 0) {
                /* found end of sorted list, go back to insertion sort */
                iter->next = NULL;
                break;
            }
            iter = unsorted;
            unsorted = unsorted->next;
        }
    }

    /* We now have in sorted a sorted linear list. Let's make that circular
     * again and return the pointer to the last item */
    last = sorted;
    while (last->next) {
        last = last->next;
    }
    last->next = sorted;

    /* write the new last to the list */
    list->next = last;
}
