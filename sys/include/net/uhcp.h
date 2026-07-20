/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_uhcp UHCP
 * @ingroup     net
 * @brief       Provides UHCP (micro host configuration protocol)
 *
 * UHCP (micro host configuration protocol) is a RIOT-proprietary protocol that
 * was developed to have a simple drop-in replacement for
 * [DHCPv6 prefix delegation](@ref net_dhcpv6_client) (which was not implemented
 * when UHCP was introduced).
 *
 * If you have root access on your host machine or access to a DHCPv6 server
 * providing prefix delegation, DHCPv6 is preferred over UHCP.
 *
 * @{
 *
 * @file
 * @brief   UHCP header
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdint.h>
#include <stddef.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief UHCP magic number */
#define UHCP_MAGIC  (0x55484350) /* "UHCP" in hex */

/** @brief UHCP version of this header */
#define UHCP_VER    (0)

/** @brief UHCP port number */
#define UHCP_PORT       (12345U)

/** @brief UHCP port number (as string for e.g., getaddrinfo() service arg */
#define UHCP_PORT_STR   "12345"

/** @brief Enum containing possible UHCP packet types */
typedef enum {
    UHCP_REQ,               /**< packet is a request packet */
    UHCP_PUSH               /**< packet is a push / answer packet */
} uhcp_type_t;

/**
 * @brief UHCP packet header struct
 */
typedef struct __attribute__((packed)) {
    uint32_t uhcp_magic;    /**< always contains UHCP in hex */
    uint8_t ver_type;       /**< four bits version number, four bits
                                 packet type (see uchp_type_t) */
} uhcp_hdr_t;

/**
 * @brief struct for request packets
 *
 * @extends uhcp_hdr_t
 */
typedef struct __attribute__((packed)) {
    uhcp_hdr_t hdr;         /**< member holding parent type */
    uint8_t prefix_len;     /**< contains the requested prefix length */
} uhcp_req_t;

/**
 * @brief struct for push packets
 *
 * @extends uhcp_hdr_t
 */
typedef struct __attribute__((packed)) {
    uhcp_hdr_t hdr;         /**< member holding parent type */
    uint8_t prefix_len;     /**< contains the prefix length of assigned
                                 prefix */
    uint8_t prefix[];       /**< contains the assigned prefix */
} uhcp_push_t;

/** @brief typedef for interface handle */
typedef unsigned uhcp_iface_t;

/**
 * @brief handle incoming UDP packet
 *
 * This function should be called by UHCP server/client network code for every
 * incoming UDP packet destined to UCHP_PORT.
 *
 * @param[in]   buf     buffer containing UDP packet
 * @param[in]   len     length of @c buf
 * @param[in]   src     ptr to IPv6 source address
 * @param[in]   port    source port of packet
 * @param[in]   iface   interface number of incoming packet
 */
void uhcp_handle_udp(uint8_t *buf, size_t len, uint8_t *src, uint16_t port, uhcp_iface_t iface);

/**
 * @brief handle incoming UHCP request packet
 *
 * This function will be called by uhcp_handle_udp() for incoming request
 * packet.
 *
 * @internal
 *
 * @param[in]   req     ptr to UHCP request header
 * @param[in]   src     ptr to IPv6 source address
 * @param[in]   port    source port of packet
 * @param[in]   iface   number of interface the packet came in
 */
void uhcp_handle_req(uhcp_req_t *req, uint8_t *src, uint16_t port, uhcp_iface_t iface);

/**
 * @brief handle incoming UHCP push packet
 *
 * This function will be called by uhcp_handle_udp() for incoming push
 * packet.
 *
 * @internal
 *
 * @param[in]   req     ptr to UHCP push header
 * @param[in]   src     ptr to IPv6 source address
 * @param[in]   port    source port of packet
 * @param[in]   iface   number of interface the packet came in
 */
void uhcp_handle_push(uhcp_push_t *req, uint8_t *src, uint16_t port, uhcp_iface_t iface);

/**
 * @brief handle incoming prefix (as parsed from push packet)
 *
 * Supposed to be implemented by UHCP client implementations for the network
 * stack.
 *
 * The function might be called with an already configured prefix. In that
 * case, the lifetime *MUST* be updated.
 *
 * If the function is called with a different prefix than before, the old
 * prefix *MUST* be considered obsolete.
 *
 * @param[in]   prefix      ptr to assigned prefix
 * @param[in]   prefix_len  length of assigned prefix
 * @param[in]   lifetime    lifetime of prefix
 * @param[in]   src         ptr to IPv6 source address
 * @param[in]   iface       number of interface the packet came in
 */
void uhcp_handle_prefix(uint8_t *prefix, uint8_t prefix_len, uint16_t lifetime, uint8_t *src, uhcp_iface_t iface);

/**
 * @brief function to set constant values in UHCP header
 *
 * @internal
 *
 * @param[out]  hdr     hdr to set up
 * @param[in]   type    type of packet (request or push)
 */
static inline void uhcp_hdr_set(uhcp_hdr_t *hdr, uhcp_type_t type)
{
    hdr->uhcp_magic = htonl(UHCP_MAGIC);
    hdr->ver_type = (UHCP_VER << 4) | (type & 0xF);
}

/**
 * @brief UDP send function used by UHCP client / server
 *
 * Supposed to be implemented by UHCP clients.
 *
 * @param[in]   buf         buffer to send
 * @param[in]   len         length of buf
 * @param[in]   dst         ptr to IPv6 destination address
 * @param[in]   dst_port    destination port
 * @param[in]   dst_iface   interface number of destination interface
 */
int udp_sendto(uint8_t *buf, size_t len, uint8_t *dst, uint16_t dst_port, uhcp_iface_t dst_iface);

#ifdef __cplusplus
}
#endif

/** @} */
