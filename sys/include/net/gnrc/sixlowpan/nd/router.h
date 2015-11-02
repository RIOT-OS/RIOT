/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sixlowpan_nd_router    Router-part of 6LoWPAN-ND
 * @ingroup     net_gnrc_sixlowpan_nd
 * @brief       Router-part of 6LoWPAN-ND
 * @{
 *
 * @file
 * @brief   Router-definitions for 6LoWPAN-ND.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_SIXLOWPAN_ND_ROUTER_H_
#define GNRC_SIXLOWPAN_ND_ROUTER_H_

#include <stdbool.h>

#include "bitfield.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "net/gnrc/ipv6/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of registerable border routers
 *
 * @note    More than one border routers require some way of synchronization
 *          of the context information (see
 *          [RFC 6775, section 8.1](https://tools.ietf.org/html/rfc6775#section-8.1))
 */
#ifndef GNRC_SIXLOWPAN_ND_ROUTER_ABR_NUMOF
#define GNRC_SIXLOWPAN_ND_ROUTER_ABR_NUMOF  (1)
#endif

/**
 * @brief   The number of non-link-local prefixes associated with border routers
 *          at maximum.
 */
#ifndef GNRC_SIXLOWPAN_ND_ROUTER_ABR_PRF_NUMOF
/* One prefix per interface */
#define GNRC_SIXLOWPAN_ND_ROUTER_ABR_PRF_NUMOF   \
        (GNRC_SIXLOWPAN_ND_ROUTER_ABR_NUMOF * GNRC_NETIF_NUMOF)
#endif

/**
 * @brief   Representation for prefixes coming from a router
 */
typedef struct gnrc_sixlowpan_nd_router_prf {
    struct gnrc_sixlowpan_nd_router_prf *next;  /**< next prefix */
    gnrc_ipv6_netif_t *iface;                   /**< interface the prefix is registered too */
    gnrc_ipv6_netif_addr_t *prefix;             /**< prefix on the interface/in the prefix list */
} gnrc_sixlowpan_nd_router_prf_t;

/**
 * @brief   Abstract representation of a border router on all (border) routers.
 */
typedef struct {
    ipv6_addr_t addr;                       /**< the IPv6 address of the border router (BR) */
    uint32_t version;                       /**< version of the information dissiminated by the
                                             *   BR */
    uint16_t ltime;                         /**< the time in minutes until deletion */
    BITFIELD(ctxs, GNRC_SIXLOWPAN_CTX_SIZE);/**< contexts associated with BR */
    gnrc_sixlowpan_nd_router_prf_t *prfs;   /**< prefixes associated with BR */
    xtimer_t ltimer;                        /**< timer for deletion */
    msg_t ltimer_msg;                       /**< msg_t for gnrc_sixlowpan_nd_router_abr_t::ltimer */
} gnrc_sixlowpan_nd_router_abr_t;

/**
 * @brief   Removes tentetative neighbor cache entries or sets registered ones to
 *          garbage-collectible.
 *
 * @param[in] nc_entry  A neighbor cache entry.
 */
static inline void gnrc_sixlowpan_nd_router_gc_nc(gnrc_ipv6_nc_t *nc_entry)
{
    switch (gnrc_ipv6_nc_get_type(nc_entry)) {
        case GNRC_IPV6_NC_TYPE_TENTATIVE:
        case GNRC_IPV6_NC_TYPE_REGISTERED:
            gnrc_ipv6_nc_remove(nc_entry->iface, &nc_entry->ipv6_addr);
            break;
        default:
            break;
    }

}

/**
 * @brief   Set @p netif to router mode.
 *
 * @details This sets/unsets the GNRC_IPV6_NETIF_FLAGS_ROUTER and initializes or ceases router
 *          behavior for 6LoWPAN neighbor discovery.
 *
 * @param[in] netif     An IPv6 interface. Must not be NULL.
 * @param[in] enable    Status for the GNRC_IPV6_NETIF_FLAGS_ROUTER flag.
 */
static inline void gnrc_sixlowpan_nd_router_set_router(gnrc_ipv6_netif_t *netif, bool enable)
{
    if (enable) {
        netif->flags |= GNRC_IPV6_NETIF_FLAGS_ROUTER;
    }
    else {
        netif->flags &= ~GNRC_IPV6_NETIF_FLAGS_ROUTER;
    }
}

/**
 * @brief   Set/Unset GNRC_IPV6_NETIF_FLAGS_RTR_ADV flag for @p netif.
 *
 * @details GNRC_IPV6_NETIF_FLAGS_RTR_ADV and initializes or ceases
 *          periodic router advertising behavior for neighbor discovery.
 *
 * @param[in] netif     An IPv6 interface. Must not be NULL.
 * @param[in] enable    Status for the GNRC_IPV6_NETIF_FLAGS_RTR_ADV flag.
 */
void gnrc_sixlowpan_nd_router_set_rtr_adv(gnrc_ipv6_netif_t *netif, bool enable);

/**
 * @brief   Get's the border router for this router.
 *
 * @return  The border router, if one is specified.
 * @return  NULL, otherwise.
 */
gnrc_sixlowpan_nd_router_abr_t *gnrc_sixlowpan_nd_router_abr_get(void);

/**
 * @brief   Checks if the version data @p abr_opt is older than the version of the currently
 *          registered border router.
 *
 * @param[in] abr_opt   An authoritative border router option containing potentially new
 *                      information on the currently registered border router.
 *
 * @return  true, if the information in @p abr_opt is newer.
 * @return  false, if the information in @p abr_opt is older.
 */
bool gnrc_sixlowpan_nd_router_abr_older(sixlowpan_nd_opt_abr_t *abr_opt);

/**
 * @brief   Removes the border router and all the prefixes and contexts it disseminated through
 *          the network for this node.
 *
 * @param[in] abr   The border router.
 */
void gnrc_sixlowpan_nd_router_abr_remove(gnrc_sixlowpan_nd_router_abr_t *abr);

#ifdef MODULE_GNRC_SIXLOWPAN_ND_BORDER_ROUTER
/**
 * @brief   Makes this node a new border router.
 *
 * @per addr != NULL
 *
 * @param[in] addr  The local address to use in the ABROs
 * @param[in] ltime The lifetime to advertise in the ABROs. 0 assumes a default value of
 *                  @ref GNRC_SIXLOWPAN_ND_BORDER_ROUTER_DEFAULT_LTIME
 *
 * @return  The new border router object.
 * @return  NULL, on error.
 */
gnrc_sixlowpan_nd_router_abr_t *gnrc_sixlowpan_nd_router_abr_create(ipv6_addr_t *addr,
                                                                    unsigned int ltime);

/**
 * @brief   Adds a prefix for this border router to manage.
 *
 * @pre iface != NULL && prefix != NULL
 *
 * @param[in] abr       The local border router
 * @param[in] iface     The IPv6 interface the prefix was added to.
 * @param[in] prefix    The prefix.
 *
 * @return  0, on success
 * @return  -ENOMEM, if no space for the new prefix is available.
 * @return  -ENOENT, if @p abr is not registered.
 */
int gnrc_sixlowpan_nd_router_abr_add_prf(gnrc_sixlowpan_nd_router_abr_t* abr,
                                         gnrc_ipv6_netif_t *iface, gnrc_ipv6_netif_addr_t *prefix);

/**
 * @brief   Removes a prefix from this border router.
 *
 * @param[in] abr       The local border router
 * @param[in] iface     The IPv6 interface the prefix was added to.
 * @param[in] prefix    The prefix.
 */
void gnrc_sixlowpan_nd_router_abr_rem_prf(gnrc_sixlowpan_nd_router_abr_t *abr,
                                          gnrc_ipv6_netif_t *iface, gnrc_ipv6_netif_addr_t *prefix);

/**
 * @brief   Adds a context for this border router to manage.
 *
 * @param[in] abr       The local border router
 * @param[in] ctx       The context to be add.
 *
 * @return  0, on success
 * @return  -EINVAL, if @p ctx is greater than 15.
 * @return  -ENOENT, if @p abr is not registered.
 */
int gnrc_sixlowpan_nd_router_abr_add_ctx(gnrc_sixlowpan_nd_router_abr_t *abr, uint8_t cid);

/**
 * @brief   Removes a context from this border router.
 *
 * @param[in] abr       The local border router
 * @param[in] ctx       The context to be remove.
 */
void gnrc_sixlowpan_nd_router_abr_rem_ctx(gnrc_sixlowpan_nd_router_abr_t *abr, uint8_t cid);
#else
#define gnrc_sixlowpan_nd_router_abr_create(addr, ltime)    (NULL)
#endif

#ifdef __cplusplus
}
#endif

#endif /* GNRC_SIXLOWPAN_ND_ROUTER_H_ */
/** @} */
