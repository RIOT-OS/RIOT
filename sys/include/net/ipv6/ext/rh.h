/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ipv6_ext_rh IPv6 routing header extension
 * @ingroup     net_ipv6_ext
 * @brief       Implementation of IPv6 routing header extension.
 * @{
 *
 * @file
 * @brief   Routing extension header definitions.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef IPV6_EXT_RH_H_
#define IPV6_EXT_RH_H_

#include <stdint.h>

#include "net/ipv6/addr.h"
#include "net/ipv6/ext.h"
#include "net/ipv6/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Return codes for routing header processing
 * @{
 */
#define EXT_RH_CODE_ERROR       (-1)
#define EXT_RH_CODE_FORWARD     (0)
#define EXT_RH_CODE_OK          (1)
/**
 * @}
 */

/**
 * @brief   IPv6 routing extension header.
 *
 * @see <a href="https://tools.ietf.org/html/rfc2460#section-4.4">
 *          RFC 2460, section 4.4
 *      </a>
 *
 * @extends ipv6_ext_t
 */
typedef struct __attribute__((packed)) {
    uint8_t nh;         /**< next header */
    uint8_t len;        /**< length in 8 octets without first octet */
    uint8_t type;       /**< identifier of a particular routing header type */
    uint8_t seg_left;   /**< number of route segments remaining */
} ipv6_ext_rh_t;

/**
 * @brief   Process the routing header of an IPv6 packet.
 *
 * @param[in, out] ipv6     An IPv6 packet.
 * @param[in] ext           A routing header of @ipv6.
 *
 * @return  EXT_RH_CODE_ERROR
 * @return  EXT_RH_CODE_FORWARD
 * @return  EXT_RH_CODE_OK
 */
int ipv6_ext_rh_process(ipv6_hdr_t *ipv6, ipv6_ext_rh_t *ext);

#ifdef __cplusplus
}
#endif

#endif /* IPV6_EXT_RH_H_ */
/** @} */
