/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_conn_ip     Raw IPv4/IPv6 connectivity
 * @ingroup     net_conn
 * @brief       Connectivity submodule for raw IPv4/IPv6 connectivity
 * @{
 *
 * @file
 * @brief   Raw IPv4/IPv6 connectivity definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_CONN_IP_H_
#define NET_CONN_IP_H_

#include <stdint.h>
#include <stdlib.h>

#include "net/conn/ep.h"

#ifdef MODULE_GNRC_CONN_IP
#include "net/gnrc/conn.h"
#endif

#ifdef MODULE_LWIP_CONN_IP
#include "lwip/conn.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Implementation-specific type of a raw IPv4/IPv6 connectivity object
 *
 * `struct conn_ip` needs to be defined by stack-specific a implementation.
 */
typedef struct conn_ip conn_ip_t;

/**
 * @brief   Creates a new raw IPv4/IPv6 connectivity object
 *
 * @pre `(conn != NULL)`
 *
 * @param[out] conn     Preallocated connectivity object. Must fill the size of
 *                      the stack-specific connectivity descriptor.
 * @param[in] local     Local end point for the connectivity object.
 *                      May be NULL to solicit implicit bind on
 *                      @ref conn_ip_sendto() and @ref conn_ip_send().
 *                      conn_ep_ip_t::netif must either be
 *                      @ref CONN_EP_ANY_NETIF or equal to conn_ep_ip_t::netif
 *                      of @p remote if `remote != NULL`.
 * @param[in] remote    Remote end point for the connectivity object.
 *                      May be `NULL` but then the `remote` parameter of
 *                      @ref conn_ip_sendto() may not be `NULL` and
 *                      @ref conn_ip_send() will always error with return value
 *                      -ENOTCONN. conn_ep_ip_t::port may not be 0 if
 *                      `remote != NULL`.
 *                      conn_ep_ip_t::netif must either be
 *                      @ref CONN_EP_ANY_NETIF or equal to conn_ep_ip_t::netif
 *                      of @p local if `local != NULL`.
 * @param[in] proto     Protocol to use in the raw IPv4/IPv6 connectivity object
 *                      (the `protocol` header field in IPv4 and the `next_header`
 *                      field in IPv6).
 *
 * @return  0 on success.
 * @return  -EADDRINUSE, if `local != NULL` and the stack reports that @p local is
 *          already use elsewhere
 * @return  -EAFNOSUPPORT, if `local != NULL` or `remote != NULL` and
 *          conn_ep_ip_t::family of @p local or @p remote is not supported.
 * @return  -EINVAL, if `proto` is not supported or if conn_ep_ip_t::netif of
 *          @p local or @p remote is not a valid interface or contradict each
 *          other (i.e. `(local->netif != remote->netif) &&
 *          ((local->netif != CONN_EP_ANY_NETIF) ||
 *          (remote->netif != CONN_EP_ANY_NETIF))` if neither is `NULL`).
 * @return  -EPROTONOSUPPORT, if `local != NULL` or `remote != NULL` and
 *          proto is not supported by conn_ep_ip_t::family of @p local or @p
 *          remote.
 */
int conn_ip_create(conn_ip_t *conn, const conn_ep_ip_t *local,
                   const conn_ep_ip_t *remote, uint8_t proto);

/**
 * @brief   Closes a raw IPv4/IPv6 connectivity
 *
 * @pre `(conn != NULL)`
 *
 * @param[in,out] conn  A raw IPv4/IPv6 connectivity object.
 */
void conn_ip_close(conn_ip_t *conn);

/**
 * @brief   Gets the local end point of a raw IPv4/IPv6 connectivity
 *
 * @pre `(conn != NULL) && (ep != NULL)`
 *
 * @param[in] conn  A raw IPv4/IPv6 connectivity object.
 * @param[out] ep   The local end point.
 *
 * @return  0 on success.
 * @return  -EADDRNOTAVAIL, when @p conn has no end point bound to it.
 */
int conn_ip_get_local(conn_ip_t *conn, conn_ep_ip_t *ep);

/**
 * @brief   Gets the remote end point of a UDP connectivity
 *
 * @pre `(conn != NULL) && (ep != NULL)`
 *
 * @param[in] conn  A UDP connectivity object.
 * @param[out] ep   The remote end point.
 *
 * @return  0 on success.
 * @return  -ENOTCONN, when @p conn has no remote end point bound to it.
 */
int conn_ip_get_remote(conn_ip_t *conn, conn_ep_ip_t *ep);

/**
 * @brief   Receives a message over IPv4/IPv6 from remote end point
 *
 * @pre `(conn != NULL) && (data != NULL) && (max_len > 0)`
 *
 * @param[in] conn      A raw IPv4/IPv6 connectivity object.
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
 * @return  -EADDRNOTAVAIL, if local of @p conn is not given.
 * @return  -ENOBUFS, if buffer space is not large enough to store received
 *          data.
 * @return  -EPROTO, if source address of received packet did not equal
 *          the remote of @p conn.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
int conn_ip_recvfrom(conn_ip_t *conn, void *data, size_t max_len,
                     uint32_t timeout, conn_ep_ip_t *remote);

/**
 * @brief   Receives a message over IPv4/IPv6
 *
 * @pre `(conn != NULL) && (data != NULL) && (max_len > 0)`
 *
 * @param[in] conn      A raw IPv4/IPv6 connectivity object.
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
 * @note    Function blocks if no packet is currently waiting.
 *
 * @return  The number of bytes received on success.
 * @return  0, if no received data is available, but everything is in order.
 * @return  -EADDRNOTAVAIL, if local of @p conn is not given.
 * @return  -ENOBUFS, if buffer space is not large enough to store received
 *          data.
 * @return  -EPROTO, if source address of received packet did not equal
 *          the remote of @p conn.
 * @return  -ETIMEDOUT, if @p timeout expired.
 */
static inline int conn_ip_recv(conn_ip_t *conn, void *data, size_t max_len,
                               uint32_t timeout)
{
    return conn_ip_recvfrom(conn, data, max_len, timeout, NULL);
}

/**
 * @brief   Sends a message over IPv4/IPv6 to remote end point
 *
 * @pre `(if (len != 0): (data != NULL))`
 *
 * @param[in] conn      A raw IPv4/IPv6 connectivity object. May be NULL.
 *                      A sensible local end point should be selected by the
 *                      stack in that case.
 * @param[in] data      Pointer where the received data should be stored.
 *                      May be `NULL` if `len == 0`.
 * @param[in] len       Maximum space available at @p data.
 * @param[in] remote    Remote end point for the send data.
 *                      May be `NULL`, if @p conn has a remote end point.
 *                      conn_ep_ip_t::family may be AF_UNSPEC, if local
 *                      end point of @p conn provides this information.
 *
 * @note    Function blocks until packet is handed to the stack.
 *
 * @return  The number of bytes send on success.
 * @return  -EAFNOSUPPORT, if `remote != NULL` and conn_ep_ip_t::family of
 *          @p remote is != AF_UNSPEC and not supported.
 * @return  -ENOMEM, if no memory was available to send @p data.
 * @return  -ENOTCONN, if `remote == NULL`, but @p conn has no remote end point.
 */
int conn_ip_sendto(conn_ip_t *conn, const void *data, size_t len,
                   const conn_ep_ip_t *remote);


/**
 * @brief   Sends a message over IPv4/IPv6 to remote end point
 *
 * @pre `(if (len != 0): (data != NULL))`
 *
 * @param[in] conn      A raw IPv4/IPv6 connectivity object. May be NULL.
 *                      A sensible local end point should be selected by the
 *                      stack in that case.
 * @param[in] data      Pointer where the received data should be stored.
 *                      May be `NULL` if `len == 0`.
 * @param[in] len       Maximum space available at @p data.
 *
 * @note    Function blocks until packet is handed to the stack.
 *
 * @return  The number of bytes send on success.
 * @return  -EINVAL, if conn_ep_ip_t::netif of @p remote is not a valid
 *          interface or contradicts the given local interface (i.e.
 *          neither the local endpoint of `conn` nor remote are assigned to
 *          `CONN_EP_ANY_NETIF` but are nevertheless different.
 * @return  -ENOMEM, if no memory was available to send @p data.
 * @return  -ENOTCONN, if @p conn has no remote end point.
 */
static inline int conn_ip_send(conn_ip_t *conn, const void *data, size_t len)
{
    return conn_ip_sendto(conn, data, len, NULL);
}

#ifdef __cplusplus
}
#endif

#endif /* NET_CONN_IP_H_ */
/** @} */
