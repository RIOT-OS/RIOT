/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2001 Simon Tatham
 * SPDX-License-Identifier: LGPL-2.1-only
 * SPDX-License-Identifier: MIT
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       clist helper implementations
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "clist.h"

clist_node_t *_clist_sort(clist_node_t *list, clist_cmp_func_t cmp)
{
    clist_node_t *p, *q, *e;
    int insize, psize, qsize, i;

    /*
     * Silly special case: if `list' was passed in as NULL, return
     * NULL immediately.
     */
    if (!list) {
        return NULL;
    }

    insize = 1;

    while (1) {
        clist_node_t *tail = NULL;
        clist_node_t *oldhead = list;
        p = list;
        list = NULL;

        int nmerges = 0;  /* count number of merges we do in this pass */

        while (p) {
            nmerges++;  /* there exists a merge to be done */
            /* step `insize' places along from p */
            q = p;
            psize = 0;
            for (i = 0; i < insize; i++) {
                psize++;
                q = (q->next == oldhead) ? NULL : q->next;
                /* cppcheck-suppress nullPointer
                 * (reason: possible bug in cppcheck 1.6x) */
                if (!q) {
                    break;
                }
            }

            /* if q hasn't fallen off end, we have two lists to merge */
            qsize = insize;

            /* now we have two lists; merge them */
            while (psize > 0 || (qsize > 0 && q)) {

                /* decide whether next element of merge comes from p or q */
                if (psize == 0) {
                    /* p is empty; e must come from q. */
                    e = q; q = q->next; qsize--;
                    if (q == oldhead) {
                        q = NULL;
                    }
                }
                else if (qsize == 0 || !q) {
                    /* q is empty; e must come from p. */
                    e = p; p = p->next; psize--;
                    if (p == oldhead) {
                        p = NULL;
                    }
                }
                else if (cmp(p, q) <= 0) {
                    /* First element of p is lower (or same);
                     * e must come from p. */
                    e = p; p = p->next; psize--;
                    if (p == oldhead) {
                        p = NULL;
                    }
                }
                else {
                    /* First element of q is lower; e must come from q. */
                    e = q; q = q->next; qsize--;
                    if (q == oldhead) {
                        q = NULL;
                    }
                }

                /* add the next element to the merged list */
                if (tail) {
                    tail->next = e;
                }
                else {
                    list = e;
                }
                tail = e;
            }

            /* now p has stepped `insize' places along, and q has too */
            p = q;
        }

        /* cppcheck-suppress nullPointer
         * (reason: tail cannot be NULL at this point, because list != NULL) */
        tail->next = list;

        /* If we have done only one merge, we're finished. */
        if (nmerges <= 1) { /* allow for nmerges==0, the empty list case */
            return tail;
        }

        /* Otherwise repeat, merging lists twice the size */
        insize *= 2;
    }
}
