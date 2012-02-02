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

#ifdef TCP_HC

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
		current_tcp_packet[0] = 0x01;

		// Adding Context ID
		memcpy((&current_tcp_packet)+1, &tcp_context->context_id, 1);

		// Return correct header length (+2)
		packet_size = TCP_HDR_LEN + 2 + payload_length;
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
		}
	update_tcp_hc_context(false, tcp_context, &full_tcp_header);
	return packet_size;
	}
#endif
