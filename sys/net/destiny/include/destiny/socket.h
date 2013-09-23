/**
 * destiny/socket.h - Destiny socket API
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup destiny
 * @{
 * @file
 * @brief   Header for BSD socket API
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef DESTINY_SOCKET_H
#define DESTINY_SOCKET_H

#include <stdint.h>
#include "ipv6.h"
#include "destiny/in.h"

/**
 * POSIX compatible type for address family.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_socket.h.html">
 *          IEEE Std 1003.1, 2013 Edition - sys/socket.h
 *      </a>
 */
typedef uint8_t sa_family_t;
typedef uint32_t  socklen_t;    ///< POSIX compatible type for address lengths.

/*
 * Types
 */
#define	SOCK_STREAM			1					/* stream socket */
#define	SOCK_DGRAM			2					/* datagram socket */
#define	SOCK_RAW			3					/* raw-protocol interface */
#define	SOCK_RDM			4					/* reliably-delivered message */
#define	SOCK_SEQPACKET		5					/* sequenced packet stream */

/*
 * Address families.
 */
#define	AF_UNSPEC			0					/* unspecified */
#define	AF_LOCAL			1					/* local to host (pipes, portals) */
#define	AF_UNIX				AF_LOCAL			/* backward compatibility */
#define	AF_INET				2					/* internetwork: UDP, TCP, etc. */
#define	AF_IMPLINK			3					/* arpanet imp addresses */
#define	AF_PUP				4					/* pup protocols: e.g. BSP */
#define	AF_CHAOS			5					/* mit CHAOS protocols */
#define	AF_NS				6					/* XEROX NS protocols */
#define	AF_ISO				7					/* ISO protocols */
#define	AF_OSI				AF_ISO
#define	AF_ECMA				8					/* European computer manufacturers */
#define	AF_DATAKIT			9					/* datakit protocols */
#define	AF_CCITT			10					/* CCITT protocols, X.25 etc */
#define	AF_SNA				11					/* IBM SNA */
#define AF_DECnet			12					/* DECnet */
#define AF_DLI				13					/* DEC Direct data link interface */
#define AF_LAT				14					/* LAT */
#define	AF_HYLINK			15					/* NSC Hyperchannel */
#define	AF_APPLETALK		16					/* Apple Talk */
#define	AF_ROUTE			17					/* Internal Routing Protocol */
#define	AF_LINK				18					/* Link layer interface */
#define	pseudo_AF_XTP		19					/* eXpress Transfer Protocol (no AF) */
#define	AF_COIP				20					/* connection-oriented IP, aka ST II */
#define	AF_CNT				21					/* Computer Network Technology */
#define pseudo_AF_RTIP		22					/* Help Identify RTIP packets */
#define	AF_IPX				23					/* Novell Internet Protocol */
#define	AF_SIP				24					/* Simple Internet Protocol */
#define	pseudo_AF_PIP		25					/* Help Identify PIP packets */
#define	AF_ISDN				26					/* Integrated Services Digital Network*/
#define	AF_E164				AF_ISDN				/* CCITT E.164 recommendation */
#define	pseudo_AF_KEY		27					/* Internal key-management function */
#define	AF_INET6			28					/* IPv6 */
#define	AF_NATM				29					/* native ATM access */
#define	AF_ATM				30					/* ATM */
#define pseudo_AF_HDRCMPLT 	31					/* Used by BPF to not rewrite headers
												 * in interface output routine
												 */
#define	AF_NETGRAPH			32					/* Netgraph sockets */
#define	AF_MAX				33

/*
 * Protocol families, same as address families for now.
 */
