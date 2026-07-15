/*
 * SPDX-FileCopyrightText: 2016 José Ignacio Alamos
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_ppp Point-to-Point Protocol (PPP) Data Link Layer
 * @ingroup     net
 * @brief       PPP types

 * @note        Last Updated: 2016-02-04
 * @{
 *
 * @file
 * @brief       PPP type definitions
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 * @see         <a href="http://www.iana.org/assignments/ppp-numbers/ppp-numbers.xhtml#ppp-numbers-2">
 *                  IANA, PPP NUMBERS
 *              </a>
 */

#ifdef __cplusplus
extern "C" {
#endif

#define PPPTYPE_IPV6           (0x0057)     /**< IPv6 packet in PPP*/
#define PPPTYPE_IPV4           (0x0021)     /**< IPv4 packet in PPP */
#define PPPTYPE_NCP_IPV6       (0x8057)     /**< NCP packet for IPV6*/
#define PPPTYPE_NCP_IPV4       (0x8021)     /**< NCP packet for IPv4*/
#define PPPTYPE_LCP            (0xC021)     /**< LCP packet  */
#define PPPTYPE_PAP            (0xC023)     /**< PAP packet  */
#define PPPTYPE_UNKNOWN        (0x8001)     /**<Unknown  packet (Unused protocol number according to IANA) */

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
