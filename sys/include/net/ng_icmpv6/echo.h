/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_icmpv6_echo  ICMPv6 echo messages
 * @ingroup     net_ng_icmpv6
 * @brief       ICMPv6 echo request and reply
 * @{
 *
 * @file
 * @brief   ICMPv6 echo message definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_ICMPV6_ECHO_H_
#define NG_ICMPV6_ECHO_H_

#include <inttypes.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "net/ng_ipv6/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Echo request and response message format.
 * @extends ng_icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-4.1">
 *          RFC 4443, section 4.1
 *      </a>
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-4.2">
 *          RFC 4443, section 4.2
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint16_t id;    /**< identifier */
    network_uint16_t seq;   /**< Sequence number */
} ng_icmpv6_echo_t;

/**
 * @brief   Builds an ICMPv6 echo request for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-4.1">
 *          RFC 4443, section 4.1
 *      </a>
 *
 * @param[in] id        ID for the echo request in host byte-order
 * @param[in] seq       Sequence number for the echo request in host byte-order
 * @param[in] data      Payload for the echo request
 * @param[in] data_len  Length of @p data
 *
 * @return  The echo request message on success
 * @return  NULL, on failure
 */
ng_pktsnip_t *ng_icmpv6_echo_req_build(uint16_t id, uint16_t seq, uint8_t *data,
                                       size_t data_len);

/**
 * @brief   Builds an ICMPv6 echo reply for sending.
 *
 * @see <a href="https://tools.ietf.org/html/rfc4443#section-4.2">
 *          RFC 4443, section 4.2
 *      </a>
 *
 * @param[in] id        ID for the echo reply in host byte-order
 * @param[in] seq       Sequence number for the echo reply in host byte-order
 * @param[in] data      Payload for the echo reply
 * @param[in] data_len  Length of @p data
 *
 * @return  The echo reply message on success
 * @return  NULL, on failure
 */
ng_pktsnip_t *ng_icmpv6_echo_rep_build(uint16_t id, uint16_t seq, uint8_t *data,
                                       size_t data_len);

/**
 * @brief   ICMPv6 echo request handler
 *
 * @param[in] iface     The interface the echo requuest was received on.
 * @param[in] ipv6_hdr  The IPv6 header of the echo request.
 * @param[in] echo      The Echo Request message.
 * @param[in] len       Length of the echo request message (ng_ipv6_hdr_t::len
 *                      of @p ipv6_hdr minus length of extension headers).
 */
void ng_icmpv6_echo_req_handle(kernel_pid_t iface, ng_ipv6_hdr_t *ipv6_hdr,
                               ng_icmpv6_echo_t *echo, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* NG_ICMPV6_ECHO_H_ */
/** @} */
