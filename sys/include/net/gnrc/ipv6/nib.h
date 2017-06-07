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

#include <stdbool.h>
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
 * @brief   Maximum link-layer length (aligned)
 */
#if (GNRC_NETIF_HDR_L2ADDR_MAX_LEN % 8)
#define GNRC_IPV6_NIB_L2ADDR_MAX_LEN        (((GNRC_NETIF_HDR_L2ADDR_MAX_LEN >> 3) + 1) << 3)
#else
#define GNRC_IPV6_NIB_L2ADDR_MAX_LEN        (GNRC_NETIF_HDR_L2ADDR_MAX_LEN)
#endif

/**
 * @{
 * @brief   Compile flags to (de-)activate certain features for NIB
 */
#ifndef GNRC_IPV6_NIB_ALWAYS_REV_TRANS
/**
 * @brief   Always reverse translate link-local address
 *
 * @see [RFC 6775, section 5.6](https://tools.ietf.org/html/rfc6775#section-5.6)
 */
#define GNRC_IPV6_NIB_ALWAYS_REV_TRANS      (0)
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_NBR_SOL
#define GNRC_IPV6_NIB_DO_HANDLE_NBR_SOL     (1) /**< handling of Neighbor Solicitations */
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_NBR_ADV
#define GNRC_IPV6_NIB_DO_HANDLE_NBR_ADV     (1) /**< handling of Neighbor Advertisements */
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_RTR_SOL
#define GNRC_IPV6_NIB_DO_HANDLE_RTR_SOL     (0) /**< handling of Router solicitations */
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_RTR_ADV
#define GNRC_IPV6_NIB_DO_HANDLE_RTR_ADV     (1) /**< handling of Router Advertisements */
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_REDIRECT
#define GNRC_IPV6_NIB_DO_HANDLE_REDIRECT    (0) /**< handling of Redirect Messages */
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_DAR
#define GNRC_IPV6_NIB_DO_HANDLE_DAR         (0) /**< handling of Duplicate Address Confirmations */
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_DAC
#define GNRC_IPV6_NIB_DO_HANDLE_DAC         (0) /**< handling of Duplicate Address Requests */
#endif

/**
 * @brief   Handling of Source Link-layer Address Option
 */
#ifndef GNRC_IPV6_NIB_DO_HANDLE_SL2AO
#define GNRC_IPV6_NIB_DO_HANDLE_SL2AO       (1)
#endif

/**
 * @brief   Handling of Target Link-layer Address Option
 */
#ifndef GNRC_IPV6_NIB_DO_HANDLE_TL2AO
#define GNRC_IPV6_NIB_DO_HANDLE_TL2AO       (0)
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_PIO
#define GNRC_IPV6_NIB_DO_HANDLE_PIO         (1) /**< handling of Prefix Information Option */
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_RHO
#define GNRC_IPV6_NIB_DO_HANDLE_RHO         (0) /**< handling of Redirect Header Option */
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_MTUO
#define GNRC_IPV6_NIB_DO_HANDLE_MTUO        (1) /**< handling of MTU Option */
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_ARO
#define GNRC_IPV6_NIB_DO_HANDLE_ARO         (0) /**< handling of Address Registration Option */
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_6CO
#define GNRC_IPV6_NIB_DO_HANDLE_6CO         (0) /**< handling of 6LoWPAN Context Option */
#endif

#ifndef GNRC_IPV6_NIB_DO_HANDLE_ABRO
#define GNRC_IPV6_NIB_DO_HANDLE_ABRO        (0) /**< handling of Authoritive Border Router Option */
#endif
/** @} */

/**
 * @{
 * @name Additional information
 * @anchor  gnrc_ipv6_nib_info
 * @brief   Additional information for @ref gnrc_ipv6_nib_t
 */
/**
 * @brief   Mask for neighbor unreachability detection (NUD) states
 *
 * @see [RFC 4861, section 7.3.2](https://tools.ietf.org/html/rfc4861#section-7.3.2)
 * @see [RFC 7048](https://tools.ietf.org/html/rfc7048)
 */
#define GNRC_IPV6_NIB_INFO_NUD_STATE_MASK           (0x0007)
#define GNRC_IPV6_NIB_INFO_NUD_STATE_UNMANAGED      (0x0000)    /**< not managed by NUD */
#define GNRC_IPV6_NIB_INFO_NUD_STATE_UNREACHABLE    (0x0001)    /**< entry is not reachable */

