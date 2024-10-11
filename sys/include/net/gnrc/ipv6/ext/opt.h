/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_ext_opt Support for IPv6 option extension headers
 * @ingroup     net_gnrc_ipv6_ext
 * @brief       GNRC implementation of IPv6 hop-by-hop and destination option
 *              header extension
 * @{
 *
 * @file
 * @brief   GNRC hop-by-hop and destination option header definitions.
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_IPV6_EXT_OPT_H
#define NET_GNRC_IPV6_EXT_OPT_H

#include <stdint.h>

#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Processes all options within an IPv6 option header
 *
 * @pre `pkt != NULL`
 * @pre `(protnum == PROTNUM_IPV6_EXT_HOPOPT) || (protnum == PROTNUM_IPV6_EXT_DST)`
 *
 * @param[in] pkt       The packet containing the option header. The option
 *                      must be contained in the first snip, with all
 *                      preceding headers marked (in receive order).
 *                      Must not be NULL.
 * @param[in] protnum   The protocol number of the option header. Must be
 *                      @ref PROTNUM_IPV6_EXT_HOPOPT or @ref
 *                      PROTNUM_IPV6_EXT_DST
 *
 * @return  @p pkt with the option header marked on success.
 * @return  NULL, if the packet was consumed by the option handling.
 * @return  NULL, on error. @p pkt is released with EINVAL in that case and if
 *          necessary and [`gnrc_icmpv6_error`](@ref net_gnrc_icmpv6_error) is
 *          used, the according ICMPv6 error message is sent.
 */
gnrc_pktsnip_t *gnrc_ipv6_ext_opt_process(gnrc_pktsnip_t *pkt,
                                          uint8_t protnum);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_IPV6_EXT_OPT_H */
