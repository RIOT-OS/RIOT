/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ipv6_ext_rh IPv6 routing header extension
 * @ingroup     net_ipv6_ext
 * @brief       Definitions for IPv6 routing header extension.
 * @{
 *
 * @file
 * @brief   Routing extension header definitions.
 *
 * @author  Cenk Gündoğan <cnkgndgn@gmail.com>
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_IPV6_EXT_RH_H
#define NET_IPV6_EXT_RH_H

#include <stdint.h>

#include "net/ipv6/addr.h"
#include "net/ipv6/ext.h"
#include "net/ipv6/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   IPv6 routing extension header.
 *
 * @see [RFC 8200](https://tools.ietf.org/html/rfc8200#section-4.4)
 *
 * @extends ipv6_ext_t
 */
typedef struct __attribute__((packed)) {
    uint8_t nh;         /**< next header */
    uint8_t len;        /**< length in 8 octets without first octet */
    uint8_t type;       /**< identifier of a particular routing header type */
    uint8_t seg_left;   /**< number of route segments remaining */
} ipv6_ext_rh_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_IPV6_EXT_RH_H */
/** @} */