#define	PF_UNSPEC			AF_UNSPEC
#define	PF_LOCAL			AF_LOCAL
#define	PF_UNIX				PF_LOCAL			/* backward compatibility */
#define	PF_INET				AF_INET
#define	PF_IMPLINK			AF_IMPLINK
#define	PF_PUP				AF_PUP
#define	PF_CHAOS			AF_CHAOS
#define	PF_NS				AF_NS
#define	PF_ISO				AF_ISO
#define	PF_OSI				AF_ISO
#define	PF_ECMA				AF_ECMA
#define	PF_DATAKIT			AF_DATAKIT
#define	PF_CCITT			AF_CCITT
#define	PF_SNA				AF_SNA
#define PF_DECnet			AF_DECnet
#define PF_DLI				AF_DLI
#define PF_LAT				AF_LAT
#define	PF_HYLINK			AF_HYLINK
#define	PF_APPLETALK		AF_APPLETALK
#define	PF_ROUTE			AF_ROUTE
#define	PF_LINK				AF_LINK
#define	PF_XTP				pseudo_AF_XTP		/* really just proto family, no AF */
#define	PF_COIP				AF_COIP
#define	PF_CNT				AF_CNT
#define	PF_SIP				AF_SIP
#define	PF_IPX				AF_IPX				/* same format as AF_NS */
#define PF_RTIP				pseudo_AF_RTIP		/* same format as AF_INET */
#define PF_PIP				pseudo_AF_PIP
#define	PF_ISDN				AF_ISDN
#define	PF_KEY				pseudo_AF_KEY
#define	PF_INET6			AF_INET6
#define	PF_NATM				AF_NATM
#define	PF_ATM				AF_ATM
#define	PF_NETGRAPH			AF_NETGRAPH
#define	PF_MAX				AF_MAX

#define DESTINY_SOCKET_STATIC_MSS       48
#define DESTINY_SOCKET_STATIC_WINDOW    1 * DESTINY_SOCKET_STATIC_MSS
#define DESTINY_SOCKET_MAX_TCP_BUFFER   1 * DESTINY_SOCKET_STATIC_WINDOW

typedef struct __attribute__((packed)) {
    uint8_t     		sin6_family;    		/* AF_INET6 */
    uint16_t       		sin6_port;      		/* transport layer port # */
    uint32_t        	sin6_flowinfo;  		/* IPv6 flow information */
    ipv6_addr_t 		sin6_addr;      		/* IPv6 address */
} sockaddr6_t;

int destiny_socket(int domain, int type, int protocol);
int destiny_socket_connect(int socket, sockaddr6_t *addr,
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
int32_t destiny_socket_recv(int s, void *buf, uint32_t len, int flags);

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
int32_t destiny_socket_recvfrom(int s, void *buf, uint32_t len, int flags,
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
int32_t destiny_socket_send(int s, const void *buf, uint32_t len, int flags);

/**
 * Sends data *buf* through socket *s* to foreign host with IPv6 address *addr*.
 * Roughly identical to POSIX's <a href="http://man.he.net/man2/send">send(2)</a>.
 *
 * @param[in] s         The ID of the socket to send through.
 * @param[in] buf       Buffer to send the data from.
 * @param[in] len       Length of buffer.
 * @param[in] flags     Flags for possible later implementations (currently
 *                      unused).
 * @param[in] from      IPv6 Address to send data to.
 * @param[in] fromlen   Length of address in *from* in byte (always 16).
 *
 * @return Number of send bytes, -1 on error.
 */
int32_t destiny_socket_sendto(int s, const void *buf, uint32_t len, int flags,
                              sockaddr6_t *to, socklen_t tolen);
int32_t destiny_socket_send(int s, void *msg, uint32_t len, int flags);
int32_t destiny_socket_recv(int s, void *buf, uint32_t len, int flags);
int destiny_socket_close(int s);
int destiny_socket_bind(int s, sockaddr6_t *name, int namelen);
int destiny_socket_listen(int s, int backlog);
int destiny_socket_accept(int s, sockaddr6_t *addr, socklen_t *addrlen);
void destiny_socket_print_sockets(void);

/**
 * @}
 */
#endif /* DESTINY_SOCKET_H */
