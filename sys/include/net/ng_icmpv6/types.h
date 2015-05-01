/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_icmpv6_types ICMPv6 message type definitions
 * @ingroup     net_ng_icmpv6
 * @brief       Type numbers for the corresponding field in ICMPv6 messages.
 * @{
 *
 * @file
 * @brief   Macro definitions for ICMPv6 type fields
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_ICMPV6_TYPES_H_
#define NG_ICMPV6_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @{
 * @name Error message types
 * @see <a href="http://www.iana.org/assignments/icmpv6-parameters/icmpv6-parameters.xhtml#icmpv6-parameters-2">
 *          IANA, ICMPv6 "type" Numbers
 *      </a>
 */
#define NG_ICMPV6_DEST_UNR      (1)     /**< Destination unreachable message */
#define NG_ICMPV6_PKT_TOO_BIG   (2)     /**< Packet Too Big message */
#define NG_ICMPV6_TIME_EXC      (3)     /**< Time Exceeded message */
#define NG_ICMPV6_PARAM_PROB    (4)     /**< Parameter Problem message */
#define NG_ICMPV6_ERR_EXP1      (100)   /**< message type for private experimentation */
#define NG_ICMPV6_ERR_EXP2      (101)   /**< message type for private experimentation */
/**
 * @}
 */

/**
 * @{
 * @name Informational message types
 * @see <a href="http://www.iana.org/assignments/icmpv6-parameters/icmpv6-parameters.xhtml#icmpv6-parameters-2">
 *          IANA, ICMPv6 "type" Numbers
 *      </a>
 */
#define NG_ICMPV6_ECHO_REQ      (128)   /**< Echo request message (ping) */
#define NG_ICMPV6_ECHO_REP      (129)   /**< Echo reply message (pong) */
#define NG_ICMPV6_RTR_SOL       (133)   /**< NDP router solicitation message */
#define NG_ICMPV6_RTR_ADV       (134)   /**< NDP router advertisement message */
#define NG_ICMPV6_NBR_SOL       (135)   /**< NDP neighbor solicitation message */
#define NG_ICMPV6_NBR_ADV       (136)   /**< NDP neighbor advertisement message */
#define NG_ICMPV6_REDIRECT      (137)   /**< NDP redirect message */
#define NG_ICMPV6_RPL_CTRL      (155)   /**< RPL control message */
#define NG_ICMPV6_INF_EXP1      (200)   /**< message type for private experimentation */
#define NG_ICMPV6_INF_EXP2      (201)   /**< message type for private experimentation */
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* NG_ICMPV6_TYPES_H_ */
/** @} */
