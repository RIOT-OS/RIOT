/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_nib_conf  Configuration macros
 * @ingroup     net_gnrc_ipv6_nib
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

#ifdef __cplusplus
extern "C" {
#endif

/* some pseudo-module based configuration, doc: see below */
#ifdef MODULE_GNRC_IPV6_NIB_6LBR
#ifndef GNRC_IPV6_NIB_CONF_6LBR
#define GNRC_IPV6_NIB_CONF_6LBR         (1)
#endif
#ifndef GNRC_IPV6_NIB_NUMOF
#define GNRC_IPV6_NIB_NUMOF             (16)
#endif
#endif

#ifdef MODULE_GNRC_IPV6_NIB_6LR
#ifndef GNRC_IPV6_NIB_CONF_6LR
#define GNRC_IPV6_NIB_CONF_6LR          (1)
#endif
#ifndef GNRC_IPV6_NIB_CONF_SLAAC
#define GNRC_IPV6_NIB_CONF_SLAAC        (0)
#endif
#endif

#ifdef MODULE_GNRC_IPV6_NIB_6LN
#ifndef GNRC_IPV6_NIB_CONF_6LN
#define GNRC_IPV6_NIB_CONF_6LN          (1)
#endif
#ifndef GNRC_IPV6_NIB_CONF_SLAAC
#define GNRC_IPV6_NIB_CONF_SLAAC        (0)
#endif
#ifndef GNRC_IPV6_NIB_CONF_QUEUE_PKT
#define GNRC_IPV6_NIB_CONF_QUEUE_PKT    (0)
#endif
#if !GNRC_IPV6_NIB_CONF_6LR
# ifndef GNRC_IPV6_NIB_CONF_ARSM
# define GNRC_IPV6_NIB_CONF_ARSM        (0)
# endif
# ifndef GNRC_IPV6_NIB_NUMOF
/* only needs to store default router */
# define GNRC_IPV6_NIB_NUMOF            (1)
# endif
#endif
#endif

#ifdef MODULE_GNRC_IPV6_NIB_ROUTER
#define GNRC_IPV6_NIB_CONF_ROUTER       (1)
#endif

#ifdef MODULE_GNRC_IPV6_NIB_DNS
#define GNRC_IPV6_NIB_CONF_DNS          (1)
#endif

/**
 * @name    Compile flags
 * @brief   Compile flags to (de-)activate certain features for NIB
 * @{
 */
/**
 * @brief   enable features for 6Lo border router
 */
#ifndef GNRC_IPV6_NIB_CONF_6LBR
#define GNRC_IPV6_NIB_CONF_6LBR         (0)
#endif

/**
 * @brief    enable features for 6Lo router
 */
#ifndef GNRC_IPV6_NIB_CONF_6LR
#if GNRC_IPV6_NIB_CONF_6LBR
#define GNRC_IPV6_NIB_CONF_6LR          (1)
#else
#define GNRC_IPV6_NIB_CONF_6LR          (0)
#endif
#endif

/**
 * @brief    enable features for 6Lo node
 */
#ifndef GNRC_IPV6_NIB_CONF_6LN
#if GNRC_IPV6_NIB_CONF_6LR
#define GNRC_IPV6_NIB_CONF_6LN          (1)
#else
#define GNRC_IPV6_NIB_CONF_6LN          (0)
#endif
#endif

/**
 * @brief   enable features for IPv6 routers
 */
#ifndef GNRC_IPV6_NIB_CONF_ROUTER
#if GNRC_IPV6_NIB_CONF_6LR
#define GNRC_IPV6_NIB_CONF_ROUTER       (1)
#else
#define GNRC_IPV6_NIB_CONF_ROUTER       (0)
#endif
#endif

/**
 * @brief    (de-)activate router advertising at interface start-up
 */
#ifndef GNRC_IPV6_NIB_CONF_ADV_ROUTER
#if GNRC_IPV6_NIB_CONF_ROUTER && \
    (!GNRC_IPV6_NIB_CONF_6LR || GNRC_IPV6_NIB_CONF_6LBR)
#define GNRC_IPV6_NIB_CONF_ADV_ROUTER   (1)
#else
#define GNRC_IPV6_NIB_CONF_ADV_ROUTER   (0)
#endif
#endif

/**
 * @brief   (de-)activate NDP address resolution state-machine
 */
#ifndef GNRC_IPV6_NIB_CONF_ARSM
#define GNRC_IPV6_NIB_CONF_ARSM         (1)
#endif

/**
 * @brief    queue packets for address resolution
 */
#ifndef GNRC_IPV6_NIB_CONF_QUEUE_PKT
#if GNRC_IPV6_NIB_CONF_6LN
#define GNRC_IPV6_NIB_CONF_QUEUE_PKT    (0)
#else
#define GNRC_IPV6_NIB_CONF_QUEUE_PKT    (1)
#endif
#endif

/**
 * @brief   handle NDP messages according for stateless address
 *          auto-configuration (if activated on interface)
 *
 * @see [RFC 4862](https://tools.ietf.org/html/rfc4862)
 */
#ifndef GNRC_IPV6_NIB_CONF_SLAAC
#define GNRC_IPV6_NIB_CONF_SLAAC        (1)
#endif

/**
 * @brief    handle Redirect Messages
 */
#ifndef GNRC_IPV6_NIB_CONF_REDIRECT
#define GNRC_IPV6_NIB_CONF_REDIRECT     (0)
#endif

/**
 * @brief   (de-)activate destination cache
 */
#ifndef GNRC_IPV6_NIB_CONF_DC
#if GNRC_IPV6_NIB_CONF_REDIRECT
#define GNRC_IPV6_NIB_CONF_DC           (1)
#else
#define GNRC_IPV6_NIB_CONF_DC           (0)
#endif
#endif

/**
 * @brief   Support for DNS configuration options
 *
 * @see [RFC 8106](https://tools.ietf.org/html/rfc8106)
 */
#ifndef GNRC_IPV6_NIB_CONF_DNS
#define GNRC_IPV6_NIB_CONF_DNS          (0)
#endif

/**
 * @brief   Multihop prefix and 6LoWPAN context distribution
 *
 * @see [RFC 6775, section 8.1](https://tools.ietf.org/html/rfc6775#section-8.1)
 */
#ifndef GNRC_IPV6_NIB_CONF_MULTIHOP_P6C
#if GNRC_IPV6_NIB_CONF_6LR
#define GNRC_IPV6_NIB_CONF_MULTIHOP_P6C (1)
#else
#define GNRC_IPV6_NIB_CONF_MULTIHOP_P6C (0)
#endif
#endif

/**
 * @brief   Multihop duplicate address detection
 *
 * @see [RFC 6775, section 8.2](https://tools.ietf.org/html/rfc6775#section-8.2)
 */
#ifndef GNRC_IPV6_NIB_CONF_MULTIHOP_DAD
#define GNRC_IPV6_NIB_CONF_MULTIHOP_DAD (0)
#endif
/** @} */

/**
 * @brief   Reset time in milliseconds for the reachability time
 *
 * @see [RFC 4861, section 6.3.4](https://tools.ietf.org/html/rfc4861#section-6.3.4)
 */
#ifndef GNRC_IPV6_NIB_CONF_REACH_TIME_RESET
#define GNRC_IPV6_NIB_CONF_REACH_TIME_RESET (7200000U)
#endif

/**
 * @brief   Disable router solicitations
 *
 * @warning Only do this if you know what you're doing
 */
#ifndef GNRC_IPV6_NIB_CONF_NO_RTR_SOL
#define GNRC_IPV6_NIB_CONF_NO_RTR_SOL       (0)
#endif

/**
 * @brief   Maximum link-layer address length (aligned)
 */
#ifndef GNRC_IPV6_NIB_L2ADDR_MAX_LEN
#define GNRC_IPV6_NIB_L2ADDR_MAX_LEN        (8U)
#endif

/**
 * @brief   Number of default routers in the default router list.
 *
 * @attention   This number has direct influence on the maximum number of
 *              default routers
 */
#ifndef GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF
#define GNRC_IPV6_NIB_DEFAULT_ROUTER_NUMOF  (1)
#endif

/**
 * @brief   Number of entries in NIB
 *
 * @attention   This number has direct influence on the maximum number of
 *              neighbors and duplicate address detection table entries
 */
#ifndef GNRC_IPV6_NIB_NUMOF
#define GNRC_IPV6_NIB_NUMOF                 (4)
#endif

/**
 * @brief   Number of off-link entries in NIB
 *
 * @attention   This number is equal to the maximum number of forwarding table
 *              and prefix list entries in NIB
 */
#ifndef GNRC_IPV6_NIB_OFFL_NUMOF
#define GNRC_IPV6_NIB_OFFL_NUMOF            (8)
#endif

#if GNRC_IPV6_NIB_CONF_MULTIHOP_P6C || defined(DOXYGEN)
/**
 * @brief   Number of authoritative border router entries in NIB
 */
#ifndef GNRC_IPV6_NIB_ABR_NUMOF
#define GNRC_IPV6_NIB_ABR_NUMOF             (1)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_NIB_CONF_H */
/** @} */
