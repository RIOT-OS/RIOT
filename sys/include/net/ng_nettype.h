/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_nettype  Protocol type
 * @ingroup     net
 * @brief       Protocol type definitions to be used with the @ref net_ng_netapi,
 *              the @ref net_ng_netdev, the @ref net_ng_netreg, and
 *              the @ref net_ng_pkt
 * @{
 *
 * @file
 * @brief   Protocol type definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_NETTYPE_H_
#define NG_NETTYPE_H_

#include <inttypes.h>

#include "net/ng_ethertype.h"
#include "net/ng_protnum.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definition of protocol types in the network stack.
 *
 * @note    Expand at will.
 */
typedef enum {
    NG_NETTYPE_UNDEF = 0,       /**< Protocol is undefined */

#ifdef MODULE_NG_SIXLOWPAN
    NG_NETTYPE_SIXLOWPAN,       /**< Protocol is 6LoWPAN */
#endif

    /**
     * @{
     * @name Network layer
     */
#ifdef MODULE_NG_IPV6
    NG_NETTYPE_IPV6,            /**< Protocol is IPv6 */
#endif
#ifdef MODULE_NG_ICMPV6
    NG_NETTYPE_ICMPV6,          /**< Protocol is ICMPv6 */
#endif
    /**
     * @}
     */

    /**
     * @{
     * @name Transport layer
     */
#ifdef MODULE_NG_TCP
    NG_NETTYPE_TCP,             /**< Protocol is TCP */
#endif
#ifdef MODULE_NG_UDP
    NG_NETTYPE_UDP,             /**< Protocol is UDP */
#endif
    /**
     * @}
     */


    /**
     * @{
     * @name Testing
     */
#ifdef TEST_SUITES
    NG_NETTYPE_TEST,
#endif

    NG_NETTYPE_NUMOF,           /**< maximum number of available protocols */
} ng_nettype_t;

/**
 * @brief   Translates an Ether Type number to @ref net_ng_nettype
 * @see     <a href="http://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml#ieee-802-numbers-1">
 *              IANA, ETHER TYPES
 *          </a>
 *
 * @param[in] type  An Ether Type number
 *
 * @return  The corresponding @ref net_ng_nettype to @p type.
 * @return  @ref NG_NETTYPE_UNDEF if @p type not translatable.
 */
static inline ng_nettype_t ng_nettype_from_ethertype(uint16_t type)
{
    switch (type) {
#ifdef MODULE_NG_IPV6
        case NG_ETHERTYPE_IPV6:
            return NG_NETTYPE_IPV6;
#endif
        default:
            return NG_NETTYPE_UNDEF;
    }
}

/**
 * @brief   Translates @ref net_ng_nettype to an Ether Type number
 * @see     <a href="http://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml#ieee-802-numbers-1">
 *              IANA, ETHER TYPES
 *          </a>
 *
 * @param[in] type  A protocol type
 *
 * @return  The corresponding Ether Type number to @p type.
 * @return  @ref NG_ETHERTYPE_RESERVED if @p type not translatable.
 */
static inline uint16_t ng_nettype_to_ethertype(ng_nettype_t type)
{
    switch (type) {
#ifdef MODULE_NG_IPV6
        case NG_NETTYPE_IPV6:
            return NG_ETHERTYPE_IPV6;
#endif
        default:
            return NG_ETHERTYPE_RESERVED;
    }
}

/**
 * @brief   Translates a Protocol Number to @ref net_ng_nettype
 * @see     <a href="http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml">
 *              IANA, Assigned Internet Protocol Numbers
 *          </a>
 *
 * @param[in] num   A Protocol Number
 *
 * @return  The corresponding @ref net_ng_nettype to @p num.
 * @return  @ref NG_NETTYPE_UNDEF if @p num not translatable.
 */
static inline ng_nettype_t ng_nettype_from_protnum(uint8_t num)
{
    switch (num) {
#ifdef MODULE_NG_ICMPV6
        case NG_PROTNUM_ICMPV6:
            return NG_NETTYPE_ICMPV6;
#endif
#ifdef MODULE_NG_IPV6
        case NG_PROTNUM_IPV6:
            return NG_NETTYPE_IPV6;
#endif
#ifdef MODULE_NG_TCP
        case NG_PROTNUM_TCP:
            return NG_NETTYPE_TCP;
#endif
#ifdef MODULE_NG_UDP
        case NG_PROTNUM_UDP:
            return NG_NETTYPE_UDP;
#endif
        default:
            return NG_NETTYPE_UNDEF;
    }
}

/**
 * @brief   Translates @ref net_ng_nettype to a Protocol Number
 * @see     <a href="http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml">
 *              IANA, Assigned Internet Protocol Numbers
 *          </a>
 *
 * @param[in] type  A protocol type
 *
 * @return  The corresponding Protocol Number to @p type.
 * @return  @ref NG_PROTNUM_RESERVED if @p type not translatable.
 */
static inline uint8_t ng_nettype_to_protnum(ng_nettype_t type)
{
    switch (type) {
#ifdef MODULE_NG_ICMPV6
        case NG_NETTYPE_ICMPV6:
            return NG_PROTNUM_ICMPV6;
#endif
#ifdef MODULE_NG_IPV6
        case NG_NETTYPE_IPV6:
            return NG_PROTNUM_IPV6;
#endif
#ifdef MODULE_NG_TCP
        case NG_NETTYPE_TCP:
            return NG_PROTNUM_TCP;
#endif
#ifdef MODULE_NG_UDP
        case NG_NETTYPE_UDP:
            return NG_PROTNUM_UDP;
#endif
        default:
            return NG_PROTNUM_RESERVED;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* NG_NETTYPE_H_ */
/** @} */
