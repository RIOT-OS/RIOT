/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup net_dhcpv6_client  DHCPv6 client
 * @ingroup  net_dhcpv6
 * @brief   DHCPv6 client implementation
 * @{
 *
 * @file
 * @brief   DHCPv6 client definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_DHCPV6_CLIENT_H
#define NET_DHCPV6_CLIENT_H

#include "byteorder.h"
#include "event.h"
#include "net/ipv6/addr.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Auto-initialization parameters
 */
#ifndef DHCPV6_CLIENT_STACK_SIZE
#define DHCPV6_CLIENT_STACK_SIZE    (THREAD_STACKSIZE_DEFAULT)  /**< stack size */
#endif

#ifndef DHCPV6_CLIENT_PRIORITY
#define DHCPV6_CLIENT_PRIORITY      (THREAD_PRIORITY_MAIN - 2)  /**< priority */
#endif
/** @} */

/**
 * @brief   Static length of the DUID
 */
#define DHCPV6_CLIENT_DUID_LEN      (sizeof(dhcpv6_duid_l2_t) + 8U)
#define DHCPV6_CLIENT_BUFLEN        (256)   /**< default length for send and receive buffer */

/**
 * @defgroup net_dhcpv6_conf DHCPv6 compile configurations
 * @ingroup  config
 * @{
 */
/**
 * @brief   Maximum number of prefix leases to be stored
 */
#ifndef CONFIG_DHCPV6_CLIENT_PFX_LEASE_MAX
#define CONFIG_DHCPV6_CLIENT_PFX_LEASE_MAX (1U)
#endif

/**
 * @brief   Maximum number of address leases to be stored
 */
#ifndef CONFIG_DHCPV6_CLIENT_ADDR_LEASE_MAX
#define CONFIG_DHCPV6_CLIENT_ADDR_LEASE_MAX (1U)
#endif

/**
 * @brief  Number of addresses needed for using DHCPv6 IA_NA.
 *
 * @note    Used for calculation of @ref CONFIG_GNRC_NETIF_IPV6_ADDRS_NUMOF.
 *          Set to 0 if `dhcpv6_client_ia_na` is not included.
 */
#if defined(MODULE_DHCPV6_CLIENT_IA_NA) || defined(DOXYGEN)
#define DHCPV6_CLIENT_ADDRS_NUMOF ((int)(CONFIG_DHCPV6_CLIENT_ADDR_LEASE_MAX))
#else
#define DHCPV6_CLIENT_ADDRS_NUMOF (0)
#endif

/**
 * @brief   MUD URL (must use the https:// scheme)
 * For more info, see the [definitions](@ref net_dhcpv6_mud_url_option) below
 */
#ifndef CONFIG_DHCPV6_CLIENT_MUD_URL
#define CONFIG_DHCPV6_CLIENT_MUD_URL "https://example.org"
#endif

/** @} */

/**
 * @name DHCPv6 unique identifier (DUID) definitions
 * @see [RFC 8415, section 11](https://tools.ietf.org/html/rfc8415#section-11)
 * @{
 */
/**
 * @brief   DUID based on link-layer address plus time
 */
typedef struct __attribute__((packed)) {
    network_uint16_t type;      /**< @ref DHCPV6_DUID_TYPE_L2 */
    network_uint16_t l2type;    /**< [hardware type](@ref net_arp_hwtype)) */
    /* link-layer address follows this header */
} dhcpv6_duid_l2_t;

#if defined(MODULE_AUTO_INIT_DHCPV6_CLIENT) || defined(DOXYGEN)
/**
 * @brief   Auto-initializes the client in its own thread
 *
 * @note    Only available with (and called by) the `dhcpv6_client_auto_init`
 *          module.
 */
void dhcpv6_client_auto_init(void);
#endif /* MODULE_DHCPV6_CLIENT_AUTO_INIT */

/**
 * @brief   Initializes the client
 *
 * @pre `event_queue->waiter != NULL`
 *
 * @param[in] event_queue   Event queue to use with the client. Needs to be
 *                          initialized in the handler thread.
 * @param[in] netif         The network interface the client should listen on.
 *                          SOCK_ADDR_ANY_NETIF for any interface
 */
void dhcpv6_client_init(event_queue_t *event_queue, uint16_t netif);

/**
 * @brief   Let the server start listening
 *
 * @pre @ref dhcpv6_client_init() was called (i.e. the internal event queue of
 *      he client was set).
 *
 * This needs to be called *after* all desired [configuration functions]
 * (@ref net_dhcpv6_client_conf) and @ref dhcpv6_client_init() were called.
 */
void dhcpv6_client_start(void);

/**
 * @name    Configuration functions
 * @anchor  net_dhcpv6_client_conf
 * @{
 */
/**
 * @brief   Configures the client to request prefix delegation for a network
 *          interface from a server
 *
 * @pre Module `dhcpv6_client_ia_pd` is compiled in.
 * @pre `pfx_len <= 128`
 *
 * Without module `dhcpv6_client_ia_pd` and `NDEBUG` set this function is a NOP.
 * Without module `dhcpv6_client_ia_pd` and `NDEBUG` unset this function will
 * abort the running code on a failed assertion.
 *
 * @param[in] netif     The interface to request the prefix delegation for.
 * @param[in] pfx_len   The desired length of the prefix (note that the server
 *                      might not consider this request). Must be <= 128
 */
void dhcpv6_client_req_ia_pd(unsigned netif, unsigned pfx_len);
/** @} */

/**
 * @brief   Configures the client to request non-temporary addresses for a network
 *          interface from a server
 * @note    For multi-hop WPAN meshes a DHCPv6 relay (which is not implemented in
 *          RIOT yet) is required, as DHCPv6 only acts in link scope.
 *
 * @param[in] netif     The interface to request non-temporaty addresses for.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is no lease entry available anymore
 * @retval -ENOTSUP when module `dhcpv6_client_ia_na` is not being used
 */
int dhcpv6_client_req_ia_na(unsigned netif);
/** @} */

/**
 * @name    Stack-specific functions
 *
 * These functions need to be provided by the network-stack implementation.
 * @{
 */
/**
 * @brief   Get the link-layer address DUID for the client
 *
 * @param[in] netif The network interface the client is bound to. May be
 *                  SOCK_ADDR_ANY_NETIF for any interface.
 * @param[out] duid The resulting DUID.
 *
 * @return  length of the @p duid on success.
 * @return  0, on error.
 */
unsigned dhcpv6_client_get_duid_l2(unsigned netif, dhcpv6_duid_l2_t *duid);

/**
 * @brief   Configures a prefix delegation lease that is provided by the server.
 *
 * @param[in] netif     Network interface the prefix delegation was for.
 * @param[in] pfx       Prefix for the prefix delegation.
 * @param[in] pfx_len   Length of @p pfx in bits.
 * @param[in] valid     Valid lifetime of the prefix delegation.
 * @param[in] pref      Preferred lifetime of the prefix delegation.
 */
void dhcpv6_client_conf_prefix(unsigned netif, const ipv6_addr_t *pfx,
                               unsigned pfx_len, uint32_t valid,
                               uint32_t pref);

/**
 * @brief   Checks if the given network interface is configured
 *          to use DHCPv6 IA_NA
 *
 * @param[in] netif     Network interface to check.
 *
 * @return  true, if the network interface is set up for IA_NA.
 */
bool dhcpv6_client_check_ia_na(unsigned netif);

/**
 * @brief   Configures a address lease that is provided by the server.
 *
 * @param[in] netif     Network interface the address was for.
 * @param[in] addr      The assigned address.
 *
 * @return sizeof(ipv6_addr_t) on success.
 * @return <0 on error.
 */
int dhcpv6_client_add_addr(unsigned netif, ipv6_addr_t *addr);

/**
 * @brief   Deprecates an existing address from an address lease.
 *
 * @param[in] netif     Network interface the address was for.
 * @param[in] addr      The address to deprecate.
 */
void dhcpv6_client_deprecate_addr(unsigned netif, const ipv6_addr_t *addr);

/**
 * @brief   Removes an existing address that originated from an address lease.
 *
 * @param[in] netif     Network interface the address was for.
 * @param[in] addr      The address to remove.
 */
void dhcpv6_client_remove_addr(unsigned netif, ipv6_addr_t *addr);

/**
 * @brief   Determines how long the prefix delegation lease is still valid.
 *
 * @param[in] netif     Network interface the prefix delegation was for.
 * @param[in] pfx       Prefix of the prefix delegation
 * @param[in] pfx_len   Length of @p pfx in bits.
 *
 * @return  Remaining valid lifetime of the prefix delegation lease in seconds.
 */
uint32_t dhcpv6_client_prefix_valid_until(unsigned netif,
                                          const ipv6_addr_t *pfx,
                                          unsigned pfx_len);
/** @} */

/**
 * @name DHCPv6 Manufacturer Usage Description (MUD) URL option definitions
 * @see [RFC 8520, section 10](https://tools.ietf.org/html/rfc8520#section-10)
 * @anchor  net_dhcpv6_mud_url_option
 * @{
 */

/**
 * @brief   Length for the send buffer if a MUD URL is included in the DHCP client's packets
 *
 * @note    Only (re)defined by the `dhcpv6_client_mud_url` pseudo-module.
 */
#if defined(MODULE_DHCPV6_CLIENT_MUD_URL) || defined(DOXYGEN)
#define DHCPV6_CLIENT_SEND_BUFLEN        (DHCPV6_CLIENT_BUFLEN + 256)
#else
#define DHCPV6_CLIENT_SEND_BUFLEN        (DHCPV6_CLIENT_BUFLEN)
#endif

/**
 * @brief   Maximal length of a MUD URL
 */
#define MAX_MUD_URL_LENGTH (0xFF - sizeof(dhcpv6_opt_mud_url_t))

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_DHCPV6_CLIENT_H */
/** @} */
