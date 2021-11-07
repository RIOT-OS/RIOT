/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_dns_msg DNS message parser and composer
 * @ingroup     net_dns
 * @brief       Parsing and composition of DNS messages
 * @{
 *
 * @file
 * @brief       Definitions for parsing and composition of DNS messages
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_DNS_MSG_H
#define NET_DNS_MSG_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_dns_msg_conf DNS message parsing and composition configuration
 * @ingroup  config
 * @{
 */
/**
 * @brief   maximum DNS message length
 */
#ifndef CONFIG_DNS_MSG_LEN
#define CONFIG_DNS_MSG_LEN         (128U)
#endif  /* CONFIG_DNS_MSG_LEN */
/** @} */

/**
 * @brief DNS internal structure
 *
 * @see [RFC 1035, section 4.1.1](https://tools.ietf.org/html/rfc1035#section-4.1.1)
 */
typedef struct __attribute__((packed)) {
    uint16_t id;        /**< identifier */
    /**
     * @brief   flags
     *
     *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     *     |QR|   Opcode  |AA|TC|RD|RA|Z |AD|CD|   RCODE   |
     *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
     *
     * - QR: Query (0) or Response (1)
     * - Opcode: Kind of query
     * - AA: Authoritative Answer
     * - TC: Truncated
     * - RD: Recursion Desired
     * - RA: Recursion Available
     * - Z: Reserved
     * - AD: Authentic Data (see [RFC 4035](https://tools.ietf.org/html/4035))
     * - CD: Checking Disabled (see [RFC 4035](https://tools.ietf.org/html/4035))
     * - RCODE: Response Code
     */
    uint16_t flags;
    uint16_t qdcount;   /**< number of question entries */
    uint16_t ancount;   /**< number of answer resource records */
    uint16_t nscount;   /**< number of name server resource records */
    uint16_t arcount;   /**< number of additional records */
    uint8_t payload[];  /**< payload */
} dns_hdr_t;

/**
 * @brief   Composes a DNS query message
 *
 * The query will request an A or AAAA IN record for @p domain_name depending on
 * @p family:
 *
 * - When @p family is `AF_INET` or `AF_UNSPEC` a query for an A record will be
 *   added
 * - When @p family is `AF_INET6` or `AF_UNSPEC` a query for an AAAA record will
 *   be added
 *
 * @param[out] dns_buf      A buffer of length @ref CONFIG_DNS_MSG_LEN
 * @param[in] domain_name   The domain name to query.
 * @param[in] id            The ID for the query.
 * @param[in] family        Either `AF_UNSPEC`, `AF_INET`, or `AF_INET6`.
 *                          Determines the address records for @p domain_name
 *                          queried.
 *
 * @return  Size of the composed query in bytes.
 */
size_t dns_msg_compose_query(void *dns_buf, const char *domain_name,
                             uint16_t id, int family);

/**
 * @brief   Parses a DNS response message
 *
 * @param[in] buf           The message to parse.
 * @param[in] len           Length of @p buf.
 * @param[in] family        The address family used to compose the query for
 *                          this response (see @ref dns_msg_compose_query())
 * @param[out] addr_out     The IP address returned by the response.
 *
 * @return  Length of the @p addr_out on success.
 * @return  -EBADMSG, when an address corresponding to @p family can not be found
 *          in @p buf.
 */
int dns_msg_parse_reply(const uint8_t *buf, size_t len, int family,
                        void *addr_out);

#ifdef __cplusplus
}
#endif

#endif /* NET_DNS_MSG_H */
/** @} */
