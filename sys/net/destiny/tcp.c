/*
 * tcp.c
 *
 *  Created on: 29.09.2011
 *      Author: Oliver
 */

#include <stdio.h>
#include <thread.h>
#include <string.h>

#include "tcp.h"
#include "in.h"
#include "socket.h"
#include "sys/net/net_help/net_help.h"
#include "sys/net/net_help/msg_help.h"

void printTCPHeader(tcp_hdr_t *tcp_header)
	{
	printf("\nBEGIN: TCP HEADER\n");
	printf("ack_nr: %lu\n", tcp_header->ack_nr);
	printf("checksum: %i\n", tcp_header->checksum);
	printf("dataOffset_reserved: %i\n", tcp_header->dataOffset_reserved);
	printf("dst_port: %i\n", tcp_header->dst_port);
	printf("reserved_flags: %i\n", tcp_header->reserved_flags);
	printf("seq_nr: %lu\n", tcp_header->seq_nr);
	printf("src_port: %i\n", tcp_header->src_port);
	printf("urg_pointer: %i\n", tcp_header->urg_pointer);
	printf("window: %i\n", tcp_header->window);
	printf("END: TCP HEADER\n");
	}

void printArrayRange_tcp(uint8_t *udp_header, uint16_t len)
	{
	int i = 0;
	printf("-------------MEMORY-------------\n");
	for (i = 0; i < len; i++)
		{
		printf("%#x ", *(udp_header+i));
		}
	printf("-------------MEMORY-------------\n");
	}

uint16_t tcp_csum(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header)
	{
    uint16_t sum;
    uint16_t len = ipv6_header->length;

	sum = len + IPPROTO_TCP;
	sum = csum(sum, (uint8_t *)&ipv6_header->srcaddr, 2 * sizeof(ipv6_addr_t));
	sum = csum(sum, (uint8_t *)tcp_header, len);
    return (sum == 0) ? 0xffff : HTONS(sum);
	}

uint8_t handle_payload(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header, socket_internal_t *tcp_socket, uint8_t *payload)
	{
	msg_t m_send_tcp;
	uint8_t tcp_payload_len = ipv6_header->length-TCP_HDR_LEN;
	uint8_t acknowledged_bytes = 0;
	if (tcp_payload_len > tcp_socket->in_socket.local_tcp_status.window)
		{
		memcpy(tcp_socket->tcp_input_buffer, payload, tcp_socket->in_socket.local_tcp_status.window);
		acknowledged_bytes = tcp_socket->in_socket.local_tcp_status.window;
		tcp_socket->in_socket.local_tcp_status.window = 0;
		tcp_socket->tcp_input_buffer_end = tcp_socket->tcp_input_buffer_end + tcp_socket->in_socket.local_tcp_status.window;
		}
	else
		{
		memcpy(tcp_socket->tcp_input_buffer, payload, tcp_payload_len);
		tcp_socket->in_socket.local_tcp_status.window = tcp_socket->in_socket.local_tcp_status.window - tcp_payload_len;
		acknowledged_bytes = tcp_payload_len;
		tcp_socket->tcp_input_buffer_end = tcp_socket->tcp_input_buffer_end + tcp_payload_len;
		}

	net_msg_send(&m_send_tcp, tcp_socket->pid, 1, FID_SOCKET_RECV);
	return acknowledged_bytes;
	}

void handle_tcp_ack_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header, socket_internal_t *tcp_socket)
	{
	msg_t m_recv_tcp, m_send_tcp;

	if (tcp_socket->in_socket.local_tcp_status.state == LAST_ACK)
		{
		net_msg_send(&m_send_tcp, tcp_socket->pid, 0, FID_SOCKET_RECV);
		memset(tcp_socket, 0, sizeof(socket_internal_t));
		return;
		}
	// TODO: Find better way of handling queued sockets ACK packets
	else if (getWaitingConnectionSocket(tcp_socket->socket_id) != NULL)
		{
		m_send_tcp.content.ptr = (char*)tcp_header;
		net_msg_send_recv(&m_send_tcp, &m_recv_tcp, tcp_socket->pid, FID_SOCKET_HANDLE_NEW_TCP_CON, FID_TCP_ACK);
		return;
		}
	else if (tcp_socket->in_socket.local_tcp_status.state == ESTABLISHED)
		{
		printf("Got regular ack for established connections payload.\n");
		return;
		}
	printf("NO WAY OF HANDLING THIS ACK!\n");
	}

void handle_tcp_rst_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header, socket_internal_t *tcp_socket)
	{

	}

