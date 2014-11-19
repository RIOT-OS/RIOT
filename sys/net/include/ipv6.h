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

#include <stdbool.h>
#include <stdint.h>

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
 * @brief   L4 protocol number for every data delivered to IPv6.
 *
 * @details This value is thought to be used for registration of
 *          "catch-all" threads with @ref netapi_register(),
 *          @ref netapi_register_current_thread, or the raw NETAPI_CMD_REG
 *          command.
 */
#define IPV6_PROTO_NUM_IPV6_ANY     (256)

/**
 * @brief   PID of the IPv6 control thread.
 */
extern kernel_pid_t ipv6_pid;

/**
 * @brief   @ref netapi configuration types for IPv6.
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
    IPV6_CONF_REGISTRY = NETAPI_CONF_REGISTRY,  /**< Get for receiver registry. */
    IPV6_CONF_DEFAULT_HOP_LIMIT,    /**< Get or set default hop limit. */
    IPV6_CONF_ROUTING_PROVIDER,     /**< Set routing provider. */
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
 * @brief   Definition of the routing provider callback.
 *
 * @param[out] if_id    Interface *dest* is reachable from.
 * @param[out] dest     IPv6 address to get the next hop for.
 *
 * @return  next hop to *dest*.
 * @return  NULL, if *dest* is unreachable.
 */
typedef ipv6_addr_t *(*ipv6_routing_provider_t)(int *if_id, ipv6_addr_t *dest);

/* XXX Note to my self for upper layer. Do this statically ** */
/*  * @brief   Definition of upper layer Internet Checksum checksum callback. */
/*  * */
/*  * @details The callback is supposed to set the checksum field of the upper layer */
/*  *          header. That's why there is no return value. */
/*  * */
/*  * @see     ipv6_reg_csum_cb() */
/*  * */
/*  * @param[in] ipv6_hdr      Generated IPv6 header with initialized source address. */
/*  * @param[in] ulh           Upper layer headers without IPv6 extension headers in */
/*  *                          decending (layer-wise) order. */
/*  * @param[in] payload       Payload of highest layer. */
/*  * @param[in] payload_len   Length of *payload*. */
/*  * @param[in] initial       initial value for checksum */
/*  *1/ */
/* typedef void (*ipv6_csum_cb_t)(const ipv6_hdr_t *ipv6_hdr, netdev_hlist_t *ulh, */
/*                                void *payload, size_t payload_len, uint16_t initial); */

/**
 * @brief   Sets the version field of *hdr* to 6
 *
 * @param[out] hdr  Pointer to an IPv6 header.
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
static inline uint8_t ipv6_hdr_get_version(const ipv6_hdr_t *hdr)
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
static inline uint8_t ipv6_hdr_is_ipv6_hdr(const ipv6_hdr_t *hdr)
{
    return (((hdr->v_tc_fl.u8[0]) & 0xf0) == 0x60);
}

/**
 * @brief   Sets the traffic class field of *hdr*
 *
 * @param[out] hdr          Pointer to an IPv6 header.
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
 * @param[out] hdr  Pointer to an IPv6 header.
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
 * @param[out] hdr  Pointer to an IPv6 header.
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
static inline uint8_t ipv6_hdr_get_trafficclass(const ipv6_hdr_t *hdr)
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
static inline uint8_t ipv6_hdr_get_trafficclass_ecn(const ipv6_hdr_t *hdr)
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
static inline uint8_t ipv6_hdr_get_trafficclass_dscp(const ipv6_hdr_t *hdr)
{
    return ((((hdr->v_tc_fl.u8[0]) & 0x03) << 4) |
            ((hdr->v_tc_fl.u8[1] & 0xf0) >> 4));
}

/**
 * @brief   Sets the flow label field of *hdr*
 *
 * @param[out] hdr      Pointer to an IPv6 header.
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
static inline uint32_t ipv6_hdr_get_flowlabel(const ipv6_hdr_t *hdr)
{
    return byteorder_ntohl(hdr->v_tc_fl) & 0x000fffff;
}

/**
 * @brief   Data type to represent an IPv6 source routing header for RPL
 *
 * @see <a href="http://tools.ietf.org/html/rfc6554#section-3">
 *          RFC 6554, section 3
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t nextheader;             /**< type of next header in this packet. */
    uint8_t hdrextlen;              /**< length of header in 8-octet units. */
    uint8_t routing_type;           /**< identify srh-variant. */
    uint8_t segments_left;          /**< remaining route segments before reaching
                                      *  destination. */
    network_uint32_t cmpr_pad_rsv;  /**< 4+4 bit, expressing prefix octets from each/last segment,
                                      *  4-bit, expressing the number of octets
                                      *  used for paddingafter addresses
                                      *  + 20 bit reseveved */
} ipv6_srh_t;

