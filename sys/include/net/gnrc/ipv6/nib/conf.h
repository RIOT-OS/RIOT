/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_nib_conf  GNRC IPv6 NIB compile configurations
 * @ingroup     net_gnrc_ipv6_nib
 * @ingroup     net_gnrc_conf
 * @brief       Configuration macros for neighbor information base
 * @{
 *
 * @file
 * @brief       Configuration macro definitions for neighbor information base
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_IPV6_NIB_CONF_H
#define NET_GNRC_IPV6_NIB_CONF_H

#include <kernel_defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Apply configurations only if not using Kconfig for this module */
#if !IS_ACTIVE(CONFIG_KCONFIG_MODULE_GNRC_IPV6_NIB) || defined(DOXYGEN)

/* some pseudo-module based configuration, doc: see below */
#ifdef MODULE_GNRC_IPV6_NIB_6LBR
#ifndef CONFIG_GNRC_IPV6_NIB_6LBR
#define CONFIG_GNRC_IPV6_NIB_6LBR                     1
#endif
#ifndef CONFIG_GNRC_IPV6_NIB_SLAAC
#define CONFIG_GNRC_IPV6_NIB_SLAAC                    1
#endif
#ifndef CONFIG_GNRC_IPV6_NIB_NUMOF
#define CONFIG_GNRC_IPV6_NIB_NUMOF                   (16)
#endif
#endif

#ifdef MODULE_GNRC_IPV6_NIB_6LR
#ifndef CONFIG_GNRC_IPV6_NIB_6LR
#define CONFIG_GNRC_IPV6_NIB_6LR                      1
#endif
#ifndef CONFIG_GNRC_IPV6_NIB_SLAAC
#define CONFIG_GNRC_IPV6_NIB_SLAAC                    0
#endif
#endif

#ifdef MODULE_GNRC_IPV6_NIB_6LN
#ifndef CONFIG_GNRC_IPV6_NIB_6LN
#define CONFIG_GNRC_IPV6_NIB_6LN                      1
#endif
#ifndef CONFIG_GNRC_IPV6_NIB_SLAAC
#define CONFIG_GNRC_IPV6_NIB_SLAAC                    0
#endif
#ifndef CONFIG_GNRC_IPV6_NIB_QUEUE_PKT
#define CONFIG_GNRC_IPV6_NIB_QUEUE_PKT                0
#endif
#if !CONFIG_GNRC_IPV6_NIB_6LR
# ifndef CONFIG_GNRC_IPV6_NIB_ARSM
# define CONFIG_GNRC_IPV6_NIB_ARSM                    0
# endif
# ifndef CONFIG_GNRC_IPV6_NIB_NUMOF
/* only needs to store default router */
# define CONFIG_GNRC_IPV6_NIB_NUMOF                  (1)
# endif
#endif
#endif

#ifdef MODULE_GNRC_IPV6_NIB_ROUTER
#define CONFIG_GNRC_IPV6_NIB_ROUTER                   1
#endif

#ifdef MODULE_GNRC_IPV6_NIB_DNS
#define CONFIG_GNRC_IPV6_NIB_DNS                      1
#endif

/**
 * @name    Compile flags
 * @brief   Compile flags to (de-)activate certain features for NIB
 * @{
 */
/**
 * @brief   enable features for 6Lo border router
 */
#ifndef CONFIG_GNRC_IPV6_NIB_6LBR
#define CONFIG_GNRC_IPV6_NIB_6LBR                     0
#endif

/**
 * @brief    enable features for 6Lo router
 */
#ifndef CONFIG_GNRC_IPV6_NIB_6LR
#if CONFIG_GNRC_IPV6_NIB_6LBR
#define CONFIG_GNRC_IPV6_NIB_6LR                      1
#else
#define CONFIG_GNRC_IPV6_NIB_6LR                      0
#endif
#endif

/**
 * @brief    enable features for 6Lo node
 */
#ifndef CONFIG_GNRC_IPV6_NIB_6LN
#if CONFIG_GNRC_IPV6_NIB_6LR
#define CONFIG_GNRC_IPV6_NIB_6LN                      1
#else
#define CONFIG_GNRC_IPV6_NIB_6LN                      0
#endif
#endif

/**
 * @brief   enable features for IPv6 routers
 */
#ifndef CONFIG_GNRC_IPV6_NIB_ROUTER
#if CONFIG_GNRC_IPV6_NIB_6LR
#define CONFIG_GNRC_IPV6_NIB_ROUTER                   1
#else
#define CONFIG_GNRC_IPV6_NIB_ROUTER                   0
#endif
#endif

/**
 * @brief    (de-)activate router advertising at interface start-up
 */
#ifndef CONFIG_GNRC_IPV6_NIB_ADV_ROUTER
#if CONFIG_GNRC_IPV6_NIB_ROUTER && \
    (!CONFIG_GNRC_IPV6_NIB_6LR || CONFIG_GNRC_IPV6_NIB_6LBR)
#define CONFIG_GNRC_IPV6_NIB_ADV_ROUTER               1
#else
#define CONFIG_GNRC_IPV6_NIB_ADV_ROUTER               0
#endif
#endif

/**
 * @brief   (de-)activate NDP address resolution state-machine
 */
#ifndef CONFIG_GNRC_IPV6_NIB_ARSM
#define CONFIG_GNRC_IPV6_NIB_ARSM                     1
#endif

/**
 * @brief    queue packets for address resolution
 */
#ifndef CONFIG_GNRC_IPV6_NIB_QUEUE_PKT
#if CONFIG_GNRC_IPV6_NIB_6LN
#define CONFIG_GNRC_IPV6_NIB_QUEUE_PKT                0
#else
#define CONFIG_GNRC_IPV6_NIB_QUEUE_PKT                1
#endif
#endif

/**
 * @brief   handle NDP messages according for stateless address
 *          auto-configuration (if activated on interface)
 *
 * @see [RFC 4862](https://tools.ietf.org/html/rfc4862)
 */
#ifndef CONFIG_GNRC_IPV6_NIB_SLAAC
#define CONFIG_GNRC_IPV6_NIB_SLAAC                    1
#endif

/**
 * @brief    handle Redirect Messages
 */
#ifndef CONFIG_GNRC_IPV6_NIB_REDIRECT
#define CONFIG_GNRC_IPV6_NIB_REDIRECT                 0
#endif

/**
 * @brief   (de-)activate destination cache
 */
#ifndef CONFIG_GNRC_IPV6_NIB_DC
#if CONFIG_GNRC_IPV6_NIB_REDIRECT
#define CONFIG_GNRC_IPV6_NIB_DC                       1
#else
#define CONFIG_GNRC_IPV6_NIB_DC                       0
#endif
#endif

/**
 * @brief   Support for DNS configuration options
 *
 * @see [RFC 8106](https://tools.ietf.org/html/rfc8106)
 */
#ifndef CONFIG_GNRC_IPV6_NIB_DNS
#define CONFIG_GNRC_IPV6_NIB_DNS                      0
#endif

/**
 * @brief   Multihop prefix and 6LoWPAN context distribution
 *
 * @see [RFC 6775, section 8.1](https://tools.ietf.org/html/rfc6775#section-8.1)
 */
#ifndef CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C
#if CONFIG_GNRC_IPV6_NIB_6LR
#define CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C             1
#else
#define CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C             0
#endif
#endif

/**
 * @brief   Multihop duplicate address detection
 *
 * @see [RFC 6775, section 8.2](https://tools.ietf.org/html/rfc6775#section-8.2)
 */
#ifndef CONFIG_GNRC_IPV6_NIB_MULTIHOP_DAD
#define CONFIG_GNRC_IPV6_NIB_MULTIHOP_DAD             0
#endif
/** @} */

/**
 * @brief   Reset time in milliseconds for the reachability time
 *
 * @see [RFC 4861, section 6.3.4](https://tools.ietf.org/html/rfc4861#section-6.3.4)
 */
#ifndef CONFIG_GNRC_IPV6_NIB_REACH_TIME_RESET
#define CONFIG_GNRC_IPV6_NIB_REACH_TIME_RESET        (7200000U)
#endif

/**
 * @brief   Disable router solicitations
 *
 * @warning Only do this if you know what you're doing
 */
#ifndef CONFIG_GNRC_IPV6_NIB_NO_RTR_SOL
#define CONFIG_GNRC_IPV6_NIB_NO_RTR_SOL               0
#endif

/**
 * @brief   Maximum link-layer address length (aligned)
 */
#ifndef CONFIG_GNRC_IPV6_NIB_L2ADDR_MAX_LEN
#define CONFIG_GNRC_IPV6_NIB_L2ADDR_MAX_LEN          (8U)
#endif

/**
 * @brief   Number of default routers in the default router list.
 *
 * @attention   This number has direct influence on the maximum number of
 *              default routers
 */
#ifndef CONFIG_GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF
#define CONFIG_GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF    (1)
#endif

/**
 * @brief   Number of entries in NIB
 *
 * @attention   This number has direct influence on the maximum number of
 *              neighbors and duplicate address detection table entries
 */
#ifndef CONFIG_GNRC_IPV6_NIB_NUMOF
#define CONFIG_GNRC_IPV6_NIB_NUMOF                   (4)
#endif

/**
 * @brief   Number of off-link entries in NIB
 *
 * @attention   This number is equal to the maximum number of forwarding table
 *              and prefix list entries in NIB
 */
#ifndef CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF
#define CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF              (8)
#endif

#if CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C || defined(DOXYGEN)
/**
 * @brief   Number of authoritative border router entries in NIB
 */
#ifndef CONFIG_GNRC_IPV6_NIB_ABR_NUMOF
#define CONFIG_GNRC_IPV6_NIB_ABR_NUMOF               (1)
#endif
#endif

#endif /* !CONFIG_KCONFIG_MODULE_GNRC_IPV6_NIB || DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_NIB_CONF_H */
/** @} */
