/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_GNRC_ICMPV6_ECHO_H
#define NET_GNRC_ICMPV6_ECHO_H
/**
 * @defgroup    net_gnrc_icmpv6_echo  ICMPv6 echo messages
 * @ingroup     net_gnrc_icmpv6
 * @brief       ICMPv6 echo request and reply
 * @{
 *
 * @file
 * @brief   ICMPv6 echo message definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <inttypes.h>

#include "byteorder.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/ipv6/hdr.h"
#include "net/icmpv6.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Builds an ICMPv6 echo message of type @p type for sending.
 *
 * @param[in] type      Type of the echo message. Expected to be either
 *                      ICMPV6_ECHO_REQ or ICMPV6_ECHO_REP.
 * @param[in] id        ID for the echo message in host byte-order
 * @param[in] seq       Sequence number for the echo message in host byte-order
 * @param[in] data      Payload for the echo message
 * @param[in] data_len  Length of @p data
 *
 * @return  The echo message on success
 * @return  NULL, on failure
 */
gnrc_pktsnip_t *gnrc_icmpv6_echo_build(uint8_t type, uint16_t id, uint16_t seq,
                                       uint8_t *data, size_t data_len);

/**
 * @brief   ICMPv6 echo request handler
 *
 * @param[in] netif     The interface the echo request was received on.
 * @param[in] ipv6_hdr  The IPv6 header of the echo request.
 * @param[in] echo      The Echo Request message.
 * @param[in] len       Length of the echo request message (ipv6_hdr_t::len
 *                      of @p ipv6_hdr minus length of extension headers).
 */
void gnrc_icmpv6_echo_req_handle(gnrc_netif_t *netif, ipv6_hdr_t *ipv6_hdr,
                                 icmpv6_echo_t *echo, uint16_t len);

/**
 * @brief   Send out ICMPv6 echo request
 *
 * @param[in] netif     The interface the echo request should be sent on.
 * @param[in] addr      The destination address of the echo request
 * @param[in] id        ID for the echo message in host byte-order
 * @param[in] seq       Sequence number for the echo message in host byte-order
 * @param[in] ttl       Hop limit of the echo request
 * @param[in] len       Length of the payload
 *
 * @return  0 on success
 * @return <0 on error
 */
int gnrc_icmpv6_echo_send(const gnrc_netif_t *netif, const ipv6_addr_t *addr,
                          uint16_t id, uint16_t seq, uint8_t ttl, size_t len);

/**
 * @brief   ICMPv6 echo response callback
 *
 * @param[in] pkt       Packet containing the ICMPv6 response
 * @param[in] corrupt   Offset of corrupt payload, -1 if no corruption detected
 * @param[in] rtt_us    round-trip-time in µs (0 if this information is not available)
 * @param[in] ctx       User supplied context
 *
 * @return  0 on success
 * @return <0 on error
 */
typedef int (*gnrc_icmpv6_echo_rsp_handle_cb_t)(gnrc_pktsnip_t *pkt,
                                                int corrupt, uint32_t rtt_us, void *ctx);
/**
 * @brief   Parse ICMPv6 echo response
 *
 * @param[in] pkt       Incoming ICMPv6 packet
 * @param[in] len       Expected echo response payload length
 * @param[in] cb        Callback function to execute
 * @param[in] ctx       Callback function context
 *
 * @return  0 on success
 * @return <0 on error
 */
int gnrc_icmpv6_echo_rsp_handle(gnrc_pktsnip_t *pkt, size_t len,
                                gnrc_icmpv6_echo_rsp_handle_cb_t cb, void *ctx);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_GNRC_ICMPV6_ECHO_H */
