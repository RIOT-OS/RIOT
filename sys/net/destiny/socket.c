/*
 * socket.c
 *
 *  Created on: 16.09.2011
 *      Author: Oliver
 */
#include <thread.h>
#include <stdio.h>
#include <string.h>
#include "udp.h"
#include "tcp.h"
#include "socket.h"

void print_socket(uint8_t socket)
	{
	socket = socket - 1;
	printf ("---SOCKET---\n");
	printf("Value: %i, Domain: %i, Type: %i, Protocol: %i \n", sockets[socket].in_socket.socket_id, sockets[socket].in_socket.domain, sockets[socket].in_socket.type, sockets[socket].in_socket.protocol);
	printf("Port: %i, Family: %i\n", sockets[socket].in_socket.sa.sin6_port, sockets[socket].in_socket.sa.sin6_family);
	ipv6_print_addr(&sockets[socket].in_socket.sa.sin6_addr);
	}

bool exists_socket(uint8_t socket)
	{
	if (sockets[socket-1].in_socket.socket_id == 0)
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

int bind_udp_socket(int s, struct sockaddr_in6 *name, int namelen, uint8_t pid)
	{
	int i;
	if (!exists_socket(s))
		{
		return -1;
		}
	for (i = 1; i < MAX_SOCKETS+1; i++)
		{
		if (isUDPSocket(i) && (getSocket(i)->in_socket.sa.sin6_port == name->sin6_port))
			{
			return -1;
			}
		}
	memcpy(&getSocket(s)->in_socket.sa, name, namelen);
	getSocket(s)->pid = pid;
	return 1;
	}

int bind_tcp_socket(int s, struct sockaddr_in6 *name, int namelen, uint8_t pid)
	{
	int i;
	if (!exists_socket(s))
		{
		return -1;
		}
	for (i = 1; i < MAX_SOCKETS+1; i++)
		{
		if (isTCPSocket(i) && (getSocket(i)->in_socket.sa.sin6_port == name->sin6_port))
			{
			return -1;
			}
		}
	memcpy(&getSocket(s)->in_socket.sa, name, namelen);
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
		current_socket->socket_id = i;
		current_socket->domain = domain;
		current_socket->type = type;
		current_socket->protocol = protocol;
		return current_socket->socket_id;
		}
	}

int connect(int socket, struct sockaddr_in6 *addr, uint32_t addrlen)
	{
	return -1;
	}

int32_t send(int s, void *msg, uint64_t len, int flags)
	{
	return -1;
	}

int32_t recv(int s, void *buf, uint64_t len, int flags)
	{
	return -1;
	}

int32_t recvfrom(int s, void *buf, uint64_t len, int flags, struct sockaddr_in6 *from, uint32_t *fromlen)
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
		memcpy(fromlen, (void*)(sizeof(sockaddr_in6)), sizeof(fromlen));
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