void handle_tcp_syn_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header, socket_internal_t *tcp_socket)
	{
	msg_t m_send_tcp;
	if (tcp_socket->in_socket.local_tcp_status.state == LISTEN)
		{
		socket_t *new_socket = new_tcp_queued_socket(ipv6_header, tcp_header, tcp_socket);
		if (new_socket != NULL)
			{
			// notify socket function accept(..) that a new connection request has arrived
			// No need to wait for an answer because the server accept() function isnt reading from anything other than the queued sockets
			msg_send(&m_send_tcp, tcp_socket->pid, 0);
			}
		else
			{
			printf("Dropped TCP SYN Message because an error occured while requesting a new queued socket!\n");
			}
		}
	else
		{
		printf("Dropped TCP SYN Message because socket was not in state LISTEN!");
		}
	}

void handle_tcp_syn_ack_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header, socket_internal_t *tcp_socket)
	{
	msg_t m_recv_tcp, m_send_tcp;
	if (tcp_socket->in_socket.local_tcp_status.state == SYN_SENT)
		{
		m_send_tcp.content.ptr = (char*) tcp_header;
		net_msg_send_recv(&m_send_tcp, &m_recv_tcp, tcp_socket->pid, FID_SOCKET_CONNECT, FID_TCP_SYN_ACK);
		}
	else
		{
		printf("Socket not in state SYN_SENT, dropping SYN-ACK-packet!");
		}
	}

void handle_tcp_fin_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header, socket_internal_t *tcp_socket)
	{
	msg_t m_send;
	socket_t *current_tcp_socket = &tcp_socket->in_socket;
	uint8_t send_buffer[BUFFER_SIZE];
	ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t*)(&send_buffer));
	tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t*)(&send_buffer[IPV6_HDR_LEN]));
	set_tcp_status(&current_tcp_socket->local_tcp_status,
			tcp_header->seq_nr+1,
			STATIC_MSS,
			current_tcp_socket->local_tcp_status.seq_nr+1,
			LAST_ACK,
			current_tcp_socket->local_tcp_status.window);
	set_tcp_status(&current_tcp_socket->foreign_tcp_status,
			tcp_header->ack_nr,
			STATIC_MSS,
			tcp_header->seq_nr,
			FIN_WAIT_1,
			current_tcp_socket->foreign_tcp_status.window);
	send_tcp(NULL, current_tcp_socket, current_tcp_packet, temp_ipv6_header, TCP_FIN_ACK, 0);

	m_send.content.value = CLOSE_CONN;
	net_msg_send(&m_send, tcp_socket->pid, 0, FID_SOCKET_RECV);
	}

void handle_tcp_fin_ack_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header, socket_internal_t *tcp_socket)
	{
	msg_t m_send;
	socket_t *current_tcp_socket = &tcp_socket->in_socket;
	uint8_t send_buffer[BUFFER_SIZE];
	ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t*)(&send_buffer));
	tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t*)(&send_buffer[IPV6_HDR_LEN]));
	set_tcp_status(&current_tcp_socket->local_tcp_status,
			tcp_header->seq_nr+1,
			STATIC_MSS,
			current_tcp_socket->local_tcp_status.seq_nr,
			CLOSED,
			current_tcp_socket->local_tcp_status.window);
	set_tcp_status(&current_tcp_socket->foreign_tcp_status,
			tcp_header->ack_nr,
			STATIC_MSS,
			tcp_header->seq_nr,
			LAST_ACK,
			current_tcp_socket->foreign_tcp_status.window);
	send_tcp(NULL, current_tcp_socket, current_tcp_packet, temp_ipv6_header, TCP_ACK, 0);
	memset(tcp_socket, 0, sizeof(socket_internal_t));

	net_msg_send(&m_send, tcp_socket->pid, 0, FID_SOCKET_CLOSE);
	}

