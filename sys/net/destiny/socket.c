/*
 * socket.c
 *
 *  Created on: 16.09.2011
 *      Author: Oliver
 */
#include <thread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "udp.h"
#include "tcp.h"
#include "socket.h"
#include "sys/net/net_help/net_help.h"
#include "sys/net/net_help/msg_help.h"

void print_tcp_flags (tcp_hdr_t *tcp_header)
	{
	printf("FLAGS: ");
	if ((tcp_header->reserved_flags & TCP_ACK) > 0)
		{
		printf("ACK ");
		}
	if ((tcp_header->reserved_flags & TCP_RST) > 0)
		{
		printf("RST ");
		}
	if ((tcp_header->reserved_flags & TCP_SYN) > 0)
		{
		printf("SYN ");
		}
	if ((tcp_header->reserved_flags & TCP_FIN) > 0)
		{
		printf("FIN ");
		}
	printf("\n\n");
	}

void print_tcp_status(int in_or_out, ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header)
	{
	printf("--- %s TCP packet: ---\n", (in_or_out == INC_PACKET ? "Incoming" : "Outgoing"));
	printf("IPv6 Source:");
	ipv6_print_addr(&ipv6_header->srcaddr);
	printf("IPv6 Dest:");
	ipv6_print_addr(&ipv6_header->destaddr);
	printf("TCP Length: %i\n", ipv6_header->length-TCP_HDR_LEN);
	printf("Source Port: %i, Dest. Port: %i\n", tcp_header->src_port, tcp_header->dst_port);
	printf("ACK: %li, SEQ: %li, Window: %i\n", tcp_header->ack_nr, tcp_header->seq_nr, tcp_header->window);
	print_tcp_flags(tcp_header);
	}

void print_socket(socket_t *current_socket)
	{
	printf("Domain: %i, Type: %i, Protocol: %i \n",
			current_socket->domain,
			current_socket->type,
			current_socket->protocol);
//	printf("Local address: \tPort: %i, \tFamily: %i\n",
//			NTOHS(current_socket->local_address.sin6_port),
//			current_socket->local_address.sin6_family);
	ipv6_print_addr(&current_socket->local_address.sin6_addr);
//	printf("Foreign address: \tPort: %i, \tFamily: %i\n",
//			NTOHS(current_socket->foreign_address.sin6_port),
//			current_socket->foreign_address.sin6_family);
	ipv6_print_addr(&current_socket->foreign_address.sin6_addr);
//	printf("Local TCP status: ACK: %li, SEQ: %li, STATE: %i\n",
//			current_socket->local_tcp_status.ack_nr,
//			current_socket->local_tcp_status.seq_nr,
//			current_socket->local_tcp_status.state);
//	printf("Foreign TCP status: ACK: %li, SEQ: %li, STATE: %i\n",
//				current_socket->foreign_tcp_status.ack_nr,
//				current_socket->foreign_tcp_status.seq_nr,
//				current_socket->foreign_tcp_status.state);
	}

void print_internal_socket(socket_internal_t *current_socket_internal)
	{
	socket_t *current_socket = &current_socket_internal->in_socket;
	printf("\n--------------------------\n");
	printf("ID: %i, PID: %i\n",	current_socket_internal->socket_id,	current_socket_internal->pid);
	print_socket(current_socket);
	printf("\n--------------------------\n");
	}

socket_internal_t *getSocket(uint8_t s)
	{
	if (exists_socket(s))
		{
		return &(sockets[s-1]);
		}
	else
		{
		return NULL;
		}
	}

void print_sockets(void)
	{
	int i;
	printf("\n---   Socket list:   ---\n");
	for (i = 1; i < MAX_SOCKETS+1; i++)
		{
		if(getSocket(i) != NULL)
			{
			print_internal_socket(getSocket(i));
			}
		}
	}

bool exists_socket(uint8_t socket)
	{
	if (sockets[socket-1].socket_id == 0)
		{
		return false;
		}
	else
		{
		return true;
		}
	}

void close_socket(socket_t *current_socket)
	{
	memset(current_socket, 0, sizeof(current_socket));
	}

