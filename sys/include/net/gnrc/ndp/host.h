/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ndp_host Host-specific part of router discovery.
 * @ingroup     net_gnrc_ndp
 * @brief       Host-specific part for the router discovery in IPv6
 *              neighbor discovery.
 * @{
 *
 * @file
 * @brief   Host-specific router discovery definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef GNRC_NDP_HOST_H_
#define GNRC_NDP_HOST_H_

#include "net/gnrc/ipv6/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initializes interface @p iface as host.
 *
 * @pre iface != NULL
 *
 * @param[in] iface An IPv6 interface
 */
void gnrc_ndp_host_init(gnrc_ipv6_netif_t *iface);

/**
 * @brief   Sends a router solicitation over interface @p iface
 *          and reset the timer for the next one.
 *
 * @pre iface != NULL
 *
 * @param[in] iface An IPv6 interface
 */
void gnrc_ndp_host_retrans_rtr_sol(gnrc_ipv6_netif_t *iface);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NDP_HOST_H_ */
/** @} */
