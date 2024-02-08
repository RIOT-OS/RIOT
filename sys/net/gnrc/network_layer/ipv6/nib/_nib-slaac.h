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
 * @name    Temporary address parameters
 * @see     [RFC 8981, section 3.8](https://www.rfc-editor.org/rfc/rfc8981.html#section-3.8)
 * @{
 */
/**
 * @brief   Maximum valid lifetime [days] of a temporary address
 */
#ifndef TEMP_VALID_LIFETIME
#define TEMP_VALID_LIFETIME            MS_PER_HOUR * HOURS_PER_DAY * (2U) /*default value*/
#endif

/**
 * @brief   Maximum preferred lifetime [days] of a temporary address
 *
 * @note    "MUST be smaller than the TEMP_VALID_LIFETIME value"
 */
#ifndef TEMP_PREFERRED_LIFETIME
#define TEMP_PREFERRED_LIFETIME        MS_PER_HOUR * HOURS_PER_DAY * (1U) /*default value*/
#endif

/**
 * @brief   Maximum time to randomly subtract from TEMP_PREFERRED_LIFETIME
 *          for a temporary address
 */
#define MAX_DESYNC_FACTOR             (TEMP_PREFERRED_LIFETIME / 10 * 4)

/**
 * @brief   Maximum number of retries for generating a temporary address
 *          in case a duplicate addresses was detected (DAD failure)
 *
 * @ref GNRC_NETIF_IPV6_ADDRS_FLAGS_IDGEN_RETRIES must be able to store this number
 */
#ifndef TEMP_IDGEN_RETRIES
#define TEMP_IDGEN_RETRIES             (3U) /*default value*/
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
#else   /* CONFIG_GNRC_IPV6_NIB_6LN || CONFIG_GNRC_IPV6_NIB_SLAAC */
#define _auto_configure_addr(netif, pfx, pfx_len) \
    (void)netif; (void)pfx; (void)pfx_len;
#endif  /* CONFIG_GNRC_IPV6_NIB_6LN || CONFIG_GNRC_IPV6_NIB_SLAAC */

#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_SLAAC_TEMPORARY_ADDRESSES) || defined(DOXYGEN)
/**
 * @brief Create a temporary address for the given prefix.
 *
 * @param[in] netif The network interface on which to create the temporary address on. Untested behavior if not the same as for the prefix.
 * @param[in] pfx The prefix which is to be used in the temporary address. The prefix length is assumed to be @ref SLAAC_PREFIX_LENGTH
 * @param[in] pfx_pref_ltime Lifetime of the prefix for which an address shall be created.
 *                       Needed to determine whether it is worth creating a temporary address.
 * @param[in] retries Number of address generation retries that is to be stored in the address flags.
 * @param[out] idx The index of the generated address.
 *
 * @return -1 on failure
 * @return ta_max_pref_lft on success
 */
int32_t _generate_temporary_addr(gnrc_netif_t *netif, const ipv6_addr_t *pfx, const uint32_t pfx_pref_ltime, const uint8_t retries,
                                 int *idx);

/**
 * @brief Check if the address is a temporary address.
 * (Assuming the provided address is a configured address. This function only checks for prefix existence.)
 */
bool is_temporary_addr(const gnrc_netif_t *netif, const ipv6_addr_t *addr);

/**
 * @brief Check if an interface identifier of an IPv6 address is reserved by IANA, i.e. it shouldn't be used
 * This is should be checked when the interface identifier is randomly generated.
 * @see [RFC5453], https://www.iana.org/assignments/ipv6-interface-ids/ipv6-interface-ids.xhtml
 * @param[in] iid The interface identifier to check for
 * @return whether the IID is IANA reserved
 */
bool _iid_is_iana_reserved(const eui64_t *iid);

/**
 * @brief Generate a random interface identifier that is not IANA reserved (@ref _iid_is_iana_reserved)
 * @param[out] iid The interface identifier to write to.
 */
void _ipv6_get_random_iid(eui64_t *iid);

/**
 * @brief Get the duration in seconds at which regeneration of a temporary address should be started before deprecation of the current one.
 * @see https://www.rfc-editor.org/rfc/rfc8981.html#section-3.8-3.2
 * @param[in] netif The interface, as the value depends on this.
 * @return REGEN_ADVANCE [seconds]
 */
uint32_t _get_netif_regen_advance(const gnrc_netif_t *netif);

/**
 * @brief For an address, get the corresponding SLAAC prefix's preferred lifetime
 * @param[in] netif The network interface the prefix is configured on.
 * @param[in] addr An IP address in the SLAAC prefix
 * @param[out] slaac_prefix_pref_until pref_until time of the SLAAC prefix
 * @return true if a corresponding prefix was found, otherwise false
 */
bool _get_slaac_prefix_pref_until(const gnrc_netif_t *netif, const ipv6_addr_t *addr, uint32_t *slaac_prefix_pref_until);

/**
 * @brief For a given SLAAC prefix, get the first-best temporary address prefix.
 * @param[in] netif The interface on which to find the @p slaac_pfc on (0 for any)
 * @param[in] slaac_pfx The SLAAC prefix to find a configured temporary address for.
 * @param[in, out] state Internal iteration state for the prefix list. Can be optionally provided to not start checking all prefixes from the beginning. Otherwise NULL.
 * @param[out] next_temp_addr The temporary address that was configured from the given @p slaac_pfx.
 * @return true if a temporary address was found, false otherwise
 */
bool _iter_slaac_prefix_to_temp_addr(const gnrc_netif_t *netif, const ipv6_addr_t *slaac_pfx, void *state,
                                     ipv6_addr_t *next_temp_addr);
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
