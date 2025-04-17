/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_sock_util   sock utility functions
 * @ingroup     net_sock
 *
 * @brief       sock utility function
 *
 * @{
 *
 * @file
 * @brief       sock utility function definitions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdbool.h>
#include <stdint.h>

#include "net/sock/config.h"

#if MODULE_GNRC_SOCK_UDP || MODULE_LWIP_SOCK_UDP || DOXYGEN
#  include "net/sock/udp.h"
#endif
#if MODULE_GNRC_SOCK_TCP || MODULE_LWIP_SOCK_TCP || DOXYGEN
#  include "net/sock/tcp.h"
#endif
#ifdef MODULE_SOCK_DTLS
#  include "net/credman.h"
#  include "net/sock/dtls.h"
#endif

#if MODULE_GNRC_SOCK_UDP || MODULE_LWIP_SOCK_UDP ||MODULE_GNRC_SOCK_TCP || MODULE_LWIP_SOCK_TCP || DOXYGEN
#  define HAVE_SOCK_TL_EP   1 /**< Indicates presence of `struct _sock_tl_ep` */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Split url to host:port and url path
 *
 * Will split e.g., "https://host.name:1234/url/path" into
 * "host.name:1234" and "/url/path".
 *
 * @note Caller has to make sure hostport and urlpath can hold the results!
 *       Make sure to provide space for @ref CONFIG_SOCK_HOSTPORT_MAXLEN respectively
 *       @ref CONFIG_SOCK_URLPATH_MAXLEN bytes, if pointers are not NULL.
 *       Scheme part of the URL is limited to @ref CONFIG_SOCK_SCHEME_MAXLEN length.
 *
 * @pre `url != NULL`
 *
 * @param[in]   url         URL to split. Must not be NULL.
 * @param[out]  hostport    where to write host:port. Can be NULL.
 * @param[out]  urlpath     where to write url path. Can be NULL.
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int sock_urlsplit(const char *url, char *hostport, char *urlpath);

/**
 * @brief   Returns a pointer to the path component in @p url
 *
 * @param[in]   url         URL to examine. Must not be NULL.
 *
 * @returns     pointer to the start of the path component in @p url
 * @returns     NULL if @p url is invalid
 */
const char *sock_urlpath(const char *url);

#if HAVE_SOCK_TL_EP
/**
 * @brief   Format common IP-based transport layer endpoint to string and port
 *
 * @param[in]   endpoint    endpoint to format
 * @param[out]  addr_str    where to write address as string
 * @param[out]  port        where to write port number as uint16_t
 *
 * @returns     number of bytes written to @p addr_str on success
 * @returns     <0 otherwise
 */
int sock_tl_ep_fmt(const struct _sock_tl_ep *endpoint,
                   char *addr_str, uint16_t *port);

/**
 * @brief   Format TCP endpoint to string and port
 *
 * @param[in]   endpoint    endpoint to format
 * @param[out]  addr_str    where to write address as string
 * @param[out]  port        where to write port number as uint16_t
 *
 * @returns     number of bytes written to @p addr_str on success
 * @returns     <0 otherwise
 */
static inline int sock_tcp_ep_fmt(const sock_tcp_ep_t *endpoint,
                                  char *addr_str, uint16_t *port)
{
    return sock_tl_ep_fmt(endpoint, addr_str, port);
}

/**
 * @brief   Format UDP endpoint to string and port
 *
 * @param[in]   endpoint    endpoint to format
 * @param[out]  addr_str    where to write address as string
 * @param[out]  port        where to write port number as uint16_t
 *
 * @returns     number of bytes written to @p addr_str on success
 * @returns     <0 otherwise
 */
static inline int sock_udp_ep_fmt(const sock_udp_ep_t *endpoint,
                                  char *addr_str, uint16_t *port)
{
    return sock_tl_ep_fmt(endpoint, addr_str, port);
}

/**
 * @brief    Convert string to common IP-based transport layer endpoint
 *
 * Takes eg., "[2001:db8::1]:1234" and converts it into the corresponding UDP
 * endpoint structure.
 *
 * @param[out]  ep_out  endpoint structure to fill
 * @param[in]   str     string to read from
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int sock_tl_str2ep(struct _sock_tl_ep *ep_out, const char *str);

/**
 * @brief   Convert string to common IP-based transport layer endpoint
 *          If the `sock_dns` module is used, this will do a DNS lookup
 *          if @p str is not an IP address.
 *
 * Takes eg., "riot-os.org:1234" and converts it into the corresponding
 * endpoint structure.
 *
 * @param[out]  ep_out  endpoint structure to fill
 * @param[in]   str     string to read from
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int sock_tl_name2ep(struct _sock_tl_ep *ep_out, const char *str);

/**
 * @brief    Convert string to TCP endpoint
 *
 * Takes eg., "[2001:db8::1]:1234" and converts it into the corresponding UDP
 * endpoint structure.
 *
 * @param[out]  ep_out  endpoint structure to fill
 * @param[in]   str     string to read from
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
static inline int sock_tcp_str2ep(sock_tcp_ep_t *ep_out, const char *str)
{
    return sock_tl_str2ep(ep_out, str);
}

/**
 * @brief    Convert string to TCP endpoint
 *           If the `sock_dns` module is used, this will do a DNS lookup
 *           if @p str is not an IP address.
 *
 * Takes eg., "exampl.com:80" or "[2001:db8::1]:1234" and converts it into
 * the corresponding UDP endpoint structure.
 *
 * @param[out]  ep_out  endpoint structure to fill
 * @param[in]   str     string to read from
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
static inline int sock_tcp_name2ep(sock_tcp_ep_t *ep_out, const char *str)
{
    return sock_tl_name2ep(ep_out, str);
}

/**
 * @brief    Convert string to UDP endpoint
 *
 * Takes eg., "[2001:db8::1]:1234" and converts it into the corresponding UDP
 * endpoint structure.
 *
 * @param[out]  ep_out  endpoint structure to fill
 * @param[in]   str     string to read from
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
static inline int sock_udp_str2ep(sock_udp_ep_t *ep_out, const char *str)
{
    return sock_tl_str2ep(ep_out, str);
}

/**
 * @brief    Convert string to UDP endpoint
 *           If the `sock_dns` module is used, this will do a DNS lookup
 *           if @p str is not an IP address.
 *
 * Takes eg., "exampl.com:80" or "[2001:db8::1]:1234" and converts it into
 * the corresponding UDP endpoint structure.
 *
 * @param[out]  ep_out  endpoint structure to fill
 * @param[in]   str     string to read from
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
static inline int sock_udp_name2ep(sock_udp_ep_t *ep_out, const char *str)
{
    return sock_tl_name2ep(ep_out, str);
}

/**
 * @brief   Compare the two given common IP-based transport layer endpoints
 *
 * The given endpoint identifiers are compared by checking their address family,
 * their addresses, and their port value.
 *
 * @param[in] a     Endpoint A
 * @param[in] b     Endpoint B
 *
 * @return  true if given endpoint identifiers point to the same destination
 * @return  false if given endpoint identifiers do not point to the same
 *          destination, or if the address family is unknown
 */
bool sock_tl_ep_equal(const struct _sock_tl_ep *a,
                      const struct _sock_tl_ep *b);

/**
 * @brief   Compare the two given TCP endpoints
 *
 * The given endpoint identifiers are compared by checking their address family,
 * their addresses, and their port value.
 *
 * @param[in] a     Endpoint A
 * @param[in] b     Endpoint B
 *
 * @return  true if given endpoint identifiers point to the same destination
 * @return  false if given endpoint identifiers do not point to the same
 *          destination, or if the address family is unknown
 */
static inline bool sock_tcp_ep_equal(const sock_tcp_ep_t *a,
                                     const sock_tcp_ep_t *b)
{
    return sock_tl_ep_equal(a, b);
}

/**
 * @brief   Compare the two given UDP endpoints
 *
 * The given endpoint identifiers are compared by checking their address family,
 * their addresses, and their port value.
 *
 * @param[in] a     Endpoint A
 * @param[in] b     Endpoint B
 *
 * @return  true if given endpoint identifiers point to the same destination
 * @return  false if given endpoint identifiers do not point to the same
 *          destination, or if the address family is unknown
 */
static inline bool sock_udp_ep_equal(const sock_udp_ep_t *a,
                                     const sock_udp_ep_t *b)
{
    return sock_tl_ep_equal(a, b);
}
#endif

#if defined(MODULE_SOCK_DTLS) || DOXYGEN
/**
 * @brief   Helper function to establish a DTLS connection
 *
 * @param[out]  sock_udp     Struct to store the underlying UDP socket
 * @param[out]  sock_dtls    Struct for the actual DTLS socket
 * @param[out]  session      Struct to store DTLS session information
 * @param[in]   tag          Credential tag to use
 * @param[in]   local        Local endpoint, must not be NULL
 * @param[in]   remote       Server endpoint to connect to
 * @param[in]   work_buf     Buffer used to negotiate connection
 * @param[in]   work_buf_len Size of @p work buf. Should be at least
 *                           160 bytes for AES_128_CCM_8 with PSK
 *
 * @return  0 on success
 * @return negative error otherwise (see @ref sock_dtls_recv_aux)
 */
int sock_dtls_establish_session(sock_udp_t *sock_udp, sock_dtls_t *sock_dtls,
                                sock_dtls_session_t *session, credman_tag_t tag,
                                sock_udp_ep_t *local, const sock_udp_ep_t *remote,
                                void *work_buf, size_t work_buf_len);
#endif

#ifdef __cplusplus
}
#endif
/** @} */