/**
 * @brief   address resolution is currently performed
 */
#define GNRC_IPV6_NIB_INFO_NUD_STATE_INCOMPLETE     (0x0002)

/**
 * @brief   address might not be reachable
 */
#define GNRC_IPV6_NIB_INFO_NUD_STATE_STALE          (0x0003)

/**
 * @brief   NUD will be performed in a moment
 */
#define GNRC_IPV6_NIB_INFO_NUD_STATE_DELAY          (0x0004)
#define GNRC_IPV6_NIB_INFO_NUD_STATE_PROBE          (0x0005)    /**< NUD is performed */
#define GNRC_IPV6_NIB_INFO_NUD_STATE_REACHABLE      (0x0006)    /**< entry is reachable */

/**
 * @brief   gnrc_ipv6_nib_t::next_hop is router
 *
 * This flag indicates that does not indicate that gnrc_ipv6_nib_t::next_hop is
 * a router, but it does not necessarily indicate that it is in the default
 * router list! A neighbor that has this flag unset however **must not** appear
 * in the default router list.
 *
 * @see [RFC 4861, Appendix D](https://tools.ietf.org/html/rfc4861#page-91)
 */
#define GNRC_IPV6_NIB_INFO_IS_ROUTER                (0x0008)

/**
 * @brief   Mask for interface identifier
 */
#define GNRC_IPV6_NIB_INFO_IFACE_MASK               (0x01f0)

/**
 * @brief   Shift position of interface identifier
 */
#define GNRC_IPV6_NIB_INFO_IFACE_POS                (8)

/**
 * @brief Mask for 6LoWPAN address registration (6Lo-AR) states
 *
 * @see [RFC 6775, section 3.5](https://tools.ietf.org/html/rfc6775#section-3.5)
 */
#define GNRC_IPV6_NIB_INFO_AR_STATE_MASK            (0x0600)
#define GNRC_IPV6_NIB_INFO_AR_STATE_NONE            (0x0000)    /**< not managed by 6Lo-AR */

/**
 * @brief   address can be removed when memory is low
 */
#define GNRC_IPV6_NIB_INFO_AR_STATE_GC              (0x0200)

/**
 * @brief   address registration still pending
 */
#define GNRC_IPV6_NIB_INFO_AR_STATE_TENTATIVE       (0x0400)
#define GNRC_IPV6_NIB_INFO_AR_STATE_REGISTERED      (0x0600)    /**< address is registered */

/**
 * @brief   Use context identifier for compression of gnrc_ipv6_nib_t::dst
 *          when using @ref net_gnrc_sixlowpan_iphc
 */
#define GNRC_IPV6_NIB_INFO_USE_FOR_COMP             (0x0800)

/**
 * @brief   Mask for context identifier for gnrc_ipv6_nib_t::dst for stateful
 *          header compression using @ref net_gnrc_sixlowpan_iphc
 */
#define GNRC_IPV6_NIB_INFO_CID_MASK                 (0xf000)

/**
 * @brief   Shift position of context identifier for stateful header compression
 */
#define GNRC_IPV6_NIB_INFO_CID_POS                  (12)
/** @} */

/**
 * @brief   A public NIB entry
 */
typedef struct {
    const ipv6_addr_t dst;          /**< A destination or prefix to destination */
    const ipv6_addr_t next_hop;     /**< Next hop to gnrc_ipv6_nib_t::dst */
#if defined(MODULE_GNRC_SIXLOWPAN_ND_ROUTER) || defined(DOXYGEN)
    /**
     * @brief   The neighbors EUI-64 (used for DAD)
     */
    eui64_t eui64;
#endif
#if !GNRC_IPV6_NIB_ALWAYS_REV_TRANS || defined(DOXYGEN)
    /**
     * @brief   Link-layer address of gnrc_ipv6_nib_t::next_hop
     *
     * @note    Only available if @ref GNRC_IPV6_NIB_ALWAYS_REV_TRANS != 1
     */
    const uint8_t l2addr[GNRC_IPV6_NIB_L2ADDR_MAX_LEN];
#endif

    /**
     * @brief   Information flags (see
     *          @ref gnrc_ipv6_nib_info "Additional information")
     */
    const uint16_t info;
    const uint8_t pfx_len;          /**< prefix-length of gnrc_ipv6_nib_t::dst */
#if !GNRC_IPV6_NIB_ALWAYS_REV_TRANS || defined(DOXYGEN)
    const uint8_t l2addr_len;       /**< length of gnrc_ipv6_nib_t::l2addr */
#endif
} gnrc_ipv6_nib_t;