bool isUDPSocket(uint8_t s)
	{
	if (	(exists_socket(s)) &&
			(getSocket(s)->in_socket.domain == PF_INET6) &&
			(getSocket(s)->in_socket.type == SOCK_DGRAM) &&
			((getSocket(s)->in_socket.protocol == IPPROTO_UDP) ||
			(getSocket(s)->in_socket.protocol == 0)))
		return true;
	else
		return false;
	}

bool isTCPSocket(uint8_t s)
	{
	if (	(exists_socket(s)) &&
			(getSocket(s)->in_socket.domain == PF_INET6) &&
			(getSocket(s)->in_socket.type == SOCK_STREAM) &&
			((getSocket(s)->in_socket.protocol == IPPROTO_TCP) ||
			(getSocket(s)->in_socket.protocol == 0)))
		return true;
	else
		return false;
	}

int bind_udp_socket(int s, sockaddr6_t *name, int namelen, uint8_t pid)
	{
	int i;
	if (!exists_socket(s))
		{
		return -1;
		}
	for (i = 1; i < MAX_SOCKETS+1; i++)
		{
		if (isUDPSocket(i) && (getSocket(i)->in_socket.local_address.sin6_port == name->sin6_port))
			{
			return -1;
			}
		}
	memcpy(&getSocket(s)->in_socket.local_address, name, namelen);
	getSocket(s)->pid = pid;
	return 1;
	}

int bind_tcp_socket(int s, sockaddr6_t *name, int namelen, uint8_t pid)
	{
	int i;
	if (!exists_socket(s))
		{
		return -1;
		}
	for (i = 1; i < MAX_SOCKETS+1; i++)
		{
		if (isTCPSocket(i) && (getSocket(i)->in_socket.local_address.sin6_port == name->sin6_port))
			{
			return -1;
			}
		}
	memcpy(&getSocket(s)->in_socket.local_address, name, namelen);
	getSocket(s)->pid = pid;
	return 1;
	}

int socket(int domain, int type, int protocol)
	{
	int i = 1;
	while (getSocket(i) != NULL)
		{
		i++;
		}
	if (i > MAX_SOCKETS+1)
		{
		return -1;
		}
	else
		{
		socket_t *current_socket = &sockets[i-1].in_socket;
		sockets[i-1].socket_id = i;
		current_socket->domain = domain;
		current_socket->type = type;
		current_socket->protocol = protocol;
		current_socket->local_tcp_status.state = CLOSED;
		current_socket->foreign_tcp_status.state = UNKNOWN;
		return sockets[i-1].socket_id;
		}
	}

socket_internal_t *get_udp_socket(ipv6_hdr_t *ipv6_header, udp_hdr_t *udp_header)
	{
	uint8_t i = 1;
	while (i < MAX_SOCKETS+1)
		{
		if ( isUDPSocket(i) &&
			(getSocket(i)->in_socket.local_address.sin6_port == udp_header->dst_port))
			{
			return getSocket(i);
			}
		i++;
		}
	return NULL;
	}

socket_internal_t *get_tcp_socket(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header)
	{
	uint8_t i = 1;
	socket_internal_t *current_socket = NULL;
	socket_internal_t *listening_socket = NULL;
	uint8_t compare[16];
	memset(compare, 0, 16);

	while (i < MAX_SOCKETS+1)
		{
		current_socket = getSocket(i);
		// Check for matching 4 touple
		if( isTCPSocket(i) &&
			(ipv6_get_addr_match(&current_socket->in_socket.local_address.sin6_addr, &ipv6_header->destaddr) == 128) &&
			(current_socket->in_socket.local_address.sin6_port == tcp_header->dst_port) &&
			(ipv6_get_addr_match(&current_socket->in_socket.foreign_address.sin6_addr, &ipv6_header->srcaddr) == 128) &&
			(current_socket->in_socket.foreign_address.sin6_port == tcp_header->src_port))
			{
			return current_socket;
			}
		// TODO: Figure out a better strategy of using *.LOCAL_ADRESS than using the last Byte
		else if ( isTCPSocket(i) &&
				((current_socket->in_socket.local_tcp_status.state == LISTEN) || (current_socket->in_socket.local_tcp_status.state == SYN_RCVD)) &&
				(current_socket->in_socket.local_address.sin6_addr.uint8[15] == ipv6_header->destaddr.uint8[15]) &&
				(current_socket->in_socket.local_address.sin6_port == tcp_header->dst_port) &&
				(current_socket->in_socket.foreign_address.sin6_addr.uint8[15] == 0x00) &&
				(current_socket->in_socket.foreign_address.sin6_port == 0))
			{
			listening_socket = current_socket;
			}
		i++;
		}
	// Return either NULL if nothing was matched or the listening 2 touple socket
	return listening_socket;
	}

