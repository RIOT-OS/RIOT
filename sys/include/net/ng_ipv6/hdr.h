/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_ipv6_hdr IPv6 header defintions
 * @ingroup     net_ng_ipv6
 * @{
 *
 * @file
 * @brief   IPv6 header
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_IPV6_HDR_H_
#define NG_IPV6_HDR_H_

#include <inttypes.h>
#include <stdbool.h>

#include "byteorder.h"
#include "net/ng_ipv6/addr.h"
#include "net/inet_csum.h"
#include "net/ng_pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Data type to represent an IPv6 packet header
 *
 * @details The structure of the header is as follows:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.unparsed}
 *                      1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Version| Traffic Class |           Flow Label                  |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |         Payload Length        |  Next Header  |   Hop Limit   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                                                               |
 * +                                                               +
 * |                                                               |
 * +                         Source Address                        +
 * |                                                               |
 * +                                                               +
 * |                                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                                                               |
 * +                                                               +
 * |                                                               |
 * +                      Destination Address                      +
 * |                                                               |
 * +                                                               +
 * |                                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @see <a href="http://tools.ietf.org/html/rfc2460#section-3">
 *          RFC 2460, section 3
 *      </a>
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief Version, traffic class, and flow label
     *
     * @details The version are the 4 most significant bits, the traffic class
     * the 8 next bit, and the remainding 20 bits are the flow label (see
     * above).
     *
     * This module provides helper functions to set, get, and check these
     * fields accordingly:
     * * ng_ipv6_hdr_set_version()
     * * ng_ipv6_hdr_get_version()
     * * ng_ipv6_hdr_is()
     * * ng_ipv6_hdr_set_tc()
     * * ng_ipv6_hdr_set_tc_ecn()
     * * ng_ipv6_hdr_set_tc_dscp()
     * * ng_ipv6_hdr_get_tc()
     * * ng_ipv6_hdr_get_tc_ecn()
     * * ng_ipv6_hdr_get_tc_dscp()
     * * ng_ipv6_hdr_set_fl()
     * * ng_ipv6_hdr_get_fl()
     */
    network_uint32_t v_tc_fl;
    network_uint16_t len;   /**< payload length of this packet. */
    uint8_t nh;             /**< type of next header in this packet. */
    uint8_t hl;             /**< hop limit for this packet. */
    ng_ipv6_addr_t src;     /**< source address of this packet. */
    ng_ipv6_addr_t dst;     /**< destination address of this packet. */
} ng_ipv6_hdr_t;

/**
 * @brief   Sets the version field of @p hdr to 6
 *
 * @param[out] hdr  Pointer to an IPv6 header.
 */
static inline void ng_ipv6_hdr_set_version(ng_ipv6_hdr_t *hdr)
{
    hdr->v_tc_fl.u8[0] &= 0x0f;
    hdr->v_tc_fl.u8[0] |= 0x60;
}

/**
 * @brief   Gets the value of the version field of @p hdr
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 *
 * @return  Value of the version field of @p hdr.
 */
static inline uint8_t ng_ipv6_hdr_get_version(const ng_ipv6_hdr_t *hdr)
{
    return ((hdr->v_tc_fl.u8[0]) >> 4);
}

/**
 * @brief   Checks if the version field is set to 6
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 *
 * @return  true, if version field is 6
 * @return  false, otherwise
 */
static inline bool ng_ipv6_hdr_is(const ng_ipv6_hdr_t *hdr)
{
    return (((hdr->v_tc_fl.u8[0]) & 0xf0) == 0x60);
}

/**
 * @brief   Sets the traffic class field of @p hdr
 *
 * @param[out] hdr  Pointer to an IPv6 header.
 * @param[in] tc    The new value for the traffic class field.
 */
