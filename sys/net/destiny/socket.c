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

void print_socket(uint8_t socket)
	{
	socket = socket - 1;
	printf ("---SOCKET---\n");
	printf("ID: %i, Domain: %i, Type: %i, Protocol: %i \n", sockets[socket].socket_id, sockets[socket].in_socket.domain, sockets[socket].in_socket.type, sockets[socket].in_socket.protocol);
	printf("Local address: Port: %i, Family: %i\n", NTOHS(sockets[socket].in_socket.local_address.sin6_port), sockets[socket].in_socket.local_address.sin6_family);
	ipv6_print_addr(&sockets[socket].in_socket.local_address.sin6_addr);

	printf("Foreign address: Port: %i, Family: %i\n", NTOHS(sockets[socket].in_socket.foreign_address.sin6_port), sockets[socket].in_socket.foreign_address.sin6_family);
	ipv6_print_addr(&sockets[socket].in_socket.foreign_address.sin6_addr);
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

socket_internal_t* getSocket(uint8_t s)
	{
	if (exists_socket(s))
		return &sockets[s-1];
	else
		return NULL;
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

int bind_udp_socket(int s, struct sockaddr6 *name, int namelen, uint8_t pid)
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

int bind_tcp_socket(int s, struct sockaddr6 *name, int namelen, uint8_t pid)
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
		struct socket_t *current_socket = &sockets[i-1].in_socket;
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
			(current_socket->in_socket.local_tcp_status.state == ESTABLISHED) &&
			CMP_IPV6_ADDR(&current_socket->in_socket.local_address, &ipv6_header->destaddr) &&
			(current_socket->in_socket.local_address.sin6_port == tcp_header->dst_port) &&
			CMP_IPV6_ADDR(&current_socket->in_socket.foreign_address, &ipv6_header->srcaddr) &&
			(current_socket->in_socket.foreign_address.sin6_port == tcp_header->src_port))
			{
			return current_socket;
			}
		// Check for matching 2 touple
		else if ( isTCPSocket(i) &&
				(current_socket->in_socket.local_tcp_status.state == LISTEN) &&
				CMP_IPV6_ADDR(&current_socket->in_socket.local_address, &ipv6_header->destaddr) &&
				(current_socket->in_socket.local_address.sin6_port == tcp_header->dst_port) &&
				CMP_IPV6_ADDR(&current_socket->in_socket.foreign_address, &compare) &&
				(current_socket->in_socket.foreign_address.sin6_port == 0))
			{
			listening_socket = *(&current_socket);
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

int connect(int socket, struct sockaddr6 *addr, uint32_t addrlen)
	{
	// Variables
	socket_t *current_tcp_socket;
	msg_t msg_from_server;
	uint8_t send_buffer[BUFFER_SIZE];
	ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t*)(&send_buffer));
	tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t*)(&send_buffer[IPV6_HDR_LEN]));

	// Fill lcoal TCP socket information
	// TODO: random number should be generated like common BSD socket implementation with a periodic timer increasing it
	current_tcp_socket = &getSocket(socket)->in_socket;
	current_tcp_socket->local_tcp_status.ack_nr = 0; // Nothing to ACK yet
	current_tcp_socket->local_tcp_status.seq_nr = rand_r((unsigned int *)(&addr->sin6_port));
	current_tcp_socket->local_tcp_status.state = SYN_SENT;
	current_tcp_socket->local_tcp_status.window = 32;
	// TODO: Add TCP MSS option field
	current_tcp_socket->local_tcp_status.mss = 32;

	// Fill foreign TCP socket information
	current_tcp_socket->foreign_tcp_status.ack_nr = 0; // Nothing acked yet
	current_tcp_socket->foreign_tcp_status.seq_nr = 0;
	current_tcp_socket->foreign_tcp_status.state = UNKNOWN;
	current_tcp_socket->foreign_tcp_status.window = 0;
	current_tcp_socket->foreign_tcp_status.mss = 0;

	// Local address information
	ipv6_get_saddr(&(current_tcp_socket->local_address.sin6_addr), &(addr->sin6_addr));
	current_tcp_socket->local_address.sin6_port = get_free_source_port(IPPROTO_TCP);
	current_tcp_socket->local_address.sin6_family = PF_INET6;
	current_tcp_socket->local_address.sin6_flowinfo = 0;

	// Foreign address information
	memcpy (&current_tcp_socket->local_address.sin6_addr, &addr->sin6_addr, 16);
	current_tcp_socket->local_address.sin6_port = addr->sin6_port;
	current_tcp_socket->local_address.sin6_family = addr->sin6_family;
	current_tcp_socket->local_address.sin6_flowinfo = addr->sin6_flowinfo;

	// TODO: Add TCP MSS option field
	current_tcp_socket->local_tcp_status.mss = 32;

	// Fill TCP SYN packet
	current_tcp_packet->ack_nr = 0;
	current_tcp_packet->dataOffset_reserved = 0;
	current_tcp_packet->dst_port = current_tcp_socket->foreign_address.sin6_port;
	SET_TCP_SYN(current_tcp_packet->reserved_flags);
	current_tcp_packet->seq_nr = current_tcp_socket->local_tcp_status.seq_nr;
	current_tcp_packet->src_port = current_tcp_socket->local_address.sin6_port;
	current_tcp_packet->urg_pointer = 0;
	current_tcp_packet->window = current_tcp_socket->local_tcp_status.window;

	// send TCP SYN packet
	memcpy(&(temp_ipv6_header->destaddr), &current_tcp_socket->foreign_address.sin6_addr, 16);
	memcpy(&(temp_ipv6_header->srcaddr), &current_tcp_socket->local_address.sin6_addr, 16);

	temp_ipv6_header->length = TCP_HDR_LEN;

	current_tcp_packet->checksum = ~tcp_csum(temp_ipv6_header, current_tcp_packet);

	printf("Content of IPv6 Packet: length: %i\n", temp_ipv6_header->length);
	printf("Content of TCP Packet: src_port: %i, dst_port: %i, checksum: %x\n", NTOHS(current_tcp_packet->src_port), NTOHS(current_tcp_packet->dst_port),
			current_tcp_packet->checksum);
	sixlowpan_send(&current_tcp_socket->foreign_address.sin6_addr, (uint8_t*)(current_tcp_packet), IPV6_HDR_LEN+TCP_HDR_LEN, IPPROTO_TCP);

	// wait for SYN ACK
	msg_receive(&msg_from_server);

	// Read packet content
	tcp_hdr_t *tcp_header = ((struct tcp_hdr_t*)(msg_from_server.content.ptr+IPV6_HDR_LEN));

	// Got SYN ACK from Server
	// Refresh foreign TCP socket information
	current_tcp_socket->foreign_tcp_status.ack_nr = tcp_header->ack_nr; // Nothing acked yet
	current_tcp_socket->foreign_tcp_status.seq_nr = tcp_header->seq_nr;
	current_tcp_socket->foreign_tcp_status.state = SYN_RCVD;
	current_tcp_socket->foreign_tcp_status.window = tcp_header->window;
	// TODO: Introduce TCP MSS Option
	current_tcp_socket->foreign_tcp_status.mss = 32;

	// Refresh local TCP socket information
	current_tcp_socket->local_tcp_status.ack_nr = tcp_header->seq_nr + 1;
	current_tcp_socket->local_tcp_status.seq_nr = tcp_header->ack_nr;
	current_tcp_socket->local_tcp_status.state = ESTABLISHED;

	current_tcp_packet->ack_nr = current_tcp_socket->local_tcp_status.ack_nr;
	current_tcp_packet->dataOffset_reserved = 0;
	current_tcp_packet->dst_port = current_tcp_socket->foreign_address.sin6_port;
	SET_TCP_ACK(current_tcp_packet->reserved_flags);
	current_tcp_packet->seq_nr = current_tcp_socket->local_tcp_status.seq_nr;
	current_tcp_packet->src_port = current_tcp_socket->local_address.sin6_port;
	current_tcp_packet->urg_pointer = 0;
	current_tcp_packet->window = current_tcp_socket->local_tcp_status.window;

	current_tcp_packet->checksum = ~tcp_csum(temp_ipv6_header, current_tcp_packet);

	printf("Content of IPv6 Packet: length: %i\n", temp_ipv6_header->length);
	printf("Content of TCP Packet: src_port: %i, dst_port: %i, checksum: %x\n", NTOHS(current_tcp_packet->src_port), NTOHS(current_tcp_packet->dst_port),
			current_tcp_packet->checksum);
	sixlowpan_send(&current_tcp_socket->foreign_address.sin6_addr, (uint8_t*)(current_tcp_packet), IPV6_HDR_LEN+TCP_HDR_LEN, IPPROTO_TCP);
	return 0;
	}

