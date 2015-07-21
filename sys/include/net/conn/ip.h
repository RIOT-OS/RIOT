/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_conn_ip     Raw IPv4/IPv6 connections
 * @ingroup     net_conn
 * @brief       Connection submodule for raw IPv4/IPv6 connections
 * @{
 *
 * @file
 * @brief   Raw IPv4/IPv6 connection definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_CONN_IP_H_
#define NET_CONN_IP_H_

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Forward declaration of @ref conn_ip_t to allow for external definition.
 */
struct conn_ip;

/**
 * @brief   Implementation-specific type of a raw IPv4/IPv6 connection object
 */
typedef struct conn_ip conn_ip_t;

/**
 * @brief   Creates a new raw IPv4/IPv6 connection object
 *
 * @param[out] conn     Preallocated connection object. Must fill the size of the stack-specific
 *                      connection desriptor.
 * @param[in] addr      The local IP address for @p conn.
 * @param[in] addr_len  Length of @p addr. Must be fitting for the @p family.
 * @param[in] family    The family of @p addr (see @ref net_af).
 * @param[in] proto     @ref net_protnum for the IPv6 packets to receive.
 *
 * @return  0 on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' bind() function specification.
 */
int conn_ip_create(conn_ip_t *conn, const void *addr, size_t addr_len, int family, int proto);

/**
 * @brief   Closes a raw IPv4/IPv6 connection
 *
 * @param[in,out] conn  A raw IPv4/IPv6 connection object.
 */
void conn_ip_close(conn_ip_t *conn);

/**
 * @brief   Gets the local address of a raw IPv4/IPv6 connection
 *
 * @param[in] conn  A raw IPv4/IPv6 connection object.
 * @param[out] addr The local IP address. Must have space for any address of the connection's
 *                  family.
 *
 * @return  length of @p addr on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' getsockname() function
 *          specification.
 */
int conn_ip_getlocaladdr(conn_ip_t *conn, void *addr);

/**
 * @brief   Receives a message over IPv4/IPv6
 *
 * @param[in] conn      A raw IPv4/IPv6 connection object.
 * @param[out] data     Pointer where the received data should be stored.
 * @param[in] max_len   Maximum space available at @p data.
 * @param[out] addr     NULL pointer or the sender's IP address. Must have space for any address
 *                      of the connection's family.
 * @param[out] addr_len Length of @p addr. Can be NULL if @p addr is NULL.
 *
 * @note    Function may block.
 *
 * @return  The number of bytes received on success.
 * @return  0, if no received data is available, but everything is in order.
 * @return  any other negative number in case of an error. For portability, implementations should
 *          draw inspiration of the errno values from the POSIX' recv(), recvfrom(), or recvmsg()
 *          function specification.
 */
int conn_ip_recvfrom(conn_ip_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len);

/**
 * @brief   Sends a message over IPv4/IPv6
 *
 * @param[in] data      Pointer where the received data should be stored.
 * @param[in] len       Maximum space available at @p data.
 * @param[in] src       The source address. May be NULL for all any interface address.
 * @param[in] src_len   Length of @p src.
 * @param[in] dst       The receiver's network address.
 * @param[in] dst_len   Length of @p dst.
 * @param[in] family    The family of @p src and @p dst (see @ref net_af).
 * @param[in] proto     @ref net_protnum for the IPv6 packets to set.
 *
 * @note    Function may block.
 *
 * @return  The number of bytes send on success.
 * @return  any other negative number in case of an error. For portability, implementations should
 *          draw inspiration of the errno values from the POSIX' send(), sendfrom(), or sendmsg()
 *          function specification.
 */
int conn_ip_sendto(const void *data, size_t len, const void *src, size_t src_len,
                   void *dst, size_t dst_len, int family, int proto);

#ifdef __cplusplus
}
#endif

#endif /* NET_CONN_IP_H_ */
/** @} */