/**
 * @brief   Gets the link-layer address for a NIB entry.
 *
 * @param[in] entry     A NIB entry.
 * @param[out] l2addr   The link-layer address of @p entry. Must at least have
 *                      space for @ref GNRC_IPV6_NIB_L2ADDR_MAX_LEN bytes.
 *
 * @return  The number of bytes in @p l2addr on success.
 * @return  0 on failure.
 */
unsigned gnrc_ipv6_nib_get_l2addr(const gnrc_ipv6_nib_t *entry,
                                  uint8_t *l2addr);

static inline unsigned gnrc_ipv6_nib_get_nud_state(const gnrc_ipv6_nib_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_INFO_NUD_STATE_MASK);
}

static inline bool gnrc_ipv6_nib_is_router(const gnrc_ipv6_nib_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_INFO_IS_ROUTER);
}

static inline unsigned gnrc_ipv6_nib_get_iface(const gnrc_ipv6_nib_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_INFO_IFACE_MASK) >>
           GNRC_IPV6_NIB_INFO_IFACE_POS;
}

static inline unsigned gnrc_ipv6_nib_get_ar_state(const gnrc_ipv6_nib_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_INFO_AR_STATE_MASK);
}

static inline bool gnrc_ipv6_nib_use_for_comp(const gnrc_ipv6_nib_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_INFO_USE_FOR_COMP);
}

static inline bool gnrc_ipv6_nib_get_cid(const gnrc_ipv6_nib_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_INFO_CID_MASK) >>
           GNRC_IPV6_NIB_INFO_CID_POS;
}

/**
 * @brief   Get NIB entry for next reachable hop to @p dst
 *
 * @param[in] iface     Interface to restrict search to search for next reachable
 *                      hop to. May be KERNEL_PID_UNDEF for any interface.
 * @param[in] dst       The destination address of @p pkt.
 * @param[in] pkt       The packet to send to @p dst. This is provided to give
 *                      the routing or address resolution protocol a way to
 *                      queue the packet for later sending if this is desired.
 * @param[out] entry    Copy of the NIB entry with the next reachable hop.
 *
 * @return 0 on success.
 * @return -ENETUNREACH, if no next hop to @p dst can be found.
 * @return -EHOSTUNREACH, if there is a next hop to @p dst, but it is currently
 *         unreachable.
 */
int gnrc_ipv6_nib_next_hop(unsigned iface, const ipv6_addr_t *dst,
                           const gnrc_pktsnip_t *pkt, gnrc_ipv6_nib_t *entry);

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
 * @return  0 on success.
 * @return  -ENOMEM, if no space is left in neighbor cache.
 */
int gnrc_ipv6_nib_set_neighbor(unsigned iface, const ipv6_addr_t *ipv6,
                               const uint8_t *l2addr, size_t l2addr_len);

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
 * via its own means (e.g. a TCP connection build-up or confirmation). Unmanaged
 * neighbor cache entries or entries which next-hop are not in the neighbor
 * cache yet are ignored.
 */
void gnrc_ipv6_nib_mark_reachable(const ipv6_addr_t *ipv6);

/**
 * @brief   Iterates over all neighbor cache entries in the NIB
 *
 * @pre (state != NULL) && (entry != NULL)
 *
 * @param[in,out] state Iteration state of the neighbor cache. Must point to
 *                      a NULL pointer to start iteration.
 * @param[out] entry    The next neighbor cache entry
 *
 * Usage example:
 *
 * ```C
 * #include <stdio.h>
 *
 * #include "net/ipv6/addr.h"
 * #include "net/gnrc/ipv6/nib.h"
 *
 * int main(void) {
 *     void *state = NULL;
 *     char addr_str[IPV6_ADDR_MAX_STR_LEN];
 *     gnrc_ipv6_nib_t entry;
 *
 *     puts("My neighbors:");
 *     while (gnrc_ipv6_nib_iter_nc(&state, &entry)) {
 *         printf("* %s\n", ipv6_addr_to_str(addr_str, &entry.next_hop,
 *                                           sizeof(addr_str)));
 *     }
 *     return 0;
 * }
 * ```
 *
 * @return  true, if iteration can be continued.
 * @return  false, if @p entry is the last neighbor cache entry in the NIB.
 */