uint16_t get_free_source_port(uint8_t protocol)
	{
	int i;
	uint16_t biggest_port = EPHEMERAL_PORTS-1;
	// Remember biggest ephemeral port number used so far and add 1
	for (i = 0; i < MAX_SOCKETS; i++)
		{
		if ((sockets[i].in_socket.protocol == protocol) && (sockets[i].in_socket.local_address.sin6_port > biggest_port))
			{
			biggest_port = sockets[i].in_socket.local_address.sin6_port;
			}
		}
	return biggest_port + 1;
	}

void set_tcp_status(tcp_socket_status_t *tcp_socket_status, uint32_t ack_nr, uint8_t mss, uint32_t seq_nr, uint8_t state, uint16_t window)
	{
	tcp_socket_status->ack_nr 	= ack_nr;
	tcp_socket_status->mss 		= mss;
	tcp_socket_status->seq_nr	= seq_nr;
	tcp_socket_status->state	= state;
	tcp_socket_status->window	= window;
	}

void set_socket_address(sockaddr6_t *sockaddr, uint8_t sin6_family, uint16_t sin6_port, uint32_t sin6_flowinfo, ipv6_addr_t *sin6_addr)
	{
	sockaddr->sin6_family 	= sin6_family;
	sockaddr->sin6_port 	= sin6_port;
	sockaddr->sin6_flowinfo	= sin6_flowinfo;
	memcpy(&sockaddr->sin6_addr, sin6_addr, 16);
	}

void set_tcp_packet(tcp_hdr_t *tcp_hdr, uint16_t src_port, uint16_t dst_port, uint32_t seq_nr, uint32_t ack_nr,
		uint8_t dataOffset_reserved, uint8_t reserved_flags, uint16_t window, uint16_t checksum, uint16_t urg_pointer)
	{
	tcp_hdr->ack_nr					= ack_nr;
	tcp_hdr->checksum				= checksum;
	tcp_hdr->dataOffset_reserved	= dataOffset_reserved;
	tcp_hdr->dst_port				= dst_port;
	tcp_hdr->reserved_flags			= reserved_flags;
	tcp_hdr->seq_nr					= seq_nr;
	tcp_hdr->src_port				= src_port;
	tcp_hdr->urg_pointer			= urg_pointer;
	tcp_hdr->window					= window;
	}

// Check for consistent ACK and SEQ number
int check_tcp_consistency(socket_t *current_tcp_socket, tcp_hdr_t *tcp_header)
	{
	if (IS_TCP_ACK(tcp_header->reserved_flags))
		{
		if(tcp_header->ack_nr > (current_tcp_socket->local_tcp_status.seq_nr+1))
			{
			// ACK of not yet sent byte, discard
			return ACK_NO_TOO_BIG;
			}
		else if (tcp_header->ack_nr < (current_tcp_socket->local_tcp_status.seq_nr+1))
			{
			// ACK of previous segments, maybe dropped?
			return ACK_NO_TOO_SMALL;
			}
		}
	if ((current_tcp_socket->foreign_tcp_status.seq_nr > 0) && (tcp_header->seq_nr <= current_tcp_socket->foreign_tcp_status.seq_nr))
		{
		// segment repetition, maybe ACK got lost?
		return SEQ_NO_SAME;
		}
	return PACKET_OK;
	}