/**
 * @brief   Get CmprI field of source routing header
 *
 * @see <a href="http://tools.ietf.org/html/rfc6554#section-3">
 *          RFC 6554, section 3
 *      </a>
 *
 * @param[in] ipv6_srh  A source routing header.
 *
 * @return  The value of the CmprI field of *ipv6_srh*.
 */
static inline uint8_t ipv6_srh_get_cmpri(const ipv6_srh_t *ipv6_srh)
{
    return (ipv6_srh->cmpr_pad_rsv.u8[0] >> 4);
}

/**
 * @brief   Set CmprI field of source routing header
 *
 * @see <a href="http://tools.ietf.org/html/rfc6554#section-3">
 *          RFC 6554, section 3
 *      </a>
 *
 * @param[out] ipv6_srh A source routing header.
 * @param[in] cmpri     New value for the CmprI field.
 */
static inline void ipv6_srh_set_cmpri(ipv6_srh_t *ipv6_srh, uint8_t cmpri)
{
    ipv6_srh->cmpr_pad_rsv.u8[0] &= 0x0f;
    ipv6_srh->cmpr_pad_rsv.u8[0] |= (cmpri << 4);
}

/**
 * @brief   Get CmprE field of source routing header
 *
 * @see <a href="http://tools.ietf.org/html/rfc6554#section-3">
 *          RFC 6554, section 3
 *      </a>
 *
 * @param[in] ipv6_srh  A source routing header.
 *
 * @return  The value of the CmprE field of *ipv6_srh*.
 */
static inline uint8_t ipv6_srh_get_cmpre(const ipv6_srh_t *ipv6_srh)
{
    return (ipv6_srh->cmpr_pad_rsv.u8[0] & 0x0f);
}

/**
 * @brief   Set CmprE field of source routing header
 *
 * @see <a href="http://tools.ietf.org/html/rfc6554#section-3">
 *          RFC 6554, section 3
 *      </a>
 *
 * @param[out] ipv6_srh A source routing header.
 * @param[in] cmpre     New value for the CmprE field.
 */
static inline void ipv6_srh_set_cmpre(ipv6_srh_t *ipv6_srh, uint8_t cmpre)
{
    ipv6_srh->cmpr_pad_rsv.u8[0] &= 0xf0;
    ipv6_srh->cmpr_pad_rsv.u8[0] |= (cmpre & 0x0f);
}

/**
 * @brief   Get Pad field of source routing header
 *
 * @see <a href="http://tools.ietf.org/html/rfc6554#section-3">
 *          RFC 6554, section 3
 *      </a>
 *
 * @param[in] ipv6_srh  A source routing header.
 *
 * @return  The value of the Pad field of *ipv6_srh*.
 */
static inline uint8_t ipv6_srh_get_pad(const ipv6_srh_t *ipv6_srh)
{
    return (ipv6_srh->cmpr_pad_rsv.u8[1] >> 4);
}

/**
 * @brief   Set Pad field of source routing header
 *
 * @see <a href="http://tools.ietf.org/html/rfc6554#section-3">
 *          RFC 6554, section 3
 *      </a>
 *
 * @param[out] ipv6_srh A source routing header.
 * @param[in] pad       New value for the Pad field.
 */
static inline void ipv6_srh_set_pad(ipv6_srh_t *ipv6_srh, uint8_t pad)
{
    ipv6_srh->cmpr_pad_rsv.u8[1] &= 0x0f;
    ipv6_srh->cmpr_pad_rsv.u8[1] |= (pad << 4);
}

