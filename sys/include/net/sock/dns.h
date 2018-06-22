/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sock_dns    DNS sock API
 * @ingroup     net_sock
 *
 * @brief       Sock DNS client
 *
 * @{
 *
 * @file
 * @brief   DNS sock definitions
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef NET_SOCK_DNS_H
#define NET_SOCK_DNS_H

#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DNS internal structure
 */
typedef struct {
    uint16_t id;        /**< read           */
    uint16_t flags;     /**< DNS            */
    uint16_t qdcount;   /**< RFC            */
    uint16_t ancount;   /**< for            */
    uint16_t nscount;   /**< detailed       */
    uint16_t arcount;   /**< explanations   */
    uint8_t payload[];  /**< !!             */
} sock_dns_hdr_t;

/**
 * @name DNS defines
 * @{
 */
#define DNS_TYPE_A              (1)
#define DNS_TYPE_AAAA           (28)
#define DNS_CLASS_IN            (1)

#define SOCK_DNS_PORT           (53)
#define SOCK_DNS_RETRIES        (2)

#define SOCK_DNS_MAX_NAME_LEN   (64U)       /* we're in embedded context. */
#define SOCK_DNS_QUERYBUF_LEN   (sizeof(sock_dns_hdr_t) + 4 + SOCK_DNS_MAX_NAME_LEN)
/** @} */

/**
 * @brief Get IP address for DNS name
 *
 * This function will synchronously try to resolve a DNS A or AAAA record by contacting
 * the DNS server specified in the global variable @ref sock_dns_server.
 *
 * By supplying AF_INET, AF_INET6 or AF_UNSPEC in @p family requesting of A
 * records (IPv4), AAAA records (IPv6) or both can be selected.
 *
 * This function will return the first DNS record it receives. IF both A and
 * AAAA are requested, AAAA will be preferred.
 *
 * @note @p addr_out needs to provide space for any possible result!
 *       (4byte when family==AF_INET, 16byte otherwise)
 *
 * @param[in]   domain_name     DNS name to resolve into address
 * @param[out]  addr_out        buffer to write result into
 * @param[in]   family          Either AF_INET, AF_INET6 or AF_UNSPEC
 *
 * @return      0 on success
 * @return      !=0 otherwise
 */
int sock_dns_query(const char *domain_name, void *addr_out, int family);

/**
 * @brief global DNS server endpoint
 */
extern sock_udp_ep_t sock_dns_server;

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_DNS_H */
/** @} */
