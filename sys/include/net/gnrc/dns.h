/*
 * Copyright (C) 2015 Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_dns DNS
 * @ingroup     net
 * @brief       Provides DNS header and helper functions
 * @{
 *
 * @file
 * @brief       DNS client implementation
 *
 * The DNS module get's it's DNS server configuration from the NDP
 * Router Advertisement message. The implementation loosely follows the
 * following RFC's:
 *
 *  - https://www.ietf.org/rfc/rfc1035.txt
 *     (RFC1035 Domain Names - Implementation and Specifications)
 *       - Only implemented A, CNAME, TXT records querying
 *       - No cache support
 *  - https://www.rfc-editor.org/rfc/rfc2782.txt
 *     (RFC2782 A DNS RR for specifying the location of services)
 *       - Fully
 *  - https://www.ietf.org/rfc/rfc3596.txt
 *     (RFC3596 DNS Extensions to Support IPv6)
 *       - Fully
 *  - https://tools.ietf.org/html/rfc6106
 *     (RFC6106 IPv6 Router Advertisement Options for DNS Configuration)
 *       - Only RDNSS, no DNSSL
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef GNRC_DNS_H_
#define GNRC_DNS_H_

#include <stdint.h>

#include "net/gnrc/ipv6/netif.h"
#include "net/ipv6/addr.h"
#include "net/ipv4/addr.h"
#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def GNRC_DNS_SERVERS_SUPPORT
 *
 * @brief   Number of DNS servers that are supported per interface
 */
#ifndef GNRC_DNS_SERVERS_SUPPORT
#define GNRC_DNS_SERVERS_SUPPORT             (3)
#endif

/**
 * @def GNRC_DNS_MSG_QUEUE_SIZE
 *
 * @brief   Default message queue size for the DNS thread
 */
#ifndef GNRC_DNS_MSG_QUEUE_SIZE
#define GNRC_DNS_MSG_QUEUE_SIZE              (8U)
#endif

/**
 * @def GNRC_DNS_PRIO
 *
 * @brief   Priority of the DNS thread
 */
#ifndef GNRC_DNS_PRIO
#define GNRC_DNS_PRIO                        (THREAD_PRIORITY_MAIN - 2)
#endif

/**
 * @def GNRC_DNS_STACK_SIZE
 *
 * @brief   Default stack size to use for the DNS thread
 */
#ifndef GNRC_DNS_STACK_SIZE
#define GNRC_DNS_STACK_SIZE                  (THREAD_STACKSIZE_DEFAULT + 1024)
#endif

/**
 * @def GNRC_DNS_RESOLVE_SRC_PORT
 *
 * @brief   Default DNS send request port
 */
#ifndef GNRC_DNS_RESOLVE_SRC_PORT
#define GNRC_DNS_RESOLVE_SRC_PORT            (53053)
#endif

/**
 * @def GNRC_DNS_RESOLVE_DST_PORT
 *
 * @brief   Default DNS send request port
 */
#ifndef GNRC_DNS_RESOLVE_DST_PORT
#define GNRC_DNS_RESOLVE_DST_PORT            (53)
#endif

/**
 * @def GNRC_DNS_MAX_QUERY_STR_LEN
 *
 * @brief   The maximum length of the DNS query string
 */
#ifndef GNRC_DNS_MAX_QUERY_STR_LEN
#define GNRC_DNS_MAX_QUERY_STR_LEN           (100)
#endif

/**
 * @def GNRC_DNS_MAX_RESPONSE_ADDRESSES
 *
 * @brief   The maximum number of IP address to be processed
 *          from the response
 */
#ifndef GNRC_DNS_MAX_RESPONSE_ADDRESSES
#define GNRC_DNS_MAX_RESPONSE_ADDRESSES      (6)
#endif

/**
 * @def GNRC_DNS_MAX_SIMULTANEOUS_REQUESTS
 *
 * @brief   The maximum number of simultaneous DNS requests
 */
#ifndef GNRC_DNS_MAX_SIMULTANEOUS_REQUESTS
#define GNRC_DNS_MAX_SIMULTANEOUS_REQUESTS   (3)
#endif

/**
 * @def GNRC_DNS_MAX_RETRIES
 *
 * @brief   The maximum number of DNS retries that should be made
 */
#ifndef GNRC_DNS_MAX_RETRIES
#define GNRC_DNS_MAX_RETRIES                 (3)
#endif

/**
 * @def GNRC_DNS_MAX_SRV_RECORDS_SUPPORTED
 *
 * @brief   The maximum number of SRV records there are placed in the DNS response struct
 */
