/**
 * 6LoWPAN data types
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowpan/types.h
 * @brief   6LoWPAN data types
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef SIXLOWPAN_TYPES_H
#define SIXLOWPAN_TYPES_H

/**
 * @brief Data type to represent an EUI-64.
 */
typedef union __attribute__((packed)) {
    uint8_t uint8[8];   ///< devided by 8 8-bit words.
    uint16_t uint16[4]; ///< devided by 4 16-bit words.
} ieee_802154_long_t;

/**
 * @brief Data type to represent IEEE 802.15.4 short address.
 */
typedef union __attribute__((packed)) {
    uint8_t uint8[2];   ///< devided by 2 8-bit words.
    uint16_t uint16[1]; ///< as a 16-bit value.
} ieee_802154_short_t;

/**
 * @brief Data type to represent an IPv6 address.
 */
typedef union __attribute__((packed)) {
    uint8_t uint8[16];  ///< devided by 16 8-bit words.
    uint16_t uint16[8]; ///< devided by 8 16-bit words.
    uint32_t uint32[4]; ///< devided by 4 32-bit words.
} ipv6_addr_t;

/**
 * @brief   Data type to represent IPv6 address types.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 */
typedef enum __attribute__((packed)) {
    IPV6_ADDR_TYPE_NONE,            ///< address has no type/is invalid.
    IPV6_ADDR_TYPE_UNICAST,         ///< address is an unicast address.
    IPV6_ADDR_TYPE_MULTICAST,       ///< address is a multicast address.
    IPV6_ADDR_TYPE_ANYCAST,         ///< address is an anycast address.
    IPV6_ADDR_TYPE_SOLICITED_NODE,  ///< address is a solicitated node
                                    ///< multicast address.
    IPV6_ADDR_TYPE_LOOPBACK,        ///< address is a loopback address.
    IPV6_ADDR_TYPE_LINK_LOCAL,      ///< address is a link-local address.
    IPV6_ADDR_TYPE_GLOBAL           ///< address is a global address.
} ipv6_addr_type_t;

/**
 * @brief   Data type to represent an IPv6 packet header
 *
 * @see <a href="http://tools.ietf.org/html/rfc2460">
 *          RFC 2460
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t version_trafficclass;   ///< Version field + first 4 bit of Traffic Class.
    uint8_t trafficclass_flowlabel; ///< last 4 bit of Traffic Class
                                    ///< and first 4 bit of Flow Label.
    uint16_t flowlabel;             ///< last 16 bit of Flow Label.
    uint16_t length;                ///< payload length of this packet.
    uint8_t nextheader;             ///< type of next header in this packet.
    uint8_t hoplimit;               ///< hop limit for this packet.
    ipv6_addr_t srcaddr;            ///< source address of this packet.
    ipv6_addr_t destaddr;           ///< destination address of this packet.
} ipv6_hdr_t;

/**
 * @brief   Data type to represent an ICMPv6 packet header.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4443">
 *          RFC 4443
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t     type;       ///< Type of the ICMPv6 message.
    uint8_t     code;       ///< Code of the ICMPv6 message.
    uint16_t    checksum;   ///< Checksum of the ICMPv6 message.
} icmpv6_hdr_t;

/**
 * @brief   Data type to represent address types according to
 *          <a href="http://tools.ietf.org/hmtl/rfc4862">RFC 4862</a>.
 */
typedef enum __attribute__((packed)) {
    NDP_ADDR_STATE_TENTATIVE,   ///< tentative address, uniqueness to be verified.
    NDP_ADDR_STATE_PREFERRED,   ///< preferred address, for unrestricted use.
    NDP_ADDR_STATE_DEPRECATED,  ///< deprecated address, use discouraged.
    NDP_ADDR_STATE_ANY          ///< addresses of this state are always permitted.
} ndp_addr_state_t;

/**
 * @}
 */
#endif /* SIXLOWPAN_TYPES_H */
