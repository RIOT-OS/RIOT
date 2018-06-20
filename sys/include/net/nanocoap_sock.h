/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap
 *
 * @{
 *
 * @file
 * @brief       nanocoap high-level API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef NET_NANOCOAP_SOCK_H
#define NET_NANOCOAP_SOCK_H

#include <stdint.h>
#include <unistd.h>

#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Start a nanocoap server instance
 *
 * This function only returns if there's an error binding to @p local, or if
 * receiving of UDP packets fails.
 *
 * @param[in]   local   local UDP endpoint to bind to
 * @param[in]   buf     input buffer to use
 * @param[in]   bufsize size of @p buf
 *
 * @returns     -1 on error
 */
int nanocoap_server(sock_udp_ep_t *local, uint8_t *buf, size_t bufsize);

/**
 * @brief   Simple synchronous CoAP get
 *
 * @param[in]   remote  remote UDP endpoint
 * @param[in]   path    remote path
 * @param[out]  buf     buffer to write response to
 * @param[in]   len     length of @p buffer
 *
 * @returns     length of response payload on success
 * @returns     <0 on error
 */
ssize_t nanocoap_get(sock_udp_ep_t *remote, const char *path, uint8_t *buf,
                     size_t len);

/**
 * @brief   Simple synchronous CoAP request
 *
 * @param[in,out]   pkt     Packet struct containing the request. Is reused for
 *                          the response
 * @param[in]       local   Local UDP endpoint, may be NULL
 * @param[in]       remote  remote UDP endpoint
 * @param[in]       len     Total length of the buffer associated with the
 *                          request
 *
 * @returns     length of response on success
 * @returns     <0 on error
 */
ssize_t nanocoap_request(coap_pkt_t *pkt, sock_udp_ep_t *local,
                         sock_udp_ep_t *remote, size_t len);

#ifdef __cplusplus
}
#endif
#endif /* NET_NANOCOAP_SOCK_H */
/** @} */