int32_t send(int s, void *msg, uint64_t len, int flags)
	{
	return -1;
	}

int32_t recv(int s, void *buf, uint64_t len, int flags)
	{
	return -1;
	}

int32_t recvfrom(int s, void *buf, uint64_t len, int flags, struct sockaddr6 *from, uint32_t *fromlen)
	{
	if (isUDPSocket(s))
		{
		msg_t m_recv, m_send;
		struct ipv6_hdr_t *ipv6_header;
		struct udp_hdr_t *udp_header;
		uint8_t *payload;
		uint16_t payload_size = 0;
		msg_receive(&m_recv);

		ipv6_header = ((struct ipv6_hdr_t*)&buffer_udp);
		udp_header = ((struct udp_hdr_t*)(&buffer_udp[IPV6_HDR_LEN]));
		payload = &buffer_udp[IPV6_HDR_LEN+UDP_HDR_LEN];

		memset(buf, 0, len);
		memcpy(buf, payload, udp_header->length);
		payload_size = udp_header->length;
		memcpy(&from->sin6_addr, &ipv6_header->srcaddr, 16);
		memset(&from->sin6_family, AF_INET6, 1);
		memset(&from->sin6_flowinfo, 0, 4);
		memcpy(&from->sin6_port, &udp_header->src_port, sizeof(udp_header->src_port));
		memcpy(fromlen, (void*)(sizeof(sockaddr6)), sizeof(fromlen));
		msg_reply(&m_recv, &m_send);
		return payload_size;
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

int32_t sendto(int s, void *msg, uint64_t len, int flags, struct sockaddr6 *to, uint32_t tolen)
	{
	if (exists_socket(s) && isUDPSocket(s) && (getSocket(s)->in_socket.foreign_address.sin6_port == 0))
		{
		uint8_t send_buffer[BUFFER_SIZE];

		struct ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t*)(&send_buffer));
		struct udp_hdr_t *current_udp_packet = ((udp_hdr_t*)(&send_buffer[IPV6_HDR_LEN]));
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

		printf("Content of IPv6 Packet: length: %i\n", temp_ipv6_header->length);
		printf("Content of UDP Packet: src_port: %i, dst_port: %i, length: %i, checksum: %x\n", NTOHS(current_udp_packet->src_port), NTOHS(current_udp_packet->dst_port),
				current_udp_packet->length, current_udp_packet->checksum);
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
	if (exists_socket(s))
		{
		memset(getSocket(s), 0, sizeof(socket_t));
		return 1;
		}
	else
		{
		return -1;
		}
	}

int bind(int s, struct sockaddr6 *name, int namelen, uint8_t pid)
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
		if (getSocket(socket)->queued_sockets[0].type != 0)
			{
			return &getSocket(socket)->queued_sockets[0];
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

	// Fill SYN ACK TCP packet, still use queued socket for port number until connection is completely established!
	// Otherwise the program doesnt return to this function and instead trys to call the new registered thread
	// which isnt prepared to complete the threeway handshake process!
	syn_ack_packet->ack_nr = current_queued_socket->local_tcp_status.seq_nr;
	syn_ack_packet->dataOffset_reserved = 0;
	syn_ack_packet->dst_port = current_queued_socket->foreign_address.sin6_port;
	SET_TCP_SYN_ACK(syn_ack_packet->reserved_flags);
	syn_ack_packet->seq_nr = current_queued_socket->local_tcp_status.seq_nr;
	syn_ack_packet->src_port = server_socket->in_socket.local_address.sin6_port;
	syn_ack_packet->urg_pointer = 0;
	syn_ack_packet->window = current_queued_socket->local_tcp_status.window;

	// Specify IPV6 target address and choose outgoing interface source address
	memcpy(&(temp_ipv6_header->destaddr), &current_queued_socket->foreign_address.sin6_addr, 16);
	ipv6_get_saddr(&(temp_ipv6_header->srcaddr), &(current_queued_socket->local_address.sin6_addr));

	temp_ipv6_header->length = TCP_HDR_LEN;

	syn_ack_packet->checksum = ~tcp_csum(temp_ipv6_header, syn_ack_packet);

	printf("Content of IPv6 Packet: length: %i\n", temp_ipv6_header->length);
	printf("Content of TCP Packet: src_port: %i, dst_port: %i, checksum: %x\n", NTOHS(syn_ack_packet->src_port), NTOHS(syn_ack_packet->dst_port),
			syn_ack_packet->checksum);
	sixlowpan_send(&current_queued_socket->foreign_address.sin6_addr, (uint8_t*)(syn_ack_packet), IPV6_HDR_LEN+TCP_HDR_LEN, IPPROTO_TCP);


	// wait for ACK from Client
	msg_receive(&msg_recv_client_ack);

	// Got ack, connection established
	current_queued_socket->local_tcp_status.state = ESTABLISHED;
	current_queued_socket->foreign_tcp_status.state = ESTABLISHED;

	// send a reply to the TCP handler after processing every information from the TCP ACK packet
	msg_reply(&msg_recv_client_ack, &msg_send_client_ack);

	new_socket = socket(current_queued_socket->domain, current_queued_socket->type, current_queued_socket->protocol);
	current_new_socket = getSocket(new_socket);

	current_new_socket->pid = pid;
	memcpy(&current_new_socket->in_socket, &current_queued_socket, sizeof(current_new_socket));
	return new_socket;
	}

int accept(int s, struct sockaddr6 *addr, uint32_t addrlen, uint8_t pid)
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
	memcpy(&current_queued_socket->foreign_address.sin6_addr, (void*) &ipv6_header->srcaddr, sizeof(ipv6_header->srcaddr));
	current_queued_socket->foreign_address.sin6_family = AF_INET6;
	current_queued_socket->foreign_address.sin6_flowinfo = ipv6_header->flowlabel;
	current_queued_socket->foreign_address.sin6_port = tcp_header->src_port;

	// Local address
	memcpy(&current_queued_socket->local_address.sin6_addr, (void*) &ipv6_header->destaddr, sizeof(ipv6_header->destaddr));
	current_queued_socket->local_address.sin6_family = AF_INET6;
	current_queued_socket->local_address.sin6_flowinfo = 0;
	current_queued_socket->local_address.sin6_port = tcp_header->dst_port;

	// Foreign TCP information
	current_queued_socket->foreign_tcp_status.ack_nr = tcp_header->ack_nr;
	current_queued_socket->foreign_tcp_status.mss = 32;
	current_queued_socket->foreign_tcp_status.seq_nr = tcp_header->seq_nr;
	current_queued_socket->foreign_tcp_status.state = SYN_SENT;
	current_queued_socket->foreign_tcp_status.window = tcp_header->window;

	// Local TCP information
	current_queued_socket->local_tcp_status.ack_nr = tcp_header->seq_nr+1;
	current_queued_socket->local_tcp_status.mss = 32;
	current_queued_socket->local_tcp_status.seq_nr = rand_r((unsigned int *)(&tcp_header->src_port));
	current_queued_socket->local_tcp_status.state = SYN_RCVD;
	current_queued_socket->local_tcp_status.window = 32;

	return current_queued_socket;
	}