static inline void ng_ipv6_hdr_set_tc(ng_ipv6_hdr_t *hdr, uint8_t tc)
{
    hdr->v_tc_fl.u8[0] &= 0xf0;
    hdr->v_tc_fl.u8[0] |= (0x0f & (tc >> 4));
    hdr->v_tc_fl.u8[1] &= 0x0f;
    hdr->v_tc_fl.u8[1] |= (0xf0 & (tc << 4));
}

/**
 * @brief   Sets the value of the Explicit Congestion Notification (ECN) part
 *          of the traffic class field of @p hdr
 *
 * @details The field is needed e.g. in context of 6LoWPAN header compression
 *
 * @see <a href="https://tools.ietf.org/html/rfc3168#section-5">
 *          RFC 3168, section 5
 *      </a>
 *
 * @param[out] hdr  Pointer to an IPv6 header.
 * @param[in] ecn   The new value for the 2-bit ECN part of the traffic class
 *                  field.
 */
static inline void ng_ipv6_hdr_set_tc_ecn(ng_ipv6_hdr_t *hdr, uint8_t ecn)
{
    hdr->v_tc_fl.u8[0] &= 0xf3;
    hdr->v_tc_fl.u8[0] |= (0x0c & (ecn << 2));
}

/**
 * @brief   Sets the value of the Differentiated Service Codepoint (DSCP) part
 *          of the traffic class field of @p hdr
 *
 * @details The field is needed e.g. in context of 6LoWPAN header compression
 *
 * @see <a href="https://tools.ietf.org/html/rfc2474#section-3">
 *          RFC 2474, section 3
 *      </a>
 *
 * @param[out] hdr  Pointer to an IPv6 header.
 * @param[in] dscp  The new value for the 6-bit DSCP ng_part of the traffic class
 *                  field.
 */
static inline void ng_ipv6_hdr_set_tc_dscp(ng_ipv6_hdr_t *hdr, uint8_t dscp)
{
    hdr->v_tc_fl.u8[0] &= 0xfc;
    hdr->v_tc_fl.u8[0] |= (0x03 & (dscp >> 4));
    hdr->v_tc_fl.u8[1] &= 0x0f;
    hdr->v_tc_fl.u8[1] |= (0xf0 & (dscp << 4));
}

/**
 * @brief   Gets the value of the traffic class field of @p hdr
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 *
 * @return  Value of the traffic class field of @p hdr.
 */
static inline uint8_t ng_ipv6_hdr_get_tc(const ng_ipv6_hdr_t *hdr)
{
    return ((((hdr->v_tc_fl.u8[0]) & 0x0f) << 4) |
            ((hdr->v_tc_fl.u8[1] & 0xf0) >> 4));
}

/**
 * @brief   Gets the value of the Explicit Congestion Notification (ECN) part
 *          of the traffic class field of @p hdr
 *
 * @details The field is needed e.g. in context of 6LoWPAN header compression
 *
 * @see <a href="https://tools.ietf.org/html/rfc3168#section-5">
 *          RFC 3168, section 5
 *      </a>
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 *
 * @return  Value of the ECN part of the traffic class field of @p hdr.
 */
static inline uint8_t ng_ipv6_hdr_get_tc_ecn(const ng_ipv6_hdr_t *hdr)
{
    return (((hdr->v_tc_fl.u8[0]) & 0x0c) >> 2);
}


/**
 * @brief   Gets the value of the Differentiated Service Codepoint (DSCP) part
 *          of the traffic class field of @p hdr
 *
 * @details The field is needed e.g. in context of 6LoWPAN header compression
 *
 * @see <a href="https://tools.ietf.org/html/rfc2474#section-3">
 *          RFC 2474, section 3
 *      </a>
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 *
 * @return  Value of the DSCP part of the traffic class field of @p hdr.
 */
static inline uint8_t ng_ipv6_hdr_get_tc_dscp(const ng_ipv6_hdr_t *hdr)
{
    return ((((hdr->v_tc_fl.u8[0]) & 0x03) << 4) |
            ((hdr->v_tc_fl.u8[1] & 0xf0) >> 4));
}

