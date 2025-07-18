/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_gnrc_nettype  gnrc_nettype: Protocol type
 * @ingroup     net_gnrc
 * @brief       Protocol type definitions and helper functions
 *
 * The protocol types are used with the @ref net_gnrc_netapi, the @ref net_gnrc_netif,
 * the @ref net_gnrc_netreg, and the @ref net_gnrc_pkt to identify network protocols
 * throughout the network stack.
 *
 *
 * To include a nettype into your build, use the corresponding module of the same name in
 * lower-case, e.g., to use @ref GNRC_NETTYPE_IPV6 in your code, use:
 *
 * ```
 * USEMODULE += gnrc_nettype_ipv6
 * ```
 *
 * @{
 *
 * @file
 * @brief   Protocol type definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <inttypes.h>

#include "modules.h"
#include "net/ethertype.h"
#include "net/protnum.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definition of protocol types in the network stack.
 *
 * @note    Expand at will.
 */
typedef enum {
    /**
     * @brief   TX synchronization data for passing up error data or
     *          auxiliary data
     */
    GNRC_NETTYPE_TX_SYNC = -3,
    /**
     * @brief   Protocol is as defined in @ref gnrc_netif_hdr_t. Not usable with
     *          @ref net_gnrc_netreg
     */
    GNRC_NETTYPE_NETIF = -1,
    GNRC_NETTYPE_UNDEF = 0,     /**< Protocol is undefined */

    /**
     * @{
     * @name Link layer
     */
#if IS_USED(MODULE_GNRC_NETTYPE_CUSTOM) || defined(DOXYGEN)
    GNRC_NETTYPE_CUSTOM,         /**< Custom ethertype */
#endif
    /** @} */

#if IS_USED(MODULE_GNRC_NETTYPE_SIXLOWPAN) || defined(DOXYGEN)
    GNRC_NETTYPE_SIXLOWPAN,     /**< Protocol is 6LoWPAN */
#endif

#if IS_USED(MODULE_GNRC_NETTYPE_LORAWAN) || defined(DOXYGEN)
/**
 * @brief       Protocol is LoRaWAN
 * @deprecated  LoRaWAN payloads do not have a special type anymore and just use
 *              @ref GNRC_NETTYPE_UNDEF. Will be removed after 2024.10 release.
 */
#define GNRC_NETTYPE_LORAWAN    GNRC_NETTYPE_UNDEF
#endif

    /**
     * @{
     * @name Network layer
     */
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6) || defined(DOXYGEN)
    GNRC_NETTYPE_IPV6,          /**< Protocol is IPv6 */
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6_EXT) || defined(DOXYGEN)
    GNRC_NETTYPE_IPV6_EXT,      /**< Protocol is IPv6 extension header */
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_ICMPV6) || defined(DOXYGEN)
    GNRC_NETTYPE_ICMPV6,        /**< Protocol is ICMPv6 */
#endif

#if IS_USED(MODULE_GNRC_NETTYPE_CCN) || defined(DOXYGEN)
    GNRC_NETTYPE_CCN,           /**< Protocol is CCN */
    GNRC_NETTYPE_CCN_CHUNK,     /**< Protocol is CCN, packet contains a content
                                     chunk */
#endif

#if IS_USED(MODULE_GNRC_NETTYPE_NDN) || defined(DOXYGEN)
    GNRC_NETTYPE_NDN,           /**< Protocol is NDN */
#endif
    /** @} */

    /**
     * @{
     * @name Transport layer
     */
#if IS_USED(MODULE_GNRC_NETTYPE_TCP) || defined(DOXYGEN)
    GNRC_NETTYPE_TCP,           /**< Protocol is TCP */
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_UDP) || defined(DOXYGEN)
    GNRC_NETTYPE_UDP,           /**< Protocol is UDP */
#endif
    /** @} */

    /**
     * @{
     * @name Testing
     */
#ifdef TEST_SUITES
    GNRC_NETTYPE_TEST,          /**< Usable with test vectors */
#endif
    /** @} */

    GNRC_NETTYPE_NUMOF,         /**< maximum number of available protocols */
} gnrc_nettype_t;

/**
 * @brief   Translates an Ether Type number to @ref gnrc_nettype_t
 * @see     [IANA, ETHER TYPES](http://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml)
 *
 * @param[in] type  An Ether Type number
 *
 * @return  The corresponding @ref gnrc_nettype_t to @p type.
 * @return  @ref GNRC_NETTYPE_UNDEF if @p type not translatable.
 */
static inline gnrc_nettype_t gnrc_nettype_from_ethertype(uint16_t type)
{
    switch (type) {
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6)
        case ETHERTYPE_IPV6:
            return GNRC_NETTYPE_IPV6;
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_CCN) || IS_USED(MODULE_GNRC_NETTYPE_NDN)
        case ETHERTYPE_NDN:
