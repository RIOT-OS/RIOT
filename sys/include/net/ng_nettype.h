/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net
 * @{
 *
 * @file
 * @brief   Protocol type definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_NETTYPE_H_
#define NG_NETTYPE_H_

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

#ifdef __cplusplus
}
#endif

#endif /* NG_NETTYPE_H_ */
/** @} */
