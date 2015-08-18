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

#include "net/gnrc/ipv6.h"

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

#define AF_UNSPEC           0           ///< unspecified address family.
#define AF_LOCAL            1           ///< local to host (pipes, portals) address family.
#define AF_UNIX             AF_LOCAL    ///< alias for AF_LOCAL for backward compatibility.
#ifndef AF_INET
#define AF_INET             2           ///< internetwork address family: UDP, TCP, etc.
#endif
#define AF_IMPLINK          3           ///< ARPAnet IMP address family.
#define AF_PUP              4           ///< PUP protocols address family: e.g. BSP
#define AF_CHAOS            5           ///< MIT CHAOS protocols address family
#define AF_NS               6           ///< XEROX NS protocols address family
#define AF_ISO              7           ///< ISO protocols address family
#define AF_OSI              AF_ISO      ///< alias for AF_ISO
#define AF_ECMA             8           ///< European computer manufacturers address family
#define AF_DATAKIT          9           ///< datakit protocols address family
#define AF_CCITT            10          ///< CCITT protocols address family, X.25 etc
#define AF_SNA              11          ///< IBM SNA address family
#define AF_DECnet           12          ///< DECnet address family
#define AF_DLI              13          ///< DEC Direct data link interface address family
#define AF_LAT              14          ///< LAT address family
#define AF_HYLINK           15          ///< NSC Hyperchannel address family
#define AF_APPLETALK        16          ///< Apple Talk address family
#define AF_ROUTE            17          ///< Internal Routing Protocol address family
#define AF_LINK             18          ///< Link layer interface address family
#define pseudo_AF_XTP       19          ///< eXpress Transfer Protocol (no AF)
#define AF_COIP             20          ///< connection-oriented IP, aka ST II address family.
#define AF_CNT              21          ///< Computer Network Technology address family
#define pseudo_AF_RTIP      22          ///< Help Identify RTIP packets address family
#define AF_IPX              23          ///< Novell Internet Protocol (no AF)
#define AF_SIP              24          ///< Simple Internet Protocol address family
#define pseudo_AF_PIP       25          ///< Help Identify PIP packets (no AF)
#define AF_ISDN             26          ///< Integrated Services Digital Network address family
#define AF_E164             AF_ISDN     ///< CCITT E.164 recommendation
#define pseudo_AF_KEY       27          ///< Internal key-management function (no AF)
#ifndef AF_INET6
/**
 * IPv6 address family.
 *
 * @see AF_INET
 */
#define AF_INET6            28
#endif
#define AF_NATM             29          ///< native ATM access address family
#define AF_ATM              30          ///< ATM address family
#define pseudo_AF_HDRCMPLT  31          ///< Used by BPF to not rewrite headers in interface output routine
#define AF_NETGRAPH         32          ///< Netgraph sockets address family
#define AF_MAX              33          ///< Maximum for address families

/*
 * Protocol families, same as address families for now.
 */
#define PF_UNSPEC       AF_UNSPEC       ///< protocol family
                                        ///< @see AF_UNSPEC
#define PF_LOCAL        AF_LOCAL        ///< protocol family
                                        ///< @see AF_LOCAL
#define PF_UNIX         PF_LOCAL        ///< alias for PF_LOCAL for backward compatibility
#define PF_INET         AF_INET         ///< protocol family
                                        ///< @see AF_INET
#define PF_IMPLINK      AF_IMPLINK      ///< protocol family
                                        ///< @see AF_IMPLINK
#define PF_PUP          AF_PUP          ///< protocol family
                                        ///< @see AF_PUP
#define PF_CHAOS        AF_CHAOS        ///< protocol family
                                        ///< @see AF_CHAOS
#define PF_NS           AF_NS           ///< protocol family
                                        ///< @see AF_NS
#define PF_ISO          AF_ISO          ///< protocol family
                                        ///< @see AF_ISO
#define PF_OSI          AF_OSI          ///< protocol family
                                        ///< @see AF_OSI
#define PF_ECMA         AF_ECMA         ///< protocol family
                                        ///< @see AF_ECMA
#define PF_DATAKIT      AF_DATAKIT      ///< protocol family
                                        ///< @see AF_DATAKIT
#define PF_CCITT        AF_CCITT        ///< protocol family
                                        ///< @see AF_CCITT
#define PF_SNA          AF_SNA          ///< protocol family
                                        ///< @see AF_SNA
#define PF_DECnet       AF_DECnet       ///< protocol family
                                        ///< @see AF_DECnet
#define PF_DLI          AF_DLI          ///< protocol family
                                        ///< @see AF_DLI
#define PF_LAT          AF_LAT          ///< protocol family
                                        ///< @see AF_LAT
#define PF_HYLINK       AF_HYLINK       ///< protocol family
                                        ///< @see AF_HYLINK
#define PF_APPLETALK    AF_APPLETALK    ///< protocol family
                                        ///< @see AF_APPLETALK
#define PF_ROUTE        AF_ROUTE        ///< protocol family
                                        ///< @see AF_ROUTE
#define PF_LINK         AF_LINK         ///< protocol family
                                        ///< @see AF_LINK
#define PF_XTP          pseudo_AF_XTP   ///< protocol family (no address family defined, only PF)
                                        ///< @see pseudo_AF_XTP
#define PF_COIP         AF_COIP         ///< protocol family
                                        ///< @see AF_COIP
#define PF_CNT          AF_CNT          ///< protocol family
                                        ///< @see AF_CNT
#define PF_SIP          AF_SIP          ///< protocol family
                                        ///< @see AF_SIP
#define PF_IPX          AF_IPX          ///< protocol family (same format as AF_NS)
                                        ///< @see AF_IPX
                                        ///< @see AF_NS
#define PF_RTIP         pseudo_AF_RTIP  ///< protocol family (same format as AF_INET)
                                        ///< @see pseudo_AF_RTIP
#define PF_PIP          pseudo_AF_PIP   ///< protocol family
                                        ///< @see pseudo_AF_PIP
#define PF_ISDN         AF_ISDN         ///< protocol family
                                        ///< @see AF_ISDN
#define PF_KEY          pseudo_AF_KEY   ///< protocol family
                                        ///< @see pseudo_AF_KEY
#define PF_INET6        AF_INET6        ///< protocol family
                                        ///< @see AF_INET6
#define PF_NATM         AF_NATM         ///< protocol family
                                        ///< @see AF_NATM
#define PF_ATM          AF_ATM          ///< protocol family
                                        ///< @see AF_ATM
#define PF_NETGRAPH     AF_NETGRAPH     ///< protocol family
                                        ///< @see AF_NETGRAPH
#define PF_MAX          AF_MAX          ///< maximum of protocol families
                                        ///< @see AF_MAX

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
