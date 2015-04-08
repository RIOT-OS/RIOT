/*
 * ng_socket.h - Tranport Layer socket API
 * Copyright 2015 Brummer Simon <brummer.simon@googlemail.com>.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup net_ng_socket  Socket Structure
 * @brief Socket Structure for interfacing with network layers.
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_socket.h.html">
 *      IEEE Std 1003.1, 2013 Edition - sys/socket.h</a>
 * @{
 * @file
 * @brief  Header of RIOTs ng_socket API.
 *
 *         ng_socket provides a basic socket implementation.
 *         It supports currently one a subset of the standardized "sys/socket.h" contents.
 *         The most important difference is the absence of a sockaddr_storage structure,
 *         to keep things simple, it is replaced by ng_sockaddr6_t.
 *         Currently only ipv6 is supported.
 *
 * @author Simon Brummer <brummer.simon@googlemail.com>
 */

#ifndef NG_SOCKET_H_
#define NG_SOCKET_H_

#include "byteorder.h"
#include "sys/types.h"
#include "net/ng_ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAXSOCKETS 5         /**< Maximum Number of Sockets in use. */

/* Socket Type Definitions */
#define SOCK_STREAM    (1)   /**< Posix compatible stream socket type. */
#define SOCK_DGRAM     (2)   /**< Posix compatible datagram socket type. */

/* Address families. */
#define AF_INET6    (28)     /**< IPv6 Address Family */

/* Protocol families. */
#define PF_INET6    AF_INET6

/* Protokol above IP Layer. See RFC 1700 @see <a href="http://tools.ietf.org/html/rfc1700"> RFC 1700 </a> */
#define IPPROTO_IP  (0)      /**< Reserved. */
#define IPPROTO_TCP (6)      /**< Transmission Control Protocol */
#define IPPROTO_UDP (17)     /**< User Datagram Protocol */

/* Definitions ng_socket_listen */
#define SOMAXCONN (0)        /**< Maximum Backlog for listen call */

/* Definitions ng_shutdown */
#define SHUTDOWN_RD   (0)    /**< Disables further receive operations */
#define SHUTDOWN_WR   (1)    /**< Disables further send operations */
#define SHUTDOWN_RDWR (2)    /**< Disables further send and receive operations */

/**
 * @brief Socket Address Type for IPv6 usage.
 */
typedef struct __attribute__((packed)) {
    uint8_t        sin6_family;    /**< Address Familie in use. AF_INET6 */
    uint16_t       sin6_port;      /**< Port Number, used in Transport Layer */
    uint32_t       sin6_flowinfo;  /**< IPv6 Flow Information */
    ng_ipv6_addr_t sin6_addr;      /**< IPv6 Address */
} ng_sockaddr6_t;

/**
 * @brief General Socket Type, currently restricted to ipv6 usage.
 */
typedef struct __attribute__((packed)) {
    uint8_t domain;                /**< Socket Domain */
    uint8_t type;                  /**< Socket type */
    uint8_t protocol;              /**< Protocol used by this socket */
    bool    in_use;                /**< Flag if socket is currently in use */
    ng_sockaddr6_t local_addr;     /**< Local Address */
    ng_sockaddr6_t peer_addr;      /**< Peer Address */
} ng_socket_t;

/**
 * @brief Type for socket length
 */
typedef uint32_t ng_socklen_t;

/**
 *  @brief Setup internal Socket structures to a valid startup State.
 */
void ng_socket_init(void);

/**
 *  @brief Creates a new socket.
 * 
 *  Creates a new socket for communitcation in address family "domain", of type "type"
 *  with protocol "protocol". Mostly similar to <a href="http://man.he.net/man2/socket">socket(2)</a>.
 *  Supported makros are defined in this File.
 *
 *  @param[in] domain     Address family. Currently only AF_INET6 support.
 *  @param[in] type       Communication type. Makros with prefix "SOCK_" are supported
 *  @param[in] protocol   Protocol type. 0 if type and domain are sufficent, if not
 *                         use makros with prefix "IPPROTO_".
 *
 *  @return  On success   Socket ID. On error a negative value is returned.
 *  @return  -1           All Sockets are in use
 *  @return  -2           Unknown or not supported Domain/Address Family
 *  @return  -3           Unknown or not supported Socket Type
 *  @return  -4           Unknown or not supported Protocol Type
 */
