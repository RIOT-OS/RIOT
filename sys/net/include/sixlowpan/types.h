/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sixlowpan_types Type definitions and data structs for 6LoWPAN and IPv6
 * @ingroup     net_sixlowpan
 * @brief       Structs, constants, and enums for 6LoWPAN and IPv6 related functions
 * @{
 *
 * @file        sixlowpan/types.h
 * @brief       6LoWPAN data types
 *
 * @author      Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef SIXLOWPAN_TYPES_H
#define SIXLOWPAN_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Data type to represent an EUI-64.
 */
typedef union __attribute__((packed)) {
    uint8_t uint8[8];   ///< divided by 8 8-bit words.
    uint16_t uint16[4]; ///< divided by 4 16-bit words.
} ieee_802154_long_t;

/**
 * @brief Data type to represent IEEE 802.15.4 short address.
 */
typedef union __attribute__((packed)) {
    uint8_t uint8[2];   ///< divided by 2 8-bit words.
    uint16_t uint16[1]; ///< as a 16-bit value.
} ieee_802154_short_t;

/**
 * @brief Data type to represent an IPv6 address.
 */
typedef union __attribute__((packed)) {
    uint8_t uint8[16];  ///< divided by 16 8-bit words.
    uint16_t uint16[8]; ///< divided by 8 16-bit words.
    uint32_t uint32[4]; ///< divided by 4 32-bit words.
} ipv6_addr_t;

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
 * @brief   Data type to represent an IPv6 source routing header for RPL
 *
 * @see [RFC 6554](http://tools.ietf.org/html/rfc6554)
 */
typedef struct __attribute__((packed)) {
    uint8_t nextheader;             /**< type of next header in this packet. */
    uint8_t hdrextlen;              /**< length of header in 8-octet units. */
    uint8_t routing_type;           /**< identify srh-variant. */
    uint8_t segments_left;          /**< remaining route segments before reaching destination. */
    unsigned int cmpri:4, cmpre:4;  /**< 4+4 bit, expressing prefix octets from each/last segment. */
    unsigned pad :4;                /**< number of octets used for padding after addresses. */
    unsigned reserved :10;          /**< reserved. Set to 0. */
    unsigned reserved2 :10;         /**< reserved. Set to 0. */
    ipv6_addr_t route[];
} ipv6_srh_t;

/**
 * @brief   Data type to represent an ICMPv6 packet header.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4443">
 *          RFC 4443
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t     type;       ///< type field.
    uint8_t     code;       ///< code field.
    uint16_t    checksum;   ///< checksum field.
} icmpv6_hdr_t;

/**
 * @brief   Data type to represent the relevant sub-part of an
 *          ICMPv6 Parameter Problem Message.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4443#section-3.4">
 *          RFC 4443, section 3.4
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t pointer;
} icmpv6_parameter_prob_hdr_t;

/**
 * @brief   Data type to represent the relevant sub-part of an
 *          ICMPv6 Echo Request.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4443#section-4.1">
 *          RFC 4443, section 4.1
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint16_t id;    ///< identifier field.
    uint16_t seq;   ///< sequence number field.
} icmpv6_echo_request_hdr_t;

/**
 * @brief   Data type to represent the relevant sub-part of an
 *          ICMPv6 Echo Reply.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4443#section-4.2">
 *          RFC 4443, section 4.2
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint16_t id;    ///< identifier field.
    uint16_t seq;   ///< sequence number field.
} icmpv6_echo_reply_hdr_t;

/*
 * ICMPv6 Router Solicitation has no fields but reserved fields
 * and is thus elided.
 */

/**
 * @brief   Data type to represent the relevant sub-part of an
 *          ICMPv6 Router Advertisement.
 *
 * @see ICMPV6_ROUTER_ADV_FLAG_MANAGED
 * @see ICMPV6_ROUTER_ADV_FLAG_OTHER
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t hoplimit;           ///< hop limit field.
    uint8_t autoconfig_flags;   ///< (M)anaged and (Other) flag.
    uint16_t router_lifetime;   ///< router lifetime field.
    uint32_t reachable_time;    ///< reachable time field.
    uint32_t retrans_timer;     ///< retrans timer field.
} icmpv6_router_adv_hdr_t;

/**
 * @brief   Bit for (M)anaged flag in Router Advertisement
 * @see icmpv6_router_adv_hdr_t
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>
 */
#define ICMPV6_ROUTER_ADV_FLAG_MANAGED  (0x80)

/**
 * @brief   Bit for (O)ther flag in Router Advertisement
 * @see icmpv6_router_adv_hdr_t
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.2">
 *          RFC 4861, section 4.2
 *      </a>
 */
#define ICMPV6_ROUTER_ADV_FLAG_OTHER    (0x40)

/**
 * @brief   Data type to represent the relevant sub-part of an
 *          ICMPv6 Neighbor Solicitation.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.3">
 *          RFC 4861, section 4.3
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint32_t reserved;          ///< reserved field.
    ipv6_addr_t target_addr;    ///< target address field.
} icmpv6_neighbor_sol_hdr_t;

/**
 * @brief   Data type to represent the relevant sub-part of an
 *          ICMPv6 Neighbor Solicitation.
 *
 * @see ICMPV6_NEIGHBOR_ADV_FLAG_ROUTER
 * @see ICMPV6_NEIGHBOR_ADV_FLAG_SOLICITED
 * @see ICMPV6_NEIGHBOR_ADV_FLAG_OVERRIDE
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.4
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t rso;                ///< flags + first 5 bits of reserved field.
    uint8_t reserved[3];        ///< rest of reserved field.
    ipv6_addr_t target_addr;    ///< target address field.
} icmpv6_neighbor_adv_hdr_t;

/**
 * @brief   Bit for (R)outer flag in Neighbor Advertisement
 * @see icmpv6_neighbor_adv_hdr_t
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.4
 *      </a>
 */
#define ICMPV6_NEIGHBOR_ADV_FLAG_ROUTER     (0x80)

/**
 * @brief   Bit for (S)olicited flag in Neighbor Advertisement
 * @see icmpv6_neighbor_adv_hdr_t
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.4
 *      </a>
 */
#define ICMPV6_NEIGHBOR_ADV_FLAG_SOLICITED  (0x40)

/**
 * @brief   Bit for (O)verride flag in Neighbor Advertisement
 * @see icmpv6_neighbor_adv_hdr_t
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.4">
 *          RFC 4861, section 4.4
 *      </a>
 */
#define ICMPV6_NEIGHBOR_ADV_FLAG_OVERRIDE   (0x20)

/**
 * @brief   Data type to represent an IPv6 Neighbor Discover Option
 *          header.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.6">
 *          RFC 4861, section 4.6
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;   ///< type field.
    uint8_t length; ///< length field.
} icmpv6_ndp_opt_hdr_t;

/**
 * @brief   Data type to represent a Source/Target Link-layer Option
 *          Address option. Link-layer address may be added in payload.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.6.1">
 *          RFC 4861, section 4.6.1
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;   ///< type field.
    uint8_t length; ///< length field.
} icmpv6_ndp_opt_stllao_t;

/**
 * @brief   Data type to represent a Prefix Information option.
 *
 * @see ICMPV6_NDP_OPT_PI_FLAG_ON_LINK
 * @see ICMPV6_NDP_OPT_PI_FLAG_AUTONOM
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.6.2">
 *          RFC 4861, section 4.6.2
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               ///< type field.
    uint8_t length;             ///< length field.
    uint8_t prefix_length;      ///< prefix length field.
    uint8_t l_a_reserved1;      ///< flags + reserved1 field.
    uint32_t val_ltime;         ///< valid lifetime field.
    uint32_t pref_ltime;        ///< preferred lifetime field.
    uint32_t reserved2;         ///< reserved2 field.
    ipv6_addr_t addr;           ///< prefix field.
} icmpv6_ndp_opt_pi_t;

/**
 * @brief   Bit for on-(L)ink flag in Prefix Information option.
 * @see icmpv6_ndp_opt_pi_t
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.6.2">
 *          RFC 4861, section 4.6.2
 *      </a>
 */
#define ICMPV6_NDP_OPT_PI_FLAG_ON_LINK (0x80)

/**
 * @brief   Bit for (A)utonoums address-configuration flag in Prefix
 *          Information option.
 * @see icmpv6_ndp_opt_pi_t
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.6.2">
 *          RFC 4861, section 4.6.2
 *      </a>
 */
#define ICMPV6_NDP_OPT_PI_FLAG_AUTONOM (0x40)

/**
 * @brief   Data type to represent a MTU option.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-4.6.4">
 *          RFC 4861, section 4.6.4
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               ///< type field.
    uint8_t length;             ///< length field.
    uint16_t reserved;          ///< reserved field.
    uint32_t mtu;               ///< MTU field.
} icmpv6_ndp_opt_mtu_t;

/**
 * @brief   Data type to represent an Address Registration Option.
 *
 * @see <a href="http://tools.ietf.org/html/rfc6775#section-4.1">
 *          RFC 6775, section 4.1
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               ///< type field.
    uint8_t length;             ///< length field.
    uint8_t status;             ///< status field.
    uint8_t reserved1;          ///< reserved1 field.
    uint16_t reserved2;         ///< reserved2 field.
    uint16_t reg_ltime;         ///< registration lifetem field.
    ieee_802154_long_t eui64;   ///< EUI-64 field.
} icmpv6_ndp_opt_aro_t;

/**
 * @brief   Data type to represent an 6LoWPAN Context Option.
 *
 * @see ICMPV6_NDP_OPT_6CO_FLAG_CCOMPR
 * @see ICMPV6_NDP_OPT_6CO_FLAG_CID
 * @see <a href="http://tools.ietf.org/html/rfc6775#section-4.2">
 *          RFC 6775, section 4.2
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               ///< type field.
    uint8_t length;             ///< length field.
    uint8_t c_length;           ///< context length field.
    uint8_t c_flags;            ///< Res(erved) field + C flag + CID field.
    uint16_t reserved;          ///< reserved field.
    uint16_t val_ltime;         ///< valid lifetime field.
} icmpv6_ndp_opt_6co_hdr_t;

/**
 * @brief   Bit for context (C)ompression flag in 6LoWPAN Context
 *          Option.
 * @see icmpv6_ndp_opt_6co_hdr_t
 * @see <a href="http://tools.ietf.org/html/rfc6775#section-4.2">
 *          RFC 6775, section 4.2
 *      </a>
 */
#define ICMPV6_NDP_OPT_6CO_FLAG_CCOMPR  (0x10)

/**
 * @brief   Bitmask for Context Identifier in 6LoWPAN Context Option.
 * @see icmpv6_ndp_opt_6co_hdr_t
 * @see <a href="http://tools.ietf.org/html/rfc6775#section-4.2">
 *          RFC 6775, section 4.2
 *      </a>
 */
#define ICMPV6_NDP_OPT_6CO_FLAG_CID     (0x0F)

/**
 * @brief   Data type to represent an Authoritative Border Router Option.
 * @note    TODO not according to RFC.
 *
 * @see <a href="http://tools.ietf.org/html/rfc6775#section-4.3">
 *          RFC 6775, section 4.3
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               ///< type field.
    uint8_t length;             ///< length field.
    uint16_t version;           ///< version field.
    uint32_t reserved;          ///< reseverd field.
    ipv6_addr_t addr;           ///< 6LBR address field.
} icmpv6_ndp_opt_abro_t;

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

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SIXLOWPAN_TYPES_H */
