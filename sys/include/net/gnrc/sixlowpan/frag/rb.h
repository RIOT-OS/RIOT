/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef NET_GNRC_SIXLOWPAN_FRAG_RB_H
#define NET_GNRC_SIXLOWPAN_FRAG_RB_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
typedef struct gnrc_sixlowpan_rbuf_int {
    /**
     * @brief   next element in fragment interval list
     */
    struct gnrc_sixlowpan_rbuf_int *next;
    uint16_t start;             /**< start byte of the fragment interval */
    uint16_t end;               /**< end byte of the fragment interval */
} gnrc_sixlowpan_rbuf_int_t;

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
    gnrc_sixlowpan_rbuf_int_t *ints;            /**< intervals of already received fragments */
    uint8_t src[IEEE802154_LONG_ADDRESS_LEN];   /**< source address */
    uint8_t dst[IEEE802154_LONG_ADDRESS_LEN];   /**< destination address */
    uint8_t src_len;                            /**< length of gnrc_sixlowpan_rbuf_t::src */
    uint8_t dst_len;                            /**< length of gnrc_sixlowpan_rbuf_t::dst */
    uint16_t tag;                               /**< the datagram's tag */
    uint16_t datagram_size;                     /**< the datagram's size */
    /**
     * @brief   The number of bytes currently received of the complete datagram
     */
    uint16_t current_size;
    uint32_t arrival;                           /**< time in microseconds of arrival of
                                                 *   last received fragment */
} gnrc_sixlowpan_rbuf_base_t;

/**
 * @brief   An entry in the 6LoWPAN reassembly buffer.
 *
 * A recipient of a fragment SHALL use
 *
 */
typedef struct {
    gnrc_sixlowpan_rbuf_base_t super;           /**< base class */
    /**
     * @brief   The reassembled packet in the packet buffer
     */
    gnrc_pktsnip_t *pkt;
} gnrc_sixlowpan_rbuf_t;

/**
 * @brief   Remove base entry
 *
 * @param[in,out] entry Entry to remove
 */
void gnrc_sixlowpan_frag_rbuf_base_rm(gnrc_sixlowpan_rbuf_base_t *entry);

/**
 * @brief   Garbage collect reassembly buffer.
 */
void gnrc_sixlowpan_frag_rbuf_gc(void);

#if defined(MODULE_GNRC_SIXLOWPAN_FRAG) || defined(DOXYGEN)
/**
 * @brief   Removes an entry from the reassembly buffer
 *
 * @pre `rbuf != NULL`
 *
 * @param[in] rbuf  A reassembly buffer entry. Must not be NULL.
 */
void gnrc_sixlowpan_frag_rbuf_remove(gnrc_sixlowpan_rbuf_t *rbuf);

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
 */
void gnrc_sixlowpan_frag_rbuf_dispatch_when_complete(gnrc_sixlowpan_rbuf_t *rbuf,
                                                     gnrc_netif_hdr_t *netif);
#else
/* NOPs to be used with gnrc_sixlowpan_iphc if gnrc_sixlowpan_frag is not
 * compiled in */
#define gnrc_sixlowpan_frag_rbuf_remove(rbuf)       (void)(rbuf)
#define gnrc_sixlowpan_frag_rbuf_dispatch_when_complete(rbuf, netif) \
    (void)(rbuf); (void)(netif)
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SIXLOWPAN_FRAG_RB_H */
/** @} */