/**
 * @brief   Sets the flow label field of @p hdr
 *
 * @param[out] hdr  Pointer to an IPv6 header.
 * @param[in] fl    The new value for the flow label field in host byte order.
 */
static inline void ng_ipv6_hdr_set_fl(ng_ipv6_hdr_t *hdr, uint32_t fl)
{
    hdr->v_tc_fl.u8[1] &= 0xf0;
    hdr->v_tc_fl.u8[1] |= (0x0f & (byteorder_htonl(fl).u8[1]));
    hdr->v_tc_fl.u16[1] = byteorder_htonl(fl).u16[1];
}

/**
 * @brief   Gets the value of the flow label field of @p hdr
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 *
 * @return  Value of the flow label field of @p hdr.
 */
static inline uint32_t ng_ipv6_hdr_get_fl(const ng_ipv6_hdr_t *hdr)
{
    return byteorder_ntohl(hdr->v_tc_fl) & 0x000fffff;
}

/**
 * @brief   Calculates the Internet Checksum for the IPv6 Pseudo Header.
 *
 * @see <a href="https://tools.ietf.org/html/rfc2460#section-8.1">
 *          RFC 2460, section 8.1
 *      </a>
 *
 * @param[in] sum       Preinialized value of the sum.
 * @param[in] prot_num  The @ref net_protnum you want to calculate the
 *                      checksum for. Can not be inferred from
 *                      ng_ipv6_hdr_t::nh, since it can be an IPv6 exentension
 *                      header.
 * @param[in] hdr       An IPv6 header to derive the Pseudo Header from.
 * @param[in] len       The upper-layer packet length for the pseudo header.
 *                      Can not be inferred from ng_ipv6_hdr_t::len, since
 *                      there can be extension headers between the IPv6 header
 *                      and the payload.
 *
 * @return  The non-normalized Internet Checksum of the given IPv6 pseudo header.
 */
static inline uint16_t ng_ipv6_hdr_inet_csum(uint16_t sum, ng_ipv6_hdr_t *hdr,
                                             uint8_t prot_num, uint16_t len)
{
    if ((sum + len + prot_num) > 0xffff) {
        /* increment by one for overflow to keep it as 1's complement sum */
        sum++;
    }

    return inet_csum(sum + len + prot_num, hdr->src.u8,
                     (2 * sizeof(ng_ipv6_addr_t)));
}

/**
 * @brief   Builds an IPv6 header for sending and adds it to the packet buffer.
 *
 * @details Initializes version field with 6, traffic class, flow label, and
 *          hop limit with 0, and next header with @ref PROTNUM_RESERVED.
 *
 * @param[in] payload   Payload for the packet.
 * @param[in] src       Source address for the header. Can be NULL if not
 *                      known or required.
 * @param[in] src_len   Length of @p src. Can be 0 if not known or required or
 *                      must be `sizeof(ng_ipv6_addr_t)`.
 * @param[in] dst       Destination address for the header. Can be NULL if not
 *                      known or required.
 * @param[in] dst_len   Length of @p dst. Can be 0 if not known or required or
 *                      must be `sizeof(ng_ipv6_addr_t)`.
 *
 * @return  The an IPv6 header in packet buffer on success.
 * @return  NULL on error.
 */
ng_pktsnip_t *ng_ipv6_hdr_build(ng_pktsnip_t *payload,
                                uint8_t *src, uint8_t src_len,
                                uint8_t *dst, uint8_t dst_len);

/**
 * @brief   Outputs an IPv6 header to stdout.
 *
 * @param[in] hdr   An IPv6 header.
 */
void ng_ipv6_hdr_print(ng_ipv6_hdr_t *hdr);

#ifdef __cplusplus
}
#endif

#endif /* NG_IPV6_HDR_H_ */
/** @} */