bool gnrc_ipv6_nib_iter_nc(void **state, gnrc_ipv6_nib_t *entry);

/**
 * @brief   Iterates over all destination cache entries in the NIB
 *
 * @pre (state != NULL) && (entry != NULL)
 *
 * @param[in,out] state Iteration state of the destination cache. Must point to
 *                      a NULL pointer to start iteration.
 * @param[out] entry    The next destination cache entry
 *
 * Usage example:
 *
 * ```C
 * #include <stdio.h>
 *
 * #include "net/ipv6/addr.h"
 * #include "net/gnrc/ipv6/nib.h"
 *
 * int main(void) {
 *     void *state = NULL;
 *     char addr_str[IPV6_ADDR_MAX_STR_LEN];
 *     gnrc_ipv6_nib_t entry;
 *
 *     puts("Stored destinations:");
 *     while (gnrc_ipv6_nib_iter_dc(&state, &entry)) {
 *         printf("* %s ->", ipv6_addr_to_str(addr_str, &entry.dst,
 *                                            sizeof(addr_str)));
 *         printf(" %s\n", ipv6_addr_to_str(addr_str, &entry.next_hop,
 *                                          sizeof(addr_str)));
 *     }
 *     return 0;
 * }
 * ```
 *
 * @return  true, if iteration can be continued.
 * @return  false, if @p entry is the last destination cache entry in the NIB.
 */
bool gnrc_ipv6_nib_iter_dc(void **state, gnrc_ipv6_nib_t *entry);

/**
 * @brief   Adds or updates a prefix to manage by the NIB
 *
 * @pre (iface > KERNEL_PID_UNDEF)
 * @pre ((pfx != NULL) && (pfx_len != 0) && (pfx_len <= 128))
 *
 * @param[in] iface     Interface the prefix was added to.
 * @param[in] pfx       The prefix.
 * @param[in] pfx_len   Length of @p pfx.
 * @param[in] valid     Valid lifetime in seconds (`UINT32_MAX` for infinite
 *                      lifetime). Setting this value to 0 is equivalent to
 *                      using @ref gnrc_ipv6_nib_del_prefix().
 * @param[in] pref      Preferred lifetime in seconds (`UINT32_MAX` for infinite
 *                      lifetime).
 *
 * This change effects the prefix list and thus may also trigger an unsolicited
 * router advertisement, if this is possible within the rate-limiting
 * constraints or restrictions specified in
 * [RFC 4861](https://tools.ietf.org/html/rfc4861) and
 * [RFC 6775](https://tools.ietf.org/html/rfc6775).
 *
 * @return  0 on success.
 * @return  -ENOMEM, if no space is left in prefix list.
 */
int gnrc_ipv6_nib_set_prefix(unsigned iface, const ipv6_addr_t *pfx,
                             uint8_t pfx_len, uint32_t valid, uint32_t pref);

/**
 * @brief   Deletes a prefix from the NIB
 *
 * @pre (if (pfx != NULL): (pfx_len > 0) && (pfx_len <= 128))
 *
 * @param[in] pfx       Destination of the route.
 *                      May be NULL for default route.
 * @param[in] pfx_len   Prefix length of @p dst if it is a prefix.
 *                      May be 0 for default route.
 *
 * This change effects the prefix list and thus may also trigger an unsolicited
 * router advertisement, if this is possible within the rate-limiting
 * constraints or restrictions specified in
 * [RFC 4861](https://tools.ietf.org/html/rfc4861) and
 * [RFC 6775](https://tools.ietf.org/html/rfc6775)
 */
void gnrc_ipv6_nib_del_prefix(const ipv6_addr_t *pfx, uint8_t pfx_len);

