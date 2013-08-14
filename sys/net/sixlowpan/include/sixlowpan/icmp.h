/**
 * sixlowpan/icmp.h - 6LoWPAN constants, data structs, and prototypes
 *                    related to ICMP
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file
 * @brief   6LoWPAN ICMP related header
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef SIXLOWPAN_ICMP_H
#define SIXLOWPAN_ICMP_H

#include <stdint.h>

#include "sixlowpan/types.h"

/*
 * @brief ICMPv6 packet type for parameter problem message.
 * @see <a href="http://tools.ietf.org/html/rfc4443#section-3.4">
 *          RFC 4443, section 3.4
 *      </a>
 */
#define ICMPV6_TYPE_PARAMETER_PROB  (4)

/*
 * @brief ICMPv6 packet type for an echo request.
 * @see <a href="http://tools.ietf.org/html/rfc4443#section-4.1">
 *          RFC 4443, section 4.1
 *      </a>
 */
#define ICMPV6_TYPE_ECHO_REQUEST    (128)

/*
 * @brief ICMPv6 packet type for an echo reply.
 * @see <a href="http://tools.ietf.org/html/rfc4443#section-4.2">
 *          RFC 4443, section 4.2
 *      </a>
 */
#define ICMPV6_TYPE_ECHO_REPLY      (129)

/*
 * @brief ICMPv6 packet type for a router solicitation.
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.1">
 *          RFC 4861, section 4.1
 *      </a>
 */
#define ICMPV6_TYPE_ROUTER_SOL      (133)

/*
 * @brief ICMPv6 packet type for a router advertisement.
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>
 */
#define ICMPV6_TYPE_ROUTER_ADV      (134)

/*
 * @brief ICMPv6 packet type for a neighbor solicitation.
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.3">
 *          RFC 4861, section 4.3
 *      </a>
 */
#define ICMPV6_TYPE_NEIGHBOR_SOL    (135)

/*
 * @brief ICMPv6 packet type for a neighbor advertisement.
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.4
 *      </a>
 */
#define ICMPV6_TYPE_NEIGHBOR_ADV    (136)

/*
 * @brief ICMPv6 packet type for a redirect message.
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.5">
 *          RFC 4861, section 4.5
 *      </a>
 */
#define ICMPV6_TYPE_REDIRECT        (137)

/*
 * @brief ICMPv6 packet type for a rpl control message.
 * @note  TODO: not RFC conform.
 */
#define ICMPV6_TYPE_RPL_CONTROL     (155)

/* parameter problem [rfc4443] */
#define ICMPV6_PARAMETER_PROB_CODE_ERR              (0)   /* Erroneous header field encountered */
#define ICMPV6_PARAMETER_PROB_CODE_NXT_HDR_UNREC    (1)   /* Unrecognized Next Header type encountered */
#define ICMPV6_PARAMETER_PROB_CODE_IPV6_OPT_UNREC   (2)   /* Unrecognized IPv6 option encountered */

void icmpv6_send_parmeter_prob(ipv6_addr_t *src, ipv6_addr_t *dest,
                               uint8_t code, uint32_t pointer,
                               uint8_t *packet, uint8_t packet_len);
void icmpv6_send_echo_request(ipv6_addr_t *destaddr, uint16_t id,
                              uint16_t seq, char *data,
                              size_t data_len);
void icmpv6_send_echo_reply(ipv6_addr_t *destaddr, uint16_t id,
                            uint16_t seq, char *data, size_t data_len);
void icmpv6_send_router_sol(uint8_t sllao);
void icmpv6_send_router_adv(ipv6_addr_t *addr, uint8_t sllao,
                            uint8_t mtu, uint8_t pi, uint8_t sixco,
                            uint8_t abro);
void icmpv6_send_neighbor_sol(ipv6_addr_t *src, ipv6_addr_t *dest,
                              ipv6_addr_t *targ, uint8_t slloa,
                              uint8_t aro);
void icmpv6_send_neighbor_adv(ipv6_addr_t *src, ipv6_addr_t *dst,
                              ipv6_addr_t *tgt, uint8_t rso,
                              uint8_t sllao, uint8_t aro,
                              uint8_t aro_state);

void icmpv6_ndp_set_llao(icmpv6_ndp_opt_stllao_t *sllao, uint8_t type, uint8_t length);

/**
 * @}
 */
#endif /* SIXLOWPAN_ICMP_H */
