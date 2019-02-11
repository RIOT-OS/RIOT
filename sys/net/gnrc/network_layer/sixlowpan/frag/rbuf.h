/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_sixlowpan_frag
 * @{
 *
 * @file
 * @internal
 * @brief   6LoWPAN reassembly buffer
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef RBUF_H
#define RBUF_H

#include <inttypes.h>
#include <stdbool.h>

#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pkt.h"

#include "net/gnrc/sixlowpan/config.h"
#include "net/gnrc/sixlowpan/frag.h"
#ifdef __cplusplus

extern "C" {
#endif

/**
 * @name Legacy defines
 * @{
 */
#define RBUF_SIZE           (GNRC_SIXLOWPAN_FRAG_RBUF_SIZE)
#define RBUF_TIMEOUT        (GNRC_SIXLOWPAN_FRAG_RBUF_TIMEOUT_US)
/** @} */

/**
 * @brief   Adds a new fragment to the reassembly buffer. If the packet is
 *          complete, dispatch the packet with the transmit information of
 *          the last fragment.
 *
 * @param[in] netif_hdr     The interface header of the fragment, with
 *                          gnrc_netif_hdr_t::if_pid and its source and
 *                          destination address set.
 * @param[in] frag          The fragment to add.
 * @param[in] offset        The fragment's offset.
 * @param[in] page          Current 6Lo dispatch parsing page.
 *
 * @internal
 */
void rbuf_add(gnrc_netif_hdr_t *netif_hdr, gnrc_pktsnip_t *frag,
              size_t offset, unsigned page);

/**
 * @brief   Checks timeouts and removes entries if necessary
 */
void rbuf_gc(void);

/**
 * @brief   Unsets a reassembly buffer entry (but does not free
 *          rbuf_t::super::pkt)
 *
 * This functions sets rbuf_t::super::pkt to NULL and removes all rbuf::ints.
 *
 * @param[in] rbuf  A reassembly buffer entry
 *
 * @internal
 */
void rbuf_rm(gnrc_sixlowpan_rbuf_t *rbuf);

/**
 * @brief   Checks if a reassembly buffer entry is unset
 *
 * @param[in] rbuf  A reassembly buffer entry
 *
 * @return  true, if @p rbuf is empty (i.e. rbuf->super.pkt is NULL).
 * @return  false, if @p rbuf is in use.
 *
 * @internal
 */
static inline bool rbuf_entry_empty(const gnrc_sixlowpan_rbuf_t *rbuf) {
    return (rbuf->pkt == NULL);
}

#if defined(TEST_SUITES) || defined(DOXYGEN)
/**
 * @brief   Resets the packet buffer to a clean state
 *
 * @note    Only available when @ref TEST_SUITES is defined
 */
void rbuf_reset(void);

/**
 * @brief   Returns a pointer to the array representing the reassembly buffer.
 *
 * @note    Only available when @ref TEST_SUITES is defined
 *
 * @return  The first element of the reassembly buffer. `const`, so that write
 *          access is immediately spotted at compile time of tests. The `const`
 *          qualifier may however be discarded if required by the tests.
 */
const gnrc_sixlowpan_rbuf_t *rbuf_array(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* RBUF_H */
/** @} */
