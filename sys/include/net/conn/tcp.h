/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_conn_tcp    TCP connectivity
 * @ingroup     net_conn
 * @brief       Connectivity submodule for TCP connectivity
 * @{
 *
 * @file
 * @brief   TCP connectivity definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_CONN_TCP_H_
#define NET_CONN_TCP_H_

#include <stdint.h>
#include <stdlib.h>

#include "net/conn/ep.h"

#ifdef MODULE_GNRC_CONN_TCP
#include "net/gnrc/conn.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Implementation-specific type of a TCP connectivity object
 *
 * `struct conn_tcp` needs to be defined by stack-specific implementation.
 */
typedef struct conn_tcp conn_tcp_t;

/**
 * @brief   Creates a new TCP connectivity object
 *
 * @pre `(conn != NULL)`
 * @pre `(local == NULL) || (local->port != 0)`
 *
 * @param[out] conn     Preallocated connectivity object. Must fill the size of
 *                      the stack-specific connectivity descriptor.
 * @param[in] local     Local end point for the connectivity object.
 *                      May be NULL to solicit implicit bind when @p remote is
 *                      not NULL.
 *                      conn_ep_tcp_t::netif must either be
 *                      @ref CONN_EP_ANY_NETIF or equal to conn_ep_tcp_t::netif
 *                      of @p remote if `remote != NULL`.
 * @param[in] remote    Remote end point for the connectivity object.
 *                      May be NULL. In this case the connectivity object is a
 *                      listening connectivity object. Call
 *                      @ref conn_tcp_accept() to wait for incoming connection
 *                      requests.
 *                      conn_ep_tcp_t::netif must either be
 *                      @ref CONN_EP_ANY_NETIF or equal to conn_ep_tcp_t::netif
 *                      of @p local if `local != NULL`.
 *
 * @return  0 on success.
 * @return  -EADDRINUSE, if `local != NULL` and the stack reports that @p local
 *          is already use elsewhere
 * @return  -EAFNOSUPPORT, if `local != NULL` or `remote != NULL` and
 *          conn_ep_tcp_t::family of @p local or @p remote is not supported.
 * @return  -ECONNREFUSED, if `remote != NULL`, but no-one is listening on this
 *          address.
 * @return  -EINVAL, if conn_ep_tcp_t::netif of @p local or @p remote is not a
 *          valid interface or contradict each other (i.e.
 *          `(local->netif != remote->netif) &&
 *          ((local->netif != CONN_EP_ANY_NETIF) ||
 *          (remote->netif != CONN_EP_ANY_NETIF))` if neither is `NULL`).
 * @return  -ENETUNREACH, if `remote != NULL`, but network is not reachable.
 * @return  -ENOMEM, if system was not able to allocate sufficient memory to
 *          establish connection.
 * @return  -EPERM, if connections to @p remote are not permitted on the system
 *          (e.g. by firewall rules).
 * @return  -ETIMEDOUT, if `remote != NULL`, but the connection attempt timed
 *          out.
 */
int conn_tcp_create(conn_tcp_t *conn, const conn_ep_tcp_t *local,
                    const conn_ep_tcp_t *remote);

/**
 * @brief   Closes a TCP connection
 *
 * @pre `(conn != NULL)`
 *
 * @param[in,out] conn  A TCP connectivity object.
 */
void conn_tcp_close(conn_tcp_t *conn);

/**
 * @brief   Gets the local end point of a TCP connectivity
 *
 * @pre `(conn != NULL) && (ep != NULL)`
 *
 * @param[in] conn  A TCP connectivity object.
 * @param[out] ep   The local end point.
 *
 * @return  0 on success.
 * @return  -EADDRNOTAVAIL, when @p conn has no local end point.
 */
int conn_tcp_get_local(conn_tcp_t *conn, conn_ep_tcp_t *ep);

/**
 * @brief   Gets the remote end point of a TCP connectivity
 *
 * @pre `(conn != NULL) && (ep != NULL)`
 *
 * @param[in] conn  A TCP connectivity object.
 * @param[out] ep   The remote end point.
 *
 * @return  0 on success.
 * @return  -ENOTCONN, when @p conn is not connected to a remote end point.
 */
int conn_tcp_get_remote(conn_tcp_t *conn, conn_ep_tcp_t *ep);

/**
 * @brief   Receives and handles TCP connection requests from other peers
 *
 * @pre `(conn != NULL) && (out_conn != NULL)`
 *
 * @param[in] conn      A TCP connectivity object.
 * @param[out] out_conn A new TCP connectivity object for the established
 *                      connectivity.
 *
 * @return  0 on success.
 * @return  -EINVAL, if @p conn is not listening (`remote != NULL` on
 *          `conn_tcp_create()`).
 * @return  -ENOMEM, if system was not able to allocate sufficient memory to
 *          establish connection.
 * @return  -EPERM, if connections on local endpoint of @p conn are not
 *          permitted on this system (e.g. by firewall rules).
 * @return  -ETIMEDOUT, if the operation timed out stack-internally.
 */
int conn_tcp_accept(conn_tcp_t *conn, conn_tcp_t *out_conn);

/**
 * @brief   Receives a TCP message
 *
 * @pre `(conn != NULL) && (data != NULL) && (max_len > 0)`
 *
 * @param[in] conn      A TCP connectivity object.
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
 *
 * @note    Function may block.
 *
 * @return  The number of bytes received on success.
 * @return  0, if no received data is available, but everything is in order.
 * @return  -ECONNREFUSED, if remote end point of @p conn refused to allow the
 *          connection.
 * @return  -ENOTCONN, when @p conn is not connected to a remote end point.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
int conn_tcp_recv(conn_tcp_t *conn, void *data, size_t max_len,
                  uint32_t timeout);

/**
 * @brief   Sends a TCP message
 *
 * @pre `(conn != NULL) && (data != NULL) && (max > 0)`
 *
 * @param[in] conn  A TCP connectivity object.
 * @param[in] data  Pointer where the received data should be stored.
 * @param[in] len   Maximum space available at @p data.
 *
 * @note    Function may block.
 *
 * @return  The number of bytes send on success.
 * @return  -ECONNRESET, if connection was reset by remote end point.
 * @return  -ENOBUFS, if no memory was available to send @p data.
 * @return  -ENOTCONN, if @p conn is not connected to a remote end point.
 */
int conn_tcp_send(conn_tcp_t *conn, const void *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* NET_CONN_TCP_H_ */
/** @} */
