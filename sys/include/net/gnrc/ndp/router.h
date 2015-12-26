/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ndp_router Router-specific part of router discovery.
 * @ingroup     net_gnrc_ndp
 * @brief       Router-specific part for the router discovery in IPv6
 *              neighbor discovery.
 * @{
 *
 * @file
 * @brief   Router-specific router discovery definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_NDP_ROUTER_H_
#define GNRC_NDP_ROUTER_H_

#include <stdbool.h>

#include "kernel_types.h"
#include "net/ipv6/hdr.h"
#include "net/ndp.h"
#include "net/gnrc/ipv6/nc.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set @p iface to router mode.
 *
 * @details This sets/unsets the GNRC_IPV6_NETIF_FLAGS_ROUTER and
 *          GNRC_IPV6_NETIF_FLAGS_RTR_ADV and initializes or ceases router
 *          behavior for neighbor discovery.
 *
 * @param[in] iface     An IPv6 interface. Must not be NULL.
 * @param[in] enable    Status for the GNRC_IPV6_NETIF_FLAGS_ROUTER and
 *                      GNRC_IPV6_NETIF_FLAGS_RTR_ADV flags.
 */
void gnrc_ndp_router_set_router(gnrc_ipv6_netif_t *iface, bool enable);

/**
 * @brief   Set/Unset GNRC_IPV6_NETIF_FLAGS_RTR_ADV flag for @p iface.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-6.2.2">
 *          RFC 4861, section 6.2.2
 *      </a>
 * @see <a href="https://tools.ietf.org/html/rfc4861#section-6.2.5">
 *          RFC 4861, section 6.2.5
 *      </a>
 *
 * @details GNRC_IPV6_NETIF_FLAGS_RTR_ADV and initializes or ceases
 *          periodic router advertising behavior for neighbor discovery.
 *
 * @param[in] iface     An IPv6 interface. Must not be NULL.
 * @param[in] enable    Status for the GNRC_IPV6_NETIF_FLAGS_RTR_ADV flags.
 */
void gnrc_ndp_router_set_rtr_adv(gnrc_ipv6_netif_t *iface, bool enable);

/**
 * @brief   Send an unsolicited router advertisement over @p iface
 *          and reset the timer for the next one if necessary.
 *
 * @param[in] iface An IPv6 interface.
 */
void gnrc_ndp_router_retrans_rtr_adv(gnrc_ipv6_netif_t *iface);

/**
 * @brief   Send an solicited router advertisement to IPv6 address of
 *          @p neighbor.
 *
 * @param[in] neighbor  A neighbor cache entry.
 */
void gnrc_ndp_router_send_rtr_adv(gnrc_ipv6_nc_t *neighbor);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NDP_ROUTER_H_ */
/** @} */
