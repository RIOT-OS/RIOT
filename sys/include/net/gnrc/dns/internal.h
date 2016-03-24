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
 * @brief       Definitions for DNS
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef GNRC_DNS_INTERNALS_H_
#define GNRC_DNS_INTERNALS_H_

#include <errno.h>
#include <string.h>
#include <time.h>

#include "net/gnrc/dns.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/udp.h"
#include "xtimer.h"

/**
 * @brief   Enable/Disable debug trace support
 */
#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Thread ID of the DNS server
 */
extern kernel_pid_t _dns_pid;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
/**
 * @brief   Compile time byte-order swapping for short constants
 */
#define CT_HTONS(x)         ((uint16_t)(          \
                                 ((uint16_t)x >> 8) & 0x00FF) | \
                             (((uint16_t)x << 8) & 0xFF00))

/**
 * @brief   Compile time byte-order swapping for integers constants
 */
#define CT_HTONL(x)         ((uint32_t)(          \
                                 ((uint32_t)x >> 24) & 0x000000FF) | \
                             (((uint32_t)x >> 8) & 0x0000FF00) | \
                             (((uint32_t)x << 8) & 0x00FF0000) | \
                             (((uint32_t)x << 24) & 0xFF000000))
#else
/**
 * @brief   Compile time byte-order swapping for short constants
 */
#define CT_HTONS(x)    ((uint16_t)x)

/**
 * @brief   Compile time byte-order swapping for integers constants
 */
#define CT_HTONL(x)    ((uint32_t)x)
#endif

/**
 * @brief   Get the lowest value of a and b
 */
#define MIN(a, b)            ((a) > (b) ? (b) : (a))

/**
 * @brief   DNS types
 * @{
 */
typedef uint16_t dns_query_type;

#define DNS_A                   CT_HTONS(0x0001)    /**< IPv4 */
#define DNS_CNAME               CT_HTONS(0x0005)    /**< CNAME */
#define DNS_TXT                 CT_HTONS(0x0010)    /**< Text */
#define DNS_AAAA                CT_HTONS(0x001C)    /**< IPv6 */
#define DNS_SRV                 CT_HTONS(0x0021)    /**< Service Query */
/**
 * @}
 */

/**
 * @brief   DNS classes
 * @{
 */
typedef uint16_t dns_query_class;

#define DNS_SQ_IN               CT_HTONS(0x0001) /**< Service Query */
/**
 * @}
 */

/**
 * @brief   DNS flags
 * @{
 */
#define HDR_FLAGS_QR            CT_HTONS(0x8000)    /**< Query (0), Response (1)*/
#define HDR_FLAGS_AA            CT_HTONS(0x0400)    /**< Authoritative Answer. */
#define HDR_FLAGS_TC            CT_HTONS(0x0200)    /**< TrunCation. */
#define HDR_FLAGS_RD            CT_HTONS(0x0100)    /**< Recursion Desired. */
#define HDR_FLAGS_RA            CT_HTONS(0x0080)    /**< Recursion Available. */
/**
 * @}
 */

/**
 * @brief   DNS thread messages
 */
typedef enum {
    GNRC_DNS_MSG_RESPONSE_TIMEDOUT          = 0x0300,   /**< Send by the timeout timer if no response is received */
    GNRC_DNS_MSG_RESOLVE_A                  = 0x0301,   /**< Request an IPv4 address */
    GNRC_DNS_MSG_RESOLVE_AAAA               = 0x0302,   /**< Request an IPv6 address */
    GNRC_DNS_MSG_RESOLVE_TXT                = 0x0303,   /**< Request an TXT record */
    GNRC_DNS_MSG_RESOLVE_SRV                = 0x0304,   /**< Request an SRV record */
    GNRC_DNS_MSG_SERVER_LIFETIME_EXPIRED    = 0x0305,   /**< Send by the lifetime expired timer if the DNS servers aren't updated in time */
    GNRC_DNS_MSG_QUERY_RESPONSE             = 0x0306,   /**< Send by the DNS thread if a valid response has been received */
    GNRC_DNS_MSG_QUERY_TIMEDOUT             = 0x0307,   /**< Send by the DNS thread if a valid response has been received */

    GNRC_DNS_MSG_MAX
} gnrc_dns_msg;

/**
 * @brief   DNS header
 */
typedef struct __attribute__((packed)) {
    uint16_t id;                        /**< ID of the DNS request */
    uint16_t flags;                     /**< Flags associated with the DNS request */
    uint16_t qd_count;                  /**< Number of questions in the packet */
    uint16_t an_count;                  /**< Number of answers in the packet */
    uint16_t ns_count;                  /**< Number of authorities in the packet */
    uint16_t ar_count;                  /**< Number of additional records in the packet */
} dns_header_t;

/**
 * @brief   DNS question footer
 */
typedef struct __attribute__((packed)) {
    dns_query_type type;                /**< DNS Query type */
    dns_query_class class;              /**< DNS Query class */
} dns_qdata_footer_t;

/**
 * @brief   DNS answer section header
 */
