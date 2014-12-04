/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup    net_sixlowpan_legacy_ndp Neighbor discovery
 * @ingroup     net_sixlowpan_legacy
 * @brief       Internet Control Message Protocol version 6
 *
 * @{
 *
 * @file        include/sixlowpan_legacy/icmp.h
 * @brief       6LoWPAN ICMP related header
 *
 * @author      Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef SIXLOWPAN_LEGACY_ICMP_H
#define SIXLOWPAN_LEGACY_ICMP_H

#include <stddef.h>
#include <stdint.h>

#include "sixlowpan_legacy/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ICMPv6 packet type for parameter problem message.
 * @see <a href="http://tools.ietf.org/html/rfc4443#section-3.4">
 *          RFC 4443, section 3.4
 *      </a>
 */
#define ICMPV6_TYPE_PARAMETER_PROB  (4)

/**
 * @brief ICMPv6 packet type for an echo request.
 * @see <a href="http://tools.ietf.org/html/rfc4443#section-4.1">
 *          RFC 4443, section 4.1
 *      </a>
 */
#define ICMPV6_TYPE_ECHO_REQUEST    (128)

/**
 * @brief ICMPv6 packet type for an echo reply.
 * @see <a href="http://tools.ietf.org/html/rfc4443#section-4.2">
 *          RFC 4443, section 4.2
 *      </a>
 */
#define ICMPV6_TYPE_ECHO_REPLY      (129)

/**
 * @brief ICMPv6 packet type for a router solicitation.
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.1">
 *          RFC 4861, section 4.1
 *      </a>
 */
#define ICMPV6_TYPE_ROUTER_SOL      (133)

/**
 * @brief ICMPv6 packet type for a router advertisement.
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>
 */
#define ICMPV6_TYPE_ROUTER_ADV      (134)

/**
 * @brief ICMPv6 packet type for a neighbor solicitation.
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.3">
 *          RFC 4861, section 4.3
 *      </a>
 */
#define ICMPV6_TYPE_NEIGHBOR_SOL    (135)

/**
 * @brief ICMPv6 packet type for a neighbor advertisement.
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.4
 *      </a>
 */
#define ICMPV6_TYPE_NEIGHBOR_ADV    (136)

/**
 * @brief ICMPv6 packet type for a redirect message.
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.5">
 *          RFC 4861, section 4.5
 *      </a>
 */
#define ICMPV6_TYPE_REDIRECT        (137)

/**
 * @brief ICMPv6 packet type for a rpl control message.
 * @note  TODO: not RFC conform.
 */
#define ICMPV6_TYPE_RPL_CONTROL     (155)

/**
 * @brief ICMPv6 parameter problem message code for
 *        "Erroneous header field encountered".
 * @see <a href="http://tools.ietf.org/html/rfc4443#section-3.4">
 *          RFC 4443, section 3.4
 *      </a>
 */
#define ICMPV6_PARAMETER_PROB_CODE_ERR              (0)

/**
 * @brief ICMPv6 parameter problem message code for
 *        "Unrecognized Next Header type encountered".
 * @see <a href="http://tools.ietf.org/html/rfc4443#section-3.4">
 *          RFC 4443, section 3.4
 *      </a>
 */
#define ICMPV6_PARAMETER_PROB_CODE_NXT_HDR_UNREC    (1)

/**
 * @brief ICMPv6 parameter problem message code for
 *        "Unrecognized IPv6 option encountered".
 * @see <a href="http://tools.ietf.org/html/rfc4443#section-3.4">
 *          RFC 4443, section 3.4
 *      </a>
 */
#define ICMPV6_PARAMETER_PROB_CODE_IPV6_LEGACY_OPT_UNREC   (2)

/**
 * @brief Send ICMPv6 parameter problem message.
 *
 * @param[in] src           Source address for IPv6 header.
 * @param[in] dest          Destination address for IPv6 header.
 * @param[in] code          Value for code field of parameter problem
 *                          message.
 * @param[in] pointer       Value for pointer field of parameter problem
 *                          message.
 * @param[in] packet        Data payload for echo request.
 * @param[in] packet_len    Length of data payload.
 */
void icmpv6_send_parameter_prob(ipv6_legacy_addr_t *src, ipv6_legacy_addr_t *dest,
                                uint8_t code, uint32_t pointer,
                                uint8_t *packet, uint8_t packet_len);

/**
 * @brief Send ICMPv6 echo request.
 *
 * @param[in] destaddr  Destination address for IPv6 header.
 * @param[in] id        Value for identifier field of echo request.
 * @param[in] seq       Value for sequence number field of echo request.
 * @param[in] data      Data payload for echo request.
 * @param[in] data_len  Length of data payload.
 */
void icmpv6_send_echo_request(ipv6_legacy_addr_t *destaddr, uint16_t id,
                              uint16_t seq, uint8_t *data,
                              size_t data_len);

/**
 * @brief Send ICMPv6 echo reply.
 *
 * @param[in] destaddr  Destination address for IPv6 header.
 * @param[in] id        Value for identifier field of echo reply.
 * @param[in] seq       Value for sequence number field of echo reply.
 * @param[in] data      Data payload for echo reply.
 * @param[in] data_len  Length of data payload.
 */
void icmpv6_send_echo_reply(ipv6_legacy_addr_t *destaddr, uint16_t id,
                            uint16_t seq, uint8_t *data, size_t data_len);

/**
 * @brief Send ICMPv6 router solicitation.
 *
 * @param[in] sllao     Flag to include source link-layer address
 *                      option. If *sllao* == OPT_SLLAO add it, else
 *                      not.
 */
void icmpv6_send_router_sol(uint8_t sllao);

/**
 * @brief Send ICMPv6 router advertisement.
 *
 * @param[in] addr      Address of node to advertise router to. If
 *                      NULL *addr* is set to the all nodes multicast
 *                      address.
 * @param[in] sllao     Flag to include source link-layer address
 *                      option. If *sllao* == OPT_SLLAO add it, else
 *                      not.
 * @param[in] mtu       Flag to include MTU option. If *mtu* == OPT_MTU
 *                      add it, else not.
 * @param[in] pi        Flag to include prefix information option. If
 *                      *pi* == OPT_PI add it, else not.
 * @param[in] sixco     Flag to include 6LoWPAN context option. If
 *                      *sixco* == OPT_6CO add it, else not.
 * @param[in] abro      Flag to include 6LoWPAN authoritive border
 *                      router option. If *abro* == OPT_ABRO add it,
 *                      else not.
 */
void icmpv6_send_router_adv(ipv6_legacy_addr_t *addr, uint8_t sllao,
                            uint8_t mtu, uint8_t pi, uint8_t sixco,
                            uint8_t abro);

/**
 * @brief Send ICMPv6 neighbor solicitation.
 *
 * @param[in] src       Source address for IPv6 header.
 * @param[in] dest      Destination address for IPv6 header.
 * @param[in] targ      Value for target address field of neighbor
 *                      solicitation.
 * @param[in] sllao     Flag to include source link-layer address
 *                      option. If *sllao* == OPT_SLLAO add it, else
 *                      not.
 * @param[in] aro       Flag to include address registration option. If
 *                      *aro* == OPT_ARO add it, else not.
 */
void icmpv6_send_neighbor_sol(ipv6_legacy_addr_t *src, ipv6_legacy_addr_t *dest,
                              ipv6_legacy_addr_t *targ, uint8_t slloa,
                              uint8_t aro);

/**
 * @brief Send ICMPv6 neighbor advertisement.
 *
 * @param[in] src       Source address for IPv6 header.
 * @param[in] dest      Destination address for IPv6 header.
 * @param[in] targ      Value for target address field of neighbor
 *                      advertisement.
 * @param[in] rso       Value for flags field of neighbor advertisement.
 * @param[in] sllao     Flag to include source link-layer address
 *                      option. If *sllao* == OPT_SLLAO add it, else
 *                      not.
 * @param[in] aro       Flag to include address registration option. If
 *                      *aro* == OPT_ARO add it, else not.
 */
void icmpv6_send_neighbor_adv(ipv6_legacy_addr_t *src, ipv6_legacy_addr_t *dst,
                              ipv6_legacy_addr_t *tgt, uint8_t rso,
                              uint8_t sllao, uint8_t aro);

/**
 * @brief Calculates the checksum for ICMPv6 packets.
 *
 * @param[in] ipv6_legacy_buf      The initialized IPv6 header of the packet.
 * @param[in] icmpv6_buf    The initialized ICMPv6_header of the packet
 *                          (except checksum, payload is expected directly
 *                          after the packet header in memory).
 *
 * @return The internet checksum of the given ICMPv6 packet.
 */
uint16_t icmpv6_csum(ipv6_legacy_hdr_t *ipv6_legacy_buf, icmpv6_hdr_t *icmpv6_buf);

#ifdef __cplusplus
}
#endif

#endif /* SIXLOWPAN_LEGACY_ICMP_H */
/** @} */
