/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup net_gnrc_sixlowpan
 * @{
 *
 * @file
 * @brief   6LoWPAN internal functions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stddef.h>

#include "net/gnrc/pkt.h"
#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Returns the PID of the 6Lo thread
 *
 * @return  The PID of the 6Lo thread on success
 * @return  KERNEL_PID_UNDEF, when 6Lo thread was not started.
 */
kernel_pid_t gnrc_sixlowpan_get_pid(void);

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

/**
 * @brief   Checks if packet fits over interface (and fragments if @ref
 *          net_gnrc_sixlowpan_frag is available and required)
 *
 * @param[in] pkt                   The packet to fit. Must not be NULL.
 * @param[in] orig_datagram_size    The original (uncompressed) datagram size.
 * @param[in] netif                 The interface to fit @p pkt over. Must not
 *                                  be NULL.
 * @param[in] page                  Current 6Lo dispatch parsing page
 */
void gnrc_sixlowpan_multiplex_by_size(gnrc_pktsnip_t *pkt,
                                      size_t orig_datagram_size,
                                      gnrc_netif_t *netif,
                                      unsigned page);
#ifdef __cplusplus
}
#endif

/** @} */
