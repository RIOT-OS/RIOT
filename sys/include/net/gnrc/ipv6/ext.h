/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "net/gnrc/pkt.h"
#include "net/ipv6/ext.h"
#include "timex.h"

#ifdef MODULE_GNRC_IPV6_EXT_RH
#include "net/gnrc/ipv6/ext/rh.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup    net_gnrc_ipv6_ext_conf GNRC IPv6 extension header compile configurations
 * @ingroup     net_gnrc_ipv6_ext
 * @ingroup     net_gnrc_conf
 * @{
 */
/**
 * @brief   IPv6 fragmentation send buffer size
 *
 * This limits the total amount of datagrams that can be fragmented at the same time.
 *
 * @note    Only applicable with [gnrc_ipv6_ext_frag](@ref net_gnrc_ipv6_ext_frag) module
 */
#ifndef CONFIG_GNRC_IPV6_EXT_FRAG_SEND_SIZE
#define CONFIG_GNRC_IPV6_EXT_FRAG_SEND_SIZE    (1U)
#endif

/**
 * @brief   IPv6 fragmentation reassembly buffer size
 *
 * This limits the total amount of datagrams that can be reassembled at the same time.
 *
 * @note    Only applicable with [gnrc_ipv6_ext_frag](@ref net_gnrc_ipv6_ext_frag) module
 */
#ifndef CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_SIZE
#define CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_SIZE        (1U)
#endif

/**
 * @brief   The number of total allocatable @ref gnrc_ipv6_ext_frag_limits_t objects
 *
 * This is the maximum number of receivable fragments, shared between all
 * fragmented datagrams
 *
 * @note    Only applicable with [gnrc_ipv6_ext_frag](@ref net_gnrc_ipv6_ext_frag) module
 */
#ifndef CONFIG_GNRC_IPV6_EXT_FRAG_LIMITS_POOL_SIZE
#define CONFIG_GNRC_IPV6_EXT_FRAG_LIMITS_POOL_SIZE (CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_SIZE * 2U)
#endif

/**
 * @brief   Timeout for IPv6 fragmentation reassembly buffer entries in microseconds
 *
 * @note    Only applicable with [gnrc_ipv6_ext_frag](@ref net_gnrc_ipv6_ext_frag) module
 */
#ifndef CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_TIMEOUT_US
#define CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_TIMEOUT_US  (10U * US_PER_SEC)
#endif

/**
 * @brief   Do not override oldest datagram when reassembly buffer is full
 *
 * @note    Only applicable with [gnrc_ipv6_ext_frag](@ref net_gnrc_ipv6_ext_frag) module
 *
 * When not set, it will cause the reassembly buffer to override the oldest
 * entry when a fragment for a new datagram is received. When set to 1, no entry
 * will be overwritten (they will still timeout normally)
 */
#ifdef DOXYGEN
#define CONFIG_GNRC_IPV6_EXT_FRAG_RBUF_DO_NOT_OVERRIDE
#endif

/** @} **/

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

#if     defined(MODULE_GNRC_IPV6_EXT) || defined(DOXYGEN)
/**
 * @brief   Processes a packet's payload as hop-by-hop option if @p protnum is
 *          pointing to a value equal to @ref PROTNUM_IPV6_EXT_HOPOPT.
 *
 * @note    Should be called by @ref net_gnrc_ipv6 before any further processing
 *          (even forwarding-related) is done to @p pkt.
 *
 * @param[in] pkt           An IPv6 packet in receive order. It assumed that
 *                          gnrc_pktsnip_t::data points to a hop-by-hop option
 *                          when @p protnum points to value equal to
 *                          @ref PROTNUM_IPV6_EXT_HOPOPT.
 * @param[in,out] protnum   **In:** The @ref net_protnum of gnrc_pktsnip_t::data
 *                          of @p pkt. <br />
 *                          **Out:** If @p protnum was
 *                          @ref PROTNUM_IPV6_EXT_HOPOPT on in and the return
 *                          value is not NULL it points to the value of the next
 *                          header field of the processed hop-by-hop option
 *                          header. Since the hop-by-hop option header is now
 *                          marked, gnrc_pktsnip_t::data of @p pkt will then be
 *                          identified by @p protnum. <br />
 *                          If @p protnum was @ref PROTNUM_IPV6_EXT_HOPOPT on in
 *                          and the return value is NULL the value @p protnum is
 *                          pointing to is undefined. <br />
 *                          If @p protnum unequal to
 *                          @ref PROTNUM_IPV6_EXT_HOPOPT on in the value
 *                          @p protnum is pointing to remains unchanged.
 *
 * @return  @p pkt with the hop-by-hop option marked on success.
 * @return  NULL, if the packet was consumed by the option handling.
 * @return  NULL, on error. @p pkt is released with EINVAL in that case.
 */
gnrc_pktsnip_t *gnrc_ipv6_ext_process_hopopt(gnrc_pktsnip_t *pkt,
                                             uint8_t *protnum);

/**
 * @brief   Processes a packet's extension headers after a potential initial
 *          hop-by-hop header.
 *
 * @note    Should be called by @ref net_gnrc_ipv6 after @ref
 *          gnrc_ipv6_ext_process_hopopt(), i.e. the (first) hop-by-hop option
 *          should already be marked in @p pkt. If a hop-by-hop option is found
 *          in gnrc_pktsnip_t::data of @p pkt, the function will return an
 *          error.
 *
 * @param[in] pkt           An IPv6 packet in receive order.
 * @param[in,out] protnum   **In:** The @ref net_protnum of gnrc_pktsnip_t::data
 *                          of @p pkt (i.e. the first extension header to be
 *                          processed). <br />
 *                          **Out:** The @ref net_protnum of header in
 *                          gnrc_pktsnip_t::data of @p pkt. The extension
 *                          headers are now marked, so their data can be found
 *                          in gnrc_pktsnip_t::next of @p pkt and the following.
 *                          <br />
 *                          If the return value is NULL, the value of @p protnum
 *                          is undefined.
 *
 * @return  @p pkt with all extension headers marked until the first
 *          non-extension header.
 * @return  NULL, if packet was consumed by the extension header handling.
 * @return  NULL, on error (including if one of the extension headers was a
 *          hop-by-hop option). @p pkt is released with EINVAL in that case.
 */
gnrc_pktsnip_t *gnrc_ipv6_ext_process_all(gnrc_pktsnip_t *pkt,
                                          uint8_t *protnum);
#else   /* defined(MODULE_GNRC_IPV6_EXT) || defined(DOXYGEN) */
/* NOPs to make the usage code more readable */
#define gnrc_ipv6_ext_process_hopopt(pkt, protnum)  (pkt)
#define gnrc_ipv6_ext_process_all(pkt, protnum)     (pkt)
#endif  /* defined(MODULE_GNRC_IPV6_EXT) || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