typedef struct __attribute__((packed)) {
    network_uint16_t compressed;        /**< If compressed the first byte is 0xc0 */
    dns_query_type type;                /**< Type of response */
    dns_query_class val;                /**< Type of class */
    network_uint32_t ttl;               /**< Time-To-Life on this entry */
    network_uint16_t data_len;          /**< Length of the data section */
    uint8_t data[];                     /**< The data of this response entry */
} dns_query_respones_t;

/**
 * @brief   DNS SRV response
 */
typedef struct __attribute__((packed)) {
    network_uint16_t priority;          /**< Priority of the SRV entry, lower is more preferred */
    network_uint16_t weight;            /**< Weight of the SRV entry, lower weight on equal priority has preferrence */
    network_uint16_t port;              /**< Port to find the service on */
    uint8_t target[];                   /**< The domain name to find the service on */
} dns_srv_response_t;

/**
 * @brief   DNS request information used during the request
 */
typedef struct {
    uint32_t tries_left;                /**< The number of tries left on this request */
    const char *query_str;              /**< A pointer to the query string send to the server */
    kernel_pid_t request_thread;        /**< The thread requesting the DNS query */
    xtimer_t timeout_timer;             /**< The timeout timer used to catch a DNS response timeout */
    msg_t timeout_msg;                  /**< The message send by the timeout timer */
    uint16_t request_id;                /**< The request ID of this DNS query */
    dns_query_type query_type;          /**< Type of DNS query we are processing */
    gnrc_dns_response_t *response;      /**< Pointer to where the DNS response may be stored */
} gnrc_dns_request_t;

/**
 * @brief   DNS server information
 */
typedef struct {
    kernel_pid_t owner;                 /**< The owner of this DNS server entry */
    uint32_t expires;                   /**< The lifetime on the current DNS server entries */
    ipv6_addr_t addr;                   /**< DNS server address */
} gnrc_dns_server_t;

/**
 * @brief   DNS server advertise information
 */
typedef struct {
    mutex_t mutex;                                          /**< DNS mutex */
    xtimer_t ttl;                                           /**< DNS lifetime timeout timer */
    msg_t ttl_msg;                                          /**< MSG send by the TTL timeout timer */
    gnrc_dns_server_t servers[GNRC_DNS_SERVERS_SUPPORT];    /**< DNS server information */
    uint8_t count;                                          /**< The number of DNS servers available */
} gnrc_dns_servers_t;

/**
 * @brief The context of the DNS server
 */
extern gnrc_dns_servers_t _dns_context;

/**
 * @brief Calculate the length of the DNS snippet for the given request.
 *
 * @param[in] req           the request to calculate the data length of
 *
 * @returns                 the length of the packet
 */
size_t _gnrc_dns_calc_snippet_len(gnrc_dns_request_t *req);

/**
 * @brief Build an DNS request from the given type and request information and store it in buffer
 *
 * @param[in] req           the request information for this query
 * @param[out] buf          the buffer where the query snippet can be placed
 * @param[in,out] length    in: maximum length of the buffer / out: size of the DNS snippet
 *
 * @returns                 true on success, otherwise false
 */
bool _gnrc_dns_build_request(gnrc_dns_request_t *req, uint8_t *buf,
                             size_t *length);

/**
 * @brief Send an request for the given message
 *
 * @param[in] m             a message where the `content.ptr` is set to the gnrc_dns_request_t
 */
extern void _gnrc_dns_send_request(msg_t *m);

/**
 * @brief Handle the DNS request timeout and act accordingly.
 *
 * @param[in] m             the message received from the timeout callback
 * @param[in] error         the error why the query must be retried
 *
 * @returns                 true when the request is retied
 * @returns                 false when it's dropped (and must be removed from the active requests)
 */
extern bool _gnrc_dns_handle_request_retry(msg_t *m, gnrc_dns_type_t error);

/**
 * @brief Send the given DNS data to the first available DNS server
 *
 * @param[in] req           the DNS request being handled
 * @param[in] packet        data that represents the DNS packet
 * @param[in] length        the length of the DNS snippet
 */
extern void _gnrc_dns_send_packet(gnrc_dns_request_t *req, uint8_t *packet,
                                  size_t length);

/**
 * @brief Parse the domain query string (pascal string types) to .'ed zero terminated notation.
 *
 * @param[in] packet_start  pointer to the start location of the packet
 * @param[in] data          pointer to the start location of the name
 * @param[in] domain        location where the zero terminated domain name can be placed, NULL can be \
 *                          given to parse the string and find the end of the domain name in the packet
 *
 * @returns                 the first byte pointer after the domain name
 * @returns                 NULL on error
 */
extern uint8_t *_gnrc_dns_parse_name(uint8_t *packet_start, uint8_t *data, char *domain);

/**
 * @brief Update the lifetime timeout callback timer
 *
 * @param[in] min_lifetime  the time the first DNS server will expire
 */
extern void _gnrc_update_server_lifetime(uint32_t min_lifetime);

#ifdef __cplusplus
};
#endif

#endif /* GNRC_DNS_INTERNALS_H_ */

/**
 * @}
 */
