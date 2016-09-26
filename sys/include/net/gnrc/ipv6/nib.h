/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup net_gnrc_ipv6_nib  IPv6 network information base
 * @ingroup net_gnrc_ipv6
 * @brief This data structure is a combination of an IPv6 neighbor cache,
 *        destination cache, prefix list, default router list, forwarding table,
 *        and 6LoWPAN compression context context table
 *
 * For the cache entries (neighbor cache, destination cache) this data structure
 * uses a CLOCK algorithm to swap out entries if no space is left. However,
 * this only applies for entries that are just part of these caches.
 * Also note, that on a 6LoWPAN router (and border router) the neighbor cache
 * [does not behave like a cache](https://tools.ietf.org/html/rfc6775#page-11)
 * but a registry, so the caching algorithm does not apply here either.
 *
 * @{
 *
 * @file
 * @brief   NIB definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef GNRC_IPV6_NIB_H_
#define GNRC_IPV6_NIB_H_

#include <stdint.h>

#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/netif/hdr.h"
#include "net/ndp.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GNRC_IPV6_NIB_SIZE
#if !defined(MODULE_GNRC_IPV6_ROUTER) || defined(DOXYGEN)
#define GNRC_IPV6_NIB_SIZE  (32U)   /**< size of the NIB. */
#else
#define GNRC_IPV6_NIB_SIZE  (2U)    /* even 1 just for default route should
                                     * suffice, but RFC 4861 suggests at least 2
                                     * (https://tools.ietf.org/html/rfc4861#section-5.3) */
#endif

/**
 * @brief   States for neighbor unreachability detection
 *
 * @see [RFC 4861, section 7.3.2](https://tools.ietf.org/html/rfc4861#section-7.3.2)
 * @see [RFC 7048](https://tools.ietf.org/html/rfc7048)
 */
typedef enum {
    GNRC_IPV6_NIB_NUD_STATE_UNMANAGED = 0,  /**< not managed by NUD */
    GNRC_IPV6_NIB_NUD_STATE_UNREACHABLE,    /**< entry is not reachable */
    GNRC_IPV6_NIB_NUD_STATE_INCOMPLETE,     /**< address resolution is currently performed */
    GNRC_IPV6_NIB_NUD_STATE_STALE,          /**< address might not be reachable */
    GNRC_IPV6_NIB_NUD_STATE_DELAY,          /**< NUD will be performed in a moment */
    GNRC_IPV6_NIB_NUD_STATE_PROBE,          /**< NUD is performed */
    GNRC_IPV6_NIB_NUD_STATE_REACHABLE,      /**< entry is reachable */
} gnrc_ipv6_nib_nud_state_t;

/**
 * @brief States for 6LoWPAN address registration (6Lo-AR)
 */
typedef enum {
    GNRC_IPV6_NIB_AR_STATE_NONE = 0,        /**< not managed by 6Lo-AR */
    GNRC_IPV6_NIB_AR_STATE_GC,              /**< address can be removed when low on memory */
    GNRC_IPV6_NIB_AR_STATE_TENTATIVE,       /**< address registration still pending */
    GNRC_IPV6_NIB_AR_STATE_REGISTERED,      /**< address is registered */
} gnrc_ipv6_nib_ar_state_t;

/**
 * @brief   A public NIB entry
 */
typedef struct {
    const ipv6_addr_t dst;          /**< A destination or prefix to destination */
    const ipv6_addr_t next_hop;     /**< Next hop to gnrc_ipv6_nib_t::dst */
#if defined(MODULE_GNRC_SIXLOWPAN_ND_ROUTER) || defined(DOXYGEN)
    /**
     * @brief   The neighbors EUI-64 (used for DAD)
     *
     * 
     */
    eui64_t eui64;
#endif
    const uint8_t l2addr[GNRC_NETIF_HDR_L2ADDR_MAX_LEN];
                                    /**< L2 address of next hop */
    const unsigned pfx_len : 8;     /**< prefix-length of gnrc_ipv6_nib_t::dst */
    const unsigned l2addr_len : 4;  /**< length of gnrc_ipv6_nib_t::l2addr */
    const gnrc_ipv6_nib_nud_state_t nud_state : 3;
                                    /**< NUD state of gnrc_ipv6_nib_t::next_hop */
    const bool is_router : 1;       /**< gnrc_ipv6_nib_t::next_hop is router */
    const unsigned iface : 5;       /**< interface to gnrc_ipv6_nib_t::dst */
    const gnrc_ipv6_nib_ar_state_t ar_state : 2;
                                    /**< 6Lo-AR state of gnrc_ipv6_nib_t::next_hop */
    /**
     * @brief   Use gnrc_ipv6_nib_t::cid for compression of gnrc_ipv6_nib_t::dst
     *          when using @ref net_gnrc_sixlowpan_iphc
     */
    const bool use_for_comp : 1;

    /**
     * @brief   Context identifier for gnrc_ipv6_nib_t::dst for stateful header
     *          compression using @ref net_gnrc_sixlowpan_iphc
     */
    const unsigned cid : 4;
} gnrc_ipv6_nib_t;

/**
 * @brief   Gets next reachable hop to @p dst
 *
 * @param[in] iface Interface to restrict search to search for next reachable
 *                  hop to. May be KERNEL_PID_UNDEF for any interface.
 * @param[in] dst   The destination address of @p pkt.
 * @param[in] pkt   The Packet
 *
 * @return The NIB to the reachable entry on success.
 * @return NULL, if no reachable next hop was found.
 */
const gnrc_ipv6_nib_t *gnrc_ipv6_nib_next_hop(unsigned iface,
                                              const ipv6_addr_t *dst,
                                              const gnrc_pktsnip_t *pkt);

/**
 * @brief   Adds an unmanaged neighbor entry to NIB
 *
 * @pre `(ipv6 != NULL) && (l2addr != NULL)`
 * @pre `l2addr_len <= GNRC_NETIF_HDR_L2ADDR_MAX_LEN`
 * @pre `iface <= KERNEL_PID_LAST`
 *
 * @param[in] iface         The interface to the neighbor.
 * @param[in] ipv6          The neighbor's IPv6 address.
 * @param[in] l2addr        The neighbor's L2 address.
 * @param[in] l2addr_len    Length of @p l2addr.
 *
 * A neighbor cache entry created this way is marked as persistent.
 * Also, a non-persistent neighbor or destination cache entry already in the
 * NIB might be removed to make room for the new entry.
 * If an entry pointing to the same IPv6 address as @p ipv6 exists already it
 * will be overwritten and marked as unmanaged.
 *
 * @return  The (created) NIB entry on success.
 * @return  NULL, if neighbor cache is full and no replacement can be used.
 */
const gnrc_ipv6_nib_t *gnrc_ipv6_nib_set_neighbor(unsigned iface,
                                                  const ipv6_addr_t *ipv6,
                                                  const uint8_t *l2addr,
                                                  size_t l2addr_len);

/**
 * @brief   Deletes neighbor with address @p ipv6 from NIB
 *
 * @pre `ipv6 != NULL`
 *
 * @param[in] ipv6 The neighbor's IPv6 address.
 *
 * If the @p ipv6 can't be found for a neighbor in the NIB nothing happens.
 */
void gnrc_ipv6_nib_delete_neighbor(const ipv6_addr_t *ipv6);

/**
 * @brief   Mark neighbor with address @p ipv6 as reachable
 *
 * @param[in] ipv6 A neighbor's IPv6 address.
 *
 * This function shall be called if an upper layer gets reachability confirmation
 * via its own means (e.g. a TCP connection build-up). Unmanaged neighbor cache
 * entries or entries which next-hop are not in the neighbor cache yet are
 * ignored.
 */
void gnrc_ipv6_nib_mark_reachable(const ipv6_addr_t *ipv6);

/**
 * @brief   Iterates over all neighbor cache entries in NIB
 *
 * @param[in] prev  Previous neighbor cache entry.
 *                  May be NULL to start iteration.
 *
 * @return  Neighbor cache entry after @p prev.
 * @return  NULL if @p prev is the last neighbor cache entry in NIB.
 */
const gnrc_ipv6_nib_t *gnrc_ipv6_nib_iter_nc(const gnrc_ipv6_nib_t *prev);

/**
 * @brief   Adds or updates a prefix to manage by the NIB
 *
 * @pre (iface > KERNEL_PID_UNDEF)
 * @pre (pfx != NULL && pfx_len != 0 && pfx_len < 128)
 *
 * @param[in] iface     Interface the prefix was added to.
 * @param[in] pfx       The prefix.
 * @param[in] pfx_len   Length of @p pfx
 * @param[in] valid     Valid lifetime in seconds (`UINT32_MAX` for infinite
 *                      lifetime).
 * @param[in] pref      Preferred lifetime in seconds (`UINT32_MAX` for infinite
 *                      lifetime).
 *
 * This change effects the prefix list and thus may also trigger an unsolicited
 * router advertisement, if this is possible within the rate-limiting
 * constraints or restrictions specified in
 * [RFC 4861](https://tools.ietf.org/html/rfc4861) and
 * [RFC 6775](https://tools.ietf.org/html/rfc6775)
 *
 * @return  The (created) NIB entry on success.
 * @return  NULL, if prefix list is full.
 */
const gnrc_ipv6_nib_t *gnrc_ipv6_nib_set_prefix(unsigned iface,
                                                const ipv6_addr_t *pfx,
                                                uint8_t pfx_len,
                                                uint32_t valid
                                                uint32_t pref);

/**
 * @brief   Adds a route to the NIB
 *
 * @pre (iface > KERNEL_PID_UNDEF)
 * @pre (if dst != NULL: pfx_len > 0)
 * @pte (if dst == NULL: next_hop != NULL)
 * @pte (if next_hop == NULL: dst != NULL)
 *
 * @param[in] iface     Interface the route goes over.
 * @param[in] dst       Destination of the route.
 *                      May be NULL for default route.
 * @param[in] pfx_len   Prefix length of @p dst if it is a prefix.
 *                      May be 0 for default route.
 * @param[in] next_hop  Next hop to @p dst. May be NULL to just forward over
 *                      @p iface.
 *
 * If `dst != NULL`, `pfx_len < 128`, and `next_hop == 0` the entry is added to
 * the prefix list so this may also trigger an unsolicited router advertisement,
 * if this is possible within the rate-limiting constraints or restrictions
 * specified in
 * [RFC 4861](https://tools.ietf.org/html/rfc4861) and
 * [RFC 6775](https://tools.ietf.org/html/rfc6775). The lifetimes required in
 * @ref gnrc_ipv6_nib_set_prefix() are assumed to be infinite.
 *
 * @return  The (created) NIB entry on success.
 * @return  NULL, if forwarding table is full.
 */
const gnrc_ipv6_nib_t *gnrc_ipv6_nib_add_route(unsigned iface,
                                               const ipv6_addr_t *dst,
                                               uint8_t pfx_len,
                                               const ipv6_addr_t *next_hop);

/**
 * @brief   Deletes all occurrences of a route to the NIB
 *
 * @pre (if dst != NULgL: pfx_len > 0)
 *
 * @param[in] dst       Destination of the route.
 *                      May be NULL for default route.
 * @param[in] pfx_len   Prefix length of @p dst if it is a prefix.
 *                      May be 0 for default route.
 *
 * If this change effects the prefix list this may also trigger an unsolicited
 * router advertisement, if this is possible within the rate-limiting
 * constraints or restrictions specified in
 * [RFC 4861](https://tools.ietf.org/html/rfc4861) and
 * [RFC 6775](https://tools.ietf.org/html/rfc6775)
 */
void gnrc_ipv6_nib_del_route(const ipv6_addr_t *dst, uint8_t pfx_len);

/**
 * @brief   Iterates over all destination cache entries in the NIB
 *
 * @param[in] prev  Previous destination cache entry.
 *                  May be NULL to start iteration.
 *
 * @return  Destination cache entry after @p prev.
 * @return  NULL if @p prev is the last destination cache entry in NIB.
 */
const gnrc_ipv6_nib_t *gnrc_ipv6_nib_iter_dc(const gnrc_ipv6_nib_t *prev);

/**
 * @brief   Iterates over all prefix list entries in the NIB
 *
 * @param[in] prev  Previous prefix list entry.
 *                  May be NULL to start iteration.
 *
 * @return  Prefix list entry entry after @p prev.
 * @return  NULL if @p prev is the last prefix list entry in NIB.
 */
const gnrc_ipv6_nib_t *gnrc_ipv6_nib_iter_pl(const gnrc_ipv6_nib_t *prev);

/**
 * @brief   Iterates over all default router list entries in the NIB
 *
 * @param[in] prev  Previous default router list entry.
 *                  May be NULL to start iteration.
 *
 * @return  Default router list entry entry after @p prev.
 * @return  NULL if @p prev is the last default router list entry in NIB.
 */
const gnrc_ipv6_nib_t *gnrc_ipv6_nib_iter_drl(const gnrc_ipv6_nib_t *prev);

/**
 * @brief   Iterates over all forwarding table entries in the NIB
 *
 * @param[in] prev  Previous forwarding table entry.
 *                  May be NULL to start iteration.
 *
 * Other than the other iterators this one not only includes the entries
 * explicitly added to the forwarding table using @ref gnrc_ipv6_nib_add_route(),
 * but also the destination cache, prefix list, and default router list entries
 * that were automatically by the neighbor discovery or manually added with
 * @ref gnrc_ipv6_nib_set_prefix().
 *
 * @return  Previous forwarding table entry entry after @p prev.
 * @return  NULL if @p prev is the last forwarding table entry in NIB.
 */
const gnrc_ipv6_nib_t *gnrc_ipv6_nib_iter_ft(const gnrc_ipv6_nib_t *prev);

/**
 * @brief   Adds 6LoWPAN compression context to the NIB
 *
 * @pre (iface > KERNEL_PID_UNDEF)
 * @pre (pfx != NULL && pfx_len != 0)
 *
 * @param[in] iface     Interface the context is valid for.
 * @param[in] pfx       Prefix the context compresses.
 * @param[in] pfx_len   Length of @p pfx
 * @param[in] valid     Valid lifetime of the context in seconds.
 * @param[in] cid       The context identifier.
 *
 * This change may trigger an unsolicited router advertisement, if this is
 * possible within the rate-limiting constraints or restrictions specified in
 * [RFC 4861](https://tools.ietf.org/html/rfc4861) and
 * [RFC 6775](https://tools.ietf.org/html/rfc6775)
 *
 * @return  The (created) NIB entry on success.
 */
const gnrc_ipv6_nib_t *gnrc_ipv6_nib_add_6lo_ctx(unsigned iface,
                                                 const ipv6_addr_t *pfx,
                                                 uint8_t pfx_len,
                                                 uint32_t valid;
                                                 uint8_t cid);

/**
 * @brief   Iterates over all 6LoWPAN compression context table entries in the
 *          NIB
 *
 * @param[in] prev  Previous 6LoWPAN compression context table entry.
 *                  May be NULL to start iteration.
 *
 * @return  6LoWPAN compression context table entry after @p prev.
 * @return  NULL if @p prev is the last 6LoWPAN compression context table entry
 *          in NIB.
 */
const gnrc_ipv6_nib_t *gnrc_ipv6_nib_iter_ctx(const gnrc_ipv6_nib_t *prev);

/**
 * @brief   Handles a neighbor solicitation message and all its options and adds
 *          to the NIB accordingly
 *
 * Additional behavior not specified in RFC: if the link-layer does not have
 * link-layer addresses, a neighbor cache entry is created for ipv6_hdr_t::src
 * of @p ipv6_hdr with gnrc_ipv6_nib_t::l2addr_len := 0 (and set to 
 * @ref GNRC_IPV6_NIB_NUD_STATE_STALE), even if the neighbor solicitation does
 * not contain a SLLAO.
 *
 * @see [RFC 4861, section 7.1.1](https://tools.ietf.org/html/rfc4861#section-7.1.1)
 * @see [RFC 4861, section 7.2.3](https://tools.ietf.org/html/rfc4861#section-7.2.3)
 * @see [RFC 4862, section 5.4.3](https://tools.ietf.org/html/rfc4862#section-5.4.3)
 * @see [RFC 6775, section 6.5](https://tools.ietf.org/html/rfc6775#section-6.5)
 *
 * @param[in] iface         Interface the neighbor solicitation came over.
 * @param[in] ipv6_hdr      The IPv6 header of the neighbor solicitation.
 * @param[in] nbr_sol       The neighbor solicitation to handle.
 * @param[in] nbr_sol_len   Length of @p nbr_sol.
 */
void gnrc_ipv6_nib_handle_nbr_sol(unsigned iface, const ipv6_hdr_t *ipv6_hdr,
                                  const ndp_nbr_sol_t *nbr_sol,
                                  size_t nbr_sol_len);

/**
 * @brief   Handles a neighbor advertisement message and all its options and
 *          adds to the NIB accordingly
 *
 * @see [RFC 4861, section 7.1.2](https://tools.ietf.org/html/rfc4861#section-7.1.2)
 * @see [RFC 4861, section 7.2.5](https://tools.ietf.org/html/rfc4861#section-7.2.5)
 * @see [RFC 4862, section 5.4.4](https://tools.ietf.org/html/rfc4862#section-5.4.4)
 * @see [RFC 6775, section 5.5.2](https://tools.ietf.org/html/rfc6775#section-5.5.2)
 *
 * @param[in] iface         Interface the neighbor advertisement came over.
 * @param[in] ipv6_hdr      The IPv6 header of the neighbor advertisement.
 * @param[in] nbr_adv       The neighbor advertisement to handle.
 * @param[in] nbr_adv_len   Length of @p nbr_adv.
 */
void gnrc_ipv6_nib_handle_nbr_adv(unsigned iface, const ipv6_hdr_t *ipv6_hdr,
                                  const ndp_nbr_adv_t *nbr_adv,
                                  size_t nbr_adv_len);


/**
 * @brief   Handles a router solicitation message and all its options and adds
 *          to the NIB accordingly
 *
 * Additional behavior not specified in RFC: if the link-layer does not have
 * link-layer addresses, a neighbor cache entry is created for ipv6_hdr_t::src
 * of @p ipv6_hdr with gnrc_ipv6_nib_t::l2addr_len := 0 (and set to
 * @ref GNRC_IPV6_NIB_NUD_STATE_STALE), even if the router solicitation does
 * not contain a SLLAO.
 *
 * @see [RFC 4861, section 6.1.1](https://tools.ietf.org/html/rfc4861#section-6.1.1)
 * @see [RFC 4861, section 6.2.6](https://tools.ietf.org/html/rfc4861#section-6.2.6)
 * @see [RFC 6775, section 6.3](https://tools.ietf.org/html/rfc6775#section-6.3)
 *
 * @param[in] iface         Interface the router solicitation came over.
 * @param[in] ipv6_hdr      The IPv6 header of the router solicitation.
 * @param[in] rtr_sol       The router solicitation to handle.
 * @param[in] rtr_sol_len   Length of @p rtr_sol.
 */
void gnrc_ipv6_nib_handle_rtr_sol(unsigned iface, const ipv6_hdr_t *ipv6_hdr,
                                  const ndp_rtr_sol_t *rtr_sol,
                                  size_t rtr_sol_len);

/**
 * @brief   Handles a router advertisement message and all its options and adds
 *          to the NIB accordingly
 *
 * Additional behavior not specified in RFC: if the link-layer does not have
 * link-layer addresses, a neighbor cache entry is created for ipv6_hdr_t::src
 * of @p ipv6_hdr with gnrc_ipv6_nib_t::l2addr_len := 0 (and set to
 * @ref GNRC_IPV6_NIB_NUD_STATE_STALE), even if the router advertisement does
 * not contain a SLLAO.
 *
 * @see [RFC 4861, section 6.1.2](https://tools.ietf.org/html/rfc4861#section-6.1.2)
 * @see [RFC 4861, section 6.3.4](https://tools.ietf.org/html/rfc4861#section-6.3.4)
 * @see [RFC 4862, section 5.5.3](https://tools.ietf.org/html/rfc4862#section-5.5.3)
 * @see [RFC 6775, section 5.4](https://tools.ietf.org/html/rfc6775#section-5.4)
 * @see [RFC 6775, section 8.1.3](https://tools.ietf.org/html/rfc6775#section-8.1.3)
 *
 * @param[in] iface         Interface the router advertisement came over.
 * @param[in] ipv6_hdr      The IPv6 header of the router advertisement.
 * @param[in] rtr_adv       The router advertisement to handle.
 * @param[in] rtr_adv_len   Length of @p rtr_adv.
 */
void gnrc_ipv6_nib_handle_rtr_adv(unsigned iface, const ipv6_hdr_t *ipv6_hdr,
                                  const ndp_rtr_adv_t *rtr_adv,
                                  size_t rtr_adv_len);

/**
 * @brief   Handles a redirect message and all its options and adds to the NIB
 *          accordingly
 *
 * Additional behavior not specified in RFC: if the link-layer does not have
 * link-layer addresses, a neighbor cache entry is created for ipv6_hdr_t::src
 * of @p ipv6_hdr with gnrc_ipv6_nib_t::l2addr_len := 0 (and set to
 * @ref GNRC_IPV6_NIB_NUD_STATE_STALE), even if the redirect message does
 * not contain a TLLAO.
 *
 * @see [RFC 4861, section 8.1](https://tools.ietf.org/html/rfc4861#section-8.1)
 * @see [RFC 4861, section 8.3](https://tools.ietf.org/html/rfc4861#section-8.3)
 *
 * @param[in] iface         Interface the redirect message came over.
 * @param[in] ipv6_hdr      The IPv6 header of the redirect message.
 * @param[in] redirect      The redirect message to handle.
 * @param[in] redirect_len  Length of @p redirect.
 */
void gnrc_ipv6_nib_handle_redirect(unsigned iface, const ipv6_hdr_t *ipv6_hdr,
                                   const ndp_redirect_t *redirect,
                                   size_t redirect_len);

/**
 * @brief   Handles a duplicate address message and all its options and adds to
 *          the NIB accordingly
 *
 * @see [RFC 6775, section 8.2.1](https://tools.ietf.org/html/rfc6775#section-8.2.1)
 * @see [RFC 6775, section 8.2.4](https://tools.ietf.org/html/rfc6775#section-8.2.4)
 * @see [RFC 6775, section 8.2.5](https://tools.ietf.org/html/rfc6775#section-8.2.5)
 *
 * @param[in] iface     Interface the duplicate address message came over.
 * @param[in] ipv6_hdr  The IPv6 header of the duplicate address message.
 * @param[in] da        The duplicate address message to handle.
 * @param[in] da_len    Length of @p da.
 */
void gnrc_ipv6_nib_handle_da(unsigned iface, const ipv6_hdr_t *ipv6_hdr,
                             const ndp_redirect_t *da, size_t da_len);

/**
 * @brief   Prints a NIB entry as neighbor cache entry
 *
 * @param[in] nib   A NIB entry
 */
void gnrc_ipv6_nib_print_nc(gnrc_ipv6_nib_t *nib);

/**
 * @brief   Prints a NIB entry as destination cache entry
 *
 * @param[in] nib   A NIB entry
 */
void gnrc_ipv6_nib_print_dc(gnrc_ipv6_nib_t *nib);

/**
 * @brief   Prints a NIB entry as prefix list entry
 *
 * @param[in] nib   A NIB entry
 */
void gnrc_ipv6_nib_print_pl(gnrc_ipv6_nib_t *nib);

/**
 * @brief   Prints a NIB entry as default router list entry
 *
 * @param[in] nib   A NIB entry
 */
void gnrc_ipv6_nib_print_drl(gnrc_ipv6_nib_t *nib);

/**
 * @brief   Prints a NIB entry as default router list entry
 *
 * @param[in] nib   A NIB entry
 */
void gnrc_ipv6_nib_print_drl(gnrc_ipv6_nib_t *nib);

/**
 * @brief   Prints a NIB entry as forwarding table entry
 *
 * @param[in] nib   A NIB entry
 */
void gnrc_ipv6_nib_print_ft(gnrc_ipv6_nib_t *nib);

/**
 * @brief   Prints a NIB entry as 6LoWPAN compression context table entry
 *
 * @param[in] nib   A NIB entry
 */
void gnrc_ipv6_nib_print_ctx(gnrc_ipv6_nib_t *nib);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_IPV6_NIB_H_ */
/** @} */
