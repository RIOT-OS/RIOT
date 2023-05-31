/*
 * Copyright (C) 2017 Inria
 *               2017 Freie Universität Berlin
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * The code of _clist_sort() has been imported from
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html.
 * Original copyright notice:
 *
 * This file is copyright 2001 Simon Tatham.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL SIMON TATHAM BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
    clist_node_t *p, *q, *e = NULL;
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
            for (i = 0; i < insize && q; i++) {
                psize++;
                q = (q->next == oldhead) ? NULL : q->next;
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
                else if (p && (qsize == 0 || !q)) {
                    /* q is empty; e must come from p. */
                    e = p; p = p->next; psize--;
                    if (p == oldhead) {
                        p = NULL;
                    }
                }
                else if (p && cmp(p, q) <= 0) {
                    /* First element of p is lower (or same);
                     * e must come from p. */
                    e = p; p = p->next; psize--;
                    if (p == oldhead) {
                        p = NULL;
                    }
                }
                else if (q) {
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

        if (tail) {
            tail->next = list;
        }

        /* If we have done only one merge, we're finished. */
        if (nmerges <= 1) { /* allow for nmerges==0, the empty list case */
            return tail;
        }

        /* Otherwise repeat, merging lists twice the size */
        insize *= 2;
    }
}
