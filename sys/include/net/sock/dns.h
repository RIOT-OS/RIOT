/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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
 * @author  Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */


#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "net/dns/msg.h"

#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_AUTO_INIT_SOCK_DNS
/**
 * @brief IP version of the address provided with CONFIG_AUTO_INIT_SOCK_DNS_SERVER_ADDR
 */
#ifndef CONFIG_AUTO_INIT_SOCK_DNS_IP_VERSION
    /* IPv6 is preferred */
    #if defined(SOCK_HAS_IPV6)
        #define CONFIG_AUTO_INIT_SOCK_DNS_IP_VERSION 6
    #elif defined(SOCK_HAS_IPV4)
        #define CONFIG_AUTO_INIT_SOCK_DNS_IP_VERSION 4
    #else
        #error "Neither IPv4 nor IPv6 included in build"
    #endif
#endif

/**
 * @brief Address of the DNS server
 */
#ifndef CONFIG_AUTO_INIT_SOCK_DNS_SERVER_ADDR
    /* Default to Quad9 Public DNS */
    #if CONFIG_AUTO_INIT_SOCK_DNS_IP_VERSION == 6
        #define CONFIG_AUTO_INIT_SOCK_DNS_SERVER_ADDR "2620:fe::fe"
    #elif CONFIG_AUTO_INIT_SOCK_DNS_IP_VERSION == 4
        #define CONFIG_AUTO_INIT_SOCK_DNS_SERVER_ADDR "9.9.9.9"
    #endif
#endif

/**
 * @brief Port of the DNS server
 */
#ifndef CONFIG_AUTO_INIT_SOCK_DNS_SERVER_PORT
#define CONFIG_AUTO_INIT_SOCK_DNS_SERVER_PORT SOCK_DNS_PORT
#endif
#endif /* MODULE_AUTO_INIT_SOCK_DNS */

/**
 * @name DNS defines
 * @{
 */
#define SOCK_DNS_PORT           (53)
#define SOCK_DNS_RETRIES        (2)

#define SOCK_DNS_MAX_NAME_LEN   (CONFIG_DNS_MSG_LEN - sizeof(dns_hdr_t) - 4)
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
 * @return      the size of the resolved address on success
 * @return      < 0 otherwise
 */
int sock_dns_query(const char *domain_name, void *addr_out, int family);

/**
 * @brief global DNS server endpoint
 */
extern sock_udp_ep_t sock_dns_server;

#ifdef __cplusplus
}
#endif

/** @} */
