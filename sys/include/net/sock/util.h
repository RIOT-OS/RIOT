/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sock_util   sock utility functions
 * @ingroup     net_sock
 *
 * @brief       sock utility function
 *
 * @{
 *
 * @file
 * @brief   sock utility function definitions
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef NET_SOCK_UTIL_H
#define NET_SOCK_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   Format UDP endpoint to string and port
 *
 * @param[in]   endpoint    endpoint to format
 * @param[out]  addr_str    where to write address as string
 * @param[out]  port        where to write prt number as uint16_t
 *
 * @returns     number of bytes written to @p addr_str on success
 * @returns     <0 otherwise
 */
int sock_udp_ep_fmt(const sock_udp_ep_t *endpoint, char *addr_str, uint16_t *port);

/**
 * @brief    Split url to host:port and url path
 *
 * Will split e.g., "https://host.name:1234/url/path" into
 * "host.name:1234" and "/url/path".
 *
 * @note Caller has to make sure hostport and urlpath can hold the results!
 *       Make sure to provide space for SOCK_HOSTPORT_MAXLEN respectively
 *       SOCK_URLPATH_MAXLEN bytes.
 *
 * @param[in]   url         URL to split
 * @param[out]  hostport    where to write host:port
 * @param[out]  urlpath     where to write url path
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int sock_urlsplit(const char *url, char *hostport, char *urlpath);

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
int sock_udp_str2ep(sock_udp_ep_t *ep_out, const char *str);

/**
 * @name helper definitions
 * @{
 */
#define SOCK_HOSTPORT_MAXLEN    (64U)   /**< maximum length of host:port part for
                                             sock_urlsplit() */
#define SOCK_URLPATH_MAXLEN     (64U)   /**< maximum length path for
                                             sock_urlsplit() */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_UTIL_H */
/** @} */
