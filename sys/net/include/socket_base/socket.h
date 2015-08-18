/**
 * socket_base/socket.h - Transport Layer socket API
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup socket_base
 * @{
 * @file
 * @brief   Header for BSD socket API
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_socket.h.html">
 *          IEEE Std 1003.1, 2013 Edition - sys/socket.h
 *      </a>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef SOCKET_BASE_SOCKET_H
#define SOCKET_BASE_SOCKET_H

#include <stdint.h>

#include "net/af.h"
#include "net/ipv6/addr.h"

#include "socket_base/in.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t sa_family_t;    ///< POSIX compatible type for address family.
typedef uint32_t  socklen_t;    ///< POSIX compatible type for address length.


#define SOCK_STREAM     1   ///< POSIX compatible stream socket type.
#define SOCK_DGRAM      2   ///< POSIX compatible datagram socket type.
#define SOCK_RAW        3   ///< POSIX compatible raw-protocol interface type.
#define SOCK_RDM        4   ///< POSIX compatible reliably-delivered message type.
#define SOCK_SEQPACKET  5   ///< POSIX compatible sequenced packet stream type.

#define TRANSPORT_LAYER_SOCKET_STATIC_MSS       48  ///< Static TCP maxmimum segment size.

/**
 * Static TCP flow control window for window size 1.
 */
#define TRANSPORT_LAYER_SOCKET_STATIC_WINDOW    1 * TRANSPORT_LAYER_SOCKET_STATIC_MSS

/**
 * Maximum size of TCP buffer.
 */
#define TRANSPORT_LAYER_SOCKET_MAX_TCP_BUFFER   1 * TRANSPORT_LAYER_SOCKET_STATIC_WINDOW

/**
 * Socket address type for IPv6 communication.
 */
typedef struct __attribute__((packed)) {
    uint8_t     sin6_family;    ///< set to AF_INET6
    uint16_t    sin6_port;      ///< transport layer port number
    uint32_t    sin6_flowinfo;  ///< IPv6 flow information
    ipv6_addr_t sin6_addr;      ///< IPv6 address
} sockaddr6_t;

/**
 * Creates new socket for communication in family *domain*, of type *type*,
 * and with protocol *protocol*. Roughly identical to POSIX's
 * <a href="http://man.he.net/man2/socket">socket(2)</a>.
 *
 * @param[in] domain    Address family. Only AF_INET6 is supported by now (no
 *                      check on creation).
 * @param[in] type      Communication type.
 * @param[in] protocol  Protocol type. May be left 0 if domain and type are
 *                      sufficiant enough (I. e. domain=AF_INET6 and type=SOCK_STREAM
 *                      imply IPPROTO_TCP, etc.)
 * @return Socket ID on success, -1 otherwise.
 */
int socket_base_socket(int domain, int type, int protocol);

/**
 * Connects socket *socket* with a foreign host with IPv6 address *addr*
 * (of length *addrlen*) for connection orientated communication (e. g.
 * SOCK_STREAM). Roughly identical to POSIX's
 * <a href="http://man.he.net/man2/connect">connect(2)</a>.
 *
 * @param[in] socket    The ID of the socket.
 * @param[in] addr      The IPv6 address to connect to
 * @param[in] addrlen   Length of the IPv6 address in byte (always 16)
 *
 * @return 0 on success, -1 otherwise
 */
int socket_base_connect(int socket, sockaddr6_t *addr,
                           socklen_t addrlen);

/**
 * Receives data through socket *s* and saves it in buffer *buf*. Roughly
 * identical to POSIX's <a href="http://man.he.net/man2/recv">recv(2)</a>.
 *
 * @param[in] s         The ID of the socket to receive from.
 * @param[in] buf       Buffer to store received data in.
 * @param[in] len       Length of buffer.
 * @param[in] flags     Flags for possible later implementations (currently
 *                      unused).
 *
 * @return Number of received bytes, -1 on error.
 */
int32_t socket_base_recv(int s, void *buf, uint32_t len, int flags);