int connect(int socket, sockaddr6_t *addr, uint32_t addrlen, uint8_t tcp_client_thread)
	{
	// Variables
	socket_internal_t *current_int_tcp_socket;
	socket_t *current_tcp_socket;
	msg_t msg_from_server, msg_reply_fin;
	uint8_t send_buffer[BUFFER_SIZE];
	ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t*)(&send_buffer));
	tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t*)(&send_buffer[IPV6_HDR_LEN]));

	// Check if socket exists
	current_int_tcp_socket = getSocket(socket);
	if (current_int_tcp_socket == NULL)
		{
		return -1;
		}

	current_tcp_socket = &current_int_tcp_socket->in_socket;

	// Set client thread process ID
	getSocket(socket)->pid = tcp_client_thread;

	// TODO: random number should be generated like common BSD socket implementation with a periodic timer increasing it
	// TODO: Add TCP MSS option field
	// Fill lcoal TCP socket information
	srand(addr->sin6_port);
	set_tcp_status(&current_tcp_socket->local_tcp_status, 0, STATIC_MSS, rand(), SYN_SENT, STATIC_WINDOW);

	// Fill foreign TCP socket information
	set_tcp_status(&current_tcp_socket->foreign_tcp_status, 0, 0, 0, UNKNOWN, 0);

	// Local address information
	ipv6_addr_t src_addr;
	ipv6_get_saddr(&src_addr, &addr->sin6_addr);
	set_socket_address(&current_tcp_socket->local_address, PF_INET6, HTONS(get_free_source_port(IPPROTO_TCP)), 0, &src_addr);

	// Foreign address information
	set_socket_address(&current_tcp_socket->foreign_address, addr->sin6_family, addr->sin6_port, addr->sin6_flowinfo, &addr->sin6_addr);

	// Fill TCP SYN packet
	set_tcp_packet(current_tcp_packet, current_tcp_socket->local_address.sin6_port, current_tcp_socket->foreign_address.sin6_port,
			current_tcp_socket->local_tcp_status.seq_nr, 0, 0, TCP_SYN, current_tcp_socket->local_tcp_status.window, 0, 0);
	// Fill IPv6 Header
	memcpy(&(temp_ipv6_header->destaddr), &current_tcp_socket->foreign_address.sin6_addr, 16);
	memcpy(&(temp_ipv6_header->srcaddr), &current_tcp_socket->local_address.sin6_addr, 16);
	temp_ipv6_header->length = TCP_HDR_LEN;

	current_tcp_packet->checksum = ~tcp_csum(temp_ipv6_header, current_tcp_packet);

	// send TCP SYN packet
	sixlowpan_send(&current_tcp_socket->foreign_address.sin6_addr, (uint8_t*)(current_tcp_packet), TCP_HDR_LEN, IPPROTO_TCP);

	// wait for SYN ACK
	msg_receive(&msg_from_server);

	// Read packet content
	tcp_hdr_t *tcp_header = ((tcp_hdr_t*)(buffer_tcp+IPV6_HDR_LEN));

	// Check for consistency
	if (check_tcp_consistency(current_tcp_socket, tcp_header) == -1)
		{
		printf("TCP packets not consistent!\n");
		}

	// Got SYN ACK from Server
	// Refresh foreign TCP socket information
	set_tcp_status(&current_tcp_socket->foreign_tcp_status, tcp_header->ack_nr, STATIC_MSS, tcp_header->seq_nr, ESTABLISHED, tcp_header->window);

	// Refresh local TCP socket information
	set_tcp_status(&current_tcp_socket->local_tcp_status, tcp_header->seq_nr + 1, STATIC_MSS, tcp_header->ack_nr, ESTABLISHED, STATIC_WINDOW);

	// Fill TCP ACK packet, use same IPv6 header
	set_tcp_packet(current_tcp_packet, current_tcp_socket->local_address.sin6_port, current_tcp_socket->foreign_address.sin6_port,
			current_tcp_socket->local_tcp_status.seq_nr, current_tcp_socket->local_tcp_status.ack_nr, 0, TCP_ACK, current_tcp_socket->local_tcp_status.window,
			0, 0);
	current_tcp_packet->checksum = ~tcp_csum(temp_ipv6_header, current_tcp_packet);

	sixlowpan_send(&current_tcp_socket->foreign_address.sin6_addr, (uint8_t*)(current_tcp_packet), TCP_HDR_LEN, IPPROTO_TCP);
	msg_reply(&msg_from_server, &msg_reply_fin);
	return 0;
	}

