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

size_t pkt_total_header_len(const pkt_t *pkt)
{
    pkt_hlist_t *ptr = pkt->headers;
    size_t length = 0;

    if (ptr == NULL) {
        return 0;
    }

    do {
        length += (size_t)ptr->header_len;
        pkt_hlist_advance(&ptr);
    } while (ptr != pkt->headers);

    return length;
}

/** @} */
