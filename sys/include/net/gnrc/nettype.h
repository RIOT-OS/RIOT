/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_nettype  Protocol type
 * @ingroup     net_gnrc
 * @brief       Protocol type definitions and helper functions
 *
 * The protocol types are used with the @ref net_gnrc_netapi, the @ref net_gnrc_netif,
 * the @ref net_gnrc_netreg, and the @ref net_gnrc_pkt to identify network protocols
 * throughout the network stack.
 *
 * @{
 *
 * @file
 * @brief   Protocol type definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETTYPE_H
#define NET_GNRC_NETTYPE_H

#include <inttypes.h>

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
     * @brief   Not so much protocol but data type that is passed to network
     *          devices using the netdev interface
     */
    GNRC_NETTYPE_IOVEC = -2,
    /**
     * @brief   Protocol is as defined in @ref gnrc_netif_hdr_t. Not usable with
     *          @ref net_gnrc_netreg
     */
    GNRC_NETTYPE_NETIF = -1,
    GNRC_NETTYPE_UNDEF = 0,     /**< Protocol is undefined */

#ifdef MODULE_GNRC_SIXLOWPAN
    GNRC_NETTYPE_SIXLOWPAN,     /**< Protocol is 6LoWPAN */
#endif

    /**
     * @{
     * @name Link layer
     */
#ifdef MODULE_GNRC_GOMACH
    GNRC_NETTYPE_GOMACH,         /**< Protocol is GoMacH */
#endif
    /**
     * @}
     */

    /**
     * @{
     * @name Link layer
     */
#ifdef MODULE_GNRC_LWMAC
    GNRC_NETTYPE_LWMAC,          /**< Protocol is lwMAC */
#endif
    /**
     * @}
     */

    /**
     * @{
     * @name Network layer
     */
#ifdef MODULE_GNRC_IPV6
    GNRC_NETTYPE_IPV6,          /**< Protocol is IPv6 */
#endif
#ifdef MODULE_GNRC_IPV6_EXT
    GNRC_NETTYPE_IPV6_EXT,      /**< Protocol is IPv6 extension header */
#endif
#ifdef MODULE_GNRC_ICMPV6
    GNRC_NETTYPE_ICMPV6,        /**< Protocol is ICMPv6 */
#endif
    /**
     * @}
     */

    /**
     * @{
     * @name Transport layer
     */
#ifdef MODULE_GNRC_TCP
    GNRC_NETTYPE_TCP,           /**< Protocol is TCP */
#endif
#ifdef MODULE_GNRC_UDP
    GNRC_NETTYPE_UDP,           /**< Protocol is UDP */
#endif
    /**
     * @}
     */

#ifdef MODULE_CCN_LITE
    GNRC_NETTYPE_CCN,           /**< Protocol is CCN */
    GNRC_NETTYPE_CCN_CHUNK,     /**< Protocol is CCN, packet contains a content
                                     chunk */
#endif

#ifdef MODULE_NDN_RIOT
    GNRC_NETTYPE_NDN,           /**< Protocol is NDN */
#endif

    /**
     * @{
     * @name Testing
     */
#ifdef TEST_SUITES
    GNRC_NETTYPE_TEST,
#endif
    /**
     * @}
     */

    GNRC_NETTYPE_NUMOF,         /**< maximum number of available protocols */
} gnrc_nettype_t;

/**
 * @brief   Translates an Ether Type number to @ref net_gnrc_nettype
 * @see     <a href="http://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml#ieee-802-numbers-1">
 *              IANA, ETHER TYPES
 *          </a>
 *
 * @param[in] type  An Ether Type number
 *
 * @return  The corresponding @ref net_gnrc_nettype to @p type.
 * @return  @ref GNRC_NETTYPE_UNDEF if @p type not translatable.
 */
static inline gnrc_nettype_t gnrc_nettype_from_ethertype(uint16_t type)
{
    switch (type) {
#ifdef MODULE_GNRC_IPV6
        case ETHERTYPE_IPV6:
            return GNRC_NETTYPE_IPV6;
#endif
#if defined(MODULE_CCN_LITE) || defined(MODULE_NDN_RIOT)
        case ETHERTYPE_NDN:
#if defined(MODULE_CCN_LITE)
            return GNRC_NETTYPE_CCN;
#elif defined(MODULE_NDN_RIOT)
            return GNRC_NETTYPE_NDN;
#endif
#endif
        default:
            return GNRC_NETTYPE_UNDEF;
    }
}

/**
 * @brief   Translates @ref net_gnrc_nettype to an Ether Type number
 * @see     <a href="http://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml#ieee-802-numbers-1">
 *              IANA, ETHER TYPES
 *          </a>
 *
 * @param[in] type  A protocol type
 *
 * @return  The corresponding Ether Type number to @p type.
 * @return  @ref ETHERTYPE_RESERVED if @p type not translatable.
 */
static inline uint16_t gnrc_nettype_to_ethertype(gnrc_nettype_t type)
{
    switch (type) {
#ifdef MODULE_GNRC_IPV6
        case GNRC_NETTYPE_IPV6:
            return ETHERTYPE_IPV6;
#endif
#ifdef MODULE_CCN_LITE
        case GNRC_NETTYPE_CCN:
            return ETHERTYPE_NDN;
#endif
#ifdef MODULE_NDN_RIOT
        case GNRC_NETTYPE_NDN:
            return ETHERTYPE_NDN;
#endif
        default:
            return ETHERTYPE_UNKNOWN;
    }
}

/**
 * @brief   Translates a Protocol Number to @ref net_gnrc_nettype
 * @see     <a href="http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml">
 *              IANA, Assigned Internet Protocol Numbers
 *          </a>
 *
 * @param[in] num   A Protocol Number
 *
 * @return  The corresponding @ref net_gnrc_nettype to @p num.
 * @return  @ref GNRC_NETTYPE_UNDEF if @p num not translatable.
 */
static inline gnrc_nettype_t gnrc_nettype_from_protnum(uint8_t num)
{
    switch (num) {
#ifdef MODULE_GNRC_ICMPV6
        case PROTNUM_ICMPV6:
            return GNRC_NETTYPE_ICMPV6;
#endif
#ifdef MODULE_GNRC_IPV6
        case PROTNUM_IPV6:
            return GNRC_NETTYPE_IPV6;
#endif
#ifdef MODULE_GNRC_TCP
        case PROTNUM_TCP:
            return GNRC_NETTYPE_TCP;
#endif
#ifdef MODULE_GNRC_UDP
        case PROTNUM_UDP:
            return GNRC_NETTYPE_UDP;
#endif
#ifdef MODULE_GNRC_IPV6_EXT
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
 * @brief   Translates @ref net_gnrc_nettype to a Protocol Number
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
#ifdef MODULE_GNRC_ICMPV6
        case GNRC_NETTYPE_ICMPV6:
            return PROTNUM_ICMPV6;
#endif
#ifdef MODULE_GNRC_IPV6
        case GNRC_NETTYPE_IPV6:
            return PROTNUM_IPV6;
#endif
#ifdef MODULE_GNRC_TCP
        case GNRC_NETTYPE_TCP:
            return PROTNUM_TCP;
#endif
#ifdef MODULE_GNRC_UDP
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

#endif /* NET_GNRC_NETTYPE_H */
/** @} */