/**
 * @brief   Iterates over all prefix list entries in the NIB
 *
 * @pre (state != NULL) && (entry != NULL)
 *
 * @param[in,out] state Iteration state of the prefix list. Must point to
 *                      a NULL pointer to start iteration.
 * @param[out] entry    The next prefix list entry
 *
 * Usage example:
 *
 * ```C
 * #include <stdio.h>
 *
 * #include "net/ipv6/addr.h"
 * #include "net/gnrc/ipv6/nib.h"
 *
 * int main(void) {
 *     void *state = NULL;
 *     char addr_str[IPV6_ADDR_MAX_STR_LEN];
 *     gnrc_ipv6_nib_t entry;
 *
 *     puts("My prefixes:");
 *     while (gnrc_ipv6_nib_iter_pl(&state, &entry)) {
 *         printf("* %s/%u\n", ipv6_addr_to_str(addr_str, &entry.dst,
 *                                              sizeof(addr_str)),
 *                entry.pfx_len);
 *     }
 *     return 0;
 * }
 * ```
 *
 * @return  true, if iteration can be continued.
 * @return  false, if @p entry is the last prefix list entry in the NIB.
 */
bool gnrc_ipv6_nib_iter_pl(void **state, gnrc_ipv6_nib_t *entry);

/**
 * @brief   Adds a route to the NIB
 *
 * @pre (iface > KERNEL_PID_UNDEF)
 * @pre (if (dst != NULL): (pfx_len > 0) && (pfx_len <= 128))
 * @pre (if (dst == NULL): (next_hop != NULL))
 * @pre (if (next_hop == NULL): (dst != NULL))
 *
 * @param[in] iface     Interface the route goes over.
 * @param[in] dst       Destination of the route.
 *                      May be NULL for default route.
 * @param[in] pfx_len   Prefix length of @p dst if it is a prefix.
 *                      May be 0 for default route.
 * @param[in] next_hop  Next hop to @p dst. May be NULL to just forward over
 *                      @p iface.
 * @param[in] ltime     Lifetime in centiseconds. `UINT32_MAX` for infinite
 *                      lifetime.
 *
 * Adds a route to the NIB. Depending of weather @p dst is NULL or not this
 * route can either end up in the default router list (`dst == NULL`) or
 * forwarding table (`dst != NULL`).
 *
 * @return  0 on success.
 * @return  -ENOMEM, if no space is left in the forwarding information base.
 */
int gnrc_ipv6_nib_add_route(unsigned iface, const ipv6_addr_t *dst,
                            uint8_t pfx_len, const ipv6_addr_t *next_hop,
                            uint32_t ltime);

/**
 * @brief   Deletes a forwarding table entry from the NIB
 *
 * @pre (if (dst != NULL): (pfx_len > 0) && (pfx_len <= 128))
 *
 * @param[in] dst       Destination of the route.
 *                      May be NULL for default route.
 * @param[in] pfx_len   Prefix length of @p dst if it is a prefix.
 *                      May be 0 for default route.
 */
void gnrc_ipv6_nib_del_route(const ipv6_addr_t *dst, uint8_t pfx_len);

/**
 * @brief   Iterates over all default router list entries in the NIB
 *
 * @pre (state != NULL) && (entry != NULL)
 *
 * @param[in,out] state Iteration state of the default router list. Must point
 *                      to a NULL pointer to start iteration.
 * @param[out] entry    The next default router list entry
 *
 * Usage example:
 *
 * ```C
 * #include <stdio.h>
 *
 * #include "net/ipv6/addr.h"
 * #include "net/gnrc/ipv6/nib.h"
 *
 * int main(void) {
 *     void *state = NULL;
 *     char addr_str[IPV6_ADDR_MAX_STR_LEN];
 *     gnrc_ipv6_nib_t entry;
 *
 *     puts("My default routers:");
 *     while (gnrc_ipv6_nib_iter_drl(&state, &entry)) {
 *         printf("* %s\n", ipv6_addr_to_str(addr_str, &entry.next_hop,
 *                                           sizeof(addr_str));
 *     }
 *     return 0;
 * }
 * ```
 *
 * @return  true, if iteration can be continued.
 * @return  false, if @p entry is the last default router list entry in the NIB.
 */
bool gnrc_ipv6_nib_iter_drl(void **state, gnrc_ipv6_nib_t *entry);