#if IS_USED(MODULE_GNRC_NETTYPE_CCN)
            return GNRC_NETTYPE_CCN;
#elif IS_USED(MODULE_GNRC_NETTYPE_NDN)
            return GNRC_NETTYPE_NDN;
#endif
#endif
#if IS_USED(MODULE_GNRC_SIXLOENC) && IS_USED(MODULE_GNRC_NETTYPE_SIXLOWPAN)
        case ETHERTYPE_6LOENC:
            return GNRC_NETTYPE_SIXLOWPAN;
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_CUSTOM)
        case ETHERTYPE_CUSTOM:
            return GNRC_NETTYPE_CUSTOM;
#endif
        default:
            return GNRC_NETTYPE_UNDEF;
    }
}

/**
 * @brief   Translates @ref gnrc_nettype_t to an Ether Type number
 * @see     [IANA, ETHER TYPES](http://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml)
 *
 * @param[in] type  A protocol type
 *
 * @return  The corresponding Ether Type number to @p type.
 * @return  @ref ETHERTYPE_RESERVED if @p type not translatable.
 */
static inline uint16_t gnrc_nettype_to_ethertype(gnrc_nettype_t type)
{
    switch (type) {
#if IS_USED(MODULE_GNRC_NETTYPE_CUSTOM)
        case GNRC_NETTYPE_CUSTOM:
            return ETHERTYPE_CUSTOM;
#endif
#if IS_USED(MODULE_GNRC_SIXLOENC) && IS_USED(MODULE_GNRC_NETTYPE_SIXLOWPAN)
        case GNRC_NETTYPE_SIXLOWPAN:
            return ETHERTYPE_6LOENC;
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6)
        case GNRC_NETTYPE_IPV6:
            return ETHERTYPE_IPV6;
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_CCN)
        case GNRC_NETTYPE_CCN:
            return ETHERTYPE_NDN;
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_NDN)
        case GNRC_NETTYPE_NDN:
            return ETHERTYPE_NDN;
#endif
        default:
            return ETHERTYPE_UNKNOWN;
    }
}

/**
 * @brief   Translates a Protocol Number to @ref gnrc_nettype_t
 * @see     <a href="http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml">
 *              IANA, Assigned Internet Protocol Numbers
 *          </a>
 *
 * @param[in] num   A Protocol Number
 *
 * @return  The corresponding @ref gnrc_nettype_t to @p num.
 * @return  @ref GNRC_NETTYPE_UNDEF if @p num not translatable.
 */
static inline gnrc_nettype_t gnrc_nettype_from_protnum(uint8_t num)
{
    switch (num) {
#if IS_USED(MODULE_GNRC_NETTYPE_ICMPV6)
        case PROTNUM_ICMPV6:
            return GNRC_NETTYPE_ICMPV6;
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6)
        case PROTNUM_IPV6:
            return GNRC_NETTYPE_IPV6;
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_TCP)
        case PROTNUM_TCP:
            return GNRC_NETTYPE_TCP;
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_UDP)
        case PROTNUM_UDP:
            return GNRC_NETTYPE_UDP;
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6_EXT)
        case PROTNUM_IPV6_EXT_HOPOPT:
        case PROTNUM_IPV6_EXT_DST:
        case PROTNUM_IPV6_EXT_RH:
        case PROTNUM_IPV6_EXT_FRAG:
        case PROTNUM_IPV6_EXT_AH:
        case PROTNUM_IPV6_EXT_ESP:
        case PROTNUM_IPV6_EXT_MOB:
            return GNRC_NETTYPE_IPV6_EXT;
#endif
        default:
            return GNRC_NETTYPE_UNDEF;
    }
}

/**
 * @brief   Translates @ref gnrc_nettype_t to a Protocol Number
 * @see     <a href="http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml">
 *              IANA, Assigned Internet Protocol Numbers
 *          </a>
 *
 * @param[in] type  A protocol type
 *
 * @return  The corresponding Protocol Number to @p type.
 * @return  @ref PROTNUM_RESERVED if @p type not translatable.
 */
static inline uint8_t gnrc_nettype_to_protnum(gnrc_nettype_t type)
{
    switch (type) {
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6)
        case GNRC_NETTYPE_IPV6:
            return PROTNUM_IPV6;
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_ICMPV6)
        case GNRC_NETTYPE_ICMPV6:
            return PROTNUM_ICMPV6;
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_TCP)
        case GNRC_NETTYPE_TCP:
            return PROTNUM_TCP;
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_UDP)
        case GNRC_NETTYPE_UDP:
            return PROTNUM_UDP;
#endif
        default:
            return PROTNUM_RESERVED;
    }
}

#ifdef __cplusplus
}
#endif

/** @} */