void set_tcp_packet_auto(tcp_hdr_t *current_tcp_packet, socket_t *current_socket)
	{
	set_tcp_packet(current_tcp_packet, current_socket->local_address.sin6_port, current_socket->foreign_address.sin6_port, current_socket->local_tcp_status.seq_nr,
			current_socket->local_tcp_status.ack_nr, 0, 0, current_socket->local_tcp_status.window, 0, 0);
	}

int32_t send(int s, void *msg, uint64_t len, int flags)
	{
	// Variables
	int32_t sent_bytes = 0;
	socket_internal_t *current_int_tcp_socket;
	socket_t *current_tcp_socket;
	uint8_t send_buffer[BUFFER_SIZE];
	ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t*)(&send_buffer));
	tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t*)(&send_buffer[IPV6_HDR_LEN]));

	// Check if socket exists and is TCP socket
	if (!isTCPSocket(s))
		{
		return -1;
		}

	current_int_tcp_socket = getSocket(s);
	current_tcp_socket = &current_int_tcp_socket->in_socket;

	// Check for ESTABLISHED STATE
	if (current_tcp_socket->local_tcp_status.state != ESTABLISHED)
		{
		return -1;
		}

	// Refresh local TCP socket information
	current_tcp_socket->local_tcp_status.seq_nr = current_tcp_socket->local_tcp_status.seq_nr + len;

	// IPv6 information
	memcpy(&(temp_ipv6_header->destaddr), &current_tcp_socket->foreign_address.sin6_addr, 16);
	memcpy(&(temp_ipv6_header->srcaddr), &current_tcp_socket->local_address.sin6_addr, 16);
	temp_ipv6_header->length = TCP_HDR_LEN+len;

	// Fill TCP packet header
	set_tcp_packet_auto(current_tcp_packet, current_tcp_socket);

	// Add packet data
	if (len > current_tcp_socket->foreign_tcp_status.window)
		{
		memcpy(&send_buffer[IPV6_HDR_LEN+TCP_HDR_LEN], msg, current_tcp_socket->foreign_tcp_status.window);
		sent_bytes = current_tcp_socket->foreign_tcp_status.window;
		}
	else
		{
		memcpy(&send_buffer[IPV6_HDR_LEN+TCP_HDR_LEN], msg, len);
		sent_bytes = len;
		}


	// Checksum
	current_tcp_packet->checksum = ~tcp_csum(temp_ipv6_header, current_tcp_packet);

	sixlowpan_send(&current_tcp_socket->foreign_address.sin6_addr, (uint8_t*)(current_tcp_packet), TCP_HDR_LEN+len, IPPROTO_TCP);
	return sent_bytes;
	}

uint8_t read_from_socket(socket_internal_t *current_int_tcp_socket, void *buf, int len)
	{
	if (len > current_int_tcp_socket->tcp_input_buffer_end)
		{
		uint8_t read_bytes = current_int_tcp_socket->tcp_input_buffer_end;
		memcpy(buf, current_int_tcp_socket->tcp_input_buffer, current_int_tcp_socket->tcp_input_buffer_end);
		current_int_tcp_socket->tcp_input_buffer_end = 0;
		return read_bytes;
		}
	else
		{
		memcpy(buf, current_int_tcp_socket->tcp_input_buffer, len);
		memmove(current_int_tcp_socket->tcp_input_buffer, (current_int_tcp_socket->tcp_input_buffer+len), current_int_tcp_socket->tcp_input_buffer_end-len);
		current_int_tcp_socket->tcp_input_buffer_end = current_int_tcp_socket->tcp_input_buffer_end-len;
		return len;
		}
	}

