/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ipv6    IPv6
 * @ingroup     net
 * @{
 *
 * @file        ipv6.h
 * @brief       Type and prototype definitions for IPv6
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef __IPV6_H_
#define __IPV6_H_

#include "byteorder.h"
#include "ipv6/addr.h"
#include "kernel_types.h"
#include "netapi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   L4 protocol number for @ref net_tcp.
 */
#define IPV6_PROTO_NUM_TCP          (6)

/**
 * @brief   L4 protocol number for @ref net_udp.
 */
#define IPV6_PROTO_NUM_UDP          (17)

/**
 * @brief   L4 protocol number for @ref net_icmpv6.
 */
#define IPV6_PROTO_NUM_ICMPV6       (58)

/**
 * @brief   L4 protocol number for no L4 protocol in IPv6.
 */
#define IPV6_PROTO_NUM_NONE         (59)

/**
 * @brief   L4 protocol number for IPv6 destination options.
 */
#define IPV6_PROTO_NUM_IPV6_OPTS    (60)

/**
 * @brief   PID of the IPv6 control thread.
 */
extern kernel_pid_t ipv6_pid;

/**
 * @brief   @ref netapi configuration types for IPv6
 * @extends netapi_conf_type_t
 *
 * @details Since the IPv6 thread is to be supposed the only one of its kind
 *          in the system and setting of the IPv6 addresses is more dependent
 *          on the interface (equivalent to MAC control thread PID) then on
 *          IPv6 as a whole itself, addresses are set and got by the @ref ipv6_if
 *          module.
 */
typedef enum {
    /**
     * @brief   Protocol of the IPv6 layer of type netdev_proto_t; Always
     *          NETDEV_PROTO_IPV6; Only for getting.
     */
    IPV6_CONF_PROTO = NETAPI_CONF_PROTO,
    IPV6_CONF_REGISTRY = NETAPI_CONF_REGISTRY,  /**< Getter for receiver registry */
} ipv6_conf_t;

/**
 * @brief   Data type to represent an IPv6 packet header
 *
 * @see <a href="http://tools.ietf.org/html/rfc2460">
 *          RFC 2460
 *      </a>
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief Version, traffic class, and flow label
     *
     * @detail The version are the 4 most significant bits, the traffic class
     *         the 8 next bit, and the remainding 20 bits are the flow label
     *
     *         @code{.unparsed}
     *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     *         |Version| Traffic Class |           Flow Label                  |
     *         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     *         @endcode
     *
     *         This module provides helper functions to set, get, and check these
     *         fields accordingly:
     *         * ipv6_hdr_set_version()
     *         * ipv6_hdr_get_version()
     *         * ipv6_hdr_is_ipv6_hdr()
     *         * ipv6_hdr_set_trafficclass()
     *         * ipv6_hdr_set_trafficclass_ecn()
     *         * ipv6_hdr_set_trafficclass_dscp()
     *         * ipv6_hdr_get_trafficclass()
     *         * ipv6_hdr_get_trafficclass_ecn()
     *         * ipv6_hdr_get_trafficclass_dscp()
     *         * ipv6_hdr_set_flowlabel()
     *         * ipv6_hdr_get_flowlabel()
     */
    network_uint32_t v_tc_fl;
    network_uint16_t length;        /**< payload length of this packet. */
    uint8_t nextheader;             /**< type of next header in this packet. */
    uint8_t hoplimit;               /**< hop limit for this packet. */
    ipv6_addr_t srcaddr;            /**< source address of this packet. */
    ipv6_addr_t destaddr;           /**< destination address of this packet. */
} ipv6_hdr_t;

/**
 * @brief   Sets the version field of *hdr* to 6
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 */
static inline void ipv6_hdr_set_version(ipv6_hdr_t *hdr)
{
    hdr->v_tc_fl.u8[0] &= 0x0f;
    hdr->v_tc_fl.u8[0] |= 0x60;
}

/**
 * @brief   Gets the value of the version field of *hdr*
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 *
 * @return  Value of the version field of *hdr*.
 */
static inline uint8_t ipv6_hdr_get_version(ipv6_hdr_t *hdr)
{
    return ((hdr->v_tc_fl.u8[0]) >> 4);
}

/**
 * @brief   Checks if the version field is set to 6
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 *
 * @return  1, if version field is 6
 * @return  0, otherwise
 */
static inline uint8_t ipv6_hdr_is_ipv6_hdr(ipv6_hdr_t *hdr)
{
    return (((hdr->v_tc_fl.u8[0]) & 0xf0) == 0x60);
}

/**
 * @brief   Sets the traffic class field of *hdr*
 *
 * @param[in] hdr           Pointer to an IPv6 header.
 * @param[in] trafficclass  The new value for the traffic class field.
 */
