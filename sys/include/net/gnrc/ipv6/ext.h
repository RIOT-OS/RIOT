/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_ext IPv6 extension headers.
 * @ingroup     net_gnrc_ipv6
 * @brief       Implementation of IPv6 extension headers
 * @see [RFC 8200, section 4](https://tools.ietf.org/html/rfc8200#section-4)
 * @{
 *
 * @file
 * @brief       Definititions for IPv6 extension headers
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef NET_GNRC_IPV6_EXT_H
#define NET_GNRC_IPV6_EXT_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "net/gnrc/pkt.h"
#include "net/ipv6/ext.h"

#ifdef MODULE_GNRC_IPV6_EXT_RH
#include "net/gnrc/ipv6/ext/rh.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Demultiplex an extension header according to @p nh.
 *
 * @param[in] pkt       A packet with the first snip pointing to the extension
 *                      header to process.
 * @param[in] nh        Protocol number of @p pkt.
 *
 * @return  The packet for further processing.
 * @return  NULL, on error or if packet was consumed (by e.g. forwarding via
 *          a routing header). @p pkt is released in case of error.
 */
gnrc_pktsnip_t *gnrc_ipv6_ext_demux(gnrc_pktsnip_t *pkt, unsigned nh);

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
gnrc_pktsnip_t *gnrc_ipv6_ext_build(gnrc_pktsnip_t *ipv6, gnrc_pktsnip_t *next,
                                    uint8_t nh, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_EXT_H */
/**
 * @}
 */
