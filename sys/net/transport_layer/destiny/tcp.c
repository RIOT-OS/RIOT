/**
 * Destiny TCP implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup destiny
 * @{
 * @file    tcp.c
 * @brief   TCP implementation
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sixlowpan.h"
#include "thread.h"
#include "vtimer.h"

#include "destiny/in.h"

#include "net_help.h"

#include "msg_help.h"
#include "socket.h"
#include "tcp_hc.h"
#include "tcp_timer.h"

#include "tcp.h"

#ifdef TCP_HC
mutex_t             global_context_counter_mutex;
uint8_t             global_context_counter;
#endif

mutex_t             global_sequence_counter_mutex;
uint32_t            global_sequence_counter;

void printTCPHeader(tcp_hdr_t *tcp_header)
{
    printf("\nBEGIN: TCP HEADER\n");
    printf("ack_nr: %" PRIu32 "\n", tcp_header->ack_nr);
    printf("checksum: %i\n", tcp_header->checksum);
    printf("data_offset: %i\n", tcp_header->data_offset);
    printf("dst_port: %i\n", tcp_header->dst_port);
    printf("reserved_flags: %i\n", tcp_header->reserved_flags);
    printf("seq_nr: %" PRIu32 "\n", tcp_header->seq_nr);
    printf("src_port: %i\n", tcp_header->src_port);
    printf("urg_pointer: %i\n", tcp_header->urg_pointer);
    printf("window: %i\n", tcp_header->window);
    printf("END: TCP HEADER\n");
}

void printArrayRange_tcp(uint8_t *udp_header, uint16_t len)
{
    int i = 0;
    printf("-------------MEMORY-------------\n");

    for (i = 0; i < len; i++) {
        printf("%#x ", *(udp_header + i));
    }

    printf("-------------MEMORY-------------\n");
}

uint16_t tcp_csum(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header)
{
    uint16_t sum;
    uint16_t len = NTOHS(ipv6_header->length);

    sum = len + IPPROTO_TCP;
    sum = csum(sum, (uint8_t *)&ipv6_header->srcaddr, 2 * sizeof(ipv6_addr_t));
    sum = csum(sum, (uint8_t *)tcp_header, len);
    return (sum == 0) ? 0xffff : HTONS(sum);
}

uint8_t handle_payload(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header,
                       socket_internal_t *tcp_socket, uint8_t *payload)
{
    (void) tcp_header;

    msg_t m_send_tcp, m_recv_tcp;
    uint8_t tcp_payload_len = NTOHS(ipv6_header->length) - TCP_HDR_LEN;
    uint8_t acknowledged_bytes = 0;

    if (tcp_payload_len > tcp_socket->socket_values.tcp_control.rcv_wnd) {
        mutex_lock(&tcp_socket->tcp_buffer_mutex);
        memcpy(tcp_socket->tcp_input_buffer, payload,
               tcp_socket->socket_values.tcp_control.rcv_wnd);
        acknowledged_bytes = tcp_socket->socket_values.tcp_control.rcv_wnd;
        tcp_socket->socket_values.tcp_control.rcv_wnd = 0;
        tcp_socket->tcp_input_buffer_end = tcp_socket->tcp_input_buffer_end +
                                           tcp_socket->socket_values.tcp_control.rcv_wnd;
        mutex_unlock(&tcp_socket->tcp_buffer_mutex);
    }
    else {
        mutex_lock(&tcp_socket->tcp_buffer_mutex);
        memcpy(tcp_socket->tcp_input_buffer, payload, tcp_payload_len);
        tcp_socket->socket_values.tcp_control.rcv_wnd =
            tcp_socket->socket_values.tcp_control.rcv_wnd - tcp_payload_len;
        acknowledged_bytes = tcp_payload_len;
        tcp_socket->tcp_input_buffer_end = tcp_socket->tcp_input_buffer_end +
                                           tcp_payload_len;
        mutex_unlock(&tcp_socket->tcp_buffer_mutex);
    }

    if (thread_getstatus(tcp_socket->recv_pid) == STATUS_RECEIVE_BLOCKED) {
        net_msg_send_recv(&m_send_tcp, &m_recv_tcp, tcp_socket->recv_pid, UNDEFINED);
    }

    return acknowledged_bytes;
}

void handle_tcp_ack_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header,
                           socket_internal_t *tcp_socket)
{
    msg_t m_recv_tcp, m_send_tcp;
    uint8_t target_pid;

    if (tcp_socket->socket_values.tcp_control.state == TCP_LAST_ACK) {
        target_pid = tcp_socket->recv_pid;
        close_socket(tcp_socket);
        msg_send(&m_send_tcp, target_pid, 0);
        return;
    }
    else if (tcp_socket->socket_values.tcp_control.state == TCP_CLOSING) {
        msg_send(&m_send_tcp, tcp_socket->recv_pid, 0);
        msg_send(&m_send_tcp, tcp_socket->send_pid, 0);
        return;
    }
    else if (get_waiting_connection_socket(tcp_socket->socket_id, ipv6_header,
                                           tcp_header) != NULL) {
        m_send_tcp.content.ptr = (char *)tcp_header;
        net_msg_send_recv(&m_send_tcp, &m_recv_tcp, tcp_socket->recv_pid, TCP_ACK);
        return;
    }
    else if (tcp_socket->socket_values.tcp_control.state == TCP_ESTABLISHED) {
        if (check_tcp_consistency(&tcp_socket->socket_values, tcp_header, 0) == PACKET_OK) {
            m_send_tcp.content.ptr = (char *)tcp_header;
            net_msg_send(&m_send_tcp, tcp_socket->send_pid, 0, TCP_ACK);
            return;
        }
    }

    printf("NO WAY OF HANDLING THIS ACK!\n");
}

void handle_tcp_rst_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header,
                           socket_internal_t *tcp_socket)
{
    (void) ipv6_header;
    (void) tcp_header;
    (void) tcp_socket;

    /* TODO: Reset connection */
}

