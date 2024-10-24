/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ipv6_ext    IPv6 extension headers
 * @ingroup     net_ipv6
 * @brief       Provides IPv6 extension header definitions and helper functions.
 * @{
 *
 * @file
 * @brief   IPv6 extension header definitions.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_IPV6_EXT_H
#define NET_IPV6_EXT_H

#include <stdint.h>

#include "net/ipv6/ext/frag.h"
#include "net/ipv6/ext/rh.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IPV6_EXT_LEN_UNIT   (8U)    /**< Unit in byte for the extension header's
                                     *   length field */

/**
 * @brief   IPv6 extension headers.
 *
 * @see [RFC 8200, section 4](https://tools.ietf.org/html/rfc8200#section-4)
 */
typedef struct __attribute__((packed)) {
    uint8_t nh;     /**< next header */
    uint8_t len;    /**< length in 8 octets without first octet */
} ipv6_ext_t;

/**
 * @brief   Gets the next extension header in a packet.
 *
 * @param[in] ext   The current extension header.
 *
 * @return  The next extension header.
 */
static inline ipv6_ext_t *ipv6_ext_get_next(ipv6_ext_t *ext)
{
    return (ipv6_ext_t *)((uint8_t *)(ext) + (ext->len * IPV6_EXT_LEN_UNIT) +
                          IPV6_EXT_LEN_UNIT);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_IPV6_EXT_H */
