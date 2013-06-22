/**
 * Destiny socket API
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup destiny 
 * @{
 * @file    socket.h
 * @brief   header for BSD socket API
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */


#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdint.h>
#include "tcp.h"
#include "udp.h"
#include "in.h"
#include "../sixlowpan/sixlowip.h"

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

#define MAX_SOCKETS			5
// #define MAX_QUEUED_SOCKETS	2

#define EPHEMERAL_PORTS 	49152

#define STATIC_MSS			48
#define STATIC_WINDOW		1 * STATIC_MSS
#define MAX_TCP_BUFFER		1 * STATIC_WINDOW

#define INC_PACKET			0
#define OUT_PACKET			1

#define SEND_MSG_BUF_SIZE	64

typedef struct socka6 {
    uint8_t     		sin6_family;    		/* AF_INET6 */
    uint16_t       		sin6_port;      		/* transport layer port # */
    uint32_t        	sin6_flowinfo;  		/* IPv6 flow information */
    ipv6_addr_t 		sin6_addr;      		/* IPv6 address */
} sockaddr6_t;

typedef struct tcp_hc_con {
    uint16_t 		context_id;
    uint32_t 		seq_rcv; // Last received packet values
    uint32_t 		ack_rcv;
    uint16_t		wnd_rcv;
    uint32_t		seq_snd; // Last sent packet values
    uint32_t		ack_snd;
    uint16_t		wnd_snd;
    uint8_t			hc_type;
} tcp_hc_context_t;

typedef struct tcp_control_block {
    uint32_t			send_una;
    uint32_t			send_nxt;
    uint16_t			send_wnd;
    uint32_t			send_iss;

    uint32_t			rcv_nxt;
    uint16_t			rcv_wnd;
    uint32_t			rcv_irs;

    timex_t				last_packet_time;
    uint8_t				no_of_retries;
    uint16_t			mss;

    uint8_t 			state;

    double				srtt;
    double				rttvar;
    double				rto;

#ifdef TCP_HC
    tcp_hc_context_t	tcp_context;
#endif

} tcp_cb_t;

typedef struct sock_t {
    uint8_t				domain;
    uint8_t				type;
    uint8_t				protocol;
    tcp_cb_t			tcp_control;
    sockaddr6_t			local_address;
    sockaddr6_t			foreign_address;
} socket_t;

typedef struct socket_in_t {
    uint8_t				socket_id;
    uint8_t				recv_pid;
    uint8_t				send_pid;
    uint8_t				tcp_input_buffer_end;
    mutex_t				tcp_buffer_mutex;
    socket_t			socket_values;
    uint8_t				tcp_input_buffer[MAX_TCP_BUFFER];
} socket_internal_t;

extern socket_internal_t sockets[MAX_SOCKETS];

int socket(int domain, int type, int protocol);
int connect(int socket, sockaddr6_t *addr, uint32_t addrlen);
socket_internal_t *getWaitingConnectionSocket(int socket,
                                              ipv6_hdr_t *ipv6_header,
                                              tcp_hdr_t *tcp_header);
void close_socket(socket_internal_t *current_socket);
int32_t recvfrom(int s, void *buf, uint32_t len, int flags, sockaddr6_t *from,
                 uint32_t *fromlen);
int32_t sendto(int s, const void *msg, uint32_t len, int flags,
               sockaddr6_t *to, uint32_t tolen);
int32_t send(int s, void *msg, uint32_t len, int flags);
int recv(int s, void *buf, uint32_t len, int flags);
int close(int s);
int bind(int s, sockaddr6_t *name, int namelen);
int listen(int s, int backlog);
int accept(int s, sockaddr6_t *addr, uint32_t *addrlen);
void socket_init(void);
socket_internal_t *get_udp_socket(ipv6_hdr_t *ipv6_header, udp_hdr_t *udp_header);
socket_internal_t *get_tcp_socket(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header);
socket_internal_t *getSocket(uint8_t s);
void print_sockets(void);
void print_internal_socket(socket_internal_t *current_socket_internal);
void print_socket(socket_t *current_socket);
void printf_tcp_context(tcp_hc_context_t *current_tcp_context);
bool exists_socket(uint8_t socket);
socket_internal_t *new_tcp_queued_socket(ipv6_hdr_t *ipv6_header,
                                         tcp_hdr_t *tcp_header);
void print_tcp_status(int in_or_out, ipv6_hdr_t *ipv6_header,
                      tcp_hdr_t *tcp_header, socket_t *tcp_socket);
void set_socket_address(sockaddr6_t *sockaddr, uint8_t sin6_family,
                        uint16_t sin6_port, uint32_t sin6_flowinfo,
                        ipv6_addr_t *sin6_addr);
void set_tcp_cb(tcp_cb_t *tcp_control, uint32_t rcv_nxt, uint16_t rcv_wnd,
                uint32_t send_nxt, uint32_t send_una, uint16_t send_wnd);
void set_tcp_packet(tcp_hdr_t *tcp_hdr, uint16_t src_port, uint16_t dst_port,
                    uint32_t seq_nr, uint32_t ack_nr,
                    uint8_t dataOffset_reserved, uint8_t reserved_flags,
                    uint16_t window, uint16_t checksum, uint16_t urg_pointer);
int check_tcp_consistency(socket_t *current_tcp_socket, tcp_hdr_t *tcp_header);
void switch_tcp_packet_byte_order(tcp_hdr_t *current_tcp_packet);
int send_tcp(socket_internal_t *current_socket, tcp_hdr_t *current_tcp_packet,
             ipv6_hdr_t *temp_ipv6_header, uint8_t flags,
             uint8_t payload_length);
bool isTCPSocket(uint8_t s);
#endif /* SOCKET_H_ */
