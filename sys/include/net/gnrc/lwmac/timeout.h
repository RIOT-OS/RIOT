/*
 * Copyright (C) 2015 Daniel Krebs
 *               2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_lwmac
 * @{
 *
 * @file
 * @brief       Timeout handling of LWMAC
 *
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#ifndef NET_GNRC_LWMAC_TIMEOUT_H
#define NET_GNRC_LWMAC_TIMEOUT_H

#include <stdint.h>
#include <stdbool.h>

#include "net/gnrc/netif.h"
#include "net/gnrc/lwmac/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Static initializer for @ref gnrc_lwmac_timeout_t.
 */
#define GNRC_LWMAC_TIMEOUT_INITIAL  { {}, {}, false, TIMEOUT_DISABLED }

/**
 * @brief   Set LWMAC timeout of type @p type of offset @p offset.
 *
 * @param[in,out] netif        the network interface
 * @param[in]     type         LWMAC timeout type
 * @param[in]     offset       timeout offset
 */
void gnrc_lwmac_set_timeout(gnrc_netif_t *netif,
                            gnrc_lwmac_timeout_type_t type,
                            uint32_t offset);

/**
 * @brief   Clear LWMAC timeout of type @p type.
 *
 * @param[in,out] netif        the network interface
 * @param[in]     type         LWMAC timeout type
 */
void gnrc_lwmac_clear_timeout(gnrc_netif_t *netif, gnrc_lwmac_timeout_type_t type);

/**
 * @brief   Check whether LWMAC timeout of type @p type is running.
 *
 * @param[in]     netif        the network interface
 * @param[in]     type         LWMAC timeout type
 *
 * @return        true, if timeout of type @p type is running.
 * @return        false, if timeout of type @p type is not running.
 */
bool gnrc_lwmac_timeout_is_running(gnrc_netif_t *netif,
                                   gnrc_lwmac_timeout_type_t type);

/**
 * @brief   Check whether LWMAC timeout of type @p type is expired. It will clear
 *          the timeout once it is found expired.
 *
 * @param[in,out] netif        the network interface
 * @param[in]     type         LWMAC timeout type
 *
 * @return        true, if timeout of type @p type is expired.
 * @return        false, if timeout of type @p type is not expired, or not exist.
 */
bool gnrc_lwmac_timeout_is_expired(gnrc_netif_t *netif, gnrc_lwmac_timeout_type_t type);

/**
 * @brief   Reset all LWMAC timeouts.
 *
 * @param[in,out] netif        the network interface
 */
void gnrc_lwmac_reset_timeouts(gnrc_netif_t *netif);

/**
 * @brief   Make a specific LWMAC timeout expired.
 *
 * @param[in,out] timeout   LWMAC timeout
 */
void gnrc_lwmac_timeout_make_expire(gnrc_lwmac_timeout_t *timeout);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_LWMAC_TIMEOUT_H */