void handle_tcp_syn_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header,
                           socket_internal_t *tcp_socket)
{
    msg_t m_send_tcp;

    if (tcp_socket->socket_values.tcp_control.state == TCP_LISTEN) {
        socket_internal_t *new_socket = new_tcp_queued_socket(ipv6_header,
                                        tcp_header);

        if (new_socket != NULL) {
#ifdef TCP_HC
            update_tcp_hc_context(true, new_socket, tcp_header);
#endif
            /* notify socket function destiny_socket_accept(..) that a new
             * connection request has arrived. No need to wait for an answer
             * because the server destiny_socket_accept() function isnt reading
             * from anything other than the queued sockets */
            net_msg_send(&m_send_tcp, tcp_socket->recv_pid, 0, TCP_SYN);
        }
        else {
            printf("Dropped TCP SYN Message because an error occured while "\
                   "requesting a new queued socket!\n");
        }
    }
    else {
        printf("Dropped TCP SYN Message because socket was not in state TCP_LISTEN!");
    }
}

void handle_tcp_syn_ack_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header,
                               socket_internal_t *tcp_socket)
{
    (void) ipv6_header;

    msg_t m_send_tcp;

    if (tcp_socket->socket_values.tcp_control.state == TCP_SYN_SENT) {
        m_send_tcp.content.ptr = (char *) tcp_header;
        net_msg_send(&m_send_tcp, tcp_socket->recv_pid, 0, TCP_SYN_ACK);
    }
    else {
        printf("Socket not in state TCP_SYN_SENT, dropping SYN-ACK-packet!");
    }
}

void handle_tcp_fin_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header,
                           socket_internal_t *tcp_socket)
{
    (void) ipv6_header;

    msg_t m_send;
    socket_t *current_tcp_socket = &tcp_socket->socket_values;
    uint8_t send_buffer[BUFFER_SIZE];
    ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&send_buffer));
    tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t *)(&send_buffer[IPV6_HDR_LEN]));

    set_tcp_cb(&current_tcp_socket->tcp_control, tcp_header->seq_nr + 1,
               current_tcp_socket->tcp_control.send_wnd, tcp_header->ack_nr + 1,
               tcp_header->ack_nr, tcp_header->window);

