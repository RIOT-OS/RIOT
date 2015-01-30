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
 * @file
 */

#include <errno.h>

#include "pkt.h"
#include "pktbuf.h"

pkt_t *pkt_init(pkt_t *pkt, void *payload_data, pktsize_t payload_len)
{
    if (pktbuf_contains(payload_data)) {
        pkt->payload_data = payload_data;
    }
    else {
        pkt->payload_data = (payload_data == NULL) ? pktbuf_alloc(payload_len) :
                            pktbuf_insert(payload_data, payload_len);
    }

    pkt->payload_len = payload_len;
    return (pkt->payload_data) ? pkt : NULL;
}

pkt_t *pkt_realloc(pkt_t *pkt, pktsize_t payload_len)
{
    void *data = pktbuf_realloc(pkt->payload_data, payload_len);

    if (data == NULL) {
        return NULL;
    }

    pkt->payload_len = payload_len;
    return (pkt->payload_data) ? pkt : NULL;
}

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

    if (list == NULL || *list == NULL) {
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

int pkt_add_header(pkt_t *pkt, void *header_data, pktsize_t header_len,
                   pkt_proto_t header_proto)
{
    pkt_hlist_t header, *ptr;

#ifdef DEVELHELP

    if (pkt == NULL) {
        return -EINVAL;
    }

#endif

    if (header_data == NULL) {
        if ((header.header_data = pktbuf_alloc(header_len)) == NULL) {
            return -ENOMEM;
        }
    }
    else if (pktbuf_contains(header_data)) {
        header.header_data = header_data;
    }
    else {
        if ((header.header_data = pktbuf_insert(header_data, header_len)) == NULL) {
            return -ENOMEM;
        }
    }

    header.header_len = header_len;
    header.header_proto = header_proto;

    if ((ptr = pktbuf_insert(&header, sizeof(pkt_hlist_t))) == NULL) {
        pktbuf_release(header.header_data);
        return -ENOMEM;
    }

    pkt_hlist_add(&(pkt->headers), ptr);

    return 0;
}

void pkt_remove_header(pkt_t *pkt, pkt_hlist_t *header)
{
#ifdef DEVELHELP

    if (pkt == NULL) {
        return;
    }

#endif

    pkt_hlist_remove(&(pkt->headers), header);
    pkt_release(header->header_data);
    pkt_release((void *)header);
}

void pkt_hold(pkt_t *pkt)
{
    pkt_hlist_t *ptr = pkt->headers;

    pktbuf_hold(pkt->payload_data);

    while (ptr) {
        pktbuf_hold(ptr);
        pktbuf_hold(ptr->header_data);
        pkt_hlist_advance(&ptr);
    }
}

void pkt_release(pkt_t *pkt)
{
    pkt_hlist_t *ptr = pkt->headers;

    pktbuf_release(pkt->payload_data);

    while (ptr) {
        pktbuf_release(ptr);
        pktbuf_release(ptr->header_data);
        pkt_hlist_advance(&ptr);
    }
}

/** @} */
