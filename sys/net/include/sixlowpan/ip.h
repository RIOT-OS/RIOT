/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sixlowpan_ip IPv6
 * @ingroup     net_sixlowpan
 * @brief       Internet Protocol version 6
 * @{
 *
 * @file
 * @brief       6LoWPAN constants, data structs, and prototypes for network layer
 *
 * @author      Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef SIXLOWPAN_IP_H
#define SIXLOWPAN_IP_H

#include <stdint.h>

#include "inet_ntop.h"
#include "net_if.h"
#include "net_help.h"
#include "sixlowpan/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   IPv6 maximum transmission unit.
 */
#define IPV6_MTU                (256)

/**
 * @brief   Length of an IPv6 address in byte.
 */
#define IPV6_ADDR_LEN           (16)

/**
 * @brief   Length of an IPv6 address in bit.
 */
#define IPV6_ADDR_BIT_LEN       (128)

/**
 * @brief   Maximum length of an IPv6 address represented as string.
 */
#define IPV6_MAX_ADDR_STR_LEN   (40)

/**
 * @brief   L4 protocol number for TCP.
 */
#define IPV6_PROTO_NUM_TCP          (6)

/**
 * @brief   L4 protocol number for UDP.
 */
#define IPV6_PROTO_NUM_UDP          (17)

/**
* @brief  Next header value for source routing
* @see <a href="http://tools.ietf.org/html/rfc5095#section-1">
*          RFC 5095, section 1
*      </a>
*/
#define IPV6_PROTO_NUM_SRH          (43)

/**
 * @brief   L4 protocol number for ICMPv6.
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
 * @brief message type for notification
 *
 * @see ipv6_register_packet_handler()
 */
#define IPV6_PACKET_RECEIVED        (UPPER_LAYER_2)

/**
 * @brief   Get IPv6 send/receive buffer.
 *
 * @return  Pointer to IPv6 header in send/receive bouffer.
 * @note    To be deleted in later releases. Here only because it is
 *          used by the rpl module.
 */
ipv6_hdr_t *ipv6_get_buf(void);

/**
 * @brief   Send IPv6 packet to dest.
 *
 * @param[in] dest              Destination of this packet.
 * @param[in] next_header       Next header ID of payload.
 * @param[in] payload           Payload of the packet.
 * @param[in] payload_length    Length of payload.
 * @param[in] next_hop          Optional next_hop. May be NULL if not provided at call.
 *
 * @return  payload_length : on success
 *          -1             : if no route to the given dest could be obtained
 *                           Packet is dropped
 *                           In case of reactive routing: routing
 *                           is going to try to find a route
 */
int ipv6_sendto(const ipv6_addr_t *dest, uint8_t next_header,
                const uint8_t *payload, uint16_t payload_length, ipv6_addr_t *next_hop);

/**
 * @brief   Send an IPv6 packet defined by its header.
 *
 * @param[in] packet            Pointer to an prepared IPv6 packet header.
 *                              The payload is expected directly after the
 *                              packet.
 * @param[in] next_hop          Optional next-hop address. If NULL, than the function tries
 *                              to find it by methods of the active routing protocol.
 *
 * @return  length of payload : on success
 *          -1                : if no route to the given dest could be obtained
 *                              Packet is dropped
 *                              In case of reactive routing: routing is going
 *                              to try to find a route
 */
int ipv6_send_packet(ipv6_hdr_t *packet, ipv6_addr_t *next_hop);

/**
 * @brief   Determines if node is a router.
 *
 * @return  1 if node is router, 0 otherwise.
 */
uint8_t ipv6_is_router(void);

/**
 * @brief   Sets the default hop limit to use with IPv6 packets.
 *
 * @param[in] hop_limit The hop limit to set the default hop limit to.
 */
void ipv6_set_default_hop_limit(uint8_t hop_limit);

/**
 * @brief   Gets the default hop limit that is used for IPv6 packets.
 *
 * @return  The current default hop limit for IPv6 packets.
 */
uint8_t ipv6_get_default_hop_limit(void);

/**
 * @brief   Registers a handler thread for incoming IP packets.
 *
 * @param[in] pid   PID of handler thread.
 *
 * @return  0 on success, ENOMEN if maximum number of registrable
 *          threads is exceeded.
 */
uint8_t ipv6_register_packet_handler(kernel_pid_t pid);

/**
 * @brief   Registers a handler thread for L4 protocol.
 *
 * @param[in] next_header   Next header ID of the L4 protocol.
 * @param[in] pid           PID of the handler thread
 */
void ipv6_register_next_header_handler(uint8_t next_header, kernel_pid_t pid);

/**
 * @brief   Registers a handler thread for RPL options
 *
 * @param[in] pid   PID of the handler thread.
 */
void ipv6_register_rpl_handler(kernel_pid_t pid);

/**
 * @brief   Sets the first 64 bit of *ipv6_addr* to link local prefix.
 *
 * @param[in,out] ipv6_addr The address to set.
 */
static inline void ipv6_addr_set_link_local_prefix(ipv6_addr_t *ipv6_addr)
{
    ipv6_addr->uint32[0] = HTONL(0xfe800000);
    ipv6_addr->uint32[1] = 0;
}

/**
 * @brief   Sets IPv6 address *out* according to the remaining
 *          parameters.
 *
 * @param[out]  out     The resulting address.
 * @param[in]   addr0   The first 16 bit of the new address.
 * @param[in]   addr1   The second 16 bit of the new address.
 * @param[in]   addr2   The third 16 bit of the new address.
 * @param[in]   addr3   The fourth 16 bit of the new address.
 * @param[in]   addr4   The fifth 16 bit of the new address.
 * @param[in]   addr5   The sixth 16 bit of the new address.
 * @param[in]   addr6   The seventh 16 bit of the new address.
 * @param[in]   addr7   The eighth 16 bit of the new address.
 */
void ipv6_addr_init(ipv6_addr_t *out, uint16_t addr0, uint16_t addr1,
                    uint16_t addr2, uint16_t addr3, uint16_t addr4,
                    uint16_t addr5, uint16_t addr6, uint16_t addr7);

/**
 * @brief   Sets IPv6 address *out* using the given *prefix* and an interface's
 *          EUI-64.
 *
 *
 * @param[out]  out     Address to be set.
 * @param[in]   if_id   The interface to take the EUI-64 from.
 * @param[in]   prefix  64-bit network prefix to be used for *out*
 *                      (only the first 64 bit of the ipv6_addr_t type
 *                      are copied to *out*)
 *
 * @return  The Address to be set on success, NULL on error.
 */
ipv6_addr_t *ipv6_addr_set_by_eui64(ipv6_addr_t *out, int if_id,
                                    const ipv6_addr_t *prefix);

/**
 * @brief   Sets IPv6 address *out* with the first *bits* bit taken
 *          from *prefix* and the remaining bits to 0.
 *
 * @param[out]  out     Prefix to be set.
 * @param[in]   prefix  Address to take prefix from.
 * @param[in]   bits    Bits to be copied from *prefix* to *out*
 *                      (set to 128 when greater than 128).
 */
void ipv6_addr_init_prefix(ipv6_addr_t *out, const ipv6_addr_t *prefix,
                           uint8_t bits);

/**
 * @brief   Set *ipv6_addr* to the loopback address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @param[out] ipv6_addr    Is set to the loopback address.
 */
static inline void ipv6_addr_set_loopback_addr(ipv6_addr_t *ipv6_addr)
{
    ipv6_addr->uint32[0] = 0;
    ipv6_addr->uint32[1] = 0;
    ipv6_addr->uint32[2] = 0;
    ipv6_addr->uint32[3] = HTONL(1);
}

/**
 * @brief   Set *ipv6_addr* to a link-local all routers multicast
 *          address (ff02::/16 prefix).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @param[out] ipv6_addr    Is set to a link-local all routers multicast
 *                          address.
 */
static inline void ipv6_addr_set_all_routers_addr(ipv6_addr_t *ipv6_addr)
{
    ipv6_addr->uint32[0] = HTONL(0xff020000);
    ipv6_addr->uint32[1] = 0;
    ipv6_addr->uint32[2] = 0;
    ipv6_addr->uint32[3] = HTONL(2);
}

/**
 * @brief   Set *ipv6_addr* to a link-local all nodes multicast address
 *          (ff02::/16 prefix).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @param[out] ipv6_addr    Is set to a link-local all nodes multicast
 *                          address.
 */
static inline void ipv6_addr_set_all_nodes_addr(ipv6_addr_t *ipv6_addr)
{
    ipv6_addr->uint32[0] = HTONL(0xff020000);
    ipv6_addr->uint32[1] = 0;
    ipv6_addr->uint32[2] = 0;
    ipv6_addr->uint32[3] = HTONL(1);
}

/**
 * @brief   Set *ipv6_addr* to the all-RPL-nodes multicast address
 *          (ff02::1a).
 *
 * @see <a href="http://tools.ietf.org/html/rfc6550#section-6">
 *          RFC 6550
 *      </a>
 *
 * @param[out] ipv6_addr    Is set to the all-RPL-nodes multicast
 *                          address.
 */
static inline void ipv6_addr_set_all_rpl_nodes_addr(ipv6_addr_t *ipv6_addr)
{
    ipv6_addr->uint32[0] = HTONL(0xff020000);
    ipv6_addr->uint32[1] = 0;
    ipv6_addr->uint32[2] = 0;
    ipv6_addr->uint32[3] = HTONL(0x1a);
}

/**
 * @brief   Set *ipv6_addr_out* to the solicited-node multicast address
 *          computed from *ipv6_addr_in*.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @param[out]  ipv6_addr_out   Is set to solicited-node address of
 *                              this node.
 * @param[in]   ipv6_addr_in    The IPv6 address the solicited-node
 *                              address.
 */
static inline void ipv6_addr_set_solicited_node_addr(ipv6_addr_t *ipv6_addr_out,
        const ipv6_addr_t *ipv6_addr_in)
{
    /* copy only the last 24-bit of the ip-address that is being resolved */
    ipv6_addr_out->uint32[0] = HTONL(0xff020000);
    ipv6_addr_out->uint32[1] = 0;
    ipv6_addr_out->uint32[2] = HTONL(1);
    ipv6_addr_out->uint8[12] = 0xff;
    ipv6_addr_out->uint8[13] = ipv6_addr_in->uint8[13];
    ipv6_addr_out->uint16[7] = ipv6_addr_in->uint16[7];
}

/**
 * @brief   Converts IPv6 address into string.
 *
 * @param[out]  addr_str    The IPv6 address as string. Must allocate
 *                          at least IPV6_MAX_ADDR_STR_LEN byte (40
 *                          byte).
 * @param[in]   str_len     The maximum length available to *addr_str*.
 * @param[in]   ipv6_addr   IPv6 address to be converted.
 *
 * @return  Pointer to addr_str.
 */
static inline const char *ipv6_addr_to_str(char *addr_str, uint8_t str_len,
        const ipv6_addr_t *ipv6_addr)
{
    return inet_ntop(AF_INET6, ipv6_addr, addr_str, (size_t)str_len);
}

/**
 * @brief   Checks if two IPv6 addresses are equal.
 *
 * @param[in] a     An IPv6 address.
 * @param[in] b     Another IPv6 address.
 *
 * @return  1 if *a* and *b* are equal, 0 otherwise.
 */
static inline int ipv6_addr_is_equal(const ipv6_addr_t *a, const ipv6_addr_t *b)
{
    return (a->uint32[0] == b->uint32[0]) &&
           (a->uint32[1] == b->uint32[1]) &&
           (a->uint32[2] == b->uint32[2]) &&
           (a->uint32[3] == b->uint32[3]);
}

/**
 * @brief   Checks if *ipv6_addr* is unspecified (all zero).
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @param[in] ipv6_addr An IPv6 address.
 *
 * @return  1 if *ipv6_addr* is unspecified address, 0 otherwise.
 */
static inline int ipv6_addr_is_unspecified(const ipv6_addr_t *ipv6_addr)
{
    return (ipv6_addr->uint32[0] == 0) &&
           (ipv6_addr->uint32[1] == 0) &&
           (ipv6_addr->uint32[2] == 0) &&
           (ipv6_addr->uint32[3] == 0);
}

/**
 * @brief   Check if *ipv6_addr* is a multicast address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @param[in] ipv6_addr     An IPv6 address.
 *
 * @return  1 if *ipv6_addr* is multicast address, 0 otherwise.
 */
static inline int ipv6_addr_is_multicast(const ipv6_addr_t *ipv6_addr)
{
    return (ipv6_addr->uint8[0] == 0xff);
}

/**
 * @brief   Checks if *ipv6_addr* is a loopback address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @param[in] ipv6_addr An IPv6 address.
 *
 * @return  1 if *ipv6_addr* is loopback address, 0 otherwise.
 */
static inline int ipv6_addr_is_loopback(const ipv6_addr_t *ipv6_addr)
{
    return ipv6_addr->uint32[0] == 0 &&
           ipv6_addr->uint32[1] == 0 &&
           ipv6_addr->uint32[2] == 0 &&
           NTOHL(ipv6_addr->uint32[3]) == 1;
}

/**
 * @brief   Check if *ipv6_addr* is a link-local address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @param[in] ipv6_addr     An IPv6 address.
 *
 * @return  1 if *ipv6_addr* is link-local address, 0 otherwise.
 */
static inline int ipv6_addr_is_link_local(const ipv6_addr_t *ipv6_addr)
{
    return ((ipv6_addr->uint32[0] == HTONL(0xfe800000)) &&
            (ipv6_addr->uint32[1] == 0)) ||
           (ipv6_addr_is_multicast(ipv6_addr) &&
            (ipv6_addr->uint8[1] & 0x0f) == 2);
}

/**
 * @brief   Check if *ipv6_addr* is unique local unicast address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4193">
 *          RFC 4193
 *      </a>
 *
 * @param[in] ipv6_addr     An IPv6 address.
 *
 * @return  1 if *ipv6_addr* is unique local unicast address,
 *          0 otherwise.
 */
static inline int ipv6_addr_is_unique_local_unicast(const ipv6_addr_t *ipv6_addr)
{
    return ((ipv6_addr->uint8[0] == 0xfc) || (ipv6_addr->uint8[0] == 0xfd));
}

/**
 * @brief   Check if *ipv6_addr* is solicited-node multicast address.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4291">
 *          RFC 4291
 *      </a>
 *
 * @param[in] ipv6_addr     An IPv6 address.
 *
 * @return  1 if *ipv6_addr* is solicited-node multicast address,
 *          0 otherwise.
 */
static inline int ipv6_addr_is_solicited_node(const ipv6_addr_t *ipv6_addr)
{
    return (ipv6_addr->uint32[0] == HTONL(0xff020000)) &&
           (ipv6_addr->uint32[1] == 0) &&
           (ipv6_addr->uint32[2] == HTONL(1)) &&
           (ipv6_addr->uint8[12] == 0xff);
}

/**
 * @brief   Get pointer to potential EUI-64 bit of the IPv6 address.
 *
 * @param[in] ipv6_addr     An IPv6 address of this node.
 * @param[in] prefix_len    Length of the prefix. Only multiples of 8 are
 *                          possible.
 *
 * @return  The IID (as EUI-64) of this node.
 */
static inline net_if_eui64_t *ipv6_addr_get_iid(const ipv6_addr_t *ipv6_addr,
        uint8_t prefix_len)
{
    return ((net_if_eui64_t *) &ipv6_addr->uint8[prefix_len / 8]);
}

/*
 * TODO to wrap sixlowpan initialisations
 * int ipv6_iface_init(transceiver_type_t trans, ..);
 */

/**
 * @brief   Add an IPv6 address to one of this nodes interfaces.
 *
 * @see <a href="http://tools.ietf.org/html/rfc4862">
 *          RFC 4862
 *      </a>
 *
 * @param[in] if_id         The interface's ID.
 * @param[in] addr          Address to be added to the interface.
 * @param[in] state         Initial state of the address.
 * @param[in] val_ltime     Valid lifetime of this address in seconds. Set 0
 *                          for unspecified.
 * @param[in] pref_ltime    Preferred lifetime of this address in
 *                          seconds. Set 0 for unspecified.
 * @param[in] is_anycast    Determines if an address is anycast. Anycast
 *                          addresses are syntactically undistinguishable
 *                          from unicast addresses and can only be identified
 *                          with this flag. If *addr* is no unicast address
 *                          and *is_anycast* is set, this function will fail.
 *
 * @return 1 on success, 0 on failure.
 */
int ipv6_net_if_add_addr(int if_id, const ipv6_addr_t *addr,
                         ndp_addr_state_t state, uint32_t val_ltime,
                         uint32_t pref_ltime, uint8_t is_anycast);

/**
 * @brief   Tries to determine best suitable source address attached to
 *          an interface of this node based on the given destination
 *          address. The use-case for this function is to find a
 *          suitable address for the source address field of an IPv6
 *          address upon sending. *src* may be empty (all zero) if there
 *          is no suitable address attached to the interface.
 *
 * @param[out]  src     The best source address for this node (may be
 *                      all zero if there is none).
 * @param[in]   dest    The destination address for a packet we search
 *                      the source address for.
 */
void ipv6_net_if_get_best_src_addr(ipv6_addr_t *src, const ipv6_addr_t *dest);

/**
 * @brief   Registers a function that decides how to route incoming
 *          IP packets with a destination that is not this interface.
 *          The default behavior is to try forwarding such packets to
 *          the neighborhood.
 *          Register a function to change the default behavior.
 *          Such function shall return the next hop to reach the destination
 *          of the IP packet, or NULL if no such next hop is known.
 *          In this case, the packet will be discarded.
 *
 * @param   next_hop    function that returns the next hop to reach dest
 */
void ipv6_iface_set_routing_provider(ipv6_addr_t *(*next_hop)(ipv6_addr_t *dest));

/**
 * @brief   Registers a function that decides if a node in a RPL-network is actually the
 *          root and therefore a source routing header should be integrated.
 *          Only used in RPL non-storing mode by now.
 *
 */
void ipv6_iface_set_srh_indicator(uint8_t (*srh_indi)(void));

/**
 * @brief Calculates the IPv6 upper-layer checksum.
 *
 * @see <a href="http://tools.ietf.org/html/rfc2460#section-8.1">
 *          RFC 2460, section 8.1
 *      </a>
 * @param[in] ipv6_header   Pointer to the IPv6 header of the packet.
 * @param[in] buf           Pointer to the upper-layer payload of the IP datagram.
 * @param[in] len           The length of the upper-layer header and data.
 * @param[in] proto         Upper-layer protocol number according to RFC1700.
 *
 * @return The IPv6 upper-layer checksum.
 */
uint16_t ipv6_csum(ipv6_hdr_t *ipv6_header, uint8_t *buf, uint16_t len, uint8_t proto);

#ifdef __cplusplus
}
#endif

#endif /* SIXLOWPAN_IP_H */
/** @} */