int32_t recv(int s, void *buf, uint64_t len, int flags)
	{
	// Variables
	msg_t m_recv;
	socket_internal_t *current_int_tcp_socket;
	socket_t *current_tcp_socket;

	// Check if socket exists
	if (!isTCPSocket(s))
		{
		return -1;
		}

	current_int_tcp_socket = getSocket(s);
	current_tcp_socket = &current_int_tcp_socket->in_socket;


	if (current_int_tcp_socket->tcp_input_buffer_end > 0)
		{
		return read_from_socket(current_int_tcp_socket, buf, len);
		}

	net_msg_receive(&m_recv, FID_RECV);

	if (current_int_tcp_socket->tcp_input_buffer_end > 0)
		{
		return read_from_socket(current_int_tcp_socket, buf, len);
		}

	return -1;
	}

int32_t recvfrom(int s, void *buf, uint64_t len, int flags, sockaddr6_t *from, uint32_t *fromlen)
	{
	if (isUDPSocket(s))
		{
		msg_t m_recv, m_send;
		ipv6_hdr_t *ipv6_header;
		udp_hdr_t *udp_header;
		uint8_t *payload;
		net_msg_receive(&m_recv, FID_RECV_FROM);
		ipv6_header = ((ipv6_hdr_t*)&buffer_udp);
		udp_header = ((udp_hdr_t*)(&buffer_udp[IPV6_HDR_LEN]));
		payload = &buffer_udp[IPV6_HDR_LEN+UDP_HDR_LEN];

		memset(buf, 0, len);
		memcpy(buf, payload, udp_header->length);
		memcpy(&from->sin6_addr, &ipv6_header->srcaddr, 16);
		from->sin6_family = AF_INET6;
		from->sin6_flowinfo = 0;
		from->sin6_port = udp_header->src_port;
		memcpy(fromlen, (void*)(sizeof(sockaddr6_t)), sizeof(fromlen));
		net_msg_reply(&m_recv, &m_send, FID_UDP_PH);
		return udp_header->length;
		}
	else if (isTCPSocket(s))
		{
		return recv(s, buf, len, flags);
		}
	else
		{
		printf("Socket Type not supported!\n");
		return -1;
		}
	}

int32_t sendto(int s, void *msg, uint64_t len, int flags, sockaddr6_t *to, uint32_t tolen)
	{
	if (isUDPSocket(s) && (getSocket(s)->in_socket.foreign_address.sin6_port == 0))
		{
		uint8_t send_buffer[BUFFER_SIZE];

		ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t*)(&send_buffer));
		udp_hdr_t *current_udp_packet = ((udp_hdr_t*)(&send_buffer[IPV6_HDR_LEN]));
		uint8_t *payload = &send_buffer[IPV6_HDR_LEN+UDP_HDR_LEN];

		ipv6_print_addr(&to->sin6_addr);

		memcpy(&(temp_ipv6_header->destaddr), &to->sin6_addr, 16);
		ipv6_get_saddr(&(temp_ipv6_header->srcaddr), &(temp_ipv6_header->destaddr));

		current_udp_packet->src_port = get_free_source_port(IPPROTO_UDP);
		current_udp_packet->dst_port = to->sin6_port;
		current_udp_packet->checksum = 0;

		memcpy(payload, msg, len);
		current_udp_packet->length = UDP_HDR_LEN + len;
		temp_ipv6_header->length = UDP_HDR_LEN + len;

		current_udp_packet->checksum = ~udp_csum(temp_ipv6_header, current_udp_packet);

		sixlowpan_send(&to->sin6_addr, (uint8_t*)(current_udp_packet), current_udp_packet->length, IPPROTO_UDP);
		return current_udp_packet->length;
		}
	else
		{
		return -1;
		}
	}

int close(int s)
	{
	socket_internal_t *current_socket = getSocket(s);
	if (current_socket != NULL)
		{
		// TODO: Kill connection, not just delete socket!
		memset(current_socket, 0, sizeof(socket_internal_t));

		return 1;
		}
	else
		{
		return -1;
		}
	}