/**
 * @brief   Iterates over all forwarding table entries in the NIB
 *
 * @pre (state != NULL) && (entry != NULL)
 *
 * @param[in,out] state Iteration state of the forwarding table list. Must point
 *                      to a NULL pointer to start iteration.
 * @param[out] entry    The next forwarding table entry
 *
 * Usage example:
 *
 * ```C
 * #include <stdio.h>
 *
 * #include "net/ipv6/addr.h"
 * #include "net/gnrc/ipv6/nib.h"
 *
 * int main(void) {
 *     void *state = NULL;
 *     char addr_str[IPV6_ADDR_MAX_STR_LEN];
 *     gnrc_ipv6_nib_t entry;
 *
 *     puts("My default routers:");
 *     while (gnrc_ipv6_nib_iter_pl(&state, &entry)) {
 *         printf("* %s\n", ipv6_addr_to_str(addr_str, &entry.next_hop,
 *                                           sizeof(addr_str));
 *     }
 *     return 0;
 * }
 * ```
 *
 * Other than the other iterators this one not only includes the entries
 * explicitly added to the forwarding table using @ref gnrc_ipv6_nib_add_route(),
 * but also the destination cache, prefix list, and default router list entries
 * that were automatically by the neighbor discovery or manually added with
 * @ref gnrc_ipv6_nib_set_prefix().
 *
 * @return  true, if iteration can be continued.
 * @return  false, if @p entry is the last forwarding table entry in the NIB.
 */
bool gnrc_ipv6_nib_iter_ft(void **state, gnrc_ipv6_nib_t *entry);

/**
 * @brief   Adds 6LoWPAN compression context to the NIB
 *
 * @pre (iface > KERNEL_PID_UNDEF)
 * @pre ((pfx != NULL) && (pfx_len != 0) && (pfx_len < 128))
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
 * @return  0, on success
 * @return  -ENOMEM, if no space is left in the forwarding information base.
 */
int gnrc_ipv6_nib_add_6lo_ctx(unsigned iface, const ipv6_addr_t *pfx,
                              uint8_t pfx_len, uint32_t valid, uint8_t cid);

/**
 * @brief   Iterates over all 6LoWPAN compression context table entries in the
 *          NIB
 *
 * @pre (state != NULL) && (entry != NULL)
 *
 * @param[in,out] state Iteration state of the 6LoWPAN compression context
 *                      table. Must point to a NULL pointer to start iteration.
 * @param[out] entry    The next 6LoWPAN compression context table entry
 *
 * Usage example:
 *
 * ```C
 * #include <stdio.h>
 *
 * #include "net/ipv6/addr.h"
 * #include "net/gnrc/ipv6/nib.h"
 *
 * int main(void) {
 *     void *state = NULL;
 *     char addr_str[IPV6_ADDR_MAX_STR_LEN];
 *     gnrc_ipv6_nib_t entry;
 *
 *     puts("My compression contexts:");
 *     while (gnrc_ipv6_nib_iter_ctx(&state, &entry)) {
 *         printf("* %u:", gnrc_ipv6_nib_get_cid(&entry));
 *         printf("%s/%u\n", ipv6_addr_to_str(addr_str, &entry.dst,
 *                                            sizeof(addr_str), entry.pfx_len);
 *     }
 *     return 0;
 * }
 * ```
 *
 * @return  true, if iteration can be continued.
 * @return  false, if @p entry is the last 6LoWPAN compression context table
 *          entry in the NIB.
 */
bool gnrc_ipv6_nib_iter_ctx(void **state, gnrc_ipv6_nib_t *entry);

/**
 * @brief   Iterates over all entries in the NIB
 *
 * @pre (state != NULL) && (entry != NULL)
 *
 * @param[in,out] state Iteration state of the NIB. Must point to a NULL pointer
 *                      to start iteration.
 * @param[out] entry    The next NIB entry
 *
 * Usage example:
 *
 * ```C
 * #include <stdio.h>
 *
 * #include "net/ipv6/addr.h"
 * #include "net/gnrc/ipv6/nib.h"
 *
 * int main(void) {
 *     void *state = NULL;
 *     gnrc_ipv6_nib_t entry;
 *
 *     while (gnrc_ipv6_nib_iter(&state, &entry)) {
 *         // do stuff with NIB
 *     }
 *     return 0;
 * }
 * ```
 *
 * @return  true, if iteration can be continued.
 * @return  false, if @p entry is the last entry in the NIB.
 */
