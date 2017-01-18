/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_conn_tcp    TCP connections
 * @ingroup     net_conn
 * @deprecated  Please use @ref net_sock_tcp instead
 * @brief       Connection submodule for TCP connections
 * @{
 *
 * @file
 * @brief   TCP connection definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_CONN_TCP_H
#define NET_CONN_TCP_H

#include <stdint.h>
#include <stdlib.h>

#ifdef MODULE_GNRC_CONN_TCP
#include "net/gnrc/conn.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Forward declaration of @ref conn_tcp_t to allow for external definition.
 */
struct conn_tcp;

/**
 * @brief   Implementation-specific type of a TCP connection object
 */
typedef struct conn_tcp conn_tcp_t;

/**
 * @brief   Creates a new TCP connection object
 *
 * @param[out] conn     Preallocated connection object. Must fill the size of the stack-specific
 *                      connection desriptor.
 * @param[in] addr      The local network layer address for @p conn.
 * @param[in] addr_len  The length of @p addr. Must be fitting for the @p family.
 * @param[in] family    The family of @p addr (see @ref net_af).
 * @param[in] port      The local TCP port for @p conn.
 *
 * @return  0 on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' bind() function specification.
 */
int conn_tcp_create(conn_tcp_t *conn, const void *addr, size_t addr_len, int family,
                    uint16_t port);

/**
 * @brief   Closes a TCP connection
 *
 * @param[in,out] conn  A TCP connection object.
 */
void conn_tcp_close(conn_tcp_t *conn);

/**
 * @brief   Gets the local address of a TCP connection
 *
 * @param[in] conn  A TCP connection object.
 * @param[out] addr The local network layer address. Must have space for any address of
 *                  the connection's family.
 * @param[out] port The local TCP port.
 *
 * @return  length of @p addr on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' getsockname() function
 *          specification.
 */
int conn_tcp_getlocaladdr(conn_tcp_t *conn, void *addr, uint16_t *port);

/**
 * @brief   Gets the address of the connected peer of a TCP connection
 *
 * @param[in] conn  A TCP connection object.
 * @param[out] addr The network layer address of the connected peer. Must have space for any
 *                  address of the connection's family.
 * @param[out] port The TCP port of the connected peer.
 *
 * @return  length of @p addr on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' getpeername() function
 *          specification.
 */
int conn_tcp_getpeeraddr(conn_tcp_t *conn, void *addr, uint16_t *port);

/**
 * @brief   Connects to a remote TCP peer
 *
 * @param[in] conn      A TCP connection object.
 * @param[in] addr      The remote network layer address for @p conn.
 * @param[in] addr_len  Length of @p addr.
 * @param[in] port      The remote TCP port for @p conn.
 *
 * @return  0 on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' connect() function specification.
 */
int conn_tcp_connect(conn_tcp_t *conn, const void *addr, size_t addr_len, uint16_t port);

/**
 * @brief   Marks connection to listen for a connection request by a remote TCP peer
 *
 * @param[in] conn      A TCP connection object.
 * @param[in] queue_len Maximum length of the queue for connection requests.
 *                      An implementation may choose to silently adapt this value to its needs
 *                      (setting it to a minimum or maximum value). Any negative number must be
 *                      set at least to 0.
 *
 * @return  0 on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' listen() function specification.
 */
int conn_tcp_listen(conn_tcp_t *conn, int queue_len);

/**
 * @brief   Receives and handles TCP connection requests from other peers
 *
 * @param[in] conn      A TCP connection object.
 * @param[out] out_conn A new TCP connection object for the established connection.
 *
 * @return  0 on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' accept() function specification.
 */
int conn_tcp_accept(conn_tcp_t *conn, conn_tcp_t *out_conn);

/**
 * @brief   Receives a TCP message
 *
 * @param[in] conn      A TCP connection object.
 * @param[out] data     Pointer where the received data should be stored.
 * @param[in] max_len   Maximum space available at @p data.
 *
 * @note    Function may block.
 *
 * @return  The number of bytes received on success.
 * @return  0, if no received data is available, but everything is in order.
 * @return  any other negative number in case of an error. For portability, implementations should
 *          draw inspiration of the errno values from the POSIX' recv(), recvfrom(), or recvmsg()
 *          function specification.
 */
int conn_tcp_recv(conn_tcp_t *conn, void *data, size_t max_len);

/**
 * @brief   Sends a TCP message
 *
 * @param[in] conn  A TCP connection object.
 * @param[in] data  Pointer where the received data should be stored.
 * @param[in] len   Maximum space available at @p data.
 *
 * @note    Function may block.
 *
 * @return  The number of bytes send on success.
 * @return  any other negative number in case of an error. For portability, implementations should
 *          draw inspiration of the errno values from the POSIX' send(), sendfrom(), or sendmsg()
 *          function specification.
 */
int conn_tcp_send(conn_tcp_t *conn, const void *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* NET_CONN_TCP_H */
/** @} */
