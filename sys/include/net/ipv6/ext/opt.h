/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ipv6_ext_opt IPv6 destination and hop-by-hop options
 * @ingroup     net_ipv6_ext
 * @brief       Definitions for IPv6 destination and hop-by-hop options
 *              extension headers
 * @{
 *
 * @file
 * @brief   Destination and hop-by-hop options extension header definitions.
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_IPV6_EXT_OPT_H
#define NET_IPV6_EXT_OPT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Destination and hop-by-hop option types
 * @see [IANA, IPv6 parameters]
 *      (https://www.iana.org/assignments/ipv6-parameters/ipv6-parameters.xhtml#ipv6-parameters-2)
 * @{
 */
#define IPV6_EXT_OPT_PAD1           (0x00U) /**< Pad1 */
#define IPV6_EXT_OPT_PADN           (0x01U) /**< PadN */
#define IPV6_EXT_OPT_JUMBO          (0xC2U) /**< Jumbo payload */
#define IPV6_EXT_OPT_RPL            (0x63U) /**< RPL Option */
#define IPV6_EXT_OPT_TEL            (0x04U) /**< Tunnel Encapsulation Limit */
#define IPV6_EXT_OPT_RTR_ALERT      (0x05U) /**< Router Alert */
#define IPV6_EXT_OPT_QUICK_START    (0x26U) /**< Quick-Start */
#define IPV6_EXT_OPT_CALIPSO        (0x07U) /**< CALIPSO */
#define IPV6_EXT_OPT_SMF_DPD        (0x08U) /**< SMF_DPD */
#define IPV6_EXT_OPT_HOME_ADDR      (0xC9U) /**< Home Address */
#define IPV6_EXT_OPT_ILNP_NONCE     (0x8BU) /**< ILNP Nonce */
#define IPV6_EXT_OPT_LIO            (0x8CU) /**< Line-Identification Option */
#define IPV6_EXT_OPT_MPL            (0x6DU) /**< MPL Option */
#define IPV6_EXT_OPT_IP_DFF         (0xEEU) /**< IP_DFF */
#define IPV6_EXT_OPT_PDM            (0x0FU) /**< Performance and Diagnostic Metrics */
/** @} */

/**
 * @name Processing actions
 * @see [RFC 8200, section 4.2](https://tools.ietf.org/html/rfc8200#section-4.2)
 *
 * > The Option Type identifiers are internally encoded such that their
 * > highest-order 2 bits specify the action that must be taken if the
 * > processing IPv6 node does not recognize the Option Type
 * @{
 */
/**
 * @brief   mask to decode action from type
 */
#define IPV6_EXT_OPT_ACTION_MASK            (0xc0)
/**
 * @brief   skip over this option and continue processing the header
 */
#define IPV6_EXT_OPT_ACTION_SKIP            (0x00)
#define IPV6_EXT_OPT_ACTION_DISC            (0x40)  /**< discard the packet */
/**
 * @brief   discard the packet
 *
 * > and, regardless of whether or not the packet's Destination Address
 * > was a multicast address, send an ICMP Parameter Problem, Code 2,
 * > message to the packet's Source Address, pointing to the
 * > unrecognized Option Type.
 */
#define IPV6_EXT_OPT_ACTION_DISC_ERR_MCAST  (0x80)

/**
 * @brief   discard the packet
 *
 * > and, only if the packet's Destination Address was not a multicast
 * > address, send an ICMP Parameter Problem, Code 2, message to the
 * > packet's Source Address, pointing to the unrecognized Option Type.
 */
#define IPV6_EXT_OPT_ACTION_DISC_ERR        (0xc0)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_IPV6_EXT_OPT_H */
/** @} */
