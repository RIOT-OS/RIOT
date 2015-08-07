/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ng_ipv6_ext IPv6 extension headers.
 * @ingroup     net_ng_ipv6
 * @brief       Implementation of IPv6 extension headers
 * @see <a href="https://tools.ietf.org/html/rfc2460#section-4">
 *          RFC 2460, section 4
 *      </a>
 * @{
 *
 * @file
 * @brief       Definititions for IPv6 extension headers
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef NG_IPV6_EXT_H_
#define NG_IPV6_EXT_H_

#include <inttypes.h>
#include <stdbool.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "net/ng_pkt.h"

#include "net/ng_ipv6/ext/rh.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NG_IPV6_EXT_LEN_UNIT    (8U)    /**< Unit in byte for the extension header's
                                         *   length field */

/**
 * @brief   IPv6 extension headers.
 *
 * @see <a href="https://tools.ietf.org/html/rfc2460#section-4">
 *          RFC 2460, section 4.1
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t nh;     /**< next header */
    uint8_t len;    /**< length in 8 octets without first octet */
} ng_ipv6_ext_t;

/**
 * @brief   Demultiplex extension headers according to @p nh.
 *
 * @internal
 *
 * @param[in] iface The receiving interface.
 * @param[in] pkt   A packet.
 * @param[in] nh    A protocol number (see @ref net_protnum).
 *
 * @return  true, on success.
 * @return  false, on failure.
 */
bool ng_ipv6_ext_demux(kernel_pid_t iface, ng_pktsnip_t *pkt,
                       uint8_t nh);

/**
 * @brief   Gets the next extension header in a packet.
 *
 * @param[in] ext   The current extension header.
 *
 * @return  The next extension header.
 */
static inline ng_ipv6_ext_t *ng_ipv6_ext_get_next(ng_ipv6_ext_t *ext)
{
    return (ng_ipv6_ext_t *)((uint8_t *)(ext) +
                             (ext->len * NG_IPV6_EXT_LEN_UNIT) +
                             NG_IPV6_EXT_LEN_UNIT);
}

/**
 * @brief   Builds an extension header for sending.
 *
 * @param[in] ipv6  The IPv6 header. Can be NULL.
 * @param[in] next  The next header. Must be a successor to @p ipv6 if it is
 *                  not NULL.
 * @param[in] nh    @ref net_protnum of the next header.
 * @param[in] size  Size of the extension header.
 *
 * @return  The extension header on success.
 * @return  NULL, on error.
 */
ng_pktsnip_t *ng_ipv6_ext_build(ng_pktsnip_t *ipv6, ng_pktsnip_t *next,
                                uint8_t nh, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* NG_IPV6_EXT_H_ */
/**
 * @}
 */