void handle_tcp_no_flags_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header, socket_internal_t *tcp_socket, uint8_t *payload)
	{
	uint8_t tcp_payload_len = ipv6_header->length-TCP_HDR_LEN, read_bytes = 0;
	socket_t *current_tcp_socket = &tcp_socket->in_socket;
	uint8_t send_buffer[BUFFER_SIZE];
	ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t*)(&send_buffer));
	tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t*)(&send_buffer[IPV6_HDR_LEN]));

	if (tcp_payload_len > 0)
		{
		read_bytes = handle_payload(ipv6_header, tcp_header, tcp_socket, payload);
		// Refresh TCP status values
		set_tcp_status(&tcp_socket->in_socket.foreign_tcp_status,
				tcp_header->ack_nr,
				STATIC_MSS,
				tcp_header->seq_nr,
				ESTABLISHED,
				tcp_header->window);
		set_tcp_status(&tcp_socket->in_socket.local_tcp_status,
				tcp_header->seq_nr - tcp_payload_len + read_bytes + 1,
				STATIC_MSS,
				tcp_socket->in_socket.local_tcp_status.seq_nr,
				ESTABLISHED,
				tcp_socket->in_socket.local_tcp_status.window);

		// Fill IPv6 Header
		memcpy(&(temp_ipv6_header->destaddr), &current_tcp_socket->foreign_address.sin6_addr, 16);
		memcpy(&(temp_ipv6_header->srcaddr), &current_tcp_socket->local_address.sin6_addr, 16);
		temp_ipv6_header->length = TCP_HDR_LEN;

		// Fill TCP ACK packet
		set_tcp_packet(current_tcp_packet, current_tcp_socket->local_address.sin6_port, current_tcp_socket->foreign_address.sin6_port,
				current_tcp_socket->local_tcp_status.seq_nr, current_tcp_socket->local_tcp_status.ack_nr, 0, TCP_ACK, current_tcp_socket->local_tcp_status.window,
				0, 0);

		current_tcp_packet->checksum = ~tcp_csum(temp_ipv6_header, current_tcp_packet);
		sixlowpan_send(&current_tcp_socket->foreign_address.sin6_addr, (uint8_t*)(current_tcp_packet), TCP_HDR_LEN, IPPROTO_TCP);
		}
	}

void tcp_packet_handler (void)
	{
	msg_t m_recv_ip, m_send_ip;
	ipv6_hdr_t *ipv6_header;
	tcp_hdr_t *tcp_header;
	uint8_t *payload;
	socket_internal_t *tcp_socket = NULL;
	uint16_t chksum;

	while (1)
		{
		net_msg_receive(&m_recv_ip, FID_TCP_PH);

		ipv6_header = ((ipv6_hdr_t*)m_recv_ip.content.ptr);
		tcp_header = ((tcp_hdr_t*)(m_recv_ip.content.ptr + IPV6_HDR_LEN));
		payload = (uint8_t*)(m_recv_ip.content.ptr + IPV6_HDR_LEN + TCP_HDR_LEN);

		chksum = tcp_csum(ipv6_header, tcp_header);
		printf("Checksum is %x!\n", chksum);
		print_tcp_status(INC_PACKET, ipv6_header, tcp_header);
		tcp_socket = get_tcp_socket(ipv6_header, tcp_header);

		if ((chksum == 0xffff) && (tcp_socket != NULL))
			{
			// Remove reserved bits from tcp flags field
			uint8_t tcp_flags = tcp_header->reserved_flags & REMOVE_RESERVED;

			// TODO: URG Flag and PSH flag are currently being ignored
			switch (tcp_flags)
				{
				case TCP_ACK:
					{
					// only ACK Bit set
					handle_tcp_ack_packet(ipv6_header, tcp_header, tcp_socket);
					break;
					}
				case TCP_RST:
					{
					printf("RST Bit set!\n");
					// only RST Bit set
					handle_tcp_rst_packet(ipv6_header, tcp_header, tcp_socket);
					break;
					}
				case TCP_SYN:
					{
					// only SYN Bit set, look for matching, listening socket and request new queued socket
					printf("SYN Bit set!\n");
					handle_tcp_syn_packet(ipv6_header, tcp_header, tcp_socket);
					break;
					}
				case TCP_SYN_ACK:
					{
					// only SYN and ACK Bit set, complete three way handshake when socket in state SYN_SENT
					handle_tcp_syn_ack_packet(ipv6_header, tcp_header, tcp_socket);
					break;
					}
				case TCP_FIN:
					{
					printf("FIN Bit set!\n");
					// only FIN Bit set
					handle_tcp_fin_packet(ipv6_header, tcp_header, tcp_socket);
					break;
					}
				case TCP_FIN_ACK:
					{
					printf("FIN ACK Bit set!\n");
					// only FIN and ACK Bit set
					handle_tcp_fin_ack_packet(ipv6_header, tcp_header, tcp_socket);
					break;
					}
				default:
					{
					printf("DEFAULT!\n");
					handle_tcp_no_flags_packet(ipv6_header, tcp_header, tcp_socket, payload);
					}
				}
			}
		else
			{
			printf("Wrong checksum (%x) or no corresponding socket found!\n", chksum);
			}
		net_msg_reply(&m_recv_ip, &m_send_ip, FID_SIXLOWIP_TCP);
		}
	}

