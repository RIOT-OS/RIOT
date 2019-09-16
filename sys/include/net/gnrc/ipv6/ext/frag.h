/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_ext_frag Support for IPv6 fragmentation extension
 * @ingroup     net_gnrc_ipv6_ext
 * @brief       GNRC implementation of IPv6 fragmentation extension
 * @{
 *
 * @file
 * @brief   GNRC fragmentation extension definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_IPV6_EXT_FRAG_H
#define NET_GNRC_IPV6_EXT_FRAG_H

#include <stdint.h>

#include "clist.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"
#include "net/ipv6/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message type to time reassembly buffer garbage collection
 */
#define GNRC_IPV6_EXT_FRAG_RBUF_GC  (0xfe00U)

/**
 * @brief   Data type to describe limits of a single fragment in the reassembly
 *          buffer
 */
typedef struct gnrc_ipv6_ext_frag_limits {
    struct gnrc_ipv6_ext_frag_limits *next; /**< limits of next fragment */
    uint16_t start;                         /**< the start (= offset) of the fragment */
    uint16_t end;                           /**< the exclusive end (= offset + length) of the
                                             *   fragment */
} gnrc_ipv6_ext_frag_limits_t;

/**
 * @brief   A reassembly buffer entry
 */
typedef struct {
    gnrc_pktsnip_t *pkt;    /**< the (partly) reassembled packet */
    ipv6_hdr_t *ipv6;       /**< the IPv6 header of gnrc_ipv6_ext_frag_rbuf_t::pkt */
    /**
     * @brief   The limits of the fragments in the reassembled packet
     *
     * @note    Members of this list can be cast to gnrc_ipv6_ext_frag_limits_t.
     */
    clist_node_t limits;
    uint32_t id;            /**< the identification from the fragment headers */
    uint32_t arrival;       /**< arrival time of last received fragment */
    uint16_t pkt_len;       /**< length of gnrc_ipv6_ext_frag_rbuf_t::pkt */
    uint8_t last;           /**< received last fragment */
} gnrc_ipv6_ext_frag_rbuf_t;

/**
 * @brief   Initializes IPv6 fragmentation and reassembly
 * @internal
 */
void gnrc_ipv6_ext_frag_init(void);

/**
 * @brief   Reassemble fragmented IPv6 packet
 *
 * @param[in] pkt   A fragment of the IPv6 packet to be reassembled containing
 *                  the fragment header in the first snip.
 *
 * @return  The reassembled packet when @p pkt completed the reassembly
 * @return  NULL, when there are still fragments missing or an error occured
 *          during reassembly
 */
gnrc_pktsnip_t *gnrc_ipv6_ext_frag_reass(gnrc_pktsnip_t *pkt);

/**
 * @name    Reassembly buffer operations
 * @{
 */
/**
 * @brief   Get a reassembly buffer by the identifying parameters
 *
 * @internal
 * @see     [RFC 8200, section 4.5](https://tools.ietf.org/html/rfc8200#section-4.5)
 *
 * @param[in] hdr   IPv6 header to get source and destination address from.
 * @param[in] id    The identification from the fragment header.
 *
 * @return  A reassembly buffer matching @p id ipv6_hdr_t::src and ipv6_hdr::dst
 *          of @p hdr or first free reassembly buffer. Will never be NULL, as
 *          in the case of the reassembly buffer being full, the entry with the
 *          lowest gnrc_ipv6_ext_frag_rbuf_t::arrival (serial-number-like) is
 *          removed.
 */
gnrc_ipv6_ext_frag_rbuf_t *gnrc_ipv6_ext_frag_rbuf_get(ipv6_hdr_t *ipv6,
                                                       uint32_t id);

/**
 * @brief   Frees a reassembly buffer entry (but does not release its
 *          gnrc_ipv6_ext_frag_rbuf_t::pkt)
 *
 * @param[in] rbuf  A reassembly buffer entry.
 */
void gnrc_ipv6_ext_frag_rbuf_free(gnrc_ipv6_ext_frag_rbuf_t *rbuf);

/**
 * @brief   Delete a reassembly buffer entry (and release its
 *          gnrc_ipv6_ext_frag_rbuf_t::pkt)
 *
 * @note    May be used by the IPv6 thread to remove a timed out reassembly
 *          buffer entry.
 *
 * @param[in] rbuf  A reassembly buffer entry.
 */
static inline void gnrc_ipv6_ext_frag_rbuf_del(gnrc_ipv6_ext_frag_rbuf_t *rbuf)
{
    gnrc_pktbuf_release(rbuf->pkt);
    rbuf->pkt = NULL;
    gnrc_ipv6_ext_frag_rbuf_free(rbuf);
}

/**
 * @brief   Garbage-collect reassembly buffer
 *
 * This calls @ref gnrc_ipv6_ext_frag_rbuf_del() for all reassembly buffer
 * entries for which * gnrc_ipv6_ext_frag_rbuf_t::arrival is
 * @ref GNRC_IPV6_EXT_FRAG_RBUF_TIMEOUT_US in the past.
 */
void gnrc_ipv6_ext_frag_rbuf_gc(void);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_EXT_FRAG_H */
/** @} */