bool gnrc_ipv6_nib_iter(void **state, gnrc_ipv6_nib_t *entry);

#if GNRC_IPV6_NIB_DO_HANDLE_NBR_SOL || defined(DOXYGEN)
/**
 * @brief   Handles a neighbor solicitation message and all its options and adds
 *          to the NIB accordingly
 *
 * If @ref GNRC_IPV6_NIB_DO_HANDLE_NBR_SOL is 0 at compile time any incoming
 * neighbor solicitation will be silently ignored.
 *
 * Additional behavior not specified in RFC: if the link-layer does not have
 * link-layer addresses, a neighbor cache entry is created for ipv6_hdr_t::src
 * of @p ipv6_hdr with gnrc_ipv6_nib_t::l2addr_len := 0 (and set to
 * @ref GNRC_IPV6_NIB_INFO_NUD_STATE_STALE), even if the neighbor solicitation
 * does not contain a SLLAO.
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
#else   /* GNRC_IPV6_NIB_DO_HANDLE_NBR_SOL */
/* NOP if neighbor solicitations shouldn't be handled */
#define gnrc_ipv6_nib_handle_nbr_sol(iface, ipv6_hdr, nbr_sol, nbr_sol_len)
#endif  /* GNRC_IPV6_NIB_DO_HANDLE_NBR_SOL */

#if GNRC_IPV6_NIB_DO_HANDLE_NBR_ADV || defined(DOXYGEN)
/**
 * @brief   Handles a neighbor advertisement message and all its options and
 *          adds to the NIB accordingly
 *
 * If @ref GNRC_IPV6_NIB_DO_HANDLE_NBR_ADV is 0 at compile time any incoming
 * neighbor advertisement will be silently ignored.
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
#else   /* GNRC_IPV6_NIB_DO_HANDLE_NBR_ADV */
/* NOP if neighbor advertisements shouldn't be handled */
#define gnrc_ipv6_nib_handle_nbr_adv(iface, ipv6_hdr, nbr_adv, nbr_adv_len)
#endif  /* GNRC_IPV6_NIB_DO_HANDLE_NBR_ADV */

#if GNRC_IPV6_NIB_DO_HANDLE_RTR_SOL || defined(DOXYGEN)
/**
 * @brief   Handles a router solicitation message and all its options and adds
 *          to the NIB accordingly
 *
 * If @ref GNRC_IPV6_NIB_DO_HANDLE_RTR_SOL is 0 at compile time any incoming
 * router solicitation will be silently ignored.
 *
 * Additional behavior not specified in RFC: if the link-layer does not have
 * link-layer addresses, a neighbor cache entry is created for ipv6_hdr_t::src
 * of @p ipv6_hdr with gnrc_ipv6_nib_t::l2addr_len := 0 (and set to
 * @ref GNRC_IPV6_NIB_INFO_NUD_STATE_STALE), even if the router solicitation
 * does not contain a SLLAO.
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
#else   /* GNRC_IPV6_NIB_DO_HANDLE_RTR_SOL */
/* NOP if router solicitations shouldn't be handled */
#define gnrc_ipv6_nib_handle_rtr_sol(iface, ipv6_hdr, rtr_sol, rtr_sol_len)
#endif  /* GNRC_IPV6_NIB_DO_HANDLE_RTR_SOL */

#if GNRC_IPV6_NIB_DO_HANDLE_RTR_ADV || defined(DOXYGEN)
/**
 * @brief   Handles a router advertisement message and all its options and adds
 *          to the NIB accordingly
 *
 * If @ref GNRC_IPV6_NIB_DO_HANDLE_RTR_ADV is 0 at compile time any incoming
 * router advertisement will be silently ignored.
 *
 * Additional behavior not specified in RFC: if the link-layer does not have
 * link-layer addresses, a neighbor cache entry is created for ipv6_hdr_t::src
 * of @p ipv6_hdr with gnrc_ipv6_nib_t::l2addr_len := 0 (and set to
 * @ref GNRC_IPV6_NIB_INFO_NUD_STATE_STALE), even if the router advertisement
 * does not contain a SLLAO.
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
#else   /* GNRC_IPV6_NIB_DO_HANDLE_RTR_ADV */
/* NOP if router advertisements shouldn't be handled */
#define gnrc_ipv6_nib_handle_rtr_adv(iface, ipv6_hdr, rtr_adv, rtr_adv_len)
#endif  /* GNRC_IPV6_NIB_DO_HANDLE_RTR_ADV */

