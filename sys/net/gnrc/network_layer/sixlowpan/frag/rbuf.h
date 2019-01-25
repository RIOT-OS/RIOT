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

#include "net/gnrc/sixlowpan/frag.h"
#ifdef __cplusplus

extern "C" {
#endif

#define RBUF_SIZE           (4U)               /**< size of the reassembly buffer */
#define RBUF_TIMEOUT        (3U * US_PER_SEC) /**< timeout for reassembly in microseconds */

/**
 * @brief   Fragment intervals to identify limits of fragments.
 *
 * @note    Fragments MUST NOT overlap and overlapping fragments are to be
 *          discarded
 *
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 *
 * @internal
 */
typedef struct rbuf_int {
    struct rbuf_int *next;  /**< next element in interval list */
    uint16_t start;         /**< start byte of interval */
    uint16_t end;           /**< end byte of interval */
} rbuf_int_t;

/**
 * @brief   Internal representation of the 6LoWPAN reassembly buffer.
 *
 * Additional members help with correct reassembly of the buffer.
 *
 * @internal
 *
 * @extends gnrc_sixlowpan_rbuf_t
 */
typedef struct {
    gnrc_sixlowpan_rbuf_t super;        /**< exposed part of the reassembly buffer */
    rbuf_int_t *ints;                   /**< intervals of the fragment */
    uint32_t arrival;                   /**< time in microseconds of arrival of
                                         *   last received fragment */
} rbuf_t;

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

void rbuf_rm(rbuf_t *rbuf);

static inline bool rbuf_entry_empty(const rbuf_t *rbuf) {
    return (rbuf->super.pkt == NULL);
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
const rbuf_t *rbuf_array(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* RBUF_H */
/** @} */
