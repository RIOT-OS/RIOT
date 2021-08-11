/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_dhcpv6 Dynamic Host Configuration Protocol for IPv6 (DHCPv6)
 * @ingroup     net_ipv6
 * @brief       DHCPv6 definitions
 * @{
 *
 * @file
 * @brief   DHCPv6 definitions
 * @note    This header is based on [RFC 8415](https://tools.ietf.org/html/rfc8415)
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_DHCPV6_H
#define NET_DHCPV6_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    DHCPv6 ports
 * @see [RFC 8415, section 7.2]
 *      (https://tools.ietf.org/html/rfc8415#section-7.2)
 * @{
 */
#define DHCPV6_CLIENT_PORT          (546U)  /**< client port */
#ifndef DHCPV6_SERVER_PORT          /* only reconfigure for testing!!1! */
#define DHCPV6_SERVER_PORT          (547U)  /**< server and relay agent port */
#endif
/** @} */

/**
 * @name    DHCPv6 message types
 * @anchor  net_dhcp6_msg_types
 * @see [RFC 8415, section 7.3]
 *      (https://tools.ietf.org/html/rfc8415#section-7.3)
 * @{
 */
#define DHCPV6_SOLICIT              (1U)    /**< SOLICIT */
#define DHCPV6_ADVERTISE            (2U)    /**< ADVERTISE */
#define DHCPV6_REQUEST              (3U)    /**< REQUEST */
#define DHCPV6_CONFIRM              (4U)    /**< CONFIRM */
#define DHCPV6_RENEW                (5U)    /**< RENEW */
#define DHCPV6_REBIND               (6U)    /**< REBIND */
#define DHCPV6_REPLY                (7U)    /**< REPLY */
#define DHCPV6_RELEASE              (8U)    /**< RELEASE */
#define DHCPV6_DECLINE              (9U)    /**< DECLINE */
#define DHCPV6_INFO_REQUEST         (11U)   /**< INFORMATION-REQUEST */
#define DHCPV6_RELAY_FORW           (12U)   /**< RELAY-FORW */
#define DHCPV6_RELAY_REPL           (13U)   /**< RELAY-REPL */
/** @ } */

/**
 * @name    DHCPv6 option codes
 * @anchor  net_dhcp6_opt_codes
 * @see [RFC 8415, section 21]
 *      (https://tools.ietf.org/html/rfc8415#section-21)
 * @{
 */
#define DHCPV6_OPT_CID              (1U)    /**< client identifier option */
#define DHCPV6_OPT_SID              (2U)    /**< server identifier option */
#define DHCPV6_OPT_IA_NA            (3U)    /**< identity association for
                                                 non-temporary addresses option */
#define DHCPV6_OPT_IAADDR           (5U)    /**< IA address option */
#define DHCPV6_OPT_ORO              (6U)    /**< option request option */
#define DHCPV6_OPT_PREF             (7U)    /**< preference option */
#define DHCPV6_OPT_ELAPSED_TIME     (8U)    /**< elapsed time option */
#define DHCPV6_OPT_RELAY_MSG        (9U)    /**< relay message option */
#define DHCPV6_OPT_STATUS           (13U)   /**< status code option */
#define DHCPV6_OPT_IID              (18U)   /**< interface-id option */
#define DHCPV6_OPT_DNS_RNS          (23U)   /**< DNS recursive name server option */
#define DHCPV6_OPT_IA_PD            (25U)   /**< identity association for prefix
                                             *   delegation (IA_PD) option */
#define DHCPV6_OPT_IAPFX            (26U)   /**< IA prefix option */
#define DHCPV6_OPT_SMR              (82U)   /**< SOL_MAX_RT option */
#define DHCPV6_OPT_MUD_URL          (112U)  /**< MUD URL option (see RFC 8520) */
/** @} */

/**
 * @name    DHCPv6 status codes
 * @anchor  net_dhcp6_status_codes
 * @see [RFC 8415, section 21.13]
 *      (https://tools.ietf.org/html/rfc8415#section-21.13)
 * @{
 */
#define DHCPV6_STATUS_SUCCESS       (0U)    /**< Success */
/** @} */

/**
 * @name DHCPv6 unique identifier (DUID) types
 * @see [RFC 8415, section 11.1]
 *      (https://tools.ietf.org/html/rfc8415#section-11.1)
 * @{
 */
#define DHCPV6_DUID_TYPE_L2         (3U)    /**< Link-layer address */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_DHCPV6_H */
/** @} */
