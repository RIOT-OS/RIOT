/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Sebastian Meiling <s@mlng.net>
 */

#include "net/gnrc/pkt.h"

gnrc_pktsnip_t *gnrc_pktsnip_search_type(gnrc_pktsnip_t *pkt,
                                         gnrc_nettype_t type)
{
    while ((pkt != NULL) && (pkt->type != type)) {
        pkt = pkt->next;
    }
    return pkt;
}

/** @} */