#ifdef TCP_HC
    current_tcp_socket->tcp_control.tcp_context.hc_type = COMPRESSED_HEADER;
#endif

    if (current_tcp_socket->tcp_control.state == TCP_FIN_WAIT_1) {
        current_tcp_socket->tcp_control.state = TCP_CLOSING;

        send_tcp(tcp_socket, current_tcp_packet, temp_ipv6_header, TCP_FIN_ACK, 0);
    }
    else {
        current_tcp_socket->tcp_control.state = TCP_LAST_ACK;

        send_tcp(tcp_socket, current_tcp_packet, temp_ipv6_header, TCP_FIN_ACK, 0);
    }

    net_msg_send(&m_send, tcp_socket->recv_pid, 0, CLOSE_CONN);
}

void handle_tcp_fin_ack_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header,
                               socket_internal_t *tcp_socket)
{
    (void) ipv6_header;

    msg_t m_send;
    socket_t *current_tcp_socket = &tcp_socket->socket_values;
    uint8_t send_buffer[BUFFER_SIZE];
    ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&send_buffer));
    tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t *)(&send_buffer[IPV6_HDR_LEN]));

    current_tcp_socket->tcp_control.state = TCP_CLOSED;

    set_tcp_cb(&current_tcp_socket->tcp_control, tcp_header->seq_nr + 1,
               current_tcp_socket->tcp_control.send_wnd, tcp_header->ack_nr,
               tcp_header->ack_nr, tcp_header->window);

#ifdef TCP_HC
    current_tcp_socket->tcp_control.tcp_context.hc_type = COMPRESSED_HEADER;
#endif

    send_tcp(tcp_socket, current_tcp_packet, temp_ipv6_header, TCP_ACK, 0);

    msg_send(&m_send, tcp_socket->send_pid, 0);
    msg_send(&m_send, tcp_socket->recv_pid, 0);
}

void handle_tcp_no_flags_packet(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header,
                                socket_internal_t *tcp_socket, uint8_t *payload, uint8_t tcp_payload_len)
{
    uint8_t read_bytes = 0;
    socket_t *current_tcp_socket = &tcp_socket->socket_values;
    uint8_t send_buffer[BUFFER_SIZE];
    ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&send_buffer));
    tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t *)(&send_buffer[IPV6_HDR_LEN]));

    if (check_tcp_consistency(current_tcp_socket, tcp_header, tcp_payload_len) == PACKET_OK) {
        read_bytes = handle_payload(ipv6_header, tcp_header, tcp_socket, payload);

        /* Refresh TCP status values */
        current_tcp_socket->tcp_control.state = TCP_ESTABLISHED;

        set_tcp_cb(&current_tcp_socket->tcp_control,
                   tcp_header->seq_nr + read_bytes,
                   current_tcp_socket->tcp_control.rcv_wnd,
                   current_tcp_socket->tcp_control.send_nxt,
                   current_tcp_socket->tcp_control.send_una,
                   current_tcp_socket->tcp_control.send_wnd);

        /* Send packet */
        //			block_continue_thread();
#ifdef TCP_HC
        current_tcp_socket->tcp_control.tcp_context.hc_type = COMPRESSED_HEADER;
#endif
        send_tcp(tcp_socket, current_tcp_packet, temp_ipv6_header, TCP_ACK, 0);
    }
    /* ACK packet probably got lost */
    else {
        //			block_continue_thread();
#ifdef TCP_HC
        current_tcp_socket->tcp_control.tcp_context.hc_type = FULL_HEADER;
#endif
        send_tcp(tcp_socket, current_tcp_packet, temp_ipv6_header, TCP_ACK, 0);
    }
}