#ifndef GNRC_DNS_MAX_SRV_RECORDS_SUPPORTED
#define GNRC_DNS_MAX_SRV_RECORDS_SUPPORTED  (5)
#endif

/**
 * @def GNRC_DNS_RESPONSE_TIMEOUT
 *
 * @brief   The maximum milliseconds the DNS client must wait for a response
 */
#ifndef GNRC_DNS_RESPONSE_TIMEOUT
#define GNRC_DNS_RESPONSE_TIMEOUT            (2000)
#endif

/**
 * @brief   Request types and error responses
 */
typedef enum {
    GNRC_DNS_TYPE_A,
    GNRC_DNS_TYPE_AAAA,
    GNRC_DNS_TYPE_TXT,
    GNRC_DNS_TYPE_SRV,
    GNRC_DNS_ERROR_TIMEDOUT = 0x80,
    GNRC_DNS_ERROR_NO_ENTRY = 0x90,
    GNRC_DNS_ERROR_MSG      = 0xa0,

    GNRC_DNS_ERROR_MASK     = 0xf0,
} gnrc_dns_type_t;

/**
 * @brief   DNS response structure
 */
typedef struct {
    char full_name[GNRC_DNS_MAX_QUERY_STR_LEN];                 /**< FQDN of the request */
    gnrc_dns_type_t type;                                       /**< type which is stored in the data element */
    uint8_t responses;                                          /**< number of responses in the data element */
    union {
        ipv6_addr_t aaaa[GNRC_DNS_MAX_RESPONSE_ADDRESSES];      /**< array of IPv6 addresses */
        ipv4_addr_t a[GNRC_DNS_MAX_RESPONSE_ADDRESSES];         /**< array of IPv4 addresses */
        char txt[GNRC_DNS_MAX_QUERY_STR_LEN];                   /**< TXT record data */
        struct {
            char target[GNRC_DNS_MAX_QUERY_STR_LEN];            /**< FQDN of the server */
            uint16_t priority;                                  /**< priority of the server */
            uint16_t weight;                                    /**< weight of the server */
            uint16_t port;                                      /**< port on the server */
        } srv[GNRC_DNS_MAX_SRV_RECORDS_SUPPORTED];              /**< server responses */
    } data;                                                     /**< data of the DNS response */
} gnrc_dns_response_t;

/**
 * @brief   DNS server information
 */
typedef struct {
    uint32_t ttl;                                               /**< the time to live of the server */
    ipv6_addr_t ip;                                             /**< the IPv6 address of the server */
} gnrc_dns_server_info_t;

/**
 * @brief   Initialize the DNS service
 *
 * @return  PID of the DNS thread
 * @return  negative value on error
 */
int gnrc_dns_init(void);

/**
 * @brief   Update IP addresses of the DNS server list
 *
 * @param[in] lifetime      lifetime the DNS server entries
 * @param[in] items         number of items in the servers array
 * @param[in] servers       array of DNS servers available
 *
 * @return                  the number of DNS servers stored
 * @return                  -EINVAL, when on of the arguments was not valid
 */
int gnrc_dns_update_entries(uint32_t lifetime, uint8_t items,
                            ipv6_addr_t *servers);

/**
 * @brief   Add the given IP address to the DNS server list
 *
 * @param[in] server        The DNS server to add to the DNS server list
 *
 * @return                  0 on success
 * @return                  -EINVAL, when on of the arguments was not valid
 */
int gnrc_dns_add_server(ipv6_addr_t *server);

/**
 * @brief   Remove the given IP address from the DNS server list
 *
 * @param[in] server        The DNS server to remove from the DNS server list
 *
 * @return                  0 on success
 * @return                  -EINVAL, when on of the arguments was not valid
 */
int gnrc_dns_del_server(ipv6_addr_t *server);

/**
 * @brief   Get the information of the given DNS server
 *
 * @param[in] index         the index of the server to get the info of
 * @param[out] info         DNS server info will be stored here
 *
 * @return                  true if the server index exists and the info is filled
 * @returns                 false if the server index doesn't exist
 */
bool gnrc_dns_get_server(int index, gnrc_dns_server_info_t *info);

/**
 * @brief   Resolve the given URL to the given request type and place the response in response
 *
 * @param[in] url           domain name to request
 * @param[in] request       request type to make (DNS_A, DNS_AAAA, etc.)
 * @param[out] response     location where the response can be stored
 *
 * @returns                 true if the the request was successful
 * @returns                 false if an error occurred
 */
bool gnrc_dns_resolve(const char *url, gnrc_dns_type_t request,
                      gnrc_dns_response_t *response);

#endif /* GNRC_DNS_H_ */

/**
 * @}
 */
