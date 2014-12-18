/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  net_sixlowpan_ndp
 * @ingroup     net_sixlowpan
 * @brief       Neighbor discovery protocol for 6LoWPAN and IPv6
 * @{
 *
 * @file        sixlowpan/ndp.h
 * @brief       6LoWPAN constants, data structs, and prototypes related to NDP
 *
 * @author      Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef SIXLOWPAN_NDP_H
#define SIXLOWPAN_NDP_H

#include <stdint.h>

#include "net_if.h"
#include "timex.h"
#include "sixlowpan/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NDP_6LOWPAN_CONTEXT_MAX         (16)

#define NDP_OPT_SLLAO_TYPE                 	(1)
#define NDP_OPT_TLLAO_TYPE                 	(2)
#define NDP_OPT_PI_VLIFETIME_INFINITE      	(0xffffffff)
#define NDP_OPT_PI_PLIFETIME_INFINITE      	(0xffffffff)
#define NDP_OPT_ARO_STATE_SUCCESS          	(0)
#define NDP_OPT_ARO_STATE_DUP_ADDR         	(1)
#define NDP_OPT_ARO_STATE_NBR_CACHE_FULL   	(2)

/**
 * @brief   The retransmission timeout for neighbor cache entry in millisecronds.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>.
 */
#define NDP_RA_RETRANSMISSION_TIMEOUT_DEFAULT   (50)
#define NDP_NS_RETRANSMISSION_RETRIES           (5)

/**
 * @brief   The reachability timeout for neighbor cache entries in milliseconds.
 *          After this timeframe a NCE changes state from REACHABLE to STALE
 *
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>.
 */
#define NDP_RA_REACHABILITY_TIMEOUT_DEFAULT     (10000)

/**
 * @brief   The maximum length of a link-layer address in the neighbor cache.
 */
#define NDP_NCE_MAX_LLADDR_LEN                  (8)

/**
 * @brief   Neighbor cache entry state according to
 *          <a href="http://tools.ietf.org/html/rfc4861#section-7.3.2">
 *              RFC 4861, section 7.3.2
 *          </a>.
 */
typedef enum __attribute__((packed)) {
    NDP_NCE_STATUS_INCOMPLETE,
    NDP_NCE_STATUS_REACHABLE,
    NDP_NCE_STATUS_STALE,
    NDP_NCE_STATUS_DELAY,
    NDP_NCE_STATUS_PROBE,
} ndp_nce_state_t;

/**
 * @brief   Neighbor cache entry type according to
 *          <a href="http://tools.ietf.org/html/rfc6775#section-3.5">
 *              RFC 6775, section 3.5
 *          </a>.
 */
typedef enum __attribute__((packed)) {
    NDP_NCE_TYPE_GC = 1,        ///< Garbage-collectible.
    NDP_NCE_TYPE_REGISTERED,    ///< Registered.
    NDP_NCE_TYPE_TENTATIVE      ///< Tentetive.
} ndp_nce_type_t;

/**
 * @brief   Prefix list type to store information spread by prefix
 *          information option on the interface.
 *
 * @see net_if_addr_t
 */
typedef struct __attribute__((packed)) ndp_prefix_info_t {
    /**
     * @brief The next on the interface. Initialize with NULL
     */
    struct ndp_prefix_info_t *addr_next;
    /**
     * @brief The prev address on the interface. Initialize with NULL
     */
    struct ndp_prefix_info_t *addr_prev;
    /**
     * @brief Flags to define upper layer protocols this address applies to.
     *        For this layer NET_IF_L3P_IPV6_PREFIX must be set.
     */
    net_if_l3p_t prefix_protocol;
    ipv6_addr_t *prefix_data;       ///< The Prefix.
    uint8_t prefix_len;             ///< Length of the prefix.
    uint8_t inuse;                  ///< Prefix is in in use.
    /**
     * Use this information in Prefix Information Options of Router
     * Advertisements.
     */
    uint8_t advertisable;
    uint8_t flags;                  ///< flags of the prefix information option
    uint32_t valid_lifetime;        ///< valid lifetime
    uint32_t preferred_lifetime;    ///< preferred lifetime
    uint8_t infinite;               ///< flag to set to infinite lifetime
} ndp_prefix_info_t;

/**
 * @brief   Default router list to store information spread by
 *          router advertisement.
 */
typedef struct __attribute__((packed)) {
    ipv6_addr_t addr;       ///< Address of router.
    timex_t inval_time;     ///< remaining time until this entry is
    ///< invalid.
} ndp_default_router_list_t;

/**
 * @brief   Neighbor cache as defined in
 *          <a href="http://tools.ietf.org/html/rfc4861#section-3.5#section-5.1">
 *              RFC 4861, section 5.1
 *          </a>.
 */
typedef struct __attribute__((packed)) {
    int if_id;                  ///< Interface the IPv6 address is reachable
                                ///< over
    ndp_nce_type_t type;        ///< Type of neighbor cache entry.
    ndp_nce_state_t state;      ///< State of neighbor cache entry.
    uint8_t isrouter;           ///< Flag to signify that this neighbor
                                ///< is a router.
    ipv6_addr_t addr;           ///< IPv6 address of the neighbor.
    uint8_t lladdr[NDP_NCE_MAX_LLADDR_LEN]; ///< Link-layer address of the neighbor
    uint8_t lladdr_len;         ///< Length of link-layer address of the
                                ///< neighbor
    timex_t ltime;              ///< lifetime of entry.
} ndp_neighbor_cache_t;

/**
 * @brief   Authoritive border router cache as defined in
 *          <a href="http://tools.ietf.org/html/rfc6775">
 *              RFC 6775
 *          </a>.
 */
typedef struct __attribute__((packed)) {
    uint16_t version;                       ///< version of entry.
    ipv6_addr_t abr_addr;                   ///< Addres of ABR.
    uint8_t cids[NDP_6LOWPAN_CONTEXT_MAX];  ///< context IDs.
} ndp_a6br_cache_t;

ndp_default_router_list_t *ndp_default_router_list_search(ipv6_addr_t *ipaddr);
void ndp_neighbor_cache_clear(void);
int ndp_state_change_on_upper_layer_reachability_conf(ndp_nce_type_t *nce);
uint8_t ndp_neighbor_cache_add(int if_id, const ipv6_addr_t *ipaddr,
                               const void *lladdr, uint8_t lladdr_len,
                               uint8_t isrouter, ndp_nce_state_t state,
                               ndp_nce_type_t type, uint16_t ltime);

/**
 * @brief   Removes an address from the neighbor cache by IPv6 address.
 *
 * @param[in] ipaddr        IPv6 address to remove, leave NULL if you only want
 *                          to remove by Link-layer address.
 *
 * @return  1 on success, 0 otherwise.
 */
uint8_t ndp_neighbor_cache_remove(const ipv6_addr_t *ipaddr);

ndp_neighbor_cache_t *ndp_neighbor_cache_search(ipv6_addr_t *ipaddr);
ndp_neighbor_cache_t *ndp_get_ll_address(ipv6_addr_t *ipaddr, ipv6_hdr_t *packet);
int ndp_addr_is_on_link(ipv6_addr_t *dest_addr);

/**
 * @brief   Adds a prefix information to an interface. If it already exists,
 *          the values *valid_lifetime*, *preferred_lifetime*, *advertisable*,
 *          and flags will be updated accordingly and the prefix will be marked
 *          as *in_use*.
 *
 * @see     <a href="http://tools.ietf.org/html/rfc4861#section-4.6.2">
 *              RFC 4861, section 4.6.2
 *          </a>.
 *
 * @param[in]   if_id               The interface's ID.
 * @param[in]   prefix              The prefix.
 * @param[in]   prefix_len          The length of the prefix in bit.
 * @param[in]   valid_lifetime      The time in seconds this prefix is valid
 *                                  for on-link determination.
 *                                  NDP_OPT_PI_VLIFETIME_INFINITE for infinite
 *                                  lifetime.
 * @param[in]   preferred_lifetime  The time in seconds addresses generated with
 *                                  this prefix remain preferred.
 *                                  NDP_OPT_PI_PLIFETIME_INFINITE for infinite
 *                                  lifetime.
 * @param[in]   advertisable        Set this to a value != 0 to advertise this
 *                                  prefix information with the Prefix
 *                                  Information Option, set it to 0 if not.
 * @param[in]   flags               Flags for the Prefix Information Option.
 *                                  Valid values are
 *                                  ICMPV6_NDP_OPT_PI_FLAG_ON_LINK and
 *                                  ICMPV6_NDP_OPT_PI_FLAG_AUTONOM
 */
int ndp_add_prefix_info(int if_id, const ipv6_addr_t *prefix,
                        uint8_t prefix_len, uint32_t valid_lifetime,
                        uint32_t preferred_lifetime, uint8_t advertisable,
                        uint8_t flags);

/**
 * @brief   Searches the information for the longest prefix up to *up_to* bits
 *          on an interface fitting to an address *addr*.
 *
 * @param[in]   if_id   The interface's ID.
 * @param[in]   addr    The address to search the prefix for.
 * @param[in]   up_to   The number of bits up to which point the search should
 *                      go. Set to IPV6_ADDR_BIT_LEN for the whole address.
 *                      Values greater then IPV6_ADDR_BIT_LEN are set to
 *                      IPV6_ADDR_BIT_LEN.
 *
 * @return The found prefix information, NULL when none is found.
 */
ndp_prefix_info_t *ndp_prefix_info_search(int if_id, const ipv6_addr_t *addr,
        uint8_t up_to);

/**
 * @brief   Searches the information for the prefix that matches *prefix* with
 *          length *prefix_len*.
 *
 * @param[in]   if_id       The interface's ID.
 * @param[in]   prefix      The prefix to search for.
 * @param[in]   prefix_len  The length of the prefix in bit.
 *
 * @return The found prefix information, NULL when none is found.
 */
ndp_prefix_info_t *ndp_prefix_info_match(int if_id, const ipv6_addr_t *prefix,
        uint8_t prefix_len);
ndp_a6br_cache_t *ndp_a6br_cache_get_most_current(void);
ndp_a6br_cache_t *ndp_a6br_cache_get_oldest(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SIXLOWPAN_NDP_H */
