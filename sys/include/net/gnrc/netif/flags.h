/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief   Flag definitions for @ref net_gnrc_netif
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_NETIF_FLAGS_H
#define NET_GNRC_NETIF_FLAGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Auto-address configuration modes
 * @anchor  net_gnrc_netif_aac
 */
enum {
    GNRC_NETIF_AAC_NONE = 0x00, /**< no configuration */
    GNRC_NETIF_AAC_AUTO = 0x01, /**< Use some automatic bootstrapping (e.g. SLAAC with IPv6) */
    GNRC_NETIF_AAC_DHCP = 0x02, /**< Use DHCP(v6) */
    /* extend if needed */
};

/**
 * @name    Network interface flags
 * @anchor  net_gnrc_netif_flags
 * @{
 */
/**
 * @brief   Network interface has link-layer address
 *
 * There are link-layers (e.g. SLIP) that do not have (nor require) link-layer
 * addresses. This flag signifies this fact to upper layers by leaving it unset.
 */
#define GNRC_NETIF_FLAGS_HAS_L2ADDR                (0x00000001U)

/**
 * @brief   Network interface is enabled for IPv6 forwarding
 */
#define GNRC_NETIF_FLAGS_IPV6_FORWARDING           (0x00000002U)

/**
 * @brief   Network interface advertises itself as an IPv6 router
 *          (implies @ref GNRC_NETIF_FLAGS_IPV6_FORWARDING to be set)
 */
#define GNRC_NETIF_FLAGS_IPV6_RTR_ADV              (0x00000004U)

/**
 * @brief   This interface advertises its gnrc_netif_t::mtu to other nodes
 *          (implies @ref GNRC_NETIF_FLAGS_IPV6_RTR_ADV to be set)
 */
#define GNRC_NETIF_FLAGS_IPV6_ADV_MTU              (0x00000008U)

/**
 * @brief   This interface advertises its gnrc_netif_t::cur_hl to other nodes
 *          (implies @ref GNRC_NETIF_FLAGS_IPV6_RTR_ADV to be set)
 */
#define GNRC_NETIF_FLAGS_IPV6_ADV_CUR_HL           (0x00000010U)

/**
 * @brief   This interface advertises its reachable time to other nodes
 *          (implies @ref GNRC_NETIF_FLAGS_IPV6_RTR_ADV to be set)
 */
#define GNRC_NETIF_FLAGS_IPV6_ADV_REACH_TIME       (0x00000020U)

/**
 * @brief   This interface advertises its retransmission timer to other nodes
 *          (implies @ref GNRC_NETIF_FLAGS_IPV6_RTR_ADV to be set)
 */
#define GNRC_NETIF_FLAGS_IPV6_ADV_RETRANS_TIMER    (0x00000040U)

/**
 * @brief   If gnrc_netif_t::ipv6::aac_mode & GNRC_NETIF_AAC_DHCP then this
 *          flag indicates that other configuration information is available via
 *          DHCPv6 (e.g. DNS-related information)
 *
 * @see [RFC 4861, section 4.2](https://tools.ietf.org/html/rfc4861#section-4.2)
 */
#define GNRC_NETIF_FLAGS_IPV6_ADV_O_FLAG           (0x00000080U)

/**
 * @brief   This interface uses 6Lo header compression
 *
 * @see [RFC 6282](https://tools.ietf.org/html/rfc6282)
 */
#define GNRC_NETIF_FLAGS_6LO_HC                    (0x00000100U)

/**
 * @brief   This interface acts as a 6Lo border router to the LLN
 */
#define GNRC_NETIF_FLAGS_6LO_ABR                   (0x00000200U)

/**
 * @brief   This interface acts as a mesh-under node (route-over topology when
 *          unset)
 */
#define GNRC_NETIF_FLAGS_6LO_MESH                  (0x00000400U)

/**
 * @brief   Interface supports 6LoWPAN general header compression
 *
 * @attention   6CIO (which propagates this flag throughout) not implemented yet
 * @see [RFC 7400, section 3.3](https://tools.ietf.org/html/rfc7400#section-3.3)
 * @see [draft-ietf-6lo-rfc6775-update-09, section 6.3]
 *      (https://tools.ietf.org/html/draft-ietf-6lo-rfc6775-update-09#section-6.3)
 */
#define GNRC_NETIF_FLAGS_6LO_BACKBONE              (0x00000800U)

/**
 * @brief   This interface represents a 6Lo node (6LN) according to RFC 6775
 *
 * @see [RFC 6775, section 2](https://tools.ietf.org/html/rfc6775#section-2)
 */
#define GNRC_NETIF_FLAGS_6LN                       (0x00001000U)

/**
 * @brief   6Lo is activated for this interface
 *
 * @note    Most devices supporting 6Lo actually *require* 6Lo so this flag
 *          should not be configurable for them. As a consequence, this flag
 *          **must** only be changed by a @ref NETOPT_6LO message to the
 *          interface.
 */
#define GNRC_NETIF_FLAGS_6LO                       (0x00002000U)

/**
 * @brief   Network interface is configured in raw mode
 */
#define GNRC_NETIF_FLAGS_RAWMODE                   (0x00010000U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_FLAGS_H */
/** @} */
