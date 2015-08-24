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
#include "net/ipv6/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief   Extract next hop from the routing header of an IPv6 packet.
 *
 * @param[in] ipv6      An IPv6 packet.
 *
 * @return  next hop on success, on success
 * @return  NULL, if not found.
 */
ipv6_addr_t *ipv6_ext_rh_next_hop(ipv6_hdr_t *ipv6);

#ifdef __cplusplus
}
#endif

#endif /* IPV6_EXT_RH_H_ */
/** @} */
