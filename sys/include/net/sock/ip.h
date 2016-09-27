/*
 * Copyright (C) 2016 Alexander Aring <aar@pengutronix.de>
 *                    Freie Universität Berlin
 *                    HAW Hamburg
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sock_ip     Raw IPv4/IPv6 sock API
 * @ingroup     net_sock
 * @brief       Sock Submodule for raw IPv4/IPv6
 * @{
 *
 * @file
 * @brief   Raw IPv4/IPv6 sock definitions
 *
 * @author  Alexander Aring <aar@pengutronix.de>
 * @author  Simon Brummer <simon.brummer@haw-hamburg.de>
 * @author  Cenk Gündoğan <mail@cgundogan.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef NET_SOCK_IP_H_
#define NET_SOCK_IP_H_

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

#include "net/sock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Implementation-specific type of a raw IPv4/IPv6 sock object
 *
 * `struct sock_ip` needs to be defined by stack-specific `sock_types.h`.
 */
typedef struct sock_ip sock_ip_t;

/**
 * @brief   Creates a new raw IPv4/IPv6 sock object
 *
 * @pre `(sock != NULL)`
 *
 * @param[out] sock     The resulting sock object.
 * @param[in] local     Local end point for the sock object.
 *                      May be NULL to solicit implicit bind on
 *                      @ref sock_ip_send().
 *                      sock_ip_ep_t::netif must either be
 *                      @ref SOCK_ADDR_ANY_NETIF or equal to sock_ip_ep_t::netif
 *                      of @p remote if `remote != NULL`.
 * @param[in] remote    Remote end point for the sock object.
 *                      May be `NULL` but then the `remote` parameter of
 *                      @ref sock_ip_send() may not be `NULL` or it will always
 *                      error with return value -ENOTCONN.
 *                      sock_ip_ep_t::port may not be 0 if `remote != NULL`.
 *                      sock_ip_ep_t::netif must either be
 *                      @ref SOCK_ADDR_ANY_NETIF or equal to sock_ip_ep_t::netif
 *                      of @p local if `local != NULL`.
 * @param[in] proto     Protocol to use in the raw IPv4/IPv6 sock object
 *                      (the `protocol` header field in IPv4 and the `next_header`
 *                      field in IPv6).
 * @param[in] flags     Flags for the sock object. See also @ref net_sock_flags.
 *                      May be 0.
 *
 * @return  0 on success.
 * @return  -EADDRINUSE, if `local != NULL` and the stack reports that @p local
 *          is already used elsewhere
 * @return  -EAFNOSUPPORT, if `local != NULL` or `remote != NULL` and
 *          sock_ip_ep_t::family of @p local or @p remote is not supported.
 * @return  -EINVAL, if `proto` is not supported or if sock_ip_ep_t::netif of
 *          @p local or @p remote is not a valid interface or contradict each
 *          other (i.e. `(local->netif != remote->netif) &&
 *          ((local->netif != SOCK_ADDR_ANY_NETIF) ||
 *          (remote->netif != SOCK_ADDR_ANY_NETIF))` if neither is `NULL`).
 * @return  -ENOMEM, if the stack can't provide enough resources for `sock` to
 *          be created.
 * @return  -EPROTONOSUPPORT, if `local != NULL` or `remote != NULL` and
 *          proto is not supported by sock_ip_ep_t::family of @p local or @p
 *          remote.
 */
int sock_ip_create(sock_ip_t *sock, const sock_ip_ep_t *local,
                   const sock_ip_ep_t *remote, uint8_t proto, uint16_t flags);

/**
 * @brief   Closes a raw IPv4/IPv6 sock object
 *
 * @pre `(sock != NULL)`
 *
 * @param[in] sock  A raw IPv4/IPv6 sock object.
 */
void sock_ip_close(sock_ip_t *sock);

/**
 * @brief   Gets the local end point of a raw IPv4/IPv6 sock object
 *
 * @pre `(sock != NULL) && (ep != NULL)`
 *
 * @param[in] sock  A raw IPv4/IPv6 sock object.
 * @param[out] ep   The local end point.
 *
 * @return  0 on success.
 * @return  -EADDRNOTAVAIL, when @p sock has no end point bound to it.
 */
