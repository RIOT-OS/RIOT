/*
 * Copyright (C) 2017 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_gomach
 * @{
 *
 * @file
 * @brief       Timeout handling of GoMacH.
 * @internal
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#ifndef NET_GNRC_GOMACH_TIMEOUT_H
#define NET_GNRC_GOMACH_TIMEOUT_H

#include <stdint.h>
#include <stdbool.h>

#include "msg.h"
#include "xtimer.h"
#include "net/gnrc/netif.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set GoMacH timeout of type @p type of offset @p offset.
 *
 * @param[in,out] netif    the network interface.
 * @param[in]     type         GoMacH timeout type.
 * @param[in]     offset       timeout offset.
 */
void gnrc_gomach_set_timeout(gnrc_netif_t *netif,
                             gnrc_gomach_timeout_type_t type,
                             uint32_t offset);

/**
 * @brief   Clear GoMacH timeout of type @p type.
 *
 * @param[in,out] netif    the network interface.
 * @param[in]     type         GoMacH timeout type.
 */
void gnrc_gomach_clear_timeout(gnrc_netif_t *netif, gnrc_gomach_timeout_type_t type);

/**
 * @brief   Check whether GoMacH timeout of type @p type is running.
 *
 * @param[in]     netif    the network interface.
 * @param[in]     type         GoMacH timeout type.
 *
 * @return        true, if timeout of type @p type is running.
 * @return        false, if timeout of type @p type is not running.
 */
bool gnrc_gomach_timeout_is_running(gnrc_netif_t *netif, gnrc_gomach_timeout_type_t type);

/**
 * @brief   Check whether GoMacH timeout of type @p type is expired. It will clear
 *          the timeout once it is found expired.
 *
 * @param[in,out] netif    the network interface.
 * @param[in]     type         GoMacH timeout type.
 *
 * @return        true, if timeout of type @p type is expired.
 * @return        false, if timeout of type @p type is not expired, or not exist.
 */
bool gnrc_gomach_timeout_is_expired(gnrc_netif_t *netif, gnrc_gomach_timeout_type_t type);

/**
 * @brief   Reset all GoMacH timeouts.
 *
 * @param[in,out] netif    the network interface.
 */
void gnrc_gomach_reset_timeouts(gnrc_netif_t *netif);

/**
 * @brief   Make a specific GoMacH timeout expired.
 *
 * @param[in,out] timeout   GoMacH timeout
 */
static inline void gnrc_gomach_timeout_make_expire(gnrc_gomach_timeout_t *timeout)
{
    assert(timeout);

    timeout->expired = true;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_GOMACH_TIMEOUT_H */
