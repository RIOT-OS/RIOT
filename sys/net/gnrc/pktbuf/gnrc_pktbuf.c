/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <sys/uio.h>

#include "net/gnrc/pktbuf.h"

gnrc_pktsnip_t *gnrc_pktbuf_get_iovec(gnrc_pktsnip_t *pkt, size_t *len)
{
    size_t length;
    gnrc_pktsnip_t *head;
    struct iovec *vec;

    assert(len != NULL);
    if (pkt == NULL) {
        *len = 0;
        return NULL;
    }

    /* count the number of snips in the packet and allocate the IOVEC */
    length = gnrc_pkt_count(pkt);
    head = gnrc_pktbuf_add(pkt, NULL, (length * sizeof(struct iovec)),
                           GNRC_NETTYPE_IOVEC);
    if (head == NULL) {
        *len = 0;
        return NULL;
    }

    assert(head->data != NULL);
    vec = (struct iovec *)(head->data);
    /* fill the IOVEC */
    while (pkt != NULL) {
        vec->iov_base = pkt->data;
        vec->iov_len = pkt->size;
        ++vec;
        pkt = pkt->next;
    }
    *len = length;
    return head;
}

gnrc_pktsnip_t *gnrc_pktbuf_remove_snip(gnrc_pktsnip_t *pkt,
                                        gnrc_pktsnip_t *snip)
{
    LL_DELETE(pkt, snip);
    snip->next = NULL;
    gnrc_pktbuf_release(snip);

    return pkt;
}

gnrc_pktsnip_t *gnrc_pktbuf_replace_snip(gnrc_pktsnip_t *pkt,
                                         gnrc_pktsnip_t *old,
                                         gnrc_pktsnip_t *add)
{
    /* If add is a list we need to preserve its tail */
    if (add->next != NULL) {
        gnrc_pktsnip_t *tail = add->next;
        gnrc_pktsnip_t *back;
        LL_SEARCH_SCALAR(tail, back, next, NULL); /* find the last snip in add */
        /* Replace old */
        LL_REPLACE_ELEM(pkt, old, add);
        /* and wire in the tail between */
        back->next = add->next;
        add->next = tail;
    }
    else {
        /* add is a single element, has no tail, simply replace */
        LL_REPLACE_ELEM(pkt, old, add);
    }
    old->next = NULL;
    gnrc_pktbuf_release(old);

    return pkt;
}

gnrc_pktsnip_t *gnrc_pktbuf_reverse_snips(gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *reversed = NULL, *ptr = pkt;

    while (ptr != NULL) {
        gnrc_pktsnip_t *next;

        /* try to write-protect snip as its next-pointer is changed below */
        pkt = gnrc_pktbuf_start_write(ptr); /* use pkt as temporary variable */
        if (pkt == NULL) {
            gnrc_pktbuf_release(reversed);
            gnrc_pktbuf_release(ptr);
            return NULL;
        }
        /* switch around pointers */
        next = pkt->next;
        pkt->next = reversed;
        reversed = pkt;
        ptr = next;
    }
    return reversed;
}

int gnrc_pktbuf_merge(gnrc_pktsnip_t *pkt)
{
    size_t offset = pkt->size;
    size_t size = gnrc_pkt_len(pkt);
    int res = 0;

    if (pkt->size == size) {
        return res;
    }

    /* Re-allocate data */
    res = gnrc_pktbuf_realloc_data(pkt, size);
    if (res != 0) {
        return res;
    }

    /* Copy data to new buffer */
    for (gnrc_pktsnip_t *ptr = pkt->next; ptr != NULL; ptr = ptr->next) {
        memcpy(((uint8_t *)pkt->data) + offset, ptr->data, ptr->size);
        offset += ptr->size;
    }

    /* Release old pktsnips and data*/
    gnrc_pktbuf_release(pkt->next);
    pkt->next = NULL;
    return res;
}

/** @} */
