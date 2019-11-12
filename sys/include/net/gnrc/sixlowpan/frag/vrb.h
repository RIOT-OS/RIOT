/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sixlowpan_frag_vrb Virtual reassembly buffer
 * @ingroup     net_gnrc_sixlowpan_frag
 * @brief       Virtual reassembly buffer
 * @{
 *
 * @file
 * @brief       Virtual reassembly buffer definitions
 * @see         https://tools.ietf.org/html/draft-ietf-lwig-6lowpan-virtual-reassembly-01
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_SIXLOWPAN_FRAG_VRB_H
#define NET_GNRC_SIXLOWPAN_FRAG_VRB_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "net/gnrc/netif.h"
#include "net/gnrc/sixlowpan/config.h"
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG
#include "net/gnrc/sixlowpan/frag.h"
#endif /* MODULE_GNRC_SIXLOWPAN_FRAG */
#include "net/gnrc/sixlowpan/frag/rb.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Representation of the virtual reassembly buffer entry
 *
 * gnrc_sixlowpan_frag_rb_base_t::dst of gnrc_sixlowpan_frag_vrb_t::super
 * becomes the next hop destination address.
 */
typedef struct {
    gnrc_sixlowpan_frag_rb_base_t super;    /**< base type */
    /**
     * @brief   Outgoing interface to gnrc_sixlowpan_frag_rb_base_t::dst
     */
    gnrc_netif_t *out_netif;
    /**
     * @brief   Outgoing tag to gnrc_sixlowpan_frag_rb_base_t::dst
     */
    uint16_t out_tag;
} gnrc_sixlowpan_frag_vrb_t;

/**
 * @brief   Adds a new reassembly buffer entry
 *
 * @param[in] base          Base data of the datagram. Must not be `NULL`.
 * @param[in] out_netif     Network interface that is out-going to @p out_dst.
 * @param[in] out_dst       Link-layer destination address to which to forward
 *                          fragments identified by @p base. Must not be `NULL`.
 * @param[in] out_dst_len   Length of @p out_dst. Must be greater than 0.
 *
 * @pre `base != NULL`
 * @pre `out_dst != NULL`
 * @pre `out_dst_len > 0`
 *
 * @return  A new VRB entry.
 * @return  NULL, if VRB is full.
 */
gnrc_sixlowpan_frag_vrb_t *gnrc_sixlowpan_frag_vrb_add(
        const gnrc_sixlowpan_frag_rb_base_t *base,
        gnrc_netif_t *out_netif, const uint8_t *out_dst, size_t out_dst_len);

/**
 * @brief   Generate reassembly buffer from a header's forwarding information.
 *
 * @param[in] base  Base data of the datagram. Must not be `NULL`.
 * @param[in] netif Restict route to this interface. May be `NULL` for any
 *                  interface.
 * @param[in] hdr   Header from which to take the forwarding information from
 *                  (e.g. IPv6 header implies `hdr->type == GNRC_NETTYPE_IPV6`).
 *
 * @pre `base != NULL`
 * @pre `(hdr != NULL) && (hdr->data != NULL) && (hdr->size > 0)`
 *
 * @return  The VRB entry pointing to the next hop based on the forwarding
 *          information provided in @p hdr and present in the respective
 *          forwarding information base for `hdr->type`.
 * @return  NULL, if VRB is full or if there is no route to destination in
 *          @p hdr.
 */
gnrc_sixlowpan_frag_vrb_t *gnrc_sixlowpan_frag_vrb_from_route(
            const gnrc_sixlowpan_frag_rb_base_t *base,
            gnrc_netif_t *netif, const gnrc_pktsnip_t *hdr);

/**
 * @brief   Checks timeouts and removes entries if necessary
 */
void gnrc_sixlowpan_frag_vrb_gc(void);

/**
 * @brief   Gets a VRB entry
 *
 * @param[in] src           Link-layer source address of the original fragment.
 * @param[in] src_len       Length of @p src.
 * @param[in] src_tag       Tag of the original fragment.
 *
 * @return  The VRB entry identified by the given parameters.
 * @return  NULL, if there is no entry in the VRB that could be identified
 *          by the given parameters.
 */
gnrc_sixlowpan_frag_vrb_t *gnrc_sixlowpan_frag_vrb_get(
        const uint8_t *src, size_t src_len, unsigned src_tag);

/**
 * @brief   Removes an entry from the VRB
 *
 * @param[in] vrb   A VRB entry
 */
static inline void gnrc_sixlowpan_frag_vrb_rm(gnrc_sixlowpan_frag_vrb_t *vrb)
{
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG
    gnrc_sixlowpan_frag_rb_base_rm(&vrb->super);
#endif  /* MODULE_GNRC_SIXLOWPAN_FRAG */
    vrb->super.src_len = 0;
}

/**
 * @brief   Determines if a VRB entry is empty
 *
 * @param[in] vrb   A VRB entry
 *
 * @return  true, if @p vrb entry is empty.
 * @return  false, if @p vrb entry is not empty.
 */
static inline bool gnrc_sixlowpan_frag_vrb_entry_empty(gnrc_sixlowpan_frag_vrb_t *vrb)
{
    return (vrb->super.src_len == 0);
}

#if defined(TEST_SUITES) || defined(DOXYGEN)
/**
 * @brief   Resets the VRB to a clean state
 *
 * @note    Only available when @ref TEST_SUITES is defined
 */
void gnrc_sixlowpan_frag_vrb_reset(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SIXLOWPAN_FRAG_VRB_H */
/** @} */