int sock_ip_get_local(sock_ip_t *sock, sock_ip_ep_t *ep);

/**
 * @brief   Gets the remote end point of a UDP sock object
 *
 * @pre `(sock != NULL) && (ep != NULL)`
 *
 * @param[in] sock  A UDP sock object.
 * @param[out] ep   The remote end point.
 *
 * @return  0 on success.
 * @return  -ENOTCONN, when @p sock has no remote end point bound to it.
 */
int sock_ip_get_remote(sock_ip_t *sock, sock_ip_ep_t *ep);

/**
 * @brief   Receives a message over IPv4/IPv6 from remote end point
 *
 * @pre `(sock != NULL) && (data != NULL) && (max_len > 0)`
 *
 * @param[in] sock      A raw IPv4/IPv6 sock object.
 * @param[out] data     Pointer where the received data should be stored.
 * @param[in] max_len   Maximum space available at @p data.
 *                      If received data exceeds @p max_len the data is
 *                      truncated and the remaining data can be retrieved
 *                      later on.
 * @param[in] timeout   Timeout for receive in microseconds.
 *                      This value can be ignored (no timeout) if the
 *                      @ref sys_xtimer module is not present and the stack does
 *                      not support timeouts on its own.
 *                      May be 0 for no timeout.
 * @param[out] remote   Remote end point of the received data.
 *                      May be NULL, if it is not required by the application.
 *
 * @note    Function blocks if no packet is currently waiting.
 *
 * @return  The number of bytes received on success.
 * @return  0, if no received data is available, but everything is in order.
 * @return  -EADDRNOTAVAIL, if local of @p sock is not given.
 * @return  -ENOBUFS, if buffer space is not large enough to store received
 *          data.
 * @return  -ENOMEM, if no memory was available to receive @p data.
 * @return  -EPROTO, if source address of received packet did not equal
 *          the remote of @p sock.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
ssize_t sock_ip_recv(sock_ip_t *sock, void *data, size_t max_len,
                     uint32_t timeout, sock_ip_ep_t *remote);

/**
 * @brief   Sends a message over IPv4/IPv6 to remote end point
 *
 * @pre `((sock != NULL || remote != NULL)) && (if (len != 0): (data != NULL))`
 *
 * @param[in] sock      A raw IPv4/IPv6 sock object. May be NULL.
 *                      A sensible local end point should be selected by the
 *                      stack in that case.
 * @param[in] data      Pointer where the received data should be stored.
 *                      May be `NULL` if `len == 0`.
 * @param[in] len       Maximum space available at @p data.
 * @param[in] proto     Protocol to use in the packet send, in case
 *                      `sock == NULL`. If `sock != NULL` this parameter will be
 *                      ignored.
 * @param[in] remote    Remote end point for the send data.
 *                      May be `NULL`, if @p sock has a remote end point.
 *                      sock_ip_ep_t::family may be AF_UNSPEC, if local
 *                      end point of @p sock provides this information.
 *
 * @note    Function blocks until packet is handed to the stack.
 *
 * @return  The number of bytes send on success.
 * @return  -EAFNOSUPPORT, if `remote != NULL` and sock_ip_ep_t::family of
 *          @p remote is != AF_UNSPEC and not supported.
 * @return  -EHOSTUNREACH, if @p remote or remote end point of @p sock is not
 *          reachable.
 * @return  -ENOMEM, if no memory was available to send @p data.
 * @return  -ENOTCONN, if `remote == NULL`, but @p sock has no remote end point.
 * @return  -EPROTOTYPE, if `sock == NULL` and @p proto is not by
 *          sock_ip_ep_t::family of @p remote.
 */
ssize_t sock_ip_send(sock_ip_t *sock, const void *data, size_t len,
                     uint8_t proto, const sock_ip_ep_t *remote);

#include "sock_types.h"

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_IP_H_ */
/** @} */
