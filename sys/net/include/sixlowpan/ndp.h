/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     net_sixlowpan
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

#include "timex.h"
#include "sixlowpan/types.h"

#define NDP_6LOWPAN_CONTEXT_MAX         (16)

#define NDP_OPT_SLLAO_TYPE                 	(1)
#define NDP_OPT_TLLAO_TYPE                 	(2)
#define NDP_OPT_PI_VLIFETIME_INFINITE      	(0xffffffff)
#define NDP_OPT_ARO_STATE_SUCCESS          	(0)
#define NDP_OPT_ARO_STATE_DUP_ADDR         	(1)
#define NDP_OPT_ARO_STATE_NBR_CACHE_FULL   	(2)

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
 *          information option.
 */
typedef struct __attribute__((packed)) {
    uint8_t inuse;          ///< Prefix is in in use.
    uint8_t adv;
    ipv6_addr_t addr;       ///< The Prefix.
    uint8_t length;         ///< Length of the prefix.
    uint8_t l_a_reserved1;  ///< L and A flag of prefix information option
    uint32_t val_ltime;     ///< valid lifetime
    uint32_t pref_ltime;    ///< preferred lifetime
    uint8_t infinite;       ///< flag to set to infinite lifetime
} ndp_prefix_list_t;

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
    ndp_nce_type_t type;        ///< Type of neighbor cache entry.
    ndp_nce_state_t state;      ///< State of neighbor cache entry.
    uint8_t isrouter;           ///< Flag to signify that this neighbor
                                ///< is a router.
    ipv6_addr_t addr;           ///< IPv6 address of the neighbor.
    ieee_802154_long_t laddr;   ///< EUI-64 of neighbor
    ieee_802154_short_t saddr;  ///< IEEE 802.15.4 16-bit short address
                                ///< of neighbor.
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
ndp_neighbor_cache_t *ndp_neighbor_cache_search(ipv6_addr_t *ipaddr);
/*TODO: to implement*/
uint8_t ndp_prefix_list_search(ipv6_addr_t *addr);
ndp_a6br_cache_t *ndp_a6br_cache_get_most_current(void);
ndp_a6br_cache_t *ndp_a6br_cache_get_oldest(void);

/** @} */
#endif /* SIXLOWPAN_NDP_H */