#if GNRC_IPV6_NIB_DO_HANDLE_REDIRECT || defined(DOXYGEN)
/**
 * @brief   Handles a redirect message and all its options and adds to the NIB
 *          accordingly
 *
 * If @ref GNRC_IPV6_NIB_DO_HANDLE_REDIRECT is 0 at compile time any incoming
 * redirect message will be silently ignored.
 *
 * Additional behavior not specified in RFC: if the link-layer does not have
 * link-layer addresses, a neighbor cache entry is created for ipv6_hdr_t::src
 * of @p ipv6_hdr with gnrc_ipv6_nib_t::l2addr_len := 0 (and set to
 * @ref GNRC_IPV6_NIB_INFO_NUD_STATE_STALE), even if the redirect message does
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
#else   /* GNRC_IPV6_NIB_DO_HANDLE_REDIRECT */
/* NOP if redirect messages shouldn't be handled */
#define gnrc_ipv6_nib_handle_redirect(iface, ipv6_hdr, redirect, redirect_len)
#endif  /* GNRC_IPV6_NIB_DO_HANDLE_REDIRECT */

#if GNRC_IPV6_NIB_DO_HANDLE_DAR || defined(DOXYGEN)
/**
 * @brief   Handles a duplicate address requests and all its options and adds to
 *          the NIB accordingly
 *
 * If @ref GNRC_IPV6_NIB_DO_HANDLE_DAR is 0 at compile time any incoming
 * duplicate address request will be silently ignored.
 *
 * @see [RFC 6775, section 8.2.1](https://tools.ietf.org/html/rfc6775#section-8.2.1)
 * @see [RFC 6775, section 8.2.4](https://tools.ietf.org/html/rfc6775#section-8.2.4)
 *
 * @param[in] iface     Interface the duplicate address request came over.
 * @param[in] ipv6_hdr  The IPv6 header of the duplicate address request.
 * @param[in] dar       The duplicate address request to handle.
 * @param[in] dar_len   Length of @p dar.
 */
void gnrc_ipv6_nib_handle_dar(unsigned iface, const ipv6_hdr_t *ipv6_hdr,
                              const sixlowpan_nd_da_t *dar, size_t dar_len);
#else   /* GNRC_IPV6_NIB_DO_HANDLE_DAR */
/* NOP if duplicate address requests shouldn't be handled */
#define gnrc_ipv6_nib_handle_dar(iface, ipv6_hdr, da, da_len)
#endif  /* GNRC_IPV6_NIB_DO_HANDLE_DAR */

#if GNRC_IPV6_NIB_DO_HANDLE_DAR || defined(DOXYGEN)
/**
 * @brief   Handles a duplicate address confirmation and all its options and adds to
 *          the NIB accordingly
 *
 * If @ref GNRC_IPV6_NIB_DO_HANDLE_DAC is 0 at compile time any incoming
 * duplicate address request will be silently ignored.
 *
 * @see [RFC 6775, section 8.2.1](https://tools.ietf.org/html/rfc6775#section-8.2.1)
 * @see [RFC 6775, section 8.2.5](https://tools.ietf.org/html/rfc6775#section-8.2.5)
 *
 * @param[in] iface     Interface the duplicate address confirmation came over.
 * @param[in] ipv6_hdr  The IPv6 header of the duplicate address confirmation.
 * @param[in] dac       The duplicate address confirmation to handle.
 * @param[in] dac_len   Length of @p dac.
 */
void gnrc_ipv6_nib_handle_dac(unsigned iface, const ipv6_hdr_t *ipv6_hdr,
                              const sixlowpan_nd_da_t *dac, size_t dac_len);
#else   /* GNRC_IPV6_NIB_DO_HANDLE_DAC */
/* NOP if duplicate address confirmation shouldn't be handled */
#define gnrc_ipv6_nib_handle_dac(iface, ipv6_hdr, da, da_len)
#endif  /* GNRC_IPV6_NIB_DO_HANDLE_DAC */

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
