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


#ifndef GNRC_IPV6_EXT_H_
#define GNRC_IPV6_EXT_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "kernel_types.h"
#include "net/gnrc/pkt.h"
#include "net/ipv6/ext.h"

#ifdef __cplusplus
extern "C" {
#endif

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
bool gnrc_ipv6_ext_demux(kernel_pid_t iface, gnrc_pktsnip_t *pkt,
                         uint8_t nh);

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

#endif /* GNRC_IPV6_EXT_H_ */
/**
 * @}
 */
