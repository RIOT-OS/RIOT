/*
 * tcp_hc.c
 *
 *  Created on: 01.02.2012
 *      Author: Oliver
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "tcp_hc.h"
#include "socket.h"
#include "tcp.h"
#include "sys/net/sixlowpan/sixlowip.h"
#include "sys/net/net_help/net_help.h"

#ifdef TCP_HC

socket_internal_t *get_tcp_socket_by_context(ipv6_hdr_t *current_ipv6_header, uint8_t current_context)
	{
	socket_internal_t *temp_socket;
	for (int i = 1; i < MAX_SOCKETS+1; i++)
		{
		temp_socket = getSocket(i);
		if ((temp_socket != NULL) &&
				(ipv6_get_addr_match(&temp_socket->socket_values.foreign_address.sin6_addr, &current_ipv6_header->srcaddr) == 128) &&
				(ipv6_get_addr_match(&temp_socket->socket_values.local_address.sin6_addr, &current_ipv6_header->destaddr) == 128) &&
				(temp_socket->socket_values.tcp_control.tcp_context.context_id == current_context))
			{
			return temp_socket;
			}
		}
	return NULL;
	}

void update_tcp_hc_context(bool incoming, tcp_hc_context_t *current_context, tcp_hdr_t *current_tcp_packet)
	{
	if (incoming)
		{
		current_context->ack_rcv = current_tcp_packet->ack_nr;
		current_context->seq_rcv = current_tcp_packet->seq_nr;
		current_context->wnd_rcv = current_tcp_packet->window;
		}
	else
		{
		current_context->ack_snd = current_tcp_packet->ack_nr;
		current_context->seq_snd = current_tcp_packet->seq_nr;
		current_context->wnd_snd = current_tcp_packet->window;
		}
	}

uint16_t compress_tcp_packet(socket_t *current_tcp_socket, uint8_t *current_tcp_packet, ipv6_hdr_t *temp_ipv6_header, uint8_t flags, uint8_t payload_length)
	{
	tcp_hc_context_t *tcp_context = &current_tcp_socket->tcp_control.tcp_context;
	tcp_cb_t *tcp_cb = &current_tcp_socket->tcp_control;
	tcp_hdr_t full_tcp_header;
	uint16_t packet_size = 0;

	if (tcp_cb->state != ESTABLISHED)
		{
		// draft-aayadi-6lowpan-tcphc-01: 5.1 Full header TCP segment. Establishing Connection

		// Move tcp packet 2 bytes to add padding and Context ID
		memmove(current_tcp_packet+2, current_tcp_packet, TCP_HDR_LEN+payload_length);

		// 1 padding byte with value 0x01 to introduce full header TCP_HC segment
		memset(current_tcp_packet, 0x01, 1);

		// Adding Context ID
		memcpy(current_tcp_packet + 1, &tcp_context->context_id, 1);

		// Return correct header length (+2)
		packet_size = TCP_HDR_LEN + 2 + payload_length;

		print_tcp_status(OUT_PACKET, temp_ipv6_header, (tcp_hdr_t *)(current_tcp_packet+2), current_tcp_socket);

		update_tcp_hc_context(false, tcp_context, (tcp_hdr_t *)(current_tcp_packet+2));
		return packet_size;
		}
	else
		{
		// draft-aayadi-6lowpan-tcphc-01: 5.1 Compressed header TCP segment.

		// Temporary variable for TCP_HC_Header Bytes
		uint16_t tcp_hc_header = 0x0000;

		// Save TCP_Header to refresh TCP Context values after compressing the packet
		memcpy(&full_tcp_header, current_tcp_packet, TCP_HDR_LEN);

		// Temporary variable for storing TCP header beginning
		uint8_t *tcp_packet_begin = current_tcp_packet;

		// Position for first TCP header value, TCP_HC_Header and Context ID
		current_tcp_packet += 3;

		// Packet size value
		packet_size += 3;

		// 5.2.  LOWPAN_TCPHC Format

		// First 3 bits of TCP_HC_Header are not exactly specified. In this implementation they are always (1|1|0)
		tcp_hc_header |= 0xC000;

		// CID is always 8 bits in this implementation, so no handling needed (0)

		/*----------------------------------*/
		/*|		Sequence number handling   |*/
		/*----------------------------------*/
		if (full_tcp_header.seq_nr == tcp_context->seq_snd)
			{
			// Nothing to do, Seq = (0|0)
			}
		// If the 24 most significant bits haven't changed from previous packet, don't transmit them
		else if ((full_tcp_header.seq_nr&0xFFF0) == (tcp_context->seq_snd&0xFFF0))
			{
			// Seq = (0|1)
			tcp_hc_header |= 0x0400;

			// Copy first 8 less significant bits of sequence number into buffer
			memcpy(current_tcp_packet, &full_tcp_header.seq_nr, 1);
			current_tcp_packet += 1;
			packet_size += 1;
			}
		// If the 16 most significant bits haven't changed from previous packet, don't transmit them
		else if ((full_tcp_header.seq_nr&0xFF00) == (tcp_context->seq_snd&0xFF00))
			{
			// Seq = (1|0)
			tcp_hc_header |= 0x0800;

			// Copy first 16 less significant bits of sequence number into buffer
			memcpy(current_tcp_packet, &full_tcp_header.seq_nr, 2);
			current_tcp_packet += 2;
			packet_size += 2;
			}
		// Sending uncompressed sequence number
		else
			{
			// Seq = (1|1)
			tcp_hc_header |= 0x0C00;

			// Copy all bits of sequence number into buffer
			memcpy(current_tcp_packet, &full_tcp_header.seq_nr, 4);
			current_tcp_packet += 4;
			packet_size += 4;
			}

		/*----------------------------------*/
		/*|	Acknowledgment number handling |*/
		/*----------------------------------*/
		if (full_tcp_header.ack_nr == tcp_context->ack_snd)
			{
			// Nothing to do, Ack = (0|0)
			}
		// If the 24 most significant bits haven't changed from previous packet, don't transmit them
		else if ((full_tcp_header.ack_nr&0xFFF0) == (tcp_context->ack_snd&0xFFF0))
			{
			// Ack = (0|1)
			tcp_hc_header |= 0x0100;

			// Copy first 8 less significant bits of acknowledgment number into buffer
			memcpy(current_tcp_packet, &full_tcp_header.ack_nr, 1);
			current_tcp_packet += 1;
			packet_size += 1;
			}
		// If the 16 most significant bits haven't changed from previous packet, don't transmit them
		else if ((full_tcp_header.ack_nr&0xFF00) == (tcp_context->ack_snd&0xFF00))
			{
			// Ack = (1|0)
			tcp_hc_header |= 0x0200;

			// Copy first 16 less significant bits of acknowledgment number into buffer
			memcpy(current_tcp_packet, &full_tcp_header.ack_nr, 2);
			current_tcp_packet += 2;
			packet_size += 2;
			}
		// Sending uncompressed acknowledgment number
		else
			{
			// Ack = (1|1)
			tcp_hc_header |= 0x0300;

			// Copy all bits of acknowledgment number into buffer
			memcpy(current_tcp_packet, &full_tcp_header.ack_nr, 4);
			current_tcp_packet += 4;
			packet_size += 4;
			}

		/*----------------------------------*/
		/*|			Window handling 	   |*/
		/*----------------------------------*/
		if (full_tcp_header.window == tcp_context->wnd_snd)
			{
			// Nothing to do, Wnd = (0|0)
			}
		// If the 8 most significant bits haven't changed from previous packet, don't transmit them
		else if ((full_tcp_header.window&0xF0) == (tcp_context->wnd_snd&0xF0))
			{
			// Wnd = (0|1)
			tcp_hc_header |= 0x0040;

			// Copy first 8 less significant bits of window size into buffer
			memcpy(current_tcp_packet, &full_tcp_header.window, 1);
			current_tcp_packet += 1;
			packet_size += 1;
			}
		// If the 8 less significant bits haven't changed from previous packet, don't transmit them
		else if ((full_tcp_header.window&0x0F) == (tcp_context->wnd_snd&0x0F))
			{
			// Wnd = (1|0)
			tcp_hc_header |= 0x0080;

			// Copy first 8 most significant bits of window size into buffer
			memcpy(current_tcp_packet, (&full_tcp_header.window)+1, 1);
			current_tcp_packet += 1;
			packet_size += 1;
			}
		// Sending uncompressed window
		else
			{
			// Wnd = (1|1)
			tcp_hc_header |= 0x00C0;

			// Copy all bits of window size into buffer
			memcpy(current_tcp_packet, &full_tcp_header.window, 2);
			current_tcp_packet += 2;
			packet_size += 2;
			}

		// FIN flag
		if (IS_TCP_FIN(full_tcp_header.reserved_flags))
			{
			// F = (1)
			tcp_hc_header |= 0x0008;
			}

		// Copy checksum into buffer
		memcpy(current_tcp_packet, &full_tcp_header.checksum, 2);
		current_tcp_packet += 2;
		packet_size += 2;

		// Copy TCP_HC Bytes into buffer
		memcpy(tcp_packet_begin, &tcp_hc_header, 2);

		// Copy TCP_HC Context ID into buffer
		memcpy(tcp_packet_begin+2, &tcp_context->context_id, 1);

		// Move payload to end of tcp header
		memmove(current_tcp_packet, tcp_packet_begin+TCP_HDR_LEN, payload_length);

		// Adding TCP payload length to TCP_HC header length
		packet_size += payload_length;

		print_tcp_status(OUT_PACKET, temp_ipv6_header, &full_tcp_header, current_tcp_socket);
		update_tcp_hc_context(false, tcp_context, &full_tcp_header);
		return packet_size;
		}
	}