static inline void ipv6_hdr_set_trafficclass(ipv6_hdr_t *hdr, uint8_t trafficclass)
{
    hdr->v_tc_fl.u8[0] &= 0xf0;
    hdr->v_tc_fl.u8[0] |= (0x0f & (trafficclass >> 4));
    hdr->v_tc_fl.u8[1] &= 0x0f;
    hdr->v_tc_fl.u8[1] |= (0xf0 & (trafficclass << 4));
}

/**
 * @brief   Sets the value of the Explicit Congestion Notification (ECN) part
 *          of the traffic class field of *hdr*
 *
 * @detail  The field is needed e.g. in context of 6LoWPAN header compression
 *
 * @see <a href="https://tools.ietf.org/html/rfc3168#section-5">
 *          RFC 3168, section 5
 *      </a>
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 * @param[in] ecn   The new value for the 2-bit ECN part of the traffic class
 *                  field.
 */
static inline void ipv6_hdr_set_trafficclass_ecn(ipv6_hdr_t *hdr, uint8_t ecn)
{
    hdr->v_tc_fl.u8[0] &= 0xf3;
    hdr->v_tc_fl.u8[0] |= (0x0c & (ecn << 2));
}

/**
 * @brief   Sets the value of the Defferentiaated Service Codepoint (DSCP) part
 *          of the traffic class field of *hdr*
 *
 * @detail  The field is needed e.g. in context of 6LoWPAN header compression
 *
 * @see <a href="https://tools.ietf.org/html/rfc2474#section-3">
 *          RFC 2474, section 3
 *      </a>
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 * @param[in] dscp  The new value for the 6-bit DSCP part of the traffic class
 *                  field.
 */
static inline void ipv6_hdr_set_trafficclass_dscp(ipv6_hdr_t *hdr, uint8_t dscp)
{
    hdr->v_tc_fl.u8[0] &= 0xfc;
    hdr->v_tc_fl.u8[0] |= (0x03 & (dscp >> 4));
    hdr->v_tc_fl.u8[1] &= 0x0f;
    hdr->v_tc_fl.u8[1] |= (0xf0 & (dscp << 4));
}

/**
 * @brief   Gets the value of the traffic class field of *hdr*
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 *
 * @return  Value of the traffic class field of *hdr*.
 */
static inline uint8_t ipv6_hdr_get_trafficclass(ipv6_hdr_t *hdr)
{
    return ((((hdr->v_tc_fl.u8[0]) & 0x0f) << 4) |
            ((hdr->v_tc_fl.u8[1] & 0xf0) >> 4));
}

/**
 * @brief   Gets the value of the Explicit Congestion Notification (ECN) part
 *          of the traffic class field of *hdr*
 *
 * @detail  The field is needed e.g. in context of 6LoWPAN header compression
 *
 * @see <a href="https://tools.ietf.org/html/rfc3168#section-5">
 *          RFC 3168, section 5
 *      </a>
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 *
 * @return  Value of the ECN part of the traffic class field of *hdr*.
 */
static inline uint8_t ipv6_hdr_get_trafficclass_ecn(ipv6_hdr_t *hdr)
{
    return (((hdr->v_tc_fl.u8[0]) & 0x0c) >> 2);
}


/**
 * @brief   Gets the value of the Defferentiaated Service Codepoint (DSCP) part
 *          of the traffic class field of *hdr*
 *
 * @detail  The field is needed e.g. in context of 6LoWPAN header compression
 *
 * @see <a href="https://tools.ietf.org/html/rfc2474#section-3">
 *          RFC 2474, section 3
 *      </a>
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 *
 * @return  Value of the DSCP part of the traffic class field of *hdr*.
 */
static inline uint8_t ipv6_hdr_get_trafficclass_dscp(ipv6_hdr_t *hdr)
{
    return ((((hdr->v_tc_fl.u8[0]) & 0x03) << 4) |
            ((hdr->v_tc_fl.u8[1] & 0xf0) >> 4));
}

/**
 * @brief   Sets the flow label field of *hdr*
 *
 * @param[in] hdr       Pointer to an IPv6 header.
 * @param[in] flowlabel The new value for the flow label field in host byte
 *                      order.
 */
static inline void ipv6_hdr_set_flowlabel(ipv6_hdr_t *hdr, uint32_t flowlabel)
{
    hdr->v_tc_fl.u8[1] &= 0xf0;
    hdr->v_tc_fl.u8[1] |= (0x0f & (byteorder_htonl(flowlabel).u8[1]));
    hdr->v_tc_fl.u16[1] = byteorder_htonl(flowlabel).u16[1];
}

/**
 * @brief   Gets the value of the flow label field of *hdr*
 *
 * @param[in] hdr   Pointer to an IPv6 header.
 *
 * @return  Value of the flow label field of *hdr*.
 */
static inline uint32_t ipv6_hdr_get_flowlabel(ipv6_hdr_t *hdr)
{
    return byteorder_ntohl(hdr->v_tc_fl) & 0x000fffff;
}

#ifdef __cplusplus
}
#endif

#endif /* __IPV6_H_ */
/**
 * @}
 */