/**
 * Receives data through socket *s* and saves it in buffer *buf*. The address
 * of the sender is stored in *from*. Roughly identical to POSIX's
 * <a href="http://man.he.net/man2/recvfrom">recvfrom(2)</a>.
 *
 * @param[in] s         The ID of the socket to receive from.
 * @param[in] buf       Buffer to store received data in.
 * @param[in] len       Length of buffer.
 * @param[in] flags     Flags for possible later implementations (currently
 *                      unused).
 * @param[in] from      IPv6 Address of the data's sender.
 * @param[in] fromlen   Length of address in *from* in byte (always 16).
 *
 * @return Number of received bytes, -1 on error.
 */
int32_t socket_base_recvfrom(int s, void *buf, uint32_t len, int flags,
                                sockaddr6_t *from, socklen_t *fromlen);

/**
 * Sends data *buf* through socket *s*. Roughly identical to POSIX's
 * <a href="http://man.he.net/man2/send">send(2)</a>.
 *
 * @param[in] s         The ID of the socket to send through.
 * @param[in] buf       Buffer to send the data from.
 * @param[in] len       Length of buffer.
 * @param[in] flags     Flags for possible later implementations (currently
 *                      unused).
 *
 * @return Number of send bytes, -1 on error.
 */
int32_t socket_base_send(int s, const void *buf, uint32_t len, int flags);

/**
 * Sends data *buf* through socket *s* to foreign host with IPv6 address *addr*.
 * Roughly identical to POSIX's <a href="http://man.he.net/man2/send">send(2)</a>.
 *
 * @param[in] s         The ID of the socket to send through.
 * @param[in] buf       Buffer to send the data from.
 * @param[in] len       Length of buffer.
 * @param[in] flags     Flags for possible later implementations (currently
 *                      unused).
 * @param[in] to        IPv6 Address to send data to.
 * @param[in] tolen     Length of address in *to* in byte (always 16).
 *
 * @return Number of send bytes, -1 on error.
 */
int32_t socket_base_sendto(int s, const void *buf, uint32_t len, int flags,
                              sockaddr6_t *to, socklen_t tolen);

/**
 * Closes the socket *s* and removes it.
 *
 * @param[in] s The ID of the socket to close.
 *
 * @return 0 on success, -1 otherwise.
 */
int socket_base_close(int s);

/**
 * Assigns an IPv6 address *addr* to the socket *s*. Roughly identical to
 * POSIX's <a href="http://man.he.net/man2/bind">bind(2)</a>.
 *
 * @param[in] s         The ID of the socket.
 * @param[in] addr      The IPv6 address to bind to.
 * @param[in] addrlen   Length of address in byte (always 16)
 *
 * @return 0 on success, -1 otherwise.
 */
int socket_base_bind(int s, sockaddr6_t *addr, int addrlen);

/**
 * Marks socket *s* as an passive socket, that listens for incoming messages.
 * Roughly identical to POSIX's <a href="http://man.he.net/man2/listen">listen(2)</a>.
 *
 * @param[in] s         The ID of the socket.
 * @param[in] backlog   Not implemented yet, may be 0.
 *
 * @return 0 on success, -1 otherwise.
 */
int socket_base_listen(int s, int backlog);

/**
 * Blocks the current thread and waits for incoming communication on the listening
 * socket *s*. Roughly identical to POSIX's
 * <a href="http://man.he.net/man2/accept">accept(2)</a>.
 *
 * @param[in] s         The ID of the socket.
 * @param[in] addr      The IPv6 address of the peer socket, or NULL if not
 *                      needed (not implemented yet).
 * @param[in] addrlen   The length of *addr*, or NULL if not needed.
 *
 * @return New socket ID for communication. -1 on error.
 */
int socket_base_accept(int s, sockaddr6_t *addr, socklen_t *addrlen);

/**
 * Outputs a list of all open sockets to stdout. Information includes its
 * creation parameters, local and foreign address and ports, it's ID and the
 * PIDs of the send and receive thread.
 */
void socket_base_print_sockets(void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* SOCKET_BASE_SOCKET_H */
