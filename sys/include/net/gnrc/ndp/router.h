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
#ifndef NET_GNRC_NDP_ROUTER_H
#define NET_GNRC_NDP_ROUTER_H

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
 * @brief   Send an solicited router advertisement to IPv6 address of
 *          @p neighbor.
 *
 * @param[in] neighbor  A neighbor cache entry.
 */
void gnrc_ndp_router_send_rtr_adv(gnrc_ipv6_nc_t *neighbor);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NDP_ROUTER_H */
/** @} */