int bind(int s, sockaddr6_t *name, int namelen, uint8_t pid)
	{
	if (exists_socket(s))
		{
		socket_t *current_socket = &getSocket(s)->in_socket;
		switch (current_socket->domain)
			{
			case (PF_INET):
				{
				// Not provided
				return -1;
				break;
				}
			case (PF_INET6):
				{
				switch (current_socket->type)
					{
					// TCP
					case (SOCK_STREAM):
						{
						if ((current_socket->protocol == 0) || (current_socket->protocol == IPPROTO_TCP))
							{
							return bind_tcp_socket(s, name, namelen, pid);
							break;
							}
						else
							{
							return -1;
							break;
							}
						break;
						}
					// UDP
					case (SOCK_DGRAM):
						{
						if ((current_socket->protocol == 0) || (current_socket->protocol == IPPROTO_UDP))
							{
							return bind_udp_socket(s, name, namelen, pid);
							break;
							}
						else
							{
							return -1;
							break;
							}
						break;
						}
					case (SOCK_SEQPACKET):
						{
						// not provided
						return -1;
						break;
						}
					case (SOCK_RAW):
						{
						// not provided
						return -1;
						break;
						}
					default:
						{
						return -1;
						break;
						}
					}
				break;
				}
			case (PF_UNIX):
				{
				// Not provided
				return -1;
				break;
				}
			}
		}
	else
		{
		printf("SOCKET DOES NOT EXIST!\n");
		return -1;
		}
	return -1;
	}

int listen(int s, int backlog)
	{
	if (isTCPSocket(s) && getSocket(s)->in_socket.local_tcp_status.state == CLOSED)
		{
		socket_internal_t *current_socket = getSocket(s);
		current_socket->in_socket.local_tcp_status.state = LISTEN;
		memset(current_socket->queued_sockets, 0, MAX_QUEUED_SOCKETS*sizeof(socket_t));
		return 0;
		}
	else
		{
		return -1;
		}
	}

socket_t *getWaitingConnectionSocket(int socket)
	{
	int i;
	for (i = 0; i < MAX_QUEUED_SOCKETS; i++)
		{
		if (getSocket(socket)->queued_sockets[i].local_tcp_status.state == SYN_RCVD)
			{
			return &getSocket(socket)->queued_sockets[i];
			}
		}
	return NULL;
	}

int handle_new_tcp_connection(socket_t *current_queued_socket, socket_internal_t *server_socket, uint8_t pid)
	{
	// Got new connection request (SYN), get new socket number and establish connection (send SYN ACK)
	int new_socket;
	msg_t msg_recv_client_ack, msg_send_client_ack;
	socket_internal_t *current_new_socket;
	uint8_t send_buffer[BUFFER_SIZE];
	ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t*)(&send_buffer));
	tcp_hdr_t *syn_ack_packet = ((tcp_hdr_t*)(&send_buffer[IPV6_HDR_LEN]));

	// Set status of internal socket to SYN_RCVD until connection with second socket is established!
	server_socket->in_socket.local_tcp_status.state = SYN_RCVD;

	// Fill SYN ACK TCP packet, still use queued socket for port number until connection is completely established!
	// Otherwise the program doesnt return to this function and instead trys to call the new registered thread
	// which isnt prepared to complete the threeway handshake process!
	set_tcp_packet(syn_ack_packet, server_socket->in_socket.local_address.sin6_port, current_queued_socket->foreign_address.sin6_port,
			current_queued_socket->local_tcp_status.seq_nr, current_queued_socket->local_tcp_status.ack_nr, 0, TCP_SYN_ACK,
			current_queued_socket->local_tcp_status.window, 0, 0);

	// Specify IPV6 target address and choose outgoing interface source address
	memcpy(&(temp_ipv6_header->destaddr), &current_queued_socket->foreign_address.sin6_addr, 16);
	ipv6_get_saddr(&(temp_ipv6_header->srcaddr), &(temp_ipv6_header->destaddr));
	temp_ipv6_header->length = TCP_HDR_LEN;

	syn_ack_packet->checksum = ~tcp_csum(temp_ipv6_header, syn_ack_packet);

	printf("BEFORE SENDING SYN ACK PACKET!\n");
	sixlowpan_send(&current_queued_socket->foreign_address.sin6_addr, (uint8_t*)(syn_ack_packet), TCP_HDR_LEN, IPPROTO_TCP);

	// wait for ACK from Client
	msg_receive(&msg_recv_client_ack);

	ipv6_hdr_t *ipv6_header;
	tcp_hdr_t *tcp_header;

	ipv6_header = ((ipv6_hdr_t*)(buffer_tcp));
	tcp_header = ((tcp_hdr_t*)(buffer_tcp+IPV6_HDR_LEN));

	// Check for consistency
	if (check_tcp_consistency(current_queued_socket, tcp_header) != PACKET_OK)
		{
		printf("TCP packets not consistent!\n");
		}

	// Got ack, connection established, refresh local and foreign tcp socket status
	set_tcp_status(&current_queued_socket->foreign_tcp_status, tcp_header->ack_nr, STATIC_MSS, tcp_header->seq_nr, ESTABLISHED, tcp_header->window);
	set_tcp_status(&current_queued_socket->local_tcp_status, tcp_header->seq_nr+1, STATIC_MSS, tcp_header->ack_nr, ESTABLISHED, STATIC_WINDOW);

	// Set status of internal socket back to LISTEN
	server_socket->in_socket.local_tcp_status.state = LISTEN;

	// send a reply to the TCP handler after processing every information from the TCP ACK packet
	msg_reply(&msg_recv_client_ack, &msg_send_client_ack);

	new_socket = socket(current_queued_socket->domain, current_queued_socket->type, current_queued_socket->protocol);
	current_new_socket = getSocket(new_socket);

	current_new_socket->pid = pid;
	memcpy(&current_new_socket->in_socket, current_queued_socket, sizeof(socket_t));
	close_socket(current_queued_socket);
	print_sockets();
	return new_socket;
	}