/**
 * @brief   Initialize IPv6 layer.
 *
 * @return  0, on success.
 * @return  -ENOBUFS, space for mandatory addresses is not enough.
 * @return  -ECANCELED, if IPv6 control thread could not be started.
 */
int ipv6_init(void);

/**
 * @fn      bool ipv6_is_router(void);
 * @brief   Determines if node is a router.
 *
 * @return  true, if node is a router
 * @return  false, otherwise
 */

/**
 * @fn      int ipv6_register_routing_provider(ipv6_routing_provider_t rp);
 * @brief   Registers a function that decides how to route incomming
 *          IP packets with a destination that is not this interface.
 *          The default behaviour is to try forwarding such packets to
 *          the neighborhood.
 *          Register a function to change the default behaviour.
 *          Such function shall return the next hop to reach the destination
 *          of the IP packet, or NULL if no such next hop is known.
 *          In this case, the packet will be discarded.
 *
 * @param   rp  A routing provider.
 *
 * @return 0, on success
 * @return -ENOBUFS, if there is already a routing provider registered.
 */
#ifdef MODULE_IPV6_ROUTER

bool ipv6_is_router(void);
int ipv6_register_routing_provider(ipv6_routing_provider_t rp);

/**
 * @brief   Initialize IPv6 layer as router.
 *
 * @return  0, on success.
 * @return  -ENOBUFS, space for mandatory addresses is not enough.
 * @return  -ECANCELED, if IPv6 control thread could not be started.
 */
int ipv6_init_router(void);

#else   /* MODULE_IPV6_ROUTER */

static inline bool ipv6_is_router(void)
{
    return false;
}

static inline int ipv6_register_routing_provider(ipv6_routing_provider_t rp)
{
    return 0;
}
#endif /* MODULE_IPV6_ROUTER */

/**
 * @brief   Send IPv6 packet to *dest*.
 *
 * @param[in] dest              Destination of this packet.
 * @param[in] next_headers      List of next headers.
 * @param[in] payload           Payload of the packet.
 * @param[in] payload_len       Length of *payload*.
 *
 * @return  total length of transmitted data, on success.
 * @return  -EHOSTUNREACH, if no route to the given dest could be obtained.
 * @return  -EINPROGRESS, In case of reactive routing: routing is going to try
 *          to find a route.
 */
static inline int ipv6_sendto(const ipv6_addr_t *dest, netdev_hlist_t *next_headers,
                              void *payload, size_t payload_len)
{
    return netapi_send_data2(ipv6_pid, next_headers, (void *)dest,
                             sizeof(ipv6_addr_t), payload, payload_len);
}

/**
 * @brief   IPv6 internal function to send a packet.
 *
 * @internal
 *
 * @note    MUST be called from within the IPv6 control thread.
 *
 * @param[in] ipv6_hdr      The IPv6 header.
 * @param[in] next_headers  List of next headers.
 * @param[in] payload       Payload of the packet.
 * @param[in] payload_len   Length of *payload*.
 *
 * @return  total length of transmitted data, on success.
 * @return  -EHOSTUNREACH, if no route to the given dest could be obtained.
 * @return  -EINPROGRESS, In case of reactive routing: routing is going to try
 *          to find a route.
 * @return  -EACCES, caller PID was not ipv6_pid.
 */
int ipv6_send_packet(const ipv6_hdr_t *ipv6_hdr, netdev_hlist_t *next_headers,
                     void *payload, size_t payload_len);

/**
 * @brief   IPv6 internal function to send a packet with highest priority.
 *
 * @internal
 *
 * @note    MUST be called from within the IPv6 control thread.
 *
 * @param[in] ipv6_hdr      The IPv6 header.
 * @param[in] next_headers  List of next headers.
 * @param[in] payload       Payload of the packet.
 * @param[in] payload_len   Length of *payload*.
 *
 * @return  total length of transmitted data, on success.
 * @return  -EHOSTUNREACH, if no route to the given dest could be obtained.
 * @return  -EINPROGRESS, In case of reactive routing: routing is going to try
 *          to find a route.
 * @return  -EACCES, caller PID was not ipv6_pid.
 */
