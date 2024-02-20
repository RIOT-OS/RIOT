/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup net_gnrc_ipv6_nib
 * @brief
 * @{
 *
 * @file
 * @brief   Definions related to SLAAC functionality of the NIB
 * @see     @ref CONFIG_GNRC_IPV6_NIB_SLAAC
 * @internal
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <kernel_defines.h>
#include <stdint.h>

#include "net/gnrc/ipv6/nib/conf.h"
#include "net/gnrc/netif.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * > An IPv6 address prefix used for stateless autoconfiguration [ACONF]
 *   of an Ethernet interface must have a length of 64 bits.
 * - https://datatracker.ietf.org/doc/html/rfc2464
 *
 * > An IPv6 address prefix used for stateless autoconfiguration [RFC4862]
 *   of an IEEE 802.15.4 interface MUST have a length of 64 bits.
 * - https://datatracker.ietf.org/doc/html/rfc4944
 *
 * Also see
 * @ref INTERFACE_IDENTIFIER_LENGTH
 * in combination with "sum" "does not equal 128 bits"
 * from https://datatracker.ietf.org/doc/html/rfc4862#section-5.5.3 d)
 */
#define SLAAC_PREFIX_LENGTH (64U)

/**
 * "the address architecture [RFC4291] also defines the length of the interface identifiers"
 * - https://datatracker.ietf.org/doc/html/rfc4862#section-2
 *
 * "Interface IDs are required to be 64 bits long"
 * - https://datatracker.ietf.org/doc/html/rfc4291#section-2.5.1
 */
#define INTERFACE_IDENTIFIER_LENGTH (64U)

/**
 * @name    Stable privacy host constants
 * @see     [RFC 7217, section 7](https://tools.ietf.org/html/rfc7217#section-7)
 * @{
 */
/**
 * @brief   Limit for stable privacy (IDGEN) addr. generation retries for subsequent DAD failures
 *
 * @note    Must not be greater than 3 for @ref net_gnrc since
 *          @ref GNRC_NETIF_IPV6_ADDRS_FLAGS_IDGEN_RETRIES restricts it to
 *          that number.
 */
#ifndef STABLE_PRIVACY_IDGEN_RETRIES
#define STABLE_PRIVACY_IDGEN_RETRIES            (3U) /*default value*/
#endif

/**
 * @brief Upper limit for random time to wait between IDGEN retries
 */
#ifndef STABLE_PRIVACY_IDGEN_DELAY_MS
#define STABLE_PRIVACY_IDGEN_DELAY_MS 1000 /*default value*/
#endif
/** @} */

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LN) || IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC) || defined(DOXYGEN)
/**
 * @brief   Auto-configures an address from a given prefix
 *
 * @param[in] netif     The network interface the address should be added to.
 * @param[in] pfx       The prefix for the address.
 * @param[in] pfx_len   Length of @p pfx in bits.
 */
void _auto_configure_addr(gnrc_netif_t *netif, const ipv6_addr_t *pfx,
                          uint8_t pfx_len);
#if IS_ACTIVE(CONFIG_GNRC_IPV6_STABLE_PRIVACY) || defined(DOXYGEN)
/**
 * @brief Overload of @ref _auto_configure_addr
 * @param dad_ctr rfc7217 DAD_Counter
 */
void _auto_configure_addr_with_dad_ctr(gnrc_netif_t *netif, const ipv6_addr_t *pfx,
                          uint8_t pfx_len, uint8_t dad_ctr);
#endif

#else   /* CONFIG_GNRC_IPV6_NIB_6LN || CONFIG_GNRC_IPV6_NIB_SLAAC */
#define _auto_configure_addr(netif, pfx, pfx_len) \
    (void)netif; (void)pfx; (void)pfx_len;
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN || CONFIG_GNRC_IPV6_NIB_SLAAC */

#if IS_ACTIVE(CONFIG_GNRC_IPV6_STABLE_PRIVACY) || defined(DOXYGEN)
/**
 * @brief Check if an interface identifier of an IPv6 address
 * is reserved by IANA, i.e. it shouldn't be used
 * This is should be checked when the interface identifier is randomly generated.
 * @see [RFC5453], https://www.iana.org/assignments/ipv6-interface-ids/ipv6-interface-ids.xhtml
 * @param[in] iid The interface identifier to check for
 * @return whether the IID is IANA reserved
 */
bool _iid_is_iana_reserved(const eui64_t *iid);

/**
 * @param[in,out] dad_ctr
 * @param[in] reason
 */
bool _stable_privacy_should_retry_idgen(uint8_t *dad_ctr, const char *reason);

/**
 * @brief
 * @pre @p iid is all 0 bits (iid->uint64.u64 == 0)
 * @param[out] iid where to store the generated interface identifier
 * @param[in] netif The network interface to use as source for IID generation.
 * ("Net_iface" in rfc7217)
 * @param[in] pfx The prefix for which the IID is to be generated.
 * ("Prefix" in rfc7217)
 * @param[in,out] dad_ctr ("DAD_Counter" in rfc7217)
 * Value may increase and is to be stored associated with the address by caller
 * @return 0 on success
 * @return -1 if failed, because retry limit reached
 * @return  `-ENOTSUP`, if interface has no link-layer address.
 */
int ipv6_get_rfc7217_iid(eui64_t *iid, const gnrc_netif_t *netif, const ipv6_addr_t *pfx,
                         uint8_t *dad_ctr);
#endif

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC) || defined(DOXYGEN)
/**
 * @brief   Removes a tentative address from the interface and tries to
 *          reconfigure a new address
 *
 * @param[in] netif The network interface the address is to be removed from.
 * @param[in] addr  The address to remove.
 */
void _remove_tentative_addr(gnrc_netif_t *netif, const ipv6_addr_t *addr);

/**
 * @brief   Handle @ref GNRC_IPV6_NIB_DAD event
 *
 * @param[in] addr  A TENTATIVE address.
 */
void _handle_dad(const ipv6_addr_t *addr);

/**
 * @brief   Handle @ref GNRC_IPV6_NIB_VALID_ADDR event
 *
 * @param[in] addr  A TENTATIVE address.
 */
void _handle_valid_addr(const ipv6_addr_t *addr);
#else   /* CONFIG_GNRC_IPV6_NIB_SLAAC */
#define _remove_tentative_addr(netif, addr) \
    (void)netif; (void)addr
#define _handle_dad(addr)           (void)addr
#define _handle_valid_addr(addr)    (void)addr
#endif  /* CONFIG_GNRC_IPV6_NIB_SLAAC */

#ifdef __cplusplus
}
#endif

/** @} */
