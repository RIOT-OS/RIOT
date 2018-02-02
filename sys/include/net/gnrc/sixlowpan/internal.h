/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_sixlowpan
 * @{
 *
 * @file
 * @brief   6LoWPAN internal functions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_SIXLOWPAN_INTERNAL_H
#define NET_GNRC_SIXLOWPAN_INTERNAL_H

#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Delegates a packet to the network layer
 *
 * @param[in] pkt       A packet
 * @param[in] context   Context for the packet. May be NULL.
 * @param[in] page      Current 6Lo dispatch parsing page
 */
void gnrc_sixlowpan_dispatch_recv(gnrc_pktsnip_t *pkt, void *context,
                                  unsigned page);

/**
 * @brief   Delegates a packet to the network interface
 *
 * @param[in] pkt       A packet
 * @param[in] context   Context for the packet. May be NULL.
 * @param[in] page      Current 6Lo dispatch parsing page
 */
void gnrc_sixlowpan_dispatch_send(gnrc_pktsnip_t *pkt, void *context,
                                  unsigned page);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SIXLOWPAN_INTERNAL_H */
/** @} */
