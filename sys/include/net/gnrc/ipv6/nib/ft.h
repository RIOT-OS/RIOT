/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_nib_ft    Forwarding table
 * @ingroup     net_gnrc_ipv6_nib
 * @brief
 * @{
 *
 * @file
 * @brief   Forwarding table definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_IPV6_NIB_FT_H
#define NET_GNRC_IPV6_NIB_FT_H

#include <stdint.h>

#include "net/gnrc/pkt.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Forwarding table entry view on NIB
 */
typedef struct {
    ipv6_addr_t dst;        /**< destination or prefix */
    ipv6_addr_t next_hop;   /**< next hop to gnrc_ipv6_nib_ft_t::dst */
    uint8_t dst_len;        /**< prefix-length in bits of
                             *   gnrc_ipv6_nib_ft_t::dst */
    uint8_t primary;        /**< != 0 if gnrc_ipv6_nib_ft_t::dst is preferred
                             *   default route */
    uint16_t iface;         /**< interface to gnrc_ipv6_nib_ft_t::next_hop */
} gnrc_ipv6_nib_ft_t;

/**
 * @brief   Gets the best matching forwarding table entry to a destination
 *
 * @pre `(dst != NULL) && (fte != NULL)`
 *
 * @param[in] dst   The destination.
 * @param[in] pkt   Packet that is supposed to go to that destination
 *                  (is handed over to a reactive routing protocol if one exists
 *                  on the interface found and no route is found)
 * @param[out] fte  The resulting forwarding table entry.
 *
 * @return  0, on success.
 * @return  -ENETUNREACH, if no route was found.
 */
int gnrc_ipv6_nib_ft_get(const ipv6_addr_t *dst, gnrc_pktsnip_t *pkt,
                         gnrc_ipv6_nib_ft_t *fte);

/**
 * @brief   Adds a new route to the forwarding table
 *
 * If @p dst is the default route, the route will be configured to be the
 * default route.
 *
 * @param[in] dst       The destination to the route. May be NULL or `::` for
 *                      default route.
 * @param[in] dst_len   The prefix length of @p dst in bits. May be 0 for
 *                      default route.
 * @param[in] next_hop  The next hop to @p dst/@p dst_len. May be NULL, if
 *                      @p dst/@p dst_len is no the default route.
 * @param[in] iface     The interface to @p next_hop. May not be 0.
 * @param[in] lifetime  Lifetime of the route in seconds. 0 for infinite
 *                      lifetime.
 *
 * @return  0, on success.
 * @return  -EINVAL, if a parameter was of invalid value.
 * @return  -ENOMEM, if there was no space left in forwarding table.
 */
int gnrc_ipv6_nib_ft_add(const ipv6_addr_t *dst, unsigned dst_len,
                         const ipv6_addr_t *next_hop, unsigned iface,
                         uint32_t lifetime);

/**
 * @brief   Deletes a route from forwarding table.
 *
 * If @p dst is the default route, the function assures, that the current
 * primary default route is removed first.
 *
 * @param[in] dst       The destination of the route. May be NULL or `::` for
 *                      default route.
 * @param[in] dst_len   The prefix length of @p dst in bits. May be 0 for
 *                      default route.
 */
void gnrc_ipv6_nib_ft_del(const ipv6_addr_t *dst, unsigned dst_len);

/**
 * @brief   Iterates over all forwarding table entries in the NIB
 *
 * @pre `(state != NULL) && (fte != NULL)`
 *
 * @param[in] next_hop      Restrict iteration to entries to this next hop. NULL
 *                          for any next hop. Can be used to build a source
 *                          routing tree.
 * @param[in] iface         Restrict iteration to entries on this interface.
 *                          0 for any interface.
 * @param[in,out] state     Iteration state of the forwarding table. Must point
 *                          to a NULL pointer to start iteration.
 * @param[out] fte          The next forwarding table entry.
 *
 * The iteration over all forwarding table entries in the NIB includes all
 * entries added via @p gnrc_ipv6_nib_ft_add() and entries that are currently
 * in the Destination Cache, in the Prefix List (only if they're on-link),
 * and in the Default Router List.
 *
 * Usage example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "net/gnrc/ipv6/nib/ft.h"
 *
 * int main(void) {
 *     void *state = NULL;
 *     gnrc_ipv6_nib_ft_t fte;
 *
 *     puts("My neighbors:");
 *     while (gnrc_ipv6_nib_ft_iter(NULL, 0, &state, &fte)) {
 *         gnrc_ipv6_nib_ft_print(&fte);
 *     }
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note    The list may change during iteration.
 *
 * @return  true, if iteration can be continued.
 * @return  false, if @p fte is the last neighbor cache entry in the NIB.
 */
bool gnrc_ipv6_nib_ft_iter(const ipv6_addr_t *next_hop, unsigned iface,
                           void **state, gnrc_ipv6_nib_ft_t *fte);

/**
 * @brief   Prints a forwarding table entry
 *
 * @pre `fce != NULL`
 *
 * @param[in] fte   A forwarding table entry.
 */
void gnrc_ipv6_nib_ft_print(const gnrc_ipv6_nib_ft_t *fte);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_NIB_FT_H */
/** @} */
