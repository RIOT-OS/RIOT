/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file    pkt.c
 */

#include "pkt.h"

pktsize_t pkt_hlist_len(pkt_hlist_t *list)
{
    pktsize_t len = 0;

    while (list) {
        len += list->header_len;
        pkt_hlist_advance(&list);
    }

    return len;
}

pkt_hlist_t *pkt_hlist_remove_first(pkt_hlist_t **list)
{
    pkt_hlist_t *res;

    if (list == NULL) {
        return NULL;
    }

    res = *list;
    *list = res->next;
    res->next = NULL;

    return res;
}

void pkt_hlist_remove(pkt_hlist_t **list, pkt_hlist_t *header)
{
    if (list == NULL || *list == NULL || header == NULL) {
        return;
    }

    if ((*list) == header) {
        pkt_hlist_remove_first(list);
    }
    else {
        pkt_hlist_t *ptr = (*list)->next, *prev = *list;

        pkt_hlist_advance(&ptr);

        while (ptr != NULL) {
            if (ptr == header) {
                prev->next = ptr->next;
                ptr->next = NULL;
            }

            pkt_hlist_advance(&ptr);
            pkt_hlist_advance(&prev);
        }
    }

}

/** @} */
