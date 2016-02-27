/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>

#include "net/gnrc/pkt.h"

gnrc_pktsnip_t *gnrc_pktsnip_search_type(gnrc_pktsnip_t *ptr,
                                         gnrc_nettype_t type)
{
    while (ptr != NULL) {
        if (ptr->type == type) {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

/** @} */