int ng_socket_socket(int domain, int type, int protocol);

/**
 * @brief Binds an local address to an existing Socket.
 *
 * Similar to <a href="http://man.he.net/man2/bind">bind</a>.
 *
 * @param[in] socket        Reference to a Socket, to bind address to.
 * @param[in] address       Structure that contains local address information.
 * @param[in] address_len   Size of address structure in bytes.
 *
 * @return On success       Zero is returned. On error a negative value is returned.
 * @return -1               Referenced socket doesn't exist.
 * @return -2               Socket already bound to an address.
 * @return -3               Given Address is not valid for sockets domain
 * @return -4               Address already used by another socket.
 */
int ng_socket_bind(int socket, const ng_sockaddr6_t *address, ng_socklen_t address_len);

/**
 * @brief Connects an existing Socket to a foreign Address specified by "address".
 *
 * Similar to <a href="http://man.he.net/man2/connect">connect</a>.
 *
 * @param[in] socket        Reference to a Socket, to connect address to.
 * @param[in] address       Structure that contains foreign address information.
 * @param[in] address_len   Size of address structure in bytes.
 *
 * @return On success       Zero is returned. On error a negative value is returned.
 * @return -1               Referenced socket doesn't exist.
 * @return -2               Socket is not of type SOCK_STREAM.
 * @return -3               Function currently not implemented.
 */
int ng_socket_connect(int socket, ng_sockaddr6_t *address, ng_socklen_t address_len);

/**
 * @brief Marks socket as passive Socket. 
 *
 * This socket can be used to accept incomming connection requests.
 * Connection Requests will be queued.
 * Similar to <a href="http://man.he.net/man2/listen">listen</a>.
 *
 * @param[in] socket        Reference to a Socket, to listen on.
 * @param[in] backlog       Maximum pending connections to socket.
 *
 * @return On success       Zero is returned. On error a negative value is returned.
 * @return -1               Referenced socket doesn't exist.
 * @return -2               Socket is not of type SOCK_STREAM.
 * @return -3               Function currently not implemented.
 */
int ng_socket_listen(int socket, int backlog);

/**
 * @brief Accept a pending connection from a listening socket. 
 *
 * This call takes a pending connection
 * out of the listening queue, creates a new socket and returns a reference to that socket.
 * This call blocks until a pending connection is received.
 * The structure address will be filled with the peer address.
 * Similar to <a href="http://man.he.net/man2/accept">accept</a>.
 *
 * @param[in]  socket        Reference to a socket in listening state.
 * @param[out] address       Structure to extract peer address into.
 * @param[in]  address_len   Size of address structure in bytes.
 *
 * @return On success        Zero is returned. On error a negative value is returned.
 * @return -1                Referenced socket doesn't exist.
 * @return -2                Socket is not of type SOCK_STREAM.
 * @return -3                Function currently not implemented.
 */
int ng_socket_accept(int socket, ng_sockaddr6_t *address, ng_socklen_t *address_len);

/**
 * @brief Receive Data from a connection-oriented Socket.
 *
 * The ng_socket_recv call blocks until a message arrives.
 * ng_socket_recv is identical to ng_socket_recvfrom with NULL as "from"-Argument.
 * If the Message doesn't fit into the supplied buffer, execessive
 * bytes may be discarded, depends on the socket type.
 * Similar to <a href="http://man.he.net/man2/recv">recv</a>.
 *
 * @param[in]  socket    Reference to a socket to receive data from.
 * @param[out] buf       Buffer to read received data into.
 * @param[in]  len       Buffersize of buf in bytes.
 *
 * @return On success    The number of received bytes, is returned.
 *                       On error a negative value is returned.
 *                       The return value will be Zero, if the peer
 *                       has performed an orderly shutdown.
 * @return -1            Referenced socket doesn't exist.
 * @return -2            Socket type is not supported.
 * @return -3            Function currently not implemented.
 */
ssize_t ng_socket_recv(int socket, void *buf, size_t len, int flags);

