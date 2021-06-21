/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_ipv6_nib
 * @internal
 * @{
 *
 * @file
 * @brief   Definitions related to router functionality of NIB
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef PRIV_NIB_ROUTER_H
#define PRIV_NIB_ROUTER_H

#include <kernel_defines.h>

#include "net/gnrc/ipv6/nib/conf.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/netif/ipv6.h"
#include "net/ipv6/addr.h"
#include "net/ndp.h"

#include "_nib-internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ROUTER) || defined(DOXYGEN)
/**
 * @brief   Initializes interface for router behavior
 *
 * @param[in] netif An interface.
 */
static inline void _init_iface_router(gnrc_netif_t *netif)
{
    netif->ipv6.rtr_ltime = NDP_RTR_LTIME_SEC;
    netif->ipv6.last_ra = UINT32_MAX;
    netif->ipv6.ra_sent = 0;
    netif->flags |= GNRC_NETIF_FLAGS_IPV6_FORWARDING;

    if (IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ADV_ROUTER)) {
        netif->flags |= GNRC_NETIF_FLAGS_IPV6_RTR_ADV;
    }

    if (IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LBR)) {
        netif->flags |= GNRC_NETIF_FLAGS_6LO_ABR;
    }

    gnrc_netif_ipv6_group_join_internal(netif, &ipv6_addr_all_routers_link_local);
}

/**
 * @brief   Helper function to safely call the
 *          [route info callback](@ref gnrc_netif_ipv6_t::route_info_cb) of an
 *          interface
 *
 * @pre `netif != NULL`.
 *
 * @param[in] netif     An interface.
 * @param[in] type      [Type](@ref net_gnrc_ipv6_nib_route_info_type) of the
 *                      route info.
 * @param[in] ctx_addr  Context address of the route info.
 * @param[in] ctx       Further context of the route info.
 */
static inline void _call_route_info_cb(gnrc_netif_t *netif, unsigned type,
                                       const ipv6_addr_t *ctx_addr,
                                       const void *ctx)
{
    assert(netif != NULL);
    if (netif->ipv6.route_info_cb != NULL) {
        netif->ipv6.route_info_cb(type, ctx_addr, ctx);
    }
}

/**
 * @brief   Handler for @ref GNRC_IPV6_NIB_REPLY_RS event handler
 *
 * @param[in] host  Host that sent the router solicitation
 */
void _handle_reply_rs(_nib_onl_entry_t *host);

/**
 * @brief   Handler for @ref GNRC_IPV6_NIB_SND_MC_RA event handler
 *
 * @param[in] netif Network interface to send multicast router advertisement
 *                  over.
 */
void _handle_snd_mc_ra(gnrc_netif_t *netif);

/**
 * @brief   Set the @ref GNRC_NETIF_FLAGS_IPV6_RTR_ADV flag for an interface
 *          and starts advertising that interface as a router
 *
 * @param[in] netif Interface to set the @ref GNRC_NETIF_FLAGS_IPV6_RTR_ADV
 *                  for.
 */
void _set_rtr_adv(gnrc_netif_t *netif);

/**
 * @brief   Send router advertisements
 *
 * If @ref CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C is not 0 this sends one router
 * advertisement per configured ABR, otherwise it just sends one single router
 * advertisement for the interface.
 *
 * @param[in] netif The interface to send the router advertisement over.
 * @param[in] dst   Destination address for the router advertisement.
 * @param[in] final The router advertisement are the final ones of the @p netif
 *                  (because it was set to be a non-forwarding interface e.g.).
 */
void _snd_rtr_advs(gnrc_netif_t *netif, const ipv6_addr_t *dst,
                  bool final);
#else  /* CONFIG_GNRC_IPV6_NIB_ROUTER */
#define _init_iface_router(netif)                       (void)netif
#define _call_route_info_cb(netif, type, ctx_addr, ctx) (void)netif; \
                                                        (void)type; \
                                                        (void)ctx_addr; \
                                                        (void)ctx
#define _handle_reply_rs(host)                          (void)host
#define _handle_snd_mc_ra(netif)                        (void)netif
#define _set_rtr_adv(netif)                             (void)netif
#define _snd_rtr_advs(netif, dst, final)                (void)netif; \
                                                        (void)dst; \
                                                        (void)final
#endif /* CONFIG_GNRC_IPV6_NIB_ROUTER */

#ifdef __cplusplus
}
#endif

#endif /* PRIV_NIB_ROUTER_H */
/** @} */
