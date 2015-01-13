/**
 * TCP header compression
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup transport_layer
 * @{
 * @file    tcp_hc.c
 * @brief   TCP HC
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ipv6.h"

#include "net_help.h"

#include "socket.h"
#include "tcp.h"

#include "tcp_hc.h"

#ifdef TCP_HC

socket_internal_t *get_tcp_socket_by_context(ipv6_hdr_t *current_ipv6_header,
        uint16_t current_context)
{
    socket_internal_t *temp_socket;

    for (int i = 1; i < MAX_SOCKETS + 1; i++) {
        temp_socket = socket_base_get_socket(i);

        if ((temp_socket != NULL) &&
            ipv6_legacy_addr_is_equal(&temp_socket->socket_values.foreign_address.sin6_addr,
                               &current_ipv6_header->srcaddr) &&
            ipv6_legacy_addr_is_equal(&temp_socket->socket_values.local_address.sin6_addr,
                               &current_ipv6_header->destaddr) &&
            (temp_socket->socket_values.tcp_control.tcp_context.context_id ==
             current_context)) {
            return temp_socket;
        }
    }

    return NULL;
}

void update_tcp_hc_context(bool incoming, socket_internal_t *current_socket,
                           tcp_hdr_t *current_tcp_packet)
{
    tcp_hc_context_t *current_context =
        &current_socket->socket_values.tcp_control.tcp_context;

    if (incoming) {
        current_context->ack_rcv = current_tcp_packet->ack_nr;
        current_context->seq_rcv = current_tcp_packet->seq_nr;
        current_context->wnd_rcv = current_tcp_packet->window;
    }
    else {
        current_context->ack_snd = current_tcp_packet->ack_nr;
        current_context->seq_snd = current_tcp_packet->seq_nr;
        current_context->wnd_snd = current_tcp_packet->window;
    }
}

uint16_t compress_tcp_packet(socket_internal_t *current_socket,
                             uint8_t *current_tcp_packet,
                             ipv6_legacy_hdr_t *temp_ipv6_header,
                             uint8_t flags,
                             uint8_t payload_length)
{
    socket_t *current_tcp_socket = &current_socket->socket_values;
    tcp_hc_context_t *tcp_context = &current_tcp_socket->tcp_control.tcp_context;
    tcp_cb_t *tcp_cb = &current_tcp_socket->tcp_control;
    tcp_hdr_t full_tcp_header;
    uint16_t packet_size = 0;

    /* Connection establisment phase, use FULL_HEADER TCP */
    if (tcp_context->hc_type == FULL_HEADER) {
        /* draft-aayadi-6lowpan-tcphc-01: 5.1 Full header TCP segment.
         * Establishing Connection */

        /* Move tcp packet 3 bytes to add padding and Context ID */
        memmove(current_tcp_packet + 3, current_tcp_packet,
                ((((tcp_hdr_t *)current_tcp_packet)->data_offset) * 4) +
                payload_length);

        /* 1 padding byte with value 0x01 to introduce full header TCP_HC
         * segment */
        memset(current_tcp_packet, 0x01, 1);

        /* Adding Context ID */
        uint16_t current_context = HTONS(tcp_context->context_id);
        memcpy(current_tcp_packet + 1, &current_context, 2);

        /* Return correct header length (+3) */
        packet_size = ((((tcp_hdr_t *)(current_tcp_packet + 3))->data_offset) * 4) + 3 +
                      payload_length;

        /* Update the tcp context fields */
        update_tcp_hc_context(false, current_socket, (tcp_hdr_t *)(current_tcp_packet + 3));

        /* Convert TCP packet to network byte order */
        switch_tcp_packet_byte_order((tcp_hdr_t *)(current_tcp_packet + 3));

        return packet_size;
    }
    /* Check for header compression type: COMPRESSED_HEADER */
    else if (tcp_context->hc_type == COMPRESSED_HEADER) {
        /* draft-aayadi-6lowpan-tcphc-01: 5.1 Compressed header TCP segment. */

        /* Temporary variable for TCP_HC_Header Bytes */
        uint16_t tcp_hc_header = 0x0000;

        /* Save TCP_Header to refresh TCP Context values after compressing the
         * packet */
        memcpy(&full_tcp_header, current_tcp_packet, TCP_HDR_LEN);

        /* Temporary variable for storing TCP header beginning */
        uint8_t *tcp_packet_begin = current_tcp_packet;

        /* Position for first TCP header value, TCP_HC_Header and Context ID */
        current_tcp_packet += 4;

        /* Packet size value */
        packet_size += 4;

        /* 5.2.  LOWPAN_TCPHC Format */

        /* First 3 bits of TCP_HC_Header are not exactly specified. In this
         * implementation they are (1|1|0) * for compressed headers and the
         * CID is always 16 bits (1) */
        /* (1|1|0|1) = D */
        tcp_hc_header |= 0xD000;

        /*----------------------------------*/
        /*|     Sequence number handling   |*/
        /*----------------------------------*/
        if (full_tcp_header.seq_nr == tcp_context->seq_snd) {
            /* Nothing to do, Seq = (0|0) */
        }
        /* If the 24 most significant bits haven't changed from previous
         * packet, don't transmit them */
        else if ((full_tcp_header.seq_nr & 0xFFFFFF00) == (tcp_context->seq_snd &
                 0xFFFFFF00)) {
            /* Seq = (0|1) */
            tcp_hc_header |= 0x0400;

            /* Copy first 8 less significant bits of sequence number into
             * buffer */
            *current_tcp_packet = (uint8_t)(full_tcp_header.seq_nr & 0x000000FF);
            current_tcp_packet += 1;
            packet_size += 1;
        }
        /* If the 16 most significant bits haven't changed from previous packet,
         * don't transmit them */
        else if ((full_tcp_header.seq_nr & 0xFFFF0000) == (tcp_context->seq_snd & 0xFFFF0000)) {
            /* Seq = (1|0) */
            tcp_hc_header |= 0x0800;

            /* Copy first 16 less significant bits of sequence number into buffer */
            *((uint16_t *)current_tcp_packet) =
                HTONS((uint16_t)(full_tcp_header.seq_nr & 0x0000FFFF));
            current_tcp_packet += 2;
            packet_size += 2;
        }
        /* Sending uncompressed sequence number */
        else {
            /* Seq = (1|1) */
            tcp_hc_header |= 0x0C00;

            /* Copy all bits of sequence number into buffer */
            uint32_t cur_seq_no = HTONL(full_tcp_header.seq_nr);
            memcpy(current_tcp_packet, &cur_seq_no, 4);
            current_tcp_packet += 4;
            packet_size += 4;
        }

        /*----------------------------------*/
        /*| Acknowledgment number handling |*/
        /*----------------------------------*/
        if ((IS_TCP_ACK(full_tcp_header.reserved_flags) &&
             (tcp_cb->tcp_context.ack_snd == full_tcp_header.ack_nr))) {
            tcp_context->ack_snd = tcp_context->seq_rcv;
        }

        if (full_tcp_header.ack_nr == tcp_context->ack_snd) {
            /* Nothing to do, Ack = (0|0) */
        }
        /* If the 24 most significant bits haven't changed from previous packet,
         * don't transmit them */
        else if ((full_tcp_header.ack_nr & 0xFFFFFF00) == (tcp_context->ack_snd &
                 0xFFFFFF00)) {
            /* Ack = (0|1) */
            tcp_hc_header |= 0x0100;

            /* Copy first 8 less significant bits of acknowledgment number into
             * buffer */
            *current_tcp_packet = (uint8_t)(full_tcp_header.ack_nr & 0x000000FF);
            current_tcp_packet += 1;
            packet_size += 1;
        }
        /* If the 16 most significant bits haven't changed from previous packet,
         * don't transmit them */
        else if ((full_tcp_header.ack_nr & 0xFFFF0000) == (tcp_context->ack_snd &
                 0xFFFF0000)) {
            /* Ack = (1|0) */
            tcp_hc_header |= 0x0200;

            /* Copy first 16 less significant bits of acknowledgment number
             * into buffer */
            *((uint16_t *)current_tcp_packet) =
                HTONS((uint16_t)(full_tcp_header.ack_nr & 0x0000FFFF));
            current_tcp_packet += 2;
            packet_size += 2;
        }
        /* Sending uncompressed acknowledgment number */
        else {
            /* Ack = (1|1) */
            tcp_hc_header |= 0x0300;

            /* Copy all bits of acknowledgment number into buffer */
            uint32_t cur_ack_nr = HTONL(full_tcp_header.ack_nr);
            memcpy(current_tcp_packet, &cur_ack_nr, 4);
            current_tcp_packet += 4;
            packet_size += 4;
        }

        /*----------------------------------*/
        /*|         Window handling        |*/
        /*----------------------------------*/
        if (full_tcp_header.window == tcp_context->wnd_snd) {
            /* Nothing to do, Wnd = (0|0) */
        }
        /* If the 8 most significant bits haven't changed from previous packet,
         * don't transmit them */
        else if ((full_tcp_header.window & 0xFF00) == (tcp_context->wnd_snd & 0xFF00)) {
            /* Wnd = (0|1) */
            tcp_hc_header |= 0x0040;

            /* Copy first 8 less significant bits of window size into buffer */
            *current_tcp_packet = (uint8_t)(full_tcp_header.window & 0x00FF);
            current_tcp_packet += 1;
            packet_size += 1;
        }
        /* If the 8 less significant bits haven't changed from previous packet,
         * don't transmit them */
        else if ((full_tcp_header.window & 0x00FF) == (tcp_context->wnd_snd &
                 0x00FF)) {
            /* Wnd = (1|0) */
            tcp_hc_header |= 0x0080;

            /* Copy first 8 most significant bits of window size into buffer */
            *current_tcp_packet = (uint8_t)(full_tcp_header.window & 0xFF00);
            current_tcp_packet += 1;
            packet_size += 1;
        }
        /* Sending uncompressed window */
        else {
            /* Wnd = (1|1) */
            tcp_hc_header |= 0x00C0;

            /* Copy all bits of window size into buffer */
            uint16_t cur_window = HTONS(full_tcp_header.window);
            memcpy(current_tcp_packet, &cur_window, 2);
            current_tcp_packet += 2;
            packet_size += 2;
        }

        /* FIN flag */
        if (IS_TCP_FIN(full_tcp_header.reserved_flags)) {
            /* F = (1) */
            tcp_hc_header |= 0x0008;
        }

        /* Copy checksum into buffer */
        uint16_t cur_chk_sum = HTONS(full_tcp_header.checksum);
        memcpy(current_tcp_packet, &cur_chk_sum, 2);
        current_tcp_packet += 2;
        packet_size += 2;

        /* Copy TCP_HC Bytes into buffer */
        uint16_t cur_tcp_hc_header = HTONS(tcp_hc_header);
        memcpy(tcp_packet_begin, &cur_tcp_hc_header, 2);

        /* Copy TCP_HC Context ID into buffer */
        uint16_t cur_context_id = HTONS(tcp_context->context_id);
        memcpy(tcp_packet_begin + 2, &cur_context_id, 2);

        /* Move payload to end of tcp header */
        memmove(current_tcp_packet, tcp_packet_begin + TCP_HDR_LEN,
                payload_length);

        /* Adding TCP payload length to TCP_HC header length */
        packet_size += payload_length;

        update_tcp_hc_context(false, current_socket, &full_tcp_header);

        return packet_size;
    }
    /* Check for header compression type: MOSTLY_COMPRESSED_HEADER */
    else if (tcp_context->hc_type == MOSTLY_COMPRESSED_HEADER) {
        /* draft-aayadi-6lowpan-tcphc-01: 5.1 Compressed header TCP segment. */

        /* Temporary variable for TCP_HC_Header Bytes */
        uint16_t tcp_hc_header = 0x0000;

        /* Save TCP_Header to refresh TCP Context values after compressing the
         * packet */
        memcpy(&full_tcp_header, current_tcp_packet, TCP_HDR_LEN);

        /* Temporary variable for storing TCP header beginning */
        uint8_t *tcp_packet_begin = current_tcp_packet;

        /* Position for first TCP header value, TCP_HC_Header and Context ID */
        current_tcp_packet += 4;

        /* Packet size value */
        packet_size += 4;

        /* 5.2.  LOWPAN_TCPHC Format */

        /* First 3 bits of TCP_HC_Header are not exactly specified. In this
         * implementation they are (1|0|0) for mostly compressed headers and
         * the CID is always 16 bits (1) */
        /* (1|0|0|1) = 9 */
        tcp_hc_header |= 0x9000;

        /*----------------------------------*/
        /*|     Sequence number handling   |*/
        /*----------------------------------*/
        /* Sending uncompressed sequence number */
        /* Seq = (1|1) */
        tcp_hc_header |= 0x0C00;

        /* Copy all bits of sequence number into buffer */
        uint32_t cur_seq_no = HTONL(full_tcp_header.seq_nr);
        memcpy(current_tcp_packet, &cur_seq_no, 4);
        current_tcp_packet += 4;
        packet_size += 4;

        /*----------------------------------*/
        /*| Acknowledgment number handling |*/
        /*----------------------------------*/
        /* Ack = (1|1) */
        tcp_hc_header |= 0x0300;

        /* Copy all bits of acknowledgment number into buffer */
        uint32_t cur_ack_nr = HTONL(full_tcp_header.ack_nr);
        memcpy(current_tcp_packet, &cur_ack_nr, 4);
        current_tcp_packet += 4;
        packet_size += 4;

        /*----------------------------------*/
        /*|         Window handling        |*/
        /*----------------------------------*/
        /* Wnd = (1|1) */
        tcp_hc_header |= 0x00C0;

        /* Copy all bits of window size into buffer */
        uint16_t cur_window = HTONS(full_tcp_header.window);
        memcpy(current_tcp_packet, &cur_window, 2);
        current_tcp_packet += 2;
        packet_size += 2;

        /* FIN flag */
        if (IS_TCP_FIN(full_tcp_header.reserved_flags)) {
            /* F = (1) */
            tcp_hc_header |= 0x0008;
        }

        /* Copy checksum into buffer */
        uint16_t cur_chk_sum = HTONS(full_tcp_header.checksum);
        memcpy(current_tcp_packet, &cur_chk_sum, 2);
        current_tcp_packet += 2;
        packet_size += 2;

        /* Copy TCP_HC Bytes into buffer */
        uint16_t cur_tcp_hc_header = HTONS(tcp_hc_header);
        memcpy(tcp_packet_begin, &cur_tcp_hc_header, 2);

        /* Copy TCP_HC Context ID into buffer */
        uint16_t cur_context_id = HTONS(tcp_context->context_id);
        memcpy(tcp_packet_begin + 2, &cur_context_id, 2);

        /* Move payload to end of tcp header */
        memmove(current_tcp_packet, tcp_packet_begin + TCP_HDR_LEN,
                payload_length);

        /* Adding TCP payload length to TCP_HC header length */
        packet_size += payload_length;

        update_tcp_hc_context(false, current_socket, &full_tcp_header);
        return packet_size;
    }

    return 0;
}

