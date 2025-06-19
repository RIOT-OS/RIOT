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

/*
 * "the address architecture [RFC4291] also defines the length of the interface identifiers"
 * - https://datatracker.ietf.org/doc/html/rfc4862#section-2
 *
 * "Interface IDs are required to be 64 bits long"
 * - https://datatracker.ietf.org/doc/html/rfc4291#section-2.5.1
 */
#define INTERFACE_IDENTIFIER_LENGTH (64U)

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
