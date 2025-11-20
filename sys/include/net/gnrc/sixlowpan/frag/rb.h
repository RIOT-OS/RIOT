/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup net_gnrc_sixlowpan_frag_rb 6LoWPAN reassembly buffer
 * @ingroup  net_gnrc_sixlowpan_frag
 * @brief    6LoWPAN reassembly buffer
 * @{
 *
 * @file
 * @brief   Reassembly buffer definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

#include "architecture.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pkt.h"
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_SFR
#include "net/sixlowpan/sfr.h"
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG_SFR */

#include "net/gnrc/sixlowpan/config.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief   Message type for triggering garbage collection reassembly buffer
 */
#define GNRC_SIXLOWPAN_FRAG_RB_GC_MSG       (0x0226)

/**
 * @brief   Fragment intervals to identify limits of fragments and duplicates.
 *
 * @note    Fragments MUST NOT overlap and overlapping fragments are to be
 *          discarded
 *
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 4944, section 5.3
 *      </a>
 */
typedef struct gnrc_sixlowpan_frag_rb_int {
    /**
     * @brief   next element in fragment interval list
     */
    struct gnrc_sixlowpan_frag_rb_int *next;
    uint16_t start;             /**< start byte of the fragment interval */
    uint16_t end;               /**< end byte of the fragment interval */
} gnrc_sixlowpan_frag_rb_int_t;

/**
 * @brief   Base class for both reassembly buffer and virtual reassembly buffer
 *
 * 1. the source address,
 * 2. the destination address,
 * 3. the datagram size, and
 * 4. the datagram tag
 *
 * to identify all fragments that belong to the given datagram.
 *
 * @see [RFC 4944, section 5.3](https://tools.ietf.org/html/rfc4944#section-5.3)
 * @see https://tools.ietf.org/html/draft-ietf-lwig-6lowpan-virtual-reassembly-01
 */
typedef struct {
    gnrc_sixlowpan_frag_rb_int_t *ints;         /**< intervals of already received fragments */
    uint8_t src[IEEE802154_LONG_ADDRESS_LEN];   /**< source address */
    uint8_t dst[IEEE802154_LONG_ADDRESS_LEN];   /**< destination address */
    uint8_t src_len;                            /**< length of gnrc_sixlowpan_frag_rb_t::src */
    uint8_t dst_len;                            /**< length of gnrc_sixlowpan_frag_rb_t::dst */
    uint16_t tag;                               /**< the datagram's tag */
    uint16_t datagram_size;                     /**< the datagram's size */
    /**
     * @brief   The number of bytes currently received of the complete datagram
     */
    uint16_t current_size;
    uint32_t arrival;                           /**< time in microseconds of arrival of
                                                 *   last received fragment */
} gnrc_sixlowpan_frag_rb_base_t;

/**
 * @brief   An entry in the 6LoWPAN reassembly buffer.
 *
 * A recipient of a fragment SHALL use
 *
 */
typedef struct {
    gnrc_sixlowpan_frag_rb_base_t super;        /**< base class */
    /**
     * @brief   The reassembled packet in the packet buffer
     */
    gnrc_pktsnip_t *pkt;
#if IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) || defined(DOXYGEN)
    /**
     * @brief   Bitmap for received fragments
     *
     * @note    Only available with module `gnrc_sixlowpan_frag_sfr` compiled
     *          in.
     */
    alignas(gnrc_sixlowpan_frag_sfr_bitmap_t)
    BITFIELD(received, SIXLOWPAN_SFR_ACK_BITMAP_SIZE);
    int8_t offset_diff;                         /**< offset change due to
                                                 *   recompression */
#endif /* IS_USED(MODULE_GNRC_SIXLOWPAN_FRAG_SFR) */
} gnrc_sixlowpan_frag_rb_t;

/**
 * @brief   Adds a new fragment to the reassembly buffer. If the packet is
 *          complete, dispatch the packet with the transmit information of
 *          the last fragment.
 *
 * @param[in] netif_hdr     The interface header of the fragment, with
 *                          gnrc_netif_hdr_t::if_pid and its source and
 *                          destination address set.
 * @param[in] frag          The fragment to add. Will be released by the
 *                          function.
 * @param[in] offset        The fragment's offset.
 * @param[in] page          Current 6Lo dispatch parsing page.
 *
 * @return  The reassembly buffer entry the fragment was added to on success.
 * @return  NULL on error.
 */
gnrc_sixlowpan_frag_rb_t *gnrc_sixlowpan_frag_rb_add(gnrc_netif_hdr_t *netif_hdr,
                                                     gnrc_pktsnip_t *frag,
                                                     size_t offset, unsigned page);

/**
 * @brief   Gets a reassembly buffer entry with a given link-layer address
 *          pair and tag.
 *
 * @pre     `netif_hdr != NULL`
 *
 * @param[in] netif_hdr An interface header to provide the (source, destination)
 *                      link-layer address pair. Must not be NULL.
 * @param[in] tag       Tag to search for.
 *
 * @note    datagram_size is not a search parameter as the primary use case
 *          for this function is [Selective Fragment Recovery]
 *          (https://tools.ietf.org/html/rfc8931) where this information only
 *          exists in the first fragment.
 *
 * @return  The reassembly buffer entry identified by the source and destination
 *          address in the @p netif_hdr and @p tag, if any such entry exist.
 * @return  NULL, if no entry with the given identifying tuple exist.
 */
gnrc_sixlowpan_frag_rb_t *gnrc_sixlowpan_frag_rb_get_by_datagram(
    const gnrc_netif_hdr_t *netif_hdr,
    uint16_t tag);

/**
 * @brief   Checks if a reassembly buffer entry with a given link-layer address
 *          pair and tag exists
 *
 * @pre     `netif_hdr != NULL`
 *
 * @param[in] netif_hdr An interface header to provide the (source, destination)
 *                      link-layer address pair. Must not be NULL.
 * @param[in] tag       Tag to search for.
 *
 * @note    datagram_size is not a search parameter as the primary use case
 *          for this function is [Selective Fragment Recovery]
 *          (https://tools.ietf.org/html/rfc8931) where this information only
 *          exists in the first fragment.
 *
 * @return  true, if an entry with the given tuple exist.
 * @return  false, if no entry with the given tuple exist.
 */
bool gnrc_sixlowpan_frag_rb_exists(const gnrc_netif_hdr_t *netif_hdr,
                                   uint16_t tag);

/**
 * @brief   Removes a reassembly buffer entry with a given link-layer address
 *          pair and tag
 *
 * @pre     `netif_hdr != NULL`
 *
 * @param[in] netif_hdr An interface header to provide the (source, destination)
 *                      link-layer address pair. Must not be NULL.
 * @param[in] tag       Tag to search for.
 *
 * @note    datagram_size is not a search parameter as the primary use case
 *          for this function is [Selective Fragment Recovery]
 *          (https://tools.ietf.org/html/rfc8931) where this information only
 *          exists in the first fragment.
 */
void gnrc_sixlowpan_frag_rb_rm_by_datagram(const gnrc_netif_hdr_t *netif_hdr,
                                           uint16_t tag);

/**
 * @brief   Checks if a reassembly buffer entry is unset
 *
 * @param[in] rbuf  A reassembly buffer entry
 *
 * @return  true, if @p rbuf is empty (i.e. rbuf->super.pkt is NULL).
 * @return  false, if @p rbuf is in use.
 */
static inline bool gnrc_sixlowpan_frag_rb_entry_empty(
        const gnrc_sixlowpan_frag_rb_t *rbuf) {
    return (rbuf->pkt == NULL);
}

#if defined(TEST_SUITES) || defined(DOXYGEN)
/**
 * @brief   Resets the packet buffer to a clean state
 *
 * @note    Only available when @ref TEST_SUITES is defined
 */
void gnrc_sixlowpan_frag_rb_reset(void);

/**
 * @brief   Returns a pointer to the array representing the reassembly buffer.
 *
 * @note    Only available when @ref TEST_SUITES is defined
 *
 * @return  The first element of the reassembly buffer. `const`, so that write
 *          access is immediately spotted at compile time of tests. The `const`
 *          qualifier may however be discarded if required by the tests.
 */
const gnrc_sixlowpan_frag_rb_t *gnrc_sixlowpan_frag_rb_array(void);
#endif

/**
 * @brief   Remove base entry
 *
 * @param[in,out] entry Entry to remove
 */
void gnrc_sixlowpan_frag_rb_base_rm(gnrc_sixlowpan_frag_rb_base_t *entry);

/**
 * @brief   Garbage collect reassembly buffer.
 */
void gnrc_sixlowpan_frag_rb_gc(void);

/**
 * @brief   Checks if a reassembly buffer entry is complete and dispatches it
 *          to the next layer if that is the case
 *
 * @pre `rbuf != NULL`
 * @pre `netif != NULL`
 *
 * @param[in] rbuf  A reassembly buffer entry. Must not be NULL.
 * @param[in] netif Original @ref gnrc_netif_hdr_t of the last received frame.
 *                  Used to construct the @ref gnrc_netif_hdr_t of the completed
 *                  datagram. Must not be NULL.
 *
 * @return  >0, when the datagram in @p rbuf was complete and dispatched.
 * @return  0, when the datagram in @p rbuf is not complete.
 * @return  -1, if the reassembled datagram was not dispatched. @p rbuf is
 *          destroyed either way.
 */
int gnrc_sixlowpan_frag_rb_dispatch_when_complete(gnrc_sixlowpan_frag_rb_t *rbuf,
                                                  gnrc_netif_hdr_t *netif);

#if defined(MODULE_GNRC_SIXLOWPAN_FRAG_RB) || defined(DOXYGEN)
/**
 * @brief   Unsets a reassembly buffer entry (but does not free
 *          rbuf_t::super::pkt)
 *
 * @pre `rbuf != NULL`
 *
 * This functions sets rbuf_t::super::pkt to NULL and removes all rbuf::ints.
 *
 * @note    Does nothing if module `gnrc_sixlowpan_frag_rb` is not included.
 *
 * @param[in] rbuf  A reassembly buffer entry. Must not be NULL.
 */
static inline void gnrc_sixlowpan_frag_rb_remove(gnrc_sixlowpan_frag_rb_t *rbuf)
{
    assert(rbuf != NULL);
    gnrc_sixlowpan_frag_rb_base_rm(&rbuf->super);
    rbuf->pkt = NULL;
}
#else
/* NOPs to be used with gnrc_sixlowpan_iphc if gnrc_sixlowpan_frag_rb is not
 * compiled in */
static inline void gnrc_sixlowpan_frag_rb_remove(gnrc_sixlowpan_frag_rb_t *rbuf)
{
    (void)rbuf;
    return;
}
#endif

#if defined(TEST_SUITES) || defined(DOXYGEN)
/**
 * @brief   Check if pool of fragment intervals is empty
 *
 * @see     @ref gnrc_sixlowpan_frag_rb_int_t
 * @note    Returns only non-true values if @ref TEST_SUITES is defined.
 *
 * @return  true, if pool of fragment intervals is empty
 * @return  false, if pool of fragment intervals is not empty
 */
bool gnrc_sixlowpan_frag_rb_ints_empty(void);
#else   /* defined(TEST_SUITES) || defined(DOXYGEN) */
/* always true without TEST_SUITES defined to optimize out when not testing,
 * as checking the status of the fragment interval pool is unnecessary in
 * production */
static inline bool gnrc_sixlowpan_frag_rb_ints_empty(void)
{
    return true;
}
#endif  /* defined(TEST_SUITES) || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

/** @} */