socket_internal_t *decompress_tcp_packet(ipv6_hdr_t *temp_ipv6_header)
{
    uint8_t *packet_buffer = ((uint8_t *)temp_ipv6_header) + IPV6_HDR_LEN;
    uint16_t tcp_hc_header;
    socket_internal_t *current_socket = NULL;

    /* Full header TCP segment */
    if (*(((uint8_t *)temp_ipv6_header) + IPV6_HDR_LEN) == 0x01) {
        switch_tcp_packet_byte_order(((tcp_hdr_t *)(((uint8_t *)temp_ipv6_header) +
                                      IPV6_LEGACY_HDR_LEN + 3)));
        current_socket = get_tcp_socket(temp_ipv6_header,
                                        ((tcp_hdr_t *)(((uint8_t *)temp_ipv6_header) +
                                                IPV6_LEGACY_HDR_LEN + 3)));

        if (current_socket != NULL) {
            if (current_socket->socket_values.tcp_control.state == LISTEN) {
                memcpy(&current_socket->socket_values.tcp_control.tcp_context.context_id,
                       ((uint8_t *)temp_ipv6_header) + IPV6_HDR_LEN + 1, 2);
                current_socket->socket_values.tcp_control.tcp_context.context_id =
                    NTOHS(current_socket->socket_values.tcp_control.tcp_context.context_id);
            }

            memmove(((uint8_t *)temp_ipv6_header) + IPV6_HDR_LEN,
                    (((uint8_t *)temp_ipv6_header) + IPV6_HDR_LEN + 3),
                    temp_ipv6_header->length - 3);
            temp_ipv6_header->length -= 3;
            return current_socket;
        }
        else {
            printf("Socket Null!\n");
            /* Found no matching socket for this packet -> Drop it */
            return NULL;
        }
    }
    /* Compressed header TCP segment */
    else {
        uint16_t packet_size = 0;

        /* Temporary TCP Header */
        tcp_hdr_t full_tcp_header;
        memset(&full_tcp_header, 0, sizeof(tcp_hdr_t));

        /* Current context ID */
        uint16_t current_context;
        memcpy(&current_context, (packet_buffer + 2), 2);
        current_context = NTOHS(current_context);

        /* Copy TCP_HC header into local variable
         * (1,0,0,1|SEQ,SEQ,0)(1,0,0,1|0,0,0,0) */
        memcpy(&tcp_hc_header, packet_buffer, 2);
        tcp_hc_header = NTOHS(tcp_hc_header);

        uint8_t header_type = UNDEFINED;

        if (BITSET(tcp_hc_header, 15) && !BITSET(tcp_hc_header, 14) &&
            !BITSET(tcp_hc_header, 13)) {
            header_type = MOSTLY_COMPRESSED_HEADER;
        }
        else if (BITSET(tcp_hc_header, 15) && BITSET(tcp_hc_header, 14) &&
                 !BITSET(tcp_hc_header, 13)) {
            header_type = COMPRESSED_HEADER;
        }

        /* Setting pointer to first tcp_hc field */
        packet_buffer += 4;
        packet_size += 4;

        /* Current socket */
        socket_internal_t *current_socket =
            get_tcp_socket_by_context(temp_ipv6_header, current_context);

        if (current_socket == NULL) {
            printf("Current Socket == NULL!\n");
            return NULL;
        }

        /* Current TCP Context values */
        tcp_hc_context_t *current_tcp_context =
            &current_socket->socket_values.tcp_control.tcp_context;

        /*----------------------------------*/
        /*|     Sequence number handling   |*/
        /*----------------------------------*/
        if (!BITSET(tcp_hc_header, 11) && !BITSET(tcp_hc_header, 10)) {
            /* Seq = (0|0), sequence number didn't change, copy old value */
            memcpy(&full_tcp_header.seq_nr, &current_tcp_context->seq_rcv, 4);
        }
        /* The 24 most significant bits haven't changed from previous packet */
        else if (!BITSET(tcp_hc_header, 11) && BITSET(tcp_hc_header, 10)) {
            /* Seq = (0|1), copy 1 byte of tcp_hc packet and 3 bytes from
             * previous packet */
            full_tcp_header.seq_nr |= *packet_buffer;
            full_tcp_header.seq_nr |= ((current_tcp_context->seq_rcv) &
                                       0xFFFFFF00);
            packet_buffer += 1;
            packet_size += 1;
        }
        /* If the 16 most significant bits haven't changed from previous packet */
        else if (BITSET(tcp_hc_header, 11) && !BITSET(tcp_hc_header, 10)) {
            /* Seq = (1|0), copy 2 bytes of tcp_hc packet and 2 bytes from
             * previous packet */
            full_tcp_header.seq_nr |= NTOHS(*((uint16_t *)packet_buffer));
            full_tcp_header.seq_nr |= ((current_tcp_context->seq_rcv) & 0xFFFF0000);
            packet_buffer += 2;
            packet_size += 2;
        }
        /* Sending uncompressed sequence number */
        else {
            /* Seq = (1|1), copy 4 bytes of tcp_hc packet */
            memcpy(&full_tcp_header.seq_nr, packet_buffer, 4);
            full_tcp_header.seq_nr = NTOHL(full_tcp_header.seq_nr);
            packet_buffer += 4;
            packet_size += 4;
        }

        /*----------------------------------*/
        /*| Acknowledgment number handling |*/
        /*----------------------------------*/
        if (!BITSET(tcp_hc_header, 9) && !BITSET(tcp_hc_header, 8)) {
            /* Ack = (0|0), acknowledgment number didn't change, copy old value */
            memcpy(&full_tcp_header.ack_nr, &current_tcp_context->ack_rcv, 4);
        }
        /* The 24 most significant bits haven't changed from previous packet */
        else if (!BITSET(tcp_hc_header, 9) && BITSET(tcp_hc_header, 8)) {
            /* Ack = (0|1), copy 1 byte of tcp_hc packet and 3 bytes from
             * previous packet */
            full_tcp_header.ack_nr |= *packet_buffer;
            full_tcp_header.ack_nr |= ((current_tcp_context->ack_rcv) & 0xFFFFFF00);
            packet_buffer += 1;
            packet_size += 1;
            SET_TCP_ACK(full_tcp_header.reserved_flags);
        }
        /* If the 16 most significant bits haven't changed from previous packet */
        else if (BITSET(tcp_hc_header, 9) && !BITSET(tcp_hc_header, 8)) {
            /* Ack = (1|0), copy 2 bytes of tcp_hc packet and 2 bytes from
             * previous packet */
            full_tcp_header.ack_nr |= NTOHS(*((uint16_t *)packet_buffer));
            full_tcp_header.ack_nr |= ((current_tcp_context->ack_rcv) & 0xFFFF0000);
            packet_buffer += 2;
            packet_size += 2;
            SET_TCP_ACK(full_tcp_header.reserved_flags);
        }
        /* Sending uncompressed acknowledgment number */
        else {
            /* Ack = (1|1), copy 4 bytes of tcp_hc packet */
            memcpy(&full_tcp_header.ack_nr, packet_buffer, 4);
            full_tcp_header.ack_nr = NTOHL(full_tcp_header.ack_nr);
            packet_buffer += 4;
            packet_size += 4;

            if (header_type == COMPRESSED_HEADER) {
                SET_TCP_ACK(full_tcp_header.reserved_flags);
            }
        }

        /*----------------------------------*/
        /*|         Window handling        |*/
        /*----------------------------------*/
        if (!BITSET(tcp_hc_header, 7) && !BITSET(tcp_hc_header, 6)) {
            /* Wnd = (0|0), copy old value */
            memcpy(&full_tcp_header.window, &current_tcp_context->wnd_rcv, 2);
        }
        /* The 8 most significant bits haven't changed from previous packet */
        else if (!BITSET(tcp_hc_header, 7) && BITSET(tcp_hc_header, 6)) {
            /* Wnd = (0|1), copy 1 byte of tcp_hc packet and 1 byte from
             * previous packet */
            full_tcp_header.window |= *packet_buffer;
            full_tcp_header.window |= ((current_tcp_context->wnd_rcv) & 0xFF00);
            packet_buffer += 1;
            packet_size += 1;
        }
        /* If the 8 less significant bits haven't changed from previous packet */
        else if (BITSET(tcp_hc_header, 7) && !BITSET(tcp_hc_header, 6)) {
            /* Wnd = (1|0), copy 1 byte of tcp_hc packet and 1 byte from previous packet */
            full_tcp_header.window |= ((*((uint16_t *)packet_buffer)) & 0xFF00);
            full_tcp_header.window |= ((current_tcp_context->wnd_rcv) & 0x00FF);
            packet_buffer += 1;
            packet_size += 1;
        }
        /* Sending uncompressed window size */
        else {
            /* Wnd = (1|1), copy 2 bytes of tcp_hc packet */
            memcpy(&full_tcp_header.window, packet_buffer, 2);
            full_tcp_header.window = NTOHS(full_tcp_header.window);
            packet_buffer += 2;
            packet_size += 2;
        }

        /* FIN flag */
        if (BITSET(tcp_hc_header, 3)) {
            /* F = (1) */
            if (IS_TCP_ACK(full_tcp_header.reserved_flags)) {
                SET_TCP_FIN_ACK(full_tcp_header.reserved_flags);
            }
            else {
                SET_TCP_FIN(full_tcp_header.reserved_flags);
            }
        }

        /* Copy checksum into into tcp header */
        memcpy(&full_tcp_header.checksum, packet_buffer, 2);
        full_tcp_header.checksum = NTOHS(full_tcp_header.checksum);
        packet_buffer += 2;
        packet_size += 2;

        /* Copy dest. and src. port into tcp header */
        memcpy(&full_tcp_header.dst_port,
               &current_socket->socket_values.local_address.sin6_port, 2);
        memcpy(&full_tcp_header.src_port,
               &current_socket->socket_values.foreign_address.sin6_port, 2);

        /* Ordinary TCP header length */
        full_tcp_header.data_offset = TCP_HDR_LEN / 4;

        /* Move payload to end of tcp header */
        memmove(((uint8_t *)temp_ipv6_header) + IPV6_HDR_LEN + TCP_HDR_LEN,
                packet_buffer, temp_ipv6_header->length - packet_size);

        /* Copy TCP uncompressed header in front of payload */
        memcpy(((uint8_t *)temp_ipv6_header) + IPV6_HDR_LEN, &full_tcp_header,
               TCP_HDR_LEN);

        /* Set IPV6 header length */
        temp_ipv6_header->length = temp_ipv6_header->length - packet_size +
                                   TCP_HDR_LEN;
        return current_socket;
    }
}

#endif