void *tcp_packet_handler(void *arg)
{
    (void) arg;

    msg_t m_recv_ip, m_send_ip;
    ipv6_hdr_t *ipv6_header;
    tcp_hdr_t *tcp_header;
    uint8_t *payload;
    socket_internal_t *tcp_socket = NULL;
    uint16_t chksum;

    while (1) {
        msg_receive(&m_recv_ip);

        ipv6_header = ((ipv6_hdr_t *)m_recv_ip.content.ptr);
        tcp_header = ((tcp_hdr_t *)(m_recv_ip.content.ptr + IPV6_HDR_LEN));
#ifdef TCP_HC
        tcp_socket = decompress_tcp_packet(ipv6_header);
#else
        switch_tcp_packet_byte_order(tcp_header);
        tcp_socket = get_tcp_socket(ipv6_header, tcp_header);
#endif
        chksum = tcp_csum(ipv6_header, tcp_header);

        payload = (uint8_t *)(m_recv_ip.content.ptr + IPV6_HDR_LEN + tcp_header->data_offset * 4);

        if ((chksum == 0xffff) && (tcp_socket != NULL)) {
#ifdef TCP_HC
            update_tcp_hc_context(true, tcp_socket, tcp_header);
#endif
            /* Remove reserved bits from tcp flags field */
            uint8_t tcp_flags = tcp_header->reserved_flags;

            switch (tcp_flags) {
                case TCP_ACK: {
                    /* only ACK Bit set */
                    uint8_t tcp_payload_len = NTOHS(ipv6_header->length) - TCP_HDR_LEN;
                    uint8_t state = tcp_socket->socket_values.tcp_control.state;

                    if ((tcp_payload_len > 0) && (state == TCP_ESTABLISHED)) {
                        /* handle data segments only when the connection was established successfully */
                        handle_tcp_no_flags_packet(ipv6_header, tcp_header, tcp_socket, payload, tcp_payload_len);
                    }
                    else if (tcp_payload_len == 0
                            && (state == TCP_ESTABLISHED || state == TCP_SYN_RCVD
                                || state == TCP_CLOSING || state == TCP_LAST_ACK)) {
                        /* no payload, acknowledging data only */
                        handle_tcp_ack_packet(ipv6_header, tcp_header, tcp_socket);
                    }
                    break;
                }

                case TCP_RST: {
                    printf("RST Bit set!\n");
                    /* only RST Bit set */
                    handle_tcp_rst_packet(ipv6_header, tcp_header, tcp_socket);
                    break;
                }

                case TCP_SYN: {
                    /* only SYN Bit set, look for matching, listening socket
                     * and request new queued socket */
                    printf("SYN Bit set!\n");
                    handle_tcp_syn_packet(ipv6_header, tcp_header, tcp_socket);
                    break;
                }

                case TCP_SYN_ACK: {
                    /* only SYN and ACK Bit set, complete three way handshake
                     * when socket in state TCP_SYN_SENT */
                    handle_tcp_syn_ack_packet(ipv6_header, tcp_header, tcp_socket);
                    break;
                }

                case TCP_FIN_ACK: {
                    if (tcp_socket->socket_values.tcp_control.state == TCP_ESTABLISHED) {
                        /* this is the first FIN */
                        printf("FIN ACK Bit set!\n");
                        handle_tcp_fin_packet(ipv6_header, tcp_header, tcp_socket);
                    }
                    else {
                        /* this is the response to FIN */
                        handle_tcp_fin_ack_packet(ipv6_header, tcp_header, tcp_socket);
                    }
                    break;
                }

                default: {
                    printf("Unable to process the incoming segment!\n");
                }
            }
        }
        else {
            printf("Wrong checksum (%x) or no corresponding socket found!\n",
                   chksum);
            printArrayRange(((uint8_t *)ipv6_header), IPV6_HDR_LEN +
                            NTOHS(ipv6_header->length), "Incoming");
            print_tcp_status(INC_PACKET, ipv6_header, tcp_header,
                             &tcp_socket->socket_values);
        }

        msg_reply(&m_recv_ip, &m_send_ip);
    }
}
