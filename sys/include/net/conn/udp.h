/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_conn_udp    UDP connections
 * @ingroup     net_conn
 * @brief       Connection submodule for UDP connections
 * @{
 *
 * @file
 * @brief   UDP connection definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_CONN_UDP_H_
#define NET_CONN_UDP_H_

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Forward declaration of @ref conn_udp_t to allow for external definition.
 */
struct conn_udp;

/**
 * @brief   Implementation-specific type of a UDP connection object
 */
typedef struct conn_udp conn_udp_t;

/**
 * @brief   Creates a new UDP connection object
 *
 * @param[out] conn     Preallocated connection object. Must fill the size of the stack-specific
 *                      connection desriptor.
 * @param[in] addr      The local network layer address for @p conn.
 * @param[in] addr_len  The length of @p addr. Must be fitting for the @p family.
 * @param[in] family    The family of @p addr (see @ref net_af).
 * @param[in] port      The local UDP port for @p conn.
 *
 * @return  0 on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' bind() function specification.
 */
int conn_udp_create(conn_udp_t *conn, const void *addr, size_t addr_len, int family,
                    uint16_t port);

/**
 * @brief   Closes a UDP connection
 *
 * @param[in,out] conn  A UDP connection object.
 */
void conn_udp_close(conn_udp_t *conn);

/**
 * @brief   Gets the local address of a UDP connection
 *
 * @param[in] conn  A UDP connection object.
 * @param[out] addr The local network layer address. Must have space for any address of
 *                  the connection's family.
 * @param[out] port The local UDP port.
 *
 * @return  length of @p addr on success.
 * @return  any other negative number in case of an error. For portability implementations should
 *          draw inspiration of the errno values from the POSIX' getsockname() function
 *          specification.
 */
int conn_udp_getlocaladdr(conn_udp_t *conn, void *addr, uint16_t *port);

/**
 * @brief   Receives a UDP message
 *
 * @param[in] conn      A UDP connection object.
 * @param[out] data     Pointer where the received data should be stored.
 * @param[in] max_len   Maximum space available at @p data.
 * @param[out] addr     NULL pointer or the sender's network layer address. Must have space
 *                      for any address of the connection's family.
 * @param[out] addr_len Length of @p addr. Can be NULL if @p addr is NULL.
 * @param[out] port     NULL pointer or the sender's UDP port.
 *
 * @note    Function may block.
 *
 * @return  The number of bytes received on success.
 * @return  0, if no received data is available, but everything is in order.
 * @return  any other negative number in case of an error. For portability, implementations should
 *          draw inspiration of the errno values from the POSIX' recv(), recvfrom(), or recvmsg()
 *          function specification.
 */
int conn_udp_recvfrom(conn_udp_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len,
                      uint16_t *port);

/**
 * @brief   Sends a UDP message
 *
 * @param[in] data      Pointer where the received data should be stored.
 * @param[in] len       Maximum space available at @p data.
 * @param[in] src       The source address. May be NULL for all any interface address.
 * @param[in] src_len   Length of @p src. May be 0 if @p src is NULL
 * @param[in] dst       The receiver's network address.
 * @param[in] dst_len   Length of @p dst.
 * @param[in] family    The family of @p src and @p dst (see @ref net_af).
 * @param[in] sport     The source UDP port.
 * @param[in] dport     The receiver's UDP port.
 *
 * @note    Function may block.
 *
 * @return  The number of bytes send on success.
 * @return  any other negative number in case of an error. For portability, implementations should
 *          draw inspiration of the errno values from the POSIX' send(), sendfrom(), or sendmsg()
 *          function specification.
 */
int conn_udp_sendto(const void *data, size_t len, const void *src, size_t src_len,
                    const void *dst, size_t dst_len, int family, uint16_t sport,
                    uint16_t dport);

#ifdef __cplusplus
}
#endif

#endif /* NET_CONN_UDP4_H_ */
/** @} */
