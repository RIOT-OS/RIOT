/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  pnet
 * @{
 */

/**
 * @file    sys/socket.h
 * @brief   Main socket header
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_socket.h.html">
 *              The Open Group Base Specifications Issue 7, <sys/socket.h>
 *          </a>
 *
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

#ifdef CPU_NATIVE
/* Ignore Linux definitions in native */
#define _BITS_SOCKADDR_H    1
#define __SOCKADDR_COMMON(sa_prefix) \
  sa_family_t sa_prefix##family

#define __SOCKADDR_COMMON_SIZE  (sizeof (unsigned short int))
#endif

#include <sys/types.h>

#include "cpu.h"

#include "socket_base/socket.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Used to define the socket address.
 */
struct __attribute__((packed)) sockaddr {
    sa_family_t sa_family;  ///< Address family
    char sa_data[14];       ///< Socket address (variable length data)
};

/**
 * @brief   Implementation based socket address table.
 */
struct __attribute__((packed)) sockaddr_storage {
    sa_family_t ss_family;  ///< Address family
    char ss_data[14];       ///< address data
};

/*
 * Omitted from original specification for now are struct msghdr,
 * struct cmesghdr, and struct linger and all related defines
 */

/**
 * @brief  *level* value for getsockopt() or setsockopt().
 */
#define SOL_SOCKET 1    ///< Options to be accessed at socket level, not
                        ///< protocol level.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_ACCEPTCONN   1   ///< Socket is accepting connections.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_BROADCAST    2   ///< Transmission of broadcast messages is supported.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_DEBUG        3   ///< Debugging information is being recorded.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_DONTROUTE    4   ///< Bypass normal routing.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_ERROR        5   ///< Socket error status.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_KEEPALIVE    6   ///< Connections are kept alive with periodic messages.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_OOBINLINE    7   ///< Out-of-band data is transmitted in line.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_RCVBUF       8   ///< Receive buffer size.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_RCVLOWAT     9   ///< Receive "low water mark".

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_RCVTIMEO    10   ///< Receive timeout.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_REUSEADDR   11   ///< Reuse of local addresses is supported.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_SNDBUF      12   ///< Send buffer size.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_SNDLOWAT    13   ///< Send "low water mark".

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_SNDTIMEO    14   ///< Send timeout.

/**
 * @brief   *option_name* value for getsockopt() or setsockopt()
 */
#define SO_TYPE        15   ///< Socket type.

#define SOMAXCONN       16  ///< Maximum *backlog* size for listen()

/**
 * @brief   *how* value for shutdown()
 */
#define SHUT_WR     1   ///< Disables further send operations.

/**
 * @brief   *how* value for shutdown()
 */
#define SHUT_RD     2   ///< Disables further receive operations.

/**
 * @brief   *how* value for shutdown()
 */
#define SHUT_RDWR   3   ///< Disables further send and receive operations.

/**
 * @brief   Accept a new connection on a socket
 * @details The accept() function shall extract the first connection on the
 *          queue of pending connections, create a new socket with the same
 *          socket type protocol and address family as the specified socket,
 *          and allocate a new file descriptor for that socket.
 *          If the listen queue is empty of connection requests and O_NONBLOCK
 *          is not set on the file descriptor for the socket, accept() shall
 *          block until a connection is present. If the listen() queue is
 *          empty of connection requests and O_NONBLOCK is set on the file
 *          descriptor for the socket, accept() shall fail and set errno
 *          to [EAGAIN] or [EWOULDBLOCK]. The accepted socket cannot itself
 *          accept more connections. The original socket remains open and can
 *          accept more connections.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/accept.html">
 *          The Open Group Base Specification Issue 7, accept
 *      </a>
 *
 * @param[in] socket        Specifies a socket that was created with socket(),
 *                          has been bound to an address with bind(),
 *                          and has issued a successful call to listen().
 * @param[out] address      Either a null pointer, or a pointer to a sockaddr
 *                          structure where the address of the connecting socket
 *                          shall be returned. If *address* is not a null
 *                          pointer, the address of the peer for the accepted
 *                          connection shall be stored in the sockaddr
 *                          structure pointed to by *address*, and the length
 *                          of this address shall be stored in the object
 *                          pointed to by *address_len*. If the actual length
 *                          of the address is greater than the length of the
 *                          supplied sockaddr structure, the stored address
 *                          shall be truncated. If the protocol permits
 *                          connections by unbound clients, and the peer is not
 *                          bound, then the value stored in the object pointed
 *                          to by address is unspecified.
 * @param[out] address_len  Either a null pointer, if *address* is a null
 *                          pointer, or a pointer to a socklen_t object which
 *                          on input specifies the length of the supplied
 *                          sockaddr structure, and on output specifies the
 *                          length of the stored address.
 * @return  Upon successful completion, accept() shall return the non-negative
 *          file descriptor of the accepted socket. Otherwise, -1 shall be
 *          returned and errno set to indicate the error.
 */
int accept(int socket, struct sockaddr *__restrict address,
           socklen_t *__restrict address_len);

/**
 * @brief   Bind a name to a socket.
 * @detail  The bind() function shall assign a local socket address *address*
 *          to a socket identified by descriptor socket that has no local
 *          socket address assigned. Sockets created with the socket() function
 *          are initially unnamed; they are identified only by their address
 *          family.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/bind.html">
 *          The Open Group Base Specification Issue 7, bind
 *      </a>
 *
 * @param socket        Specifies the file descriptor of the socket to be bound.
 * @param address       Points to a sockaddr structure containing the address
 *                      to be bound to the socket. The length and format of the
 *                      address depend on the address family of the socket.
 *                      If the address family of the socket is AF_UNIX and the
 *                      pathname in address names a symbolic link, bind() shall
 *                      fail and set errno to [EADDRINUSE].
 * @param address_len   Specifies the length of the sockaddr structure pointed
 *                      to by the *address* argument.
 * @return  Upon successful completion, bind() shall return 0; otherwise, -1
 *          shall be returned and errno set to indicate the error.
 */
int bind(int socket, const struct sockaddr *address,
         socklen_t address_len);

/**
 * @brief   Connect a socket.
 * @detail  The connect() function shall attempt to make a connection on a
 *          connection-mode socket or to set or reset the peer address of a
 *          connectionless-mode socket.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/connect.html">
 *          The Open Group Base Specification Issue 7, connect
 *      </a>
 *
 * @param[in] socket        Specifies the file descriptor associated with the
 *                          socket.
 * @param[in] address       Points to a sockaddr structure containing the peer
 *                          address. The length and format of the address depend
 *                          on the address family of the socket.
 * @param[in] address_len   Specifies the length of the sockaddr structure
 *                          pointed to by the address argument.
 * @return  Upon successful completion, connect() shall return 0; otherwise,
 *          -1 shall be returned and errno set to indicate the error.
 */
int connect(int socket, const struct sockaddr *address, socklen_t address_len);

/**
 * @brief   Get the socket options.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/getsockopt.html">
 *          The Open Group Base Specification Issue 7, getsockopt
 *      </a>
 *
 * @param[in] socket        Specifies the file descriptor associated with the socket.
 * @param[in] level         Protocol level this option applies to. Valid values
 *                          are defined in <sys/socket.h>, prefixed with
 *                          ``SOL_``.
 * @param[in] option_name   Defines the option to get. Valid values are defined
 *                          in <sys/socket.h>, prefixed with ``SO_``.
 * @param[out] option_value Buffer to write the current value of the socket
 *                          option into.
 * @param[out] option_len   Length of the option value in byte.
 *
 * @return  Upon successful completion, getsockopt() shall return 0; otherwise,
 *          -1 shall be returned and errno set to indicate the error.
 */
int getsockopt(int socket, int level, int option_name,
               void *__restrict option_value, socklen_t *__restrict option_len);

/**
 * @brief   Listen for socket connections and limit the queue of incoming
 *          connections.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/listen.html">
 *          The Open Group Base Specification Issue 7, listen
 *      </a>
 *
 * @param[in] socket    Specifies the file descriptor associated with the socket.
 * @param[in] backlog   Provides a hint to the implementation which the
 *                      implementation shall use to limit the number of
 *                      outstanding connections in the socket's listen queue.
 *                      Implementations may impose a limit on backlog and
 *                      silently reduce the specified value. Normally, a
 *                      larger backlog argument value shall result in a larger
 *                      or equal length of the listen queue. Implementations
 *                      shall support values of backlog up to SOMAXCONN,
 *                      defined in <sys/socket.h>.
 * @return  Upon successful completions, listen() shall return 0; otherwise,
 *          -1 shall be returned and errno set to indicate the error.
 */
int listen(int socket, int backlog);

/**
 * @brief   Receive a message from a connected socket.
 * @detail  Shall receive a message from a connection-mode or
 *          connectionless-mode socket. It is normally used with connected
 *          sockets because it does not permit the application to retrieve the
 *          source address of received data.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/recv.html">
 *          The Open Group Base Specification Issue 7, recv
 *      </a>
 *
 * @param[in] socket    Specifies the socket file descriptor.
 * @param[out] buffer   Points to a buffer where the message should be stored.
 * @param[in] length    Specifies the length in bytes of the buffer pointed to
 *                      by the buffer argument.
 * @param[in] flags     Specifies the type of message reception. Support for
 *                      values other than 0 is not implemented yet.
 *
 * @return  Upon successful completion, recv() shall return the length of the
 *          message in bytes. If no messages are available to be received and
 *          the peer has performed an orderly shutdown, recv() shall return 0.
 *          Otherwise, -1 shall be returned and errno set to indicate the error.
 */
ssize_t recv(int socket, void *buffer, size_t length, int flags);

/**
 * @brief   Receive a message from a socket.
 * @detail  The recvfrom() function shall receive a message from a
 *          connection-mode or connectionless-mode socket. It is normally used
 *          with connectionless-mode sockets because it permits the application
 *          to retrieve the source address of received data.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/recvfrom.html">
 *          The Open Group Base Specification Issue 7, recvfrom
 *      </a>
 *
 * @param[in] socket        Specifies the socket file descriptor.
 * @param[out] buffer       Points to a buffer where the message should be i
 *                          stored.
 * @param[in] length        Specifies the length in bytes of the buffer pointed
 *                          to by the buffer argument.
 * @param[in] flags         Specifies the type of message reception. Support
 *                          for values other than 0 is not implemented yet.
 * @param[out] address      A null pointer, or points to a sockaddr structure
 *                          in which the sending address is to be stored. The
 *                          length and format of the address depend on the
 *                          address family of the socket.
 * @param[out] address_len  Either a null pointer, if address is a null pointer,
 *                          or a pointer to a socklen_t object which on input
 *                          specifies the length of the supplied sockaddr
 *                          structure, and on output specifies the length of
 *                          the stored address.
 *
 * @return  Upon successful completion, recvfrom() shall return the length of
 *          the message in bytes. If no messages are available to be received
 *          and the peer has performed an orderly shutdown, recvfrom() shall
 *          return 0. Otherwise, the function shall return -1 and set errno to
 *          indicate the error.
 */
ssize_t recvfrom(int socket, void *__restrict buffer, size_t length, int flags,
                 struct sockaddr *__restrict address,
                 socklen_t *__restrict address_len);

/**
 * @brief   Send a message on a socket.
 * @detail  Shall initiate transmission of a message from the specified socket
 *          to its peer. The send() function shall send a message only when the
 *          socket is connected. If the socket is a connectionless-mode socket,
 *          the message shall be sent to the pre-specified peer address.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/send.html">
 *          The Open Group Base Specification Issue 7, send
 *      </a>
 *
 * @param[in] socket    Specifies the socket file descriptor.
 * @param[in] buffer    Points to the buffer containing the message to send.
 * @param[in] length    Specifies the length of the message in bytes.
 * @param[in] flags     Specifies the type of message reception. Support
 *                      for values other than 0 is not implemented yet.
 *
 * @return  Upon successful completion, send() shall return the number of bytes
 *          sent. Otherwise, -1 shall be returned and errno set to indicate the
 *          error.
 */
ssize_t send(int socket, const void *buffer, size_t length, int flags);

/**
 * @brief   Send a message on a socket.
 * @detail  Shall send a message through a connection-mode or
 *          connectionless-mode socket. If the socket is a connectionless-mode
 *          socket, the message shall be sent to the address specified by
 *          *dest_addr* if no pre-specified peer address has been set. If a
 *          peer address has been pre-specified, either the message shall be
 *          sent to the address specified by *dest_addr* (overriding the
 *          pre-specified peer address), or the function shall return -1 and
 *          set errno to EISCONN.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/sendto.html">
 *          The Open Group Base Specification Issue 7, sendto
 *      </a>
 *
 * @param[in] socket    Specifies the socket file descriptor.
 * @param[in] buffer    Points to the buffer containing the message to send.
 * @param[in] length    Specifies the length of the message in bytes.
 * @param[in] flags     Specifies the type of message reception. Support
 *                      for values other than 0 is not implemented yet.
 * @param[in] dest_addr Points to a sockaddr structure containing the
 *                      destination address. The length and format of the
 *                      address depend on the address family of the socket.
 * @param[in] dest_len  Specifies the length of the sockaddr structure pointed
 *                      to by the *dest_addr* argument.
 *
 * @return  Upon successful completion, send() shall return the number of bytes
 *          sent. Otherwise, -1 shall be returned and errno set to indicate the
 *          error.
 */
ssize_t sendto(int socket, const void *message, size_t length, int flags,
               const struct sockaddr *dest_addr, socklen_t dest_len);

/**
 * @brief   Set the socket options.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/setsockopt.html">
 *          The Open Group Base Specification Issue 7, setsockopt
 *      </a>
 *
 * @param[in] socket        Specifies the file descriptor associated with the socket.
 * @param[in] level         Protocol level this option applies to. Valid values
 *                          are defined in <sys/socket.h>, prefixed with
 *                          ``SOL_``.
 * @param[in] option_name   Defines the option to set. Valid values are defined
 *                          in <sys/socket.h>, prefixed with ``SO_``.
 * @param[in] option_value  Value for the option to set.
 * @param[in] option_len    Length of the option value in byte.
 *
 * @return  Upon successful completion, setsockopt() shall return 0; otherwise,
 *          -1 shall be returned and errno set to indicate the error.
 */
int setsockopt(int socket, int level, int option_name, const void *option_value,
               socklen_t option_len);

/**
 * @brief   Create an endpoint for communication.
 * @detail  Shall create an unbound socket in a communications domain, and
 *          return a file descriptor that can be used in later function calls
 *          that operate on sockets.
 *
 * @param[in] domain    Specifies the communications domain in which a socket
 *                      is to be created. Valid values are prefixed with ``AF_`
 *                      and defined in @ref transport_layer/socket.h.
 * @param[in] type      Specifies the type of socket to be created. Valued
 *                      values are prefixed with ``SOCK_`` and defined in
 *                      @ref transport_layer/socket.h.
 * @param[in] protocol  Specifies a particular protocol to be used with the
 *                      socket. Specifying a protocol of 0 causes socket() to
 *                      use an unspecified default protocol appropriate for
 *                      the requested socket type.
 *
 * @return  Upon successful completion, socket() shall return a non-negative
 *          integer, the socket file descriptor. Otherwise, a value of -1 shall
 *          be returned and errno set to indicate the error.
 */
int socket(int domain, int type, int protocol);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* SYS_SOCKET_H */