int ipv6_send_packet_high_priority(const ipv6_hdr_t *ipv6_hdr,
                                   netdev_hlist_t *next_headers,
                                   void *payload, size_t payload_len);

/**
 * @brief   Sets the default hop limit to use with IPv6 packets.
 *
 * @param[in] hop_limit The hop limit to set the default hop limit to.
 *
 * @return  0 on success
 * @return  negative value on failure
 */
static inline int ipv6_set_default_hop_limit(uint8_t hop_limit)
{
    return netapi_set_option(ipv6_pid, IPV6_CONF_DEFAULT_HOP_LIMIT, &hop_limit,
                             sizeof(hop_limit));
}

/**
 * @brief   Gets the default hop limit that is used for IPv6 packets.
 *
 * @return  The current default hop limit for IPv6 packets
 * @return  0, on error.
 */
static inline uint8_t ipv6_get_default_hop_limit(void)
{
    uint8_t hop_limit;

    if (netapi_get_option(ipv6_pid, IPV6_CONF_DEFAULT_HOP_LIMIT, &hop_limit,
                          sizeof(hop_limit) < 0) < 0) {
        return 0;
    }

    return hop_limit;
}

/**
 * @brief   Registers a handler thread for any incoming IPv6 packets.
 *
 * @param[in] pid   PID of handler thread.
 *
 * @return  0 on success
 * @return  -ENOBUFS, if maximum number of registrable threads is exceeded.
 */
static inline int ipv6_register_packet_handler(kernel_pid_t pid)
{
    return netapi_register(ipv6_pid, pid, IPV6_PROTO_NUM_IPV6_ANY);
}

/**
 * @brief   Registers a handler thread for incoming L4 protocol.
 *
 * @param[in] next_header   Next header ID of the L4 protocol.
 * @param[in] pid           PID of the handler thread
 */
static inline int ipv6_register_next_header_handler(uint8_t next_header,
        kernel_pid_t pid)
{
    return netapi_register(ipv6_pid, pid, (netapi_reg_demux_ctx_t)next_header);
}

/**
 * @brief   Registers a handler thread for incoming TCP packets.
 *
 * @param[in] pid           PID of the handler thread
 */
static inline int ipv6_register_tcp_handler(kernel_pid_t pid)
{
    return netapi_register(ipv6_pid, pid,
                           (netapi_reg_demux_ctx_t)IPV6_PROTO_NUM_TCP);
}

/**
 * @brief   Registers a handler thread for incoming UDP packets.
 *
 * @param[in] pid           PID of the handler thread
 */
static inline int ipv6_register_udp_handler(kernel_pid_t pid)
{
    return netapi_register(ipv6_pid, pid,
                           (netapi_reg_demux_ctx_t)IPV6_PROTO_NUM_UDP);
}

/**
 * @brief   Calculates the Internet Checksum of the IPv6 pseudo header based
 *          on *ipv6_hdr*, *ul_packet_len*, and *next_header*.
 *
 * @see <a href="https://tools.ietf.org/html/rfc2460#section-8.1">
 *          RFC 2460, section 8.1
 *      </a>
 *
 * @param[in] ipv6_hdr      An IPv6 header to get the pseudo header data from
 * @param[in] ul_packet_len ipv6_hdr::length in host byte order minus IPv6
 *                          extension headers.
 * @param[in] next_header   Next header value for upper layer. Might differ from
 *                          ipv6_hdr::nextheader, when it is an IPv6 extension
 *                          header
 * @param[in] initial       Initial value for the checksum
 *
 * @return  The checksum IPv6 pseudo header with values from *ipv6_hdr*,
 *          *ul_packet_len*, and *next_header* and intial value *initial*.
 */
uint16_t ipv6_pseudo_hdr_csum(const ipv6_hdr_t *ipv6_hdr, uint32_t ul_packet_len,
                              uint8_t next_header, uint16_t initial);

/* TODO: ICMPv6 option/header handler; IPv6 extension header handler? */

#ifdef __cplusplus
}
#endif

#endif /* __IPV6_H_ */
/**
 * @}
 */