int32_t sendto(int s, void *msg, uint64_t len, int flags, struct sockaddr_in6 *to, uint32_t tolen)
	{
	if (exists_socket(s) && isUDPSocket(s) && (getSocket(s)->in_socket.sa.sin6_port == 0))
		{
		uint8_t send_buffer[BUFFER_SIZE];

		struct ipv6_hdr_t *temp_ipv6_header = ((struct ipv6_hdr_t*)(&send_buffer));
		struct udp_hdr_t *temp_udp_header = ((struct udp_hdr_t*)(&send_buffer[IPV6_HDR_LEN]));
		uint8_t *payload = &send_buffer[IPV6_HDR_LEN+UDP_HDR_LEN];

		ipv6_print_addr(&to->sin6_addr);

		memcpy(&(temp_ipv6_header->destaddr), &to->sin6_addr, 16);
		ipv6_get_saddr(&(temp_ipv6_header->srcaddr), &(temp_ipv6_header->destaddr));
		temp_ipv6_header->version_trafficclass = IPV6_VER;
		temp_ipv6_header->trafficclass_flowlabel = 0;
		temp_ipv6_header->flowlabel = 0;
		temp_ipv6_header->nextheader = IPPROTO_UDP;
		temp_ipv6_header->hoplimit = MULTIHOP_HOPLIMIT;


		temp_udp_header->src_port = 0;
		temp_udp_header->dst_port = to->sin6_port;
		temp_udp_header->checksum = 0;

		memcpy(payload, msg, len);
		temp_udp_header->length = UDP_HDR_LEN + len;
		temp_ipv6_header->length = UDP_HDR_LEN + len;

		temp_udp_header->checksum = ~udp_csum(temp_ipv6_header, temp_udp_header);

		printf("Content of IPv6 Packet: length: %i\n", temp_ipv6_header->length);
		printf("Content of UDP Packet: src_port: %i, dst_port: %i, length: %i, checksum: %x\n", temp_udp_header->src_port, temp_udp_header->dst_port, temp_udp_header->length, temp_udp_header->checksum);
		sixlowpan_send(&to->sin6_addr, (uint8_t*)(temp_udp_header), temp_udp_header->length, IPPROTO_UDP);
		return temp_udp_header->length;
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

int bind(int s, struct sockaddr_in6 *name, int namelen, uint8_t pid)
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
	if (isTCPSocket(s) && getSocket(s)->in_socket.tcp_socket_status.state == CLOSED)
		{
		socket_internal_t *current_socket = getSocket(s);
		current_socket->in_socket.tcp_socket_status.state = LISTEN;
		memset(current_socket->queued_sockets, 0, MAX_QUEUED_SOCKETS*sizeof(socket_t));
		return 0;
		}
	else
		{
		return -1;
		}
	}

int accept(int s, struct sockaddr_in6 *addr, uint32_t addrlen)
	{
	if (isTCPSocket(s) && (getSocket(s)->in_socket.tcp_socket_status.state == LISTEN))
		{
		if (getSocket(s)->queued_sockets[0].socket_id != 0)
			{
			// Got waiting Connections (SYN received), get new socket number and establish connection
			return 1;
			}
		else
			{
			// No waiting connections, waiting for message from TCP Layer
			msg_t msg_recv;
			msg_receive(&msg_recv);
			// Got new connection request, get new socket number and establish connection:

			return 1;
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

socket_internal_t *get_udp_socket(uint16_t port)
	{
	uint8_t i = 1;
	while (i < MAX_SOCKETS+1)
		{
		if ((exists_socket(i)) && isUDPSocket(i) && (getSocket(i)->in_socket.sa.sin6_port == port))
			{
			return getSocket(i);
			}
		i++;
		}
	return NULL;
	}

socket_internal_t *get_tcp_socket(uint16_t port)
	{
	uint8_t i = 1;
	while (i < MAX_SOCKETS+1)
		{
		if ((exists_socket(i)) && isTCPSocket(i) && (getSocket(i)->in_socket.sa.sin6_port == port))
			{
			return getSocket(i);
			}
		i++;
		}
	return NULL;
	}

socket_t *new_tcp_queued_socket(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header, socket_internal_t *socket)
	{
	int i;
	for (i = 0; i < MAX_QUEUED_SOCKETS; i++)
		{
		if (socket->queued_sockets[i].socket_id == 0)
			{
			socket_t *current_socket = &socket->queued_sockets[i];
			current_socket->socket_id = i+1;
			current_socket->domain = PF_INET6;
			current_socket->type = SOCK_STREAM;
			current_socket->protocol = IPPROTO_TCP;

			memcpy(&current_socket->sa.sin6_addr, (void*) &ipv6_header->srcaddr, sizeof(ipv6_header->srcaddr));
			current_socket->sa.sin6_family = AF_INET6;

			// TODO: Decide what to do about "sa.sin6_flowlinfo"
			//memcpy(current_socket->sa.sin6_flowinfo, ipv6_header->flowlabel, sizeof(ipv6_header->flowlabel));

			memcpy(&current_socket->sa.sin6_port, (void*) &tcp_header->src_port, sizeof(tcp_header->src_port));

			// TCP Values
			memcpy(&current_socket->tcp_socket_status.ack_nr, (void*) tcp_header->ack_nr, sizeof(tcp_header->ack_nr));
			memcpy(&current_socket->tcp_socket_status.seq_nr, (void*) tcp_header->seq_nr, sizeof(tcp_header->seq_nr));
			memcpy(&current_socket->tcp_socket_status.window, (void*) &tcp_header->window, sizeof(tcp_header->window));
			current_socket->tcp_socket_status.state = SYN_RCVD;
			return current_socket;
			}
		else if (socket->queued_sockets[i].sa.sin6_port == tcp_header->src_port)
			{
			return NULL;
			}
		}
	return NULL;
	}
