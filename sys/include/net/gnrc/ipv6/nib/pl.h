/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ipv6_nib_pl    Prefix list
 * @ingroup     net_gnrc_ipv6_nib
 * @brief       Prefix list component of neighbor information base
 * @{
 *
 * @file
 * @brief   Prefix list definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_IPV6_NIB_PL_H
#define NET_GNRC_IPV6_NIB_PL_H

#include <stdint.h>

#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Prefix list entry view on NIB
 */
typedef struct {
    ipv6_addr_t pfx;        /**< prefix */
    uint8_t pfx_len;        /**< length of gnrc_ipv6_nib_pl_t::pfx in bits */
    uint16_t iface;         /**< interface gnrc_ipv6_nib_pl_t::pfx is assigned
                             *   to */
    uint32_t valid_until;   /**< timestamp (in ms) until which the prefix is
                                 valid */
    uint32_t pref_until;    /**< timestamp (in ms) until which the prefix is
                                 preferred */
} gnrc_ipv6_nib_pl_t;

/**
 * @brief   Adds (or updates) prefix to NIB
 *
 * @pre `(pfx != NULL)`
 *
 * @param[in] iface         Interface @p pfx is valid on.
 * @param[in] pfx           The prefix. May not be a link-local prefix or a
 *                          multicast address and its first @p pfx_len bits
 *                          may not be 0. Must not be `NULL`.
 * @param[in] pfx_len       Length of @p pfx in bits.
 *                          Condition @p pfx_len > 0 must hold.
 * @param[in] valid_ltime   Lifetime (in ms) until prefix expires from now.
 *                          UINT32_MAX for infinite lifetime. Addresses with
 *                          expired prefixes are removed from @p iface.
 * @param[in] pref_ltime    Lifetime (in ms) until prefix deprecates from now.
 *                          UINT32_MAX for infinite lifetime. Addresses with
 *                          deprecated prefixes should not be used for new
 *                          communication. Only applications with difficulty
 *                          changing to another address without service
 *                          disruption should use deprecated addresses. May not
 *                          be greater then @p valid_ltime.
 *
 * @return  0, on success.
 * @return  -EINVAL, if @p pfx was fe80::` or multicast,
 *          @p pfx_len was == 0, the first @p pfx_len bits of @ pfx were 0,
 *          or if pref_ltime > valid_ltime.
 * @return  -ENOMEM, if no space was left in the prefix list.
 */
int gnrc_ipv6_nib_pl_set(unsigned iface,
                         const ipv6_addr_t *pfx, unsigned pfx_len,
                         uint32_t valid_ltime, uint32_t pref_ltime);

/**
 * @brief   Deletes prefix from NIB
 *
 * @pre `pfx != NULL`
 *
 * @param[in] iface     The interface @p pfx is expected to be on (0 for any).
 * @param[in] pfx       The prefix to be removed.
 * @param[in] pfx_len   Length of @p pfx in bits.
 */
void gnrc_ipv6_nib_pl_del(unsigned iface,
                          const ipv6_addr_t *pfx, unsigned pfx_len);

/**
 * @brief   Iterates over all prefix list entries in the NIB.
 *
 * @pre `(state != NULL) && (ple != NULL)`
 *
 * @param[in] iface     Restrict iteration to entries on this interface.
 *                      0 for any interface.
 * @param[in,out] state Iteration state of the prefix list. Must point to NULL
 *                      pointer to start iteration
 * @param[out] ple      The next prefix list entry.
 *
 * Usage example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "net/gnrc/ipv6/nib/pl.h"
 *
 * int main(void) {
 *     void *state = NULL;
 *     gnrc_ipv6_nib_pl_t ple;
 *
 *     puts("My prefixes:");
 *     while (gnrc_ipv6_nib_pl_iter(0, &state, &ple)) {
 *         gnrc_ipv6_nib_pl_print(&ple);
 *     }
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note    The list may change during iteration.
 *
 * @return  true, if iteration can be continued.
 * @return  false, if @p ple is the last prefix list ple in the NIB.
 */
bool gnrc_ipv6_nib_pl_iter(unsigned iface, void **state,
                           gnrc_ipv6_nib_pl_t *ple);

/**
 * @brief   Prints a prefix list entry
 *
 * @pre `ple != NULL`
 *
 * @param[in] ple   A prefix list entry
 */
void gnrc_ipv6_nib_pl_print(gnrc_ipv6_nib_pl_t *ple);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_NIB_PL_H */
/** @} */