int accept(int s, sockaddr6_t *addr, uint32_t addrlen, uint8_t pid)
	{
	socket_internal_t *server_socket = getSocket(s);
	if (isTCPSocket(s) && (server_socket->in_socket.local_tcp_status.state == LISTEN))
		{
		socket_t *current_queued_socket = getWaitingConnectionSocket(s);
		if (current_queued_socket != NULL)
			{
			return handle_new_tcp_connection(current_queued_socket, server_socket, pid);
			}
		else
			{
			// No waiting connections, waiting for message from TCP Layer
			msg_t msg_recv_client_syn;
			msg_receive(&msg_recv_client_syn);

			current_queued_socket = getWaitingConnectionSocket(s);

			return handle_new_tcp_connection(current_queued_socket, server_socket, pid);
			}
		}
	else
		{
		return -1;
		}
	}

int shutdown(int s , int how)
	{
	return -1;
	}

socket_t *new_tcp_queued_socket(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header, socket_internal_t *socket)
	{
	int i, free_slot = -1;
	for (i = 0; i < MAX_QUEUED_SOCKETS; i++)
		{
		if (socket->queued_sockets[i].type == 0)
			{
			// Found a free slot, remember for usage later
			free_slot = i;
			}
		else if (socket->queued_sockets[i].foreign_address.sin6_port == tcp_header->src_port)
			{
			// Already registered as waiting connection
			return NULL;
			}
		}

	socket_t *current_queued_socket = &socket->queued_sockets[free_slot];
	current_queued_socket->domain = PF_INET6;
	current_queued_socket->type = SOCK_STREAM;
	current_queued_socket->protocol = IPPROTO_TCP;

	// Foreign address
	set_socket_address(&current_queued_socket->foreign_address, AF_INET6, tcp_header->src_port, ipv6_header->flowlabel, &ipv6_header->srcaddr);

	// Local address
	set_socket_address(&current_queued_socket->local_address, AF_INET6, tcp_header->dst_port, 0, &ipv6_header->destaddr);

	// Foreign TCP information
	set_tcp_status(&current_queued_socket->foreign_tcp_status, tcp_header->ack_nr, STATIC_MSS, tcp_header->seq_nr, SYN_SENT, tcp_header->window);

	// Local TCP information
	srand(tcp_header->src_port);
	set_tcp_status(&current_queued_socket->local_tcp_status, tcp_header->seq_nr+1, STATIC_MSS, rand(), SYN_RCVD, STATIC_WINDOW);

	return current_queued_socket;
	}