socket_internal_t *decompress_tcp_packet(ipv6_hdr_t *temp_ipv6_header)
	{
	uint8_t *packet_buffer = ((uint8_t*)temp_ipv6_header)+IPV6_HDR_LEN;
	uint8_t tcp_hc_header;
	socket_internal_t *current_socket = NULL;
	uint16_t packet_size = 0;

	printf("IPV6 Payload length: %u\n", temp_ipv6_header->length);

	// Full header TCP segment
	if (*(((uint8_t *)temp_ipv6_header)+IPV6_HDR_LEN) == 0x01)
		{
		printf("Full header tcp segment!\n");
		printf("Src port: %i, Dest. Port: %i\n", ((tcp_hdr_t *)(((uint8_t*)temp_ipv6_header)+IPV6_HDR_LEN+2))->src_port,
				((tcp_hdr_t *)(((uint8_t*)temp_ipv6_header)+IPV6_HDR_LEN+2))->dst_port);
		current_socket = get_tcp_socket(temp_ipv6_header, ((tcp_hdr_t *)(((uint8_t*)temp_ipv6_header)+IPV6_HDR_LEN+2)));
		if (current_socket != NULL)
			{
			printf("Socket not null!\n");
			memcpy(&current_socket->socket_values.tcp_control.tcp_context.context_id, ((uint8_t *)temp_ipv6_header)+IPV6_HDR_LEN+1, 1);
			memmove(((uint8_t *)temp_ipv6_header)+IPV6_HDR_LEN, (((uint8_t*)temp_ipv6_header)+IPV6_HDR_LEN+2), temp_ipv6_header->length-2);
			temp_ipv6_header->length -= 2;
			return current_socket;
			}
		else
			{
			printf("Socket Null!\n");
			// Found no matching socket for this packet -> Drop it
			return NULL;
			}
		}
	// Compressed header TCP segment
	else
		{
		// Temporary TCP Header
		tcp_hdr_t full_tcp_header;
		memset(&full_tcp_header, 0, sizeof(tcp_hdr_t));

		// Current context ID
		uint8_t current_context;
		memcpy(&current_context, (packet_buffer+3), 1);

		// Copy TCP_HC header into local variable
		memcpy(&tcp_hc_header, packet_buffer, 2);

		// Setting pointer to first tcp_hc field
		packet_buffer += 3;
		packet_size += 3;

		// Current socket
		socket_internal_t *current_socket = get_tcp_socket_by_context(temp_ipv6_header, current_context);

		if (current_socket == NULL)
			{
			return NULL;
			}

		// Current TCP Context values
		tcp_hc_context_t *current_tcp_context = &current_socket->socket_values.tcp_control.tcp_context;

		/*----------------------------------*/
		/*|		Sequence number handling   |*/
		/*----------------------------------*/
		if (!BITSET(tcp_hc_header, 11) && !BITSET(tcp_hc_header, 10))
			{
			// Seq = (0|0), sequence number didn't change, copy old value
			memcpy(&full_tcp_header.seq_nr, &current_tcp_context->seq_rcv, 4);
			}
		// The 24 most significant bits haven't changed from previous packet
		else if (!BITSET(tcp_hc_header, 11) && BITSET(tcp_hc_header, 10))
			{
			// Seq = (0|1), copy 1 byte of tcp_hc packet and 3 bytes from previous packet
			memcpy(&full_tcp_header.seq_nr, packet_buffer, 1);
			memcpy(&full_tcp_header.seq_nr+1, (&current_tcp_context->seq_rcv)+1, 3);
			packet_buffer += 1;
			packet_size += 1;
			}
		// If the 16 most significant bits haven't changed from previous packet
		else if (BITSET(tcp_hc_header, 11) && !BITSET(tcp_hc_header, 10))
			{
			// Seq = (1|0), copy 2 bytes of tcp_hc packet and 2 bytes from previous packet
			memcpy(&full_tcp_header.seq_nr, packet_buffer, 2);
			memcpy(&full_tcp_header.seq_nr+2, (&current_tcp_context->seq_rcv)+2, 2);
			packet_buffer += 2;
			packet_size += 2;
			}
		// Sending uncompressed sequence number
		else
			{
			// Seq = (1|1), copy 4 bytes of tcp_hc packet
			memcpy(&full_tcp_header.seq_nr, packet_buffer, 4);
			packet_buffer += 4;
			packet_size += 4;
			}

		/*----------------------------------*/
		/*|	Acknowledgment number handling |*/
		/*----------------------------------*/
		if (!BITSET(tcp_hc_header, 9) && !BITSET(tcp_hc_header, 8))
			{
			// Ack = (0|0), acknowledgment number didn't change, copy old value
			memcpy(&full_tcp_header.ack_nr, &current_tcp_context->ack_rcv, 4);
			}
		// The 24 most significant bits haven't changed from previous packet
		else if (!BITSET(tcp_hc_header, 9) && BITSET(tcp_hc_header, 8))
			{
			// Ack = (0|1), copy 1 byte of tcp_hc packet and 3 bytes from previous packet
			memcpy(&full_tcp_header.ack_nr, packet_buffer, 1);
			memcpy(&full_tcp_header.ack_nr+1, (&current_tcp_context->ack_rcv)+1, 3);
			packet_buffer += 1;
			packet_size += 1;
			SET_TCP_ACK(full_tcp_header.reserved_flags);
			}
		// If the 16 most significant bits haven't changed from previous packet
		else if (BITSET(tcp_hc_header, 9) && !BITSET(tcp_hc_header, 8))
			{
			// Ack = (1|0), copy 2 bytes of tcp_hc packet and 2 bytes from previous packet
			memcpy(&full_tcp_header.ack_nr, packet_buffer, 2);
			memcpy(&full_tcp_header.ack_nr+2, (&current_tcp_context->ack_rcv)+2, 2);
			packet_buffer += 2;
			packet_size += 2;
			SET_TCP_ACK(full_tcp_header.reserved_flags);
			}
		// Sending uncompressed acknowledgment number
		else
			{
			// Ack = (1|1), copy 4 bytes of tcp_hc packet
			memcpy(&full_tcp_header.ack_nr, packet_buffer, 4);
			packet_buffer += 4;
			packet_size += 4;
			SET_TCP_ACK(full_tcp_header.reserved_flags);
			}

		/*----------------------------------*/
		/*|			Window handling 	   |*/
		/*----------------------------------*/
		if (!BITSET(tcp_hc_header, 7) && !BITSET(tcp_hc_header, 6))
			{
			// Wnd = (0|0), copy old value
			memcpy(&full_tcp_header.window, &current_tcp_context->wnd_rcv, 2);
			}
		// The 8 most significant bits haven't changed from previous packet
		else if (!BITSET(tcp_hc_header, 7) && BITSET(tcp_hc_header, 6))
			{
			// Wnd = (0|1), copy 1 byte of tcp_hc packet and 1 byte from previous packet
			memcpy(&full_tcp_header.window, packet_buffer, 1);
			memcpy(&full_tcp_header.window+1, (&current_tcp_context->wnd_rcv)+1, 1);
			packet_buffer += 1;
			packet_size += 1;
			}
		// If the 8 less significant bits haven't changed from previous packet
		else if (BITSET(tcp_hc_header, 7) && !BITSET(tcp_hc_header, 6))
			{
			// Wnd = (1|0), copy 1 byte of tcp_hc packet and 1 byte from previous packet
			memcpy(&full_tcp_header.window+1, packet_buffer, 1);
			memcpy(&full_tcp_header.window, &current_tcp_context->wnd_rcv, 1);
			packet_buffer += 1;
			packet_size += 1;
			}
		// Sending uncompressed window size
		else
			{
			// Wnd = (1|1), copy 2 bytes of tcp_hc packet
			memcpy(&full_tcp_header.window, packet_buffer, 2);
			packet_buffer += 2;
			packet_size += 2;
			}

		// FIN flag
		if (BITSET(tcp_hc_header, 3))
			{
			// F = (1)
			if (IS_TCP_ACK(full_tcp_header.reserved_flags))
				{
				SET_TCP_FIN_ACK(full_tcp_header.reserved_flags);
				}
			else
				{
				SET_TCP_FIN(full_tcp_header.reserved_flags);
				}
			}

		// Copy checksum into into tcp header
		memcpy(&full_tcp_header.checksum, &packet_buffer, 2);
		packet_buffer += 2;
		packet_size += 2;

		// Copy dest. and src. port into tcp header
		memcpy(&full_tcp_header.dst_port, &current_socket->socket_values.local_address.sin6_port, 2);
		memcpy(&full_tcp_header.src_port, &current_socket->socket_values.foreign_address.sin6_port, 2);

		// Move payload to end of tcp header
		memmove(temp_ipv6_header+IPV6_HDR_LEN+TCP_HDR_LEN, packet_buffer, temp_ipv6_header->length-packet_size);

		// Copy TCP uncompressed header in front of payload
		memcpy(temp_ipv6_header+IPV6_HDR_LEN, &full_tcp_header, TCP_HDR_LEN);

		return current_socket;
		}
	}

#endif