/**
 * @brief Receive Data from a Socket. 
 *
 * The Socket can be connection less or connection-oriented.
 * The ng_socket_recvfrom call blocks until a message arrives.
 * If "from" is not NULL, Address information from
 * the Sending Peer will be stored in "from".
 * If the Message doesn't fit into the supplied buffer, execessive
 * bytes may be discarded, depends on the socket type.
 * Similar to <a href="http://man.he.net/man2/recv">recv</a>.
 *
 * @param[in]  socket    Reference to a socket to receive data from.
 * @param[out] buf       Buffer to read received data into.
 * @param[in]  len       Buffersize of buf in bytes.
 * @param[out] from      Contains Sender Address Information
 *                       afterwards, if underlying protocol provides it.
 * @param[out] fromlen   Size of "from" in byte.
 *
 * @return On success    The number of received bytes, is returned.
 *                       On error a negative value is returned.
 *                       The return value will be Zero, if the peer
 *                       has performed an orderly shutdown.
 * @return -1            Referenced socket doesn't exist.
 * @return -2            Socket type is not supported.
 * @return -3            Function currently not implemented.
 */
ssize_t ng_socket_recvfrom(int socket, void *buf, size_t len, int flags,
                           ng_sockaddr6_t *from, ng_socklen_t *fromlen);

/**
 * @brief Transmit a message via a connection-oriented Socket.
 *
 * ng_socket_send is identical to ng_socket_sendto then NULL is provided as "from".
 * If the message is to long to pass atomicly through an underlying protocol the message is discarded.
 * Similar to <a href="http://man.he.net/man2/send">send</a>.
 *
 *
 * @param[in] socket    Reference to a socket to send data to.
 * @param[in] buf       Buffer that contains data to transmit.
 * @param[in] len       Message to transmit has length len in bytes.
 * @param[in] flags     Here for compliance but currently ignored.
 *
 * @return              On success the number of bytes sent, is returned.
 *                      On error a negative value is returned.
 * @return -1           Referenced socket doesn't exist.
 * @return -2           Socket type is not supported.
 * @return -3           Function currently not implemented.
 */
ssize_t ng_socket_send(int socket, const void *buf, size_t len, int flags);

/**
 * @brief Transmit a message via a Socket. 
 *
 * The Socket can be connection-oriented or connectionless.
 * If the message is to long to pass atomicly through an underlying protocol the message is discarded.
 * Similar to <a href="http://man.he.net/man2/send">send</a>.
 *
 *
 * @param[in] socket  Reference to a socket to send data to.
 * @param[in] buf     Buffer that contains data to transmit.
 * @param[in] len     Message to transmit has length len in bytes.
 * @param[in] flags   Here for compliance but currently ignored.
 * @param[in] to      Contains Address Information to send Message to.
 * @param[in] tolen   Size of "to" in byte.
 *
 * @return            On success the number of bytes sent, is returned.
 *                    On error a negative value is returned.
 * @return -1         Referenced socket doesn't exist.
 * @return -2         Socket type is not supported.
 * @return -3         Function currently not implemented.
 */
ssize_t ng_socket_sendto(int socket, const void *buf, size_t len, int flags,
                         const ng_sockaddr6_t *to, ng_socklen_t tolen);

/**
 * @brief Cause all or a part of a full-duplex connection on a socket to shut down.
 *
 * If how is SHUT_RD, further receiving is forbidden.
 * If how is SHUT_WR, further sending is forbidden.
 * If how is SHUT_RDWR, further sending or receiving is forbidden.
 * Similar to <a href="http://man.he.net/man2/shutdown">shutdown</a>.
 *
 * @param[in] socket    Reference to a Socket, to shut down.
 * @param[in] how       Specify the manner, a connection is shut down.
 *
 * @return On success   Zero is returned. On error a negative value is returned.
 * @return -1           Referenced socket doesn't exist.
 * @return -2           how is unknown
 */
int ng_socket_shutdown(int socket, int how);

/**
 * @brief Closes a given socket. The closed socket can be reused after this call.
 *
 * Similar to <a href="http://man.he.net/man2/close">close</a>.
 *
 * @param[in] socket    Reference to a Socket, that should be closed.
 *
 * @return On success   Zero is returned. On error a negative value is returned.
 * @return -1           Referenced socket doesn't exist.
 */
int ng_socket_close(int socket);

#ifdef __cplusplus
}
#endif

#endif /* NG_SOCKET_H_ */
/**
 * @}
 */
