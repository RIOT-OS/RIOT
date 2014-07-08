/**
 * Destiny socket API
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup destiny
 * @{
 * @file    socket.c
 * @brief   functions for BSD socket API
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hwtimer.h"
#include "ipv6.h"
#include "thread.h"
#include "vtimer.h"

#include "net_help.h"

#include "msg_help.h"
#include "tcp.h"
#include "tcp_hc.h"
#include "tcp_timer.h"
#include "udp.h"

#include "socket.h"

#define EPHEMERAL_PORTS 	49152

socket_internal_t sockets[MAX_SOCKETS];

void print_socket(socket_t *current_socket);
void print_internal_socket(socket_internal_t *current_socket_internal);
void printf_tcp_context(tcp_hc_context_t *current_tcp_context);
int exists_socket(int socket);
void set_socket_address(sockaddr6_t *sockaddr, sa_family_t sin6_family,
                        uint16_t sin6_port, uint32_t sin6_flowinfo,
                        ipv6_addr_t *sin6_addr);
void set_tcp_packet(tcp_hdr_t *tcp_hdr, uint16_t src_port, uint16_t dst_port,
                    uint32_t seq_nr, uint32_t ack_nr,
                    uint8_t data_offset, uint8_t reserved_flags,
                    uint16_t window, uint16_t checksum, uint16_t urg_pointer);

void printf_tcp_context(tcp_hc_context_t *current_tcp_context)
{
    printf("Context: %u\n", current_tcp_context->context_id);
    printf("Rcv Seq: %" PRIu32 " Rcv Ack: %" PRIu32 ", Rcv Wnd: %u\n", current_tcp_context->seq_rcv, current_tcp_context->ack_rcv, current_tcp_context->wnd_rcv);
    printf("Snd Seq: %" PRIu32 " Snd Ack: %" PRIu32 ", Snd Wnd: %u\n", current_tcp_context->seq_snd, current_tcp_context->ack_snd, current_tcp_context->wnd_snd);
}

void print_tcp_flags(tcp_hdr_t *tcp_header)
{
    printf("FLAGS: ");

    switch (tcp_header->reserved_flags) {
        case TCP_ACK: {
            printf("ACK ");
            break;
        }

        case TCP_RST: {
            printf("RST ");
            break;
        }

        case TCP_SYN: {
            printf("SYN ");
            break;
        }

        case TCP_FIN: {
            printf("FIN ");
            break;
        }

        case TCP_URG_PSH: {
            printf("URG PSH ");
            break;
        }

        case TCP_SYN_ACK: {
            printf("SYN ACK ");
            break;
        }

        case TCP_FIN_ACK: {
            printf("FIN ACK ");
            break;
        }
    }

    printf("\n");
}

void print_tcp_cb(tcp_cb_t *cb)
{
    timex_t now;
    vtimer_now(&now);
    printf("Send_ISS: %" PRIu32 "\nSend_UNA: %" PRIu32 "\nSend_NXT: %" PRIu32 "\nSend_WND: %u\n",
           cb->send_iss, cb->send_una, cb->send_nxt, cb->send_wnd);
    printf("Rcv_IRS: %" PRIu32 "\nRcv_NXT: %" PRIu32 "\nRcv_WND: %u\n",
           cb->rcv_irs, cb->rcv_nxt, cb->rcv_wnd);
    printf("Time difference: %" PRIu64 ", No_of_retries: %u, State: %u\n\n",
           timex_uint64(timex_sub(now, cb->last_packet_time)), cb->no_of_retries, cb->state);
}

void print_tcp_status(int in_or_out, ipv6_hdr_t *ipv6_header,
                      tcp_hdr_t *tcp_header, socket_t *tcp_socket)
{
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("--- %s TCP packet: ---\n",
           (in_or_out == INC_PACKET ? "Incoming" : "Outgoing"));
    printf("IPv6 Source: %s\n",
           ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                            &ipv6_header->srcaddr));
    printf("IPv6 Dest: %s\n",
           ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                            &ipv6_header->destaddr));
    printf("TCP Length: %x\n", NTOHS(ipv6_header->length) - TCP_HDR_LEN);
    printf("Source Port: %x, Dest. Port: %x\n",
           NTOHS(tcp_header->src_port), NTOHS(tcp_header->dst_port));
    printf("Source Port: %u, Dest. Port: %u\n",
           NTOHS(tcp_header->src_port), NTOHS(tcp_header->dst_port));
    printf("ACK: %" PRIu32 ", SEQ: %" PRIu32 ", Window: %x\n",
           tcp_header->ack_nr, tcp_header->seq_nr, tcp_header->window);
    printf("ACK: %" PRIu32 ", SEQ: %" PRIu32 ", Window: %u\n",
           tcp_header->ack_nr, tcp_header->seq_nr, tcp_header->window);
    print_tcp_flags(tcp_header);
    print_tcp_cb(&tcp_socket->tcp_control);
#ifdef TCP_HC
    printf_tcp_context(&tcp_socket->tcp_control.tcp_context);
#endif
}

void print_socket(socket_t *current_socket)
{
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("Domain: %i, Type: %i, Protocol: %i \n",
           current_socket->domain,
           current_socket->type,
           current_socket->protocol);
    printf("Local address: %s\n",
           ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                            &current_socket->local_address.sin6_addr));
    printf("Foreign address: %s\n",
           ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                            &current_socket->foreign_address.sin6_addr));
    printf("Local Port: %u, Foreign Port: %u\n",
           NTOHS(current_socket->local_address.sin6_port),
           NTOHS(current_socket->foreign_address.sin6_port));
}

void print_internal_socket(socket_internal_t *current_socket_internal)
{
    socket_t *current_socket = &current_socket_internal->socket_values;
    printf("\n--------------------------\n");
    printf("ID: %i, RECV PID: %i SEND PID: %i\n",
           current_socket_internal->socket_id, current_socket_internal->recv_pid,
           current_socket_internal->send_pid);
    print_socket(current_socket);
    printf("\n--------------------------\n");
}

socket_internal_t *get_socket(int s)
{
    if (exists_socket(s)) {
        return &(sockets[s - 1]);
    }
    else {
        return NULL;
    }
}

void destiny_socket_print_sockets(void)
{
    int i;
    printf("\n---   Socket list:   ---\n");

    for (i = 1; i < MAX_SOCKETS + 1; i++) {
        if (get_socket(i) != NULL) {
            print_internal_socket(get_socket(i));
        }
    }
}

int exists_socket(int socket)
{
    if (sockets[socket - 1].socket_id == 0) {
        return false;
    }
    else {
        return true;
    }
}

void close_socket(socket_internal_t *current_socket)
{
    memset(current_socket, 0, sizeof(socket_internal_t));
}

bool isUDPSocket(uint8_t s)
{
    if ((exists_socket(s)) &&
        (get_socket(s)->socket_values.domain == PF_INET6) &&
        (get_socket(s)->socket_values.type == SOCK_DGRAM) &&
        ((get_socket(s)->socket_values.protocol == IPPROTO_UDP) ||
         (get_socket(s)->socket_values.protocol == 0))) {
        return true;
    }
    else {
        return false;
    }
}

bool is_tcp_socket(int s)
{
    if ((exists_socket(s)) &&
        (get_socket(s)->socket_values.domain == PF_INET6) &&
        (get_socket(s)->socket_values.type == SOCK_STREAM) &&
        ((get_socket(s)->socket_values.protocol == IPPROTO_TCP) ||
         (get_socket(s)->socket_values.protocol == 0))) {
        return true;
    }
    else {
        return false;
    }
}

int bind_udp_socket(int s, sockaddr6_t *name, int namelen, uint8_t pid)
{
    int i;

    if (!exists_socket(s)) {
        return -1;
    }

    for (i = 1; i < MAX_SOCKETS + 1; i++) {
        if (isUDPSocket(i) &&
            (get_socket(i)->socket_values.local_address.sin6_port == name->sin6_port)) {
            return -1;
        }
    }

    memcpy(&get_socket(s)->socket_values.local_address, name, namelen);
    get_socket(s)->recv_pid = pid;
    return 0;
}

int bind_tcp_socket(int s, sockaddr6_t *name, int namelen, uint8_t pid)
{
    int i;

    if (!exists_socket(s)) {
        return -1;
    }

    for (i = 1; i < MAX_SOCKETS + 1; i++) {
        if (is_tcp_socket(i) &&
            (get_socket(i)->socket_values.local_address.sin6_port == name->sin6_port)) {
            return -1;
        }
    }

    memcpy(&get_socket(s)->socket_values.local_address, name, namelen);
    get_socket(s)->recv_pid = pid;
    get_socket(s)->socket_values.tcp_control.rto = TCP_INITIAL_ACK_TIMEOUT;
    return 0;
}

int destiny_socket(int domain, int type, int protocol)
{
    int i = 1;

    while (get_socket(i) != NULL) {
        i++;
    }

    if (i > MAX_SOCKETS + 1) {
        return -1;
    }
    else {
        socket_t *current_socket = &sockets[i - 1].socket_values;
        sockets[i - 1].socket_id = i;
        current_socket->domain = domain;
        current_socket->type = type;
        current_socket->protocol = protocol;
        current_socket->tcp_control.state = TCP_CLOSED;
        return sockets[i - 1].socket_id;
    }
}

socket_internal_t *get_udp_socket(udp_hdr_t *udp_header)
{
    uint8_t i = 1;

    while (i < MAX_SOCKETS + 1) {
        if (isUDPSocket(i) &&
            (get_socket(i)->socket_values.local_address.sin6_port ==
             udp_header->dst_port)) {
            return get_socket(i);
        }

        i++;
    }

    return NULL;
}

bool is_four_touple(socket_internal_t *current_socket, ipv6_hdr_t *ipv6_header,
                    tcp_hdr_t *tcp_header)
{
    return (ipv6_addr_is_equal(&current_socket->socket_values.local_address.sin6_addr,
                               &ipv6_header->destaddr) &&
            (current_socket->socket_values.local_address.sin6_port == tcp_header->dst_port) &&
            ipv6_addr_is_equal(&current_socket->socket_values.foreign_address.sin6_addr,
                               &ipv6_header->srcaddr) &&
            (current_socket->socket_values.foreign_address.sin6_port == tcp_header->src_port));
}

socket_internal_t *get_tcp_socket(ipv6_hdr_t *ipv6_header, tcp_hdr_t *tcp_header)
{
    uint8_t i = 1;
    socket_internal_t *current_socket = NULL;
    socket_internal_t *listening_socket = NULL;
    uint8_t compare[16];
    memset(compare, 0, 16);

    while (i < MAX_SOCKETS + 1) {
        current_socket = get_socket(i);

        /* Check for matching 4 touple, TCP_ESTABLISHED connection */
        if (is_tcp_socket(i) && is_four_touple(current_socket, ipv6_header,
                                               tcp_header)) {
            return current_socket;
        }
        /* Sockets in TCP_LISTEN and TCP_SYN_RCVD state should only be tested on local TCP values */
        else if (is_tcp_socket(i) &&
                 ((current_socket->socket_values.tcp_control.state == TCP_LISTEN) ||
                  (current_socket->socket_values.tcp_control.state == TCP_SYN_RCVD)) &&
                 (current_socket->socket_values.local_address.sin6_addr.uint8[15] ==
                  ipv6_header->destaddr.uint8[15]) &&
                 (current_socket->socket_values.local_address.sin6_port ==
                  tcp_header->dst_port) &&
                 (current_socket->socket_values.foreign_address.sin6_addr.uint8[15] ==
                  0x00) &&
                 (current_socket->socket_values.foreign_address.sin6_port == 0)) {
            listening_socket = current_socket;
        }

        i++;
    }

    /* Return either NULL if nothing was matched or the listening 2 touple socket */
    return listening_socket;
}

uint16_t get_free_source_port(uint8_t protocol)
{
    int i;
    uint16_t biggest_port = EPHEMERAL_PORTS - 1;

    /* Remember biggest ephemeral port number used so far and add 1 */
    for (i = 0; i < MAX_SOCKETS; i++) {
        if ((sockets[i].socket_values.protocol == protocol) &&
            (sockets[i].socket_values.local_address.sin6_port > biggest_port)) {
            biggest_port = sockets[i].socket_values.local_address.sin6_port;
        }
    }

    return biggest_port + 1;
}

void set_socket_address(sockaddr6_t *sockaddr, uint8_t sin6_family,
                        uint16_t sin6_port, uint32_t sin6_flowinfo, ipv6_addr_t *sin6_addr)
{
    sockaddr->sin6_family 	= sin6_family;
    sockaddr->sin6_port 	= sin6_port;
    sockaddr->sin6_flowinfo	= sin6_flowinfo;
    memcpy(&sockaddr->sin6_addr, sin6_addr, 16);
}

void set_tcp_packet(tcp_hdr_t *tcp_hdr, uint16_t src_port, uint16_t dst_port,
                    uint32_t seq_nr, uint32_t ack_nr, uint8_t data_offset,
                    uint8_t reserved_flags, uint16_t window, uint16_t checksum,
                    uint16_t urg_pointer)
{
    tcp_hdr->ack_nr         = ack_nr;
    tcp_hdr->checksum       = checksum;
    tcp_hdr->data_offset    = data_offset;
    tcp_hdr->dst_port       = dst_port;
    tcp_hdr->reserved_flags = reserved_flags;
    tcp_hdr->reserved       = 0;
    tcp_hdr->flag_ns        = 0;
    tcp_hdr->seq_nr         = seq_nr;
    tcp_hdr->src_port       = src_port;
    tcp_hdr->urg_pointer    = urg_pointer;
    tcp_hdr->window         = window;
}

/* Check for consistent ACK and SEQ number */
int check_tcp_consistency(socket_t *current_tcp_socket, tcp_hdr_t *tcp_header, uint8_t tcp_payload_len)
{
    if (tcp_payload_len == 0) {
        if (tcp_header->ack_nr > (current_tcp_socket->tcp_control.send_nxt)) {
            /* ACK of not yet sent byte, discard */
            return ACK_NO_TOO_BIG;
        }
        else if (tcp_header->ack_nr <= (current_tcp_socket->tcp_control.send_una)) {
            /* ACK of previous segments, maybe dropped? */
            return ACK_NO_TOO_SMALL;
        }
    }
    else if ((current_tcp_socket->tcp_control.rcv_nxt > 0) && (tcp_header->seq_nr < current_tcp_socket->tcp_control.rcv_nxt)) {
        /* segment repetition, maybe ACK got lost? */
        return SEQ_NO_TOO_SMALL;
    }

    return PACKET_OK;
}

void switch_tcp_packet_byte_order(tcp_hdr_t *current_tcp_packet)
{
    if (current_tcp_packet->data_offset * 4 > TCP_HDR_LEN) {
        if (*(((uint8_t *)current_tcp_packet) + TCP_HDR_LEN) == TCP_MSS_OPTION) {
            uint8_t *packet_pointer = (uint8_t *)current_tcp_packet;
            packet_pointer += (TCP_HDR_LEN + 2);
            uint8_t mss1 = *packet_pointer;
            uint8_t mss2 = *(packet_pointer + 1);
            *packet_pointer = mss2;
            *(packet_pointer + 1) = mss1;
        }

        if (*(((uint8_t *)current_tcp_packet) + TCP_HDR_LEN) == TCP_TS_OPTION) {
            /* TODO: Timestamp option not implemented */
        }
    }

    current_tcp_packet->seq_nr = HTONL(current_tcp_packet->seq_nr);
    current_tcp_packet->ack_nr = HTONL(current_tcp_packet->ack_nr);
    current_tcp_packet->window = HTONS(current_tcp_packet->window);
    current_tcp_packet->urg_pointer = HTONS(current_tcp_packet->urg_pointer);
}

int send_tcp(socket_internal_t *current_socket, tcp_hdr_t *current_tcp_packet,
             ipv6_hdr_t *temp_ipv6_header, uint8_t flags, uint8_t payload_length)
{
    socket_t *current_tcp_socket = &current_socket->socket_values;
    uint8_t header_length = TCP_HDR_LEN / 4;

    if (IS_TCP_SYN(flags) || IS_TCP_SYN_ACK(flags)) {
        tcp_mss_option_t current_mss_option;
        header_length += sizeof(tcp_mss_option_t) / 4;

        current_mss_option.kind 	= TCP_MSS_OPTION;
        current_mss_option.len 		= sizeof(tcp_mss_option_t);
        current_mss_option.mss		= DESTINY_SOCKET_STATIC_MSS;
        memcpy(((uint8_t *)current_tcp_packet) + TCP_HDR_LEN,
               &current_mss_option, sizeof(tcp_mss_option_t));
    }

    set_tcp_packet(current_tcp_packet, current_tcp_socket->local_address.sin6_port,
                   current_tcp_socket->foreign_address.sin6_port,
                   current_tcp_socket->tcp_control.send_una,
                   (IS_TCP_ACK(flags) ? current_tcp_socket->tcp_control.rcv_nxt : 0x00), header_length, flags,
                   current_tcp_socket->tcp_control.rcv_wnd, 0, 0);

    /* Fill IPv6 Header */
    memcpy(&(temp_ipv6_header->destaddr),
           &current_tcp_socket->foreign_address.sin6_addr, 16);
    memcpy(&(temp_ipv6_header->srcaddr),
           &current_tcp_socket->local_address.sin6_addr, 16);
    temp_ipv6_header->length = HTONS(header_length * 4 + payload_length);

    current_tcp_packet->checksum = ~tcp_csum(temp_ipv6_header, current_tcp_packet);

#ifdef TCP_HC
    uint16_t compressed_size;

    compressed_size = compress_tcp_packet(current_socket,
                                          (uint8_t *)current_tcp_packet,
                                          temp_ipv6_header, flags,
                                          payload_length);

    if (compressed_size == 0) {
        /* Error in compressing tcp packet header */
        return -1;
    }

    return ipv6_sendto(&current_tcp_socket->foreign_address.sin6_addr,
                       IPPROTO_TCP, (uint8_t *)(current_tcp_packet),
                       compressed_size);
#else
    switch_tcp_packet_byte_order(current_tcp_packet);
    return ipv6_sendto(&current_tcp_socket->foreign_address.sin6_addr,
                       IPPROTO_TCP, (uint8_t *)(current_tcp_packet),
                       header_length * 4 + payload_length);
#endif
}

void set_tcp_cb(tcp_cb_t *tcp_control, uint32_t rcv_nxt, uint16_t rcv_wnd,
                uint32_t send_nxt, uint32_t send_una, uint16_t send_wnd)
{
    tcp_control->rcv_nxt = rcv_nxt;
    tcp_control->rcv_wnd = rcv_wnd;
    tcp_control->send_nxt = send_nxt;
    tcp_control->send_una = send_una;
    tcp_control->send_wnd = send_wnd;
}

int destiny_socket_connect(int socket, sockaddr6_t *addr, uint32_t addrlen)
{
    (void) addrlen;

    /* Variables */
    ipv6_addr_t src_addr;
    socket_internal_t *current_int_tcp_socket;
    socket_t *current_tcp_socket;
    msg_t msg_from_server;
    uint8_t send_buffer[BUFFER_SIZE];
    ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&send_buffer));
    tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t *)(&send_buffer[IPV6_HDR_LEN]));

    /* Check if socket exists */
    current_int_tcp_socket = get_socket(socket);

    if (current_int_tcp_socket == NULL) {
        return -1;
    }

    current_tcp_socket = &current_int_tcp_socket->socket_values;

    current_int_tcp_socket->recv_pid = thread_getpid();

    /* Local address information */
    ipv6_net_if_get_best_src_addr(&src_addr, &addr->sin6_addr);
    set_socket_address(&current_tcp_socket->local_address, PF_INET6,
                       HTONS(get_free_source_port(IPPROTO_TCP)), 0, &src_addr);

    /* Foreign address information */
    set_socket_address(&current_tcp_socket->foreign_address, addr->sin6_family,
                       addr->sin6_port, addr->sin6_flowinfo, &addr->sin6_addr);

    /* Fill lcoal TCP socket information */
    srand(addr->sin6_port);

    current_tcp_socket->tcp_control.rcv_irs	= 0;
    mutex_lock(&global_sequence_counter_mutex);
    current_tcp_socket->tcp_control.send_iss = global_sequence_counter;
    mutex_unlock(&global_sequence_counter_mutex);
    current_tcp_socket->tcp_control.state = TCP_SYN_SENT;

#ifdef TCP_HC
    /* Choosing random number Context ID */
    mutex_lock(&global_context_counter_mutex);
    current_tcp_socket->tcp_control.tcp_context.context_id = global_context_counter;
    mutex_unlock(&global_context_counter_mutex);

    current_tcp_socket->tcp_control.tcp_context.hc_type = FULL_HEADER;

    /* Remember TCP Context for possible TCP_RETRY */
    tcp_hc_context_t saved_tcp_context;
    memcpy(&saved_tcp_context, &current_tcp_socket->tcp_control.tcp_context,
           sizeof(tcp_hc_context_t));
#endif

    set_tcp_cb(&current_tcp_socket->tcp_control, 0, DESTINY_SOCKET_STATIC_WINDOW,
               current_tcp_socket->tcp_control.send_iss + 1,
               current_tcp_socket->tcp_control.send_iss, 0);

    /* Remember current time */
    timex_t now;
    vtimer_now(&now);
    current_tcp_socket->tcp_control.last_packet_time = now;
    current_tcp_socket->tcp_control.no_of_retries = 0;

    msg_from_server.type = TCP_RETRY;

    while (msg_from_server.type == TCP_RETRY) {
        /* Send packet */
        send_tcp(current_int_tcp_socket, current_tcp_packet, temp_ipv6_header,
                 TCP_SYN, 0);

        /* wait for SYN ACK or RETRY */
        msg_receive(&msg_from_server);

        if (msg_from_server.type == TCP_TIMEOUT) {
#ifdef TCP_HC
            /* We did not send anything successful so restore last context */
            memcpy(&current_tcp_socket->tcp_control.tcp_context,
                   &saved_tcp_context, sizeof(tcp_hc_context_t));
#endif
            return -1;
        }

#ifdef TCP_HC
        else if (msg_from_server.type == TCP_RETRY) {
            /* We retry sending a packet so set everything to last values again */
            memcpy(&current_tcp_socket->tcp_control.tcp_context,
                   &saved_tcp_context, sizeof(tcp_hc_context_t));
        }

#endif
    }

    /* Read packet content */
    tcp_hdr_t *tcp_header = ((tcp_hdr_t *)(msg_from_server.content.ptr));

    /* Check for consistency */
    if (tcp_header->ack_nr != current_tcp_socket->tcp_control.send_nxt) {
        printf("TCP packets not consistent!\n");
    }

    /* Got SYN ACK from Server */
    /* Refresh foreign TCP socket information */
    if ((tcp_header->data_offset * 4 > TCP_HDR_LEN) &&
        (*(((uint8_t *)tcp_header) + TCP_HDR_LEN) == TCP_MSS_OPTION)) {
        current_tcp_socket->tcp_control.mss =
            *((uint16_t *)(((uint8_t *)tcp_header) + TCP_HDR_LEN + 2));
    }
    else {
        current_tcp_socket->tcp_control.mss = DESTINY_SOCKET_STATIC_MSS;
    }

    current_tcp_socket->tcp_control.rcv_irs = tcp_header->seq_nr;
    set_tcp_cb(&current_tcp_socket->tcp_control, tcp_header->seq_nr + 1,
               current_tcp_socket->tcp_control.rcv_wnd,
               current_tcp_socket->tcp_control.send_una + 1,
               tcp_header->ack_nr,
               tcp_header->window);

    msg_from_server.type = UNDEFINED;

    /* Remember current time */
    vtimer_now(&now);
    current_tcp_socket->tcp_control.last_packet_time = now;
    current_tcp_socket->tcp_control.no_of_retries = 0;

#ifdef TCP_HC
    current_tcp_socket->tcp_control.tcp_context.hc_type = FULL_HEADER;
    /* Remember TCP Context for possible TCP_RETRY */
    memcpy(&saved_tcp_context, &current_tcp_socket->tcp_control.tcp_context,
           sizeof(tcp_hc_context_t));
#endif

    while (msg_from_server.type != TCP_RETRY) {
        /* Send packet */
        send_tcp(current_int_tcp_socket, current_tcp_packet, temp_ipv6_header,
                 TCP_ACK, 0);

        msg_receive(&msg_from_server);
#ifdef TCP_HC

        if (msg_from_server.type == TCP_SYN_ACK) {
            /* TCP_SYN_ACK from server arrived again, copy old context and
             * send TCP_ACK again */
            memcpy(&current_tcp_socket->tcp_control.tcp_context,
                   &saved_tcp_context, sizeof(tcp_hc_context_t));
        }
        else if (msg_from_server.type == TCP_RETRY) {
            /* We waited for RTT, no TCP_SYN_ACK received, so we assume the
             * TCP_ACK packet arrived safely */
        }

#endif
    }

    current_tcp_socket->tcp_control.state = TCP_ESTABLISHED;

    current_int_tcp_socket->recv_pid = 255;

    destiny_socket_print_sockets();
    return 0;
}

void calculate_rto(tcp_cb_t *tcp_control, timex_t current_time)
{
    double rtt = (double) timex_uint64(timex_sub(current_time, tcp_control->last_packet_time));
    double srtt = tcp_control->srtt;
    double rttvar = tcp_control->rttvar;
    double rto = tcp_control->rto;

    if ((srtt == 0) && (rttvar == 0) && (rto == TCP_INITIAL_ACK_TIMEOUT)) {
        /* First calculation */
        srtt = rtt;
        rttvar = 0.5 * rtt;
        rto = rtt + (((4 * rttvar) < TCP_TIMER_RESOLUTION) ?
                     (TCP_TIMER_RESOLUTION) : (4 * rttvar));
    }
    else {
        /* every other calculation */
        srtt = (1 - TCP_ALPHA) * srtt + TCP_ALPHA * rtt;
        rttvar = (1 - TCP_BETA) * rttvar + TCP_BETA * abs(srtt - rtt);
        rto = srtt + (((4 * rttvar) < TCP_TIMER_RESOLUTION) ?
                      (TCP_TIMER_RESOLUTION) : (4 * rttvar));
    }

    if (rto < SECOND) {
        rto = SECOND;
    }

    tcp_control->srtt = srtt;
    tcp_control->rttvar = rttvar;
    tcp_control->rto = rto;
}

int32_t destiny_socket_send(int s, const void *buf, uint32_t len, int flags)
{
    (void) flags;

    /* Variables */
    msg_t recv_msg;
    int32_t sent_bytes = 0;
    uint32_t total_sent_bytes = 0;
    socket_internal_t *current_int_tcp_socket;
    socket_t *current_tcp_socket;
    uint8_t send_buffer[BUFFER_SIZE];
    memset(send_buffer, 0, BUFFER_SIZE);
    ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&send_buffer));
    tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t *)(&send_buffer[IPV6_HDR_LEN]));

    /* Check if socket exists and is TCP socket */
    if (!is_tcp_socket(s)) {
        return -1;
    }

    current_int_tcp_socket = get_socket(s);
    current_tcp_socket = &current_int_tcp_socket->socket_values;

    /* Check for TCP_ESTABLISHED STATE */
    if (current_tcp_socket->tcp_control.state != TCP_ESTABLISHED) {
        return -1;
    }

    /* Add thread PID */
    current_int_tcp_socket->send_pid = thread_getpid();

    recv_msg.type = UNDEFINED;

    while (1) {
        current_tcp_socket->tcp_control.no_of_retries = 0;

#ifdef TCP_HC
        current_tcp_socket->tcp_control.tcp_context.hc_type = COMPRESSED_HEADER;
        /* Remember TCP Context for possible TCP_RETRY */
        tcp_hc_context_t saved_tcp_context;
        memcpy(&saved_tcp_context, &current_tcp_socket->tcp_control.tcp_context,
               sizeof(tcp_hc_context_t) - 1);
#endif

        while (recv_msg.type != TCP_ACK) {
            /* Add packet data */
            if (current_tcp_socket->tcp_control.send_wnd >
                current_tcp_socket->tcp_control.mss) {
                /* Window size > Maximum Segment Size */
                if ((len - total_sent_bytes) > current_tcp_socket->tcp_control.mss) {
                    memcpy(&send_buffer[IPV6_HDR_LEN + TCP_HDR_LEN], buf,
                           current_tcp_socket->tcp_control.mss);
                    sent_bytes = current_tcp_socket->tcp_control.mss;
                    total_sent_bytes += sent_bytes;
                }
                else {
                    memcpy(&send_buffer[IPV6_HDR_LEN + TCP_HDR_LEN],
                           (uint8_t *) buf + total_sent_bytes, len - total_sent_bytes);
                    sent_bytes = len - total_sent_bytes;
                    total_sent_bytes = len;
                }
            }
            else {
                /* Window size <= Maximum Segment Size */
                if ((len - total_sent_bytes) > current_tcp_socket->tcp_control.send_wnd) {
                    memcpy(&send_buffer[IPV6_HDR_LEN + TCP_HDR_LEN], buf,
                           current_tcp_socket->tcp_control.send_wnd);
                    sent_bytes = current_tcp_socket->tcp_control.send_wnd;
                    total_sent_bytes += sent_bytes;
                }
                else {
                    memcpy(&send_buffer[IPV6_HDR_LEN + TCP_HDR_LEN],
                           (uint8_t *) buf + total_sent_bytes, len - total_sent_bytes);
                    sent_bytes = len - total_sent_bytes;
                    total_sent_bytes = len;
                }
            }

            current_tcp_socket->tcp_control.send_nxt += sent_bytes;
            current_tcp_socket->tcp_control.send_wnd -= sent_bytes;

            if (send_tcp(current_int_tcp_socket, current_tcp_packet,
                         temp_ipv6_header, TCP_ACK, sent_bytes) < 0) {
                /* Error while sending tcp data */
                current_tcp_socket->tcp_control.send_nxt -= sent_bytes;
                current_tcp_socket->tcp_control.send_wnd += sent_bytes;
#ifdef TCP_HC
                memcpy(&current_tcp_socket->tcp_control.tcp_context,
                       &saved_tcp_context, sizeof(tcp_hc_context_t));
                current_tcp_socket->tcp_control.tcp_context.hc_type =
                    COMPRESSED_HEADER;
#endif
                printf("Error while sending, returning to application thread!\n");
                return -1;
            }

            /* Remember current time */
            vtimer_now(&current_tcp_socket->tcp_control.last_packet_time);
            net_msg_receive(&recv_msg);

            switch (recv_msg.type) {
                case TCP_ACK: {
                    if (current_tcp_socket->tcp_control.no_of_retries == 0) {
                        calculate_rto(&current_tcp_socket->tcp_control,
                                      current_tcp_socket->tcp_control.last_packet_time);
                    }

                    tcp_hdr_t *tcp_header = ((tcp_hdr_t *)(recv_msg.content.ptr));

                    if ((current_tcp_socket->tcp_control.send_nxt ==
                         tcp_header->ack_nr) && (total_sent_bytes == len)) {
                        current_tcp_socket->tcp_control.send_una = tcp_header->ack_nr;
                        current_tcp_socket->tcp_control.send_nxt = tcp_header->ack_nr;
                        current_tcp_socket->tcp_control.send_wnd = tcp_header->window;
                        /* Got ACK for every sent byte */
#ifdef TCP_HC
                        current_tcp_socket->tcp_control.tcp_context.hc_type =
                            COMPRESSED_HEADER;
#endif
                        return sent_bytes;
                    }
                    else if ((current_tcp_socket->tcp_control.send_nxt ==
                              tcp_header->ack_nr) && (total_sent_bytes != len)) {
                        current_tcp_socket->tcp_control.send_una = tcp_header->ack_nr;
                        current_tcp_socket->tcp_control.send_nxt = tcp_header->ack_nr;
                        current_tcp_socket->tcp_control.send_wnd = tcp_header->window;
                        /* Got ACK for every sent byte */
#ifdef TCP_HC
                        current_tcp_socket->tcp_control.tcp_context.hc_type =
                            COMPRESSED_HEADER;
#endif
                        break;
                    }

                    /* else {
                     *     TODO: If window size > MSS, ACK was valid only for
                     *     a few segments, handle retransmit of missing
                     *     segments
                     *	break;
                     * } */
                    break;
                }

                case TCP_RETRY: {
                    current_tcp_socket->tcp_control.send_nxt -= sent_bytes;
                    current_tcp_socket->tcp_control.send_wnd += sent_bytes;
                    total_sent_bytes -= sent_bytes;
#ifdef TCP_HC
                    memcpy(&current_tcp_socket->tcp_control.tcp_context,
                           $&saved_tcp_context, sizeof(tcp_hc_context_t));
                    current_tcp_socket->tcp_control.tcp_context.hc_type =
                        MOSTLY_COMPRESSED_HEADER;
#endif
                    break;
                }

                case TCP_TIMEOUT: {
                    current_tcp_socket->tcp_control.send_nxt -= sent_bytes;
                    current_tcp_socket->tcp_control.send_wnd += sent_bytes;
#ifdef TCP_HC
                    memcpy(&current_tcp_socket->tcp_control.tcp_context,
                           &saved_tcp_context, sizeof(tcp_hc_context_t));
                    current_tcp_socket->tcp_control.tcp_context.hc_type =
                        COMPRESSED_HEADER;
#endif
                    return -1;
                    break;
                }
            }
        }
    }

    return sent_bytes;
}

uint8_t read_from_socket(socket_internal_t *current_int_tcp_socket,
                         void *buf, int len)
{
    if (len >= current_int_tcp_socket->tcp_input_buffer_end) {
        mutex_lock(&current_int_tcp_socket->tcp_buffer_mutex);
        uint8_t read_bytes = current_int_tcp_socket->tcp_input_buffer_end;
        memcpy(buf, current_int_tcp_socket->tcp_input_buffer,
               current_int_tcp_socket->tcp_input_buffer_end);
        current_int_tcp_socket->tcp_input_buffer_end = 0;
        current_int_tcp_socket->socket_values.tcp_control.rcv_wnd += read_bytes;
        mutex_unlock(&current_int_tcp_socket->tcp_buffer_mutex);
        return read_bytes;
    }
    else {
        mutex_lock(&current_int_tcp_socket->tcp_buffer_mutex);
        memcpy(buf, current_int_tcp_socket->tcp_input_buffer, len);
        memmove(current_int_tcp_socket->tcp_input_buffer,
                (current_int_tcp_socket->tcp_input_buffer + len),
                current_int_tcp_socket->tcp_input_buffer_end - len);
        current_int_tcp_socket->tcp_input_buffer_end =
            current_int_tcp_socket->tcp_input_buffer_end - len;
        current_int_tcp_socket->socket_values.tcp_control.rcv_wnd += len;
        mutex_unlock(&current_int_tcp_socket->tcp_buffer_mutex);
        return len;
    }
}

int32_t destiny_socket_recv(int s, void *buf, uint32_t len, int flags)
{
    (void) flags;

    /* Variables */
    uint8_t read_bytes;
    msg_t m_recv, m_send;
    socket_internal_t *current_int_tcp_socket;

    /* Check if socket exists */
    if (!is_tcp_socket(s)) {
        printf("INFO: NO TCP SOCKET!\n");
        return -1;
    }

    current_int_tcp_socket = get_socket(s);

    /* Setting Thread PID */
    current_int_tcp_socket->recv_pid = thread_getpid();

    if (current_int_tcp_socket->tcp_input_buffer_end > 0) {
        return read_from_socket(current_int_tcp_socket, buf, len);
    }

    msg_receive(&m_recv);

    if ((exists_socket(s)) && (current_int_tcp_socket->tcp_input_buffer_end > 0)) {
        read_bytes = read_from_socket(current_int_tcp_socket, buf, len);
        net_msg_reply(&m_recv, &m_send, UNDEFINED);
        return read_bytes;
    }

    /* Received FIN */
    if (m_recv.type == CLOSE_CONN) {
        /* Sent FIN_ACK, wait for ACK */
        msg_receive(&m_recv);
        /* Received ACK, return with closed socket!*/
        return -1;
    }

    /* Received Last ACK (connection closed) or no data to read yet */
    return -1;
}

int32_t destiny_socket_recvfrom(int s, void *buf, uint32_t len, int flags,
                                sockaddr6_t *from, uint32_t *fromlen)
{
    (void) flags;

    if (isUDPSocket(s)) {
        msg_t m_recv, m_send;
        ipv6_hdr_t *ipv6_header;
        udp_hdr_t *udp_header;
        uint8_t *payload;
        get_socket(s)->recv_pid = thread_getpid();

        msg_receive(&m_recv);

        ipv6_header = ((ipv6_hdr_t *)m_recv.content.ptr);
        udp_header = ((udp_hdr_t *)(m_recv.content.ptr + IPV6_HDR_LEN));
        payload = (uint8_t *)(m_recv.content.ptr + IPV6_HDR_LEN + UDP_HDR_LEN);

        memset(buf, 0, len);
        memcpy(buf, payload, NTOHS(udp_header->length) - UDP_HDR_LEN);
        memcpy(&from->sin6_addr, &ipv6_header->srcaddr, 16);
        from->sin6_family = AF_INET6;
        from->sin6_flowinfo = 0;
        from->sin6_port = NTOHS(udp_header->src_port);
        *fromlen = sizeof(sockaddr6_t);

        msg_reply(&m_recv, &m_send);
        return NTOHS(udp_header->length) - UDP_HDR_LEN;
    }
    else if (is_tcp_socket(s)) {
        return destiny_socket_recv(s, buf, len, flags);
    }
    else {
        printf("Socket Type not supported!\n");
        return -1;
    }
}

int32_t destiny_socket_sendto(int s, const void *buf, uint32_t len, int flags,
                              sockaddr6_t *to, uint32_t tolen)
{
    (void) flags;
    (void) tolen;

    if (isUDPSocket(s) &&
        (get_socket(s)->socket_values.foreign_address.sin6_port == 0)) {
        uint8_t send_buffer[BUFFER_SIZE];

        ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&send_buffer));
        udp_hdr_t *current_udp_packet = ((udp_hdr_t *)(&send_buffer[IPV6_HDR_LEN]));
        uint8_t *payload = &send_buffer[IPV6_HDR_LEN + UDP_HDR_LEN];

        memcpy(&(temp_ipv6_header->destaddr), &to->sin6_addr, 16);
        ipv6_net_if_get_best_src_addr(&(temp_ipv6_header->srcaddr), &(temp_ipv6_header->destaddr));

        current_udp_packet->src_port = get_free_source_port(IPPROTO_UDP);
        current_udp_packet->dst_port = to->sin6_port;
        current_udp_packet->checksum = 0;

        memcpy(payload, buf, len);
        current_udp_packet->length = HTONS(UDP_HDR_LEN + len);
        temp_ipv6_header->length = UDP_HDR_LEN + len;

        current_udp_packet->checksum = ~ipv6_csum(temp_ipv6_header,
                                       (uint8_t *) current_udp_packet,
                                       UDP_HDR_LEN + len,
                                       IPPROTO_UDP);

        return ipv6_sendto(&to->sin6_addr, IPPROTO_UDP,
                           (uint8_t *)(current_udp_packet),
                           NTOHS(current_udp_packet->length));
    }
    else {
        return -1;
    }
}

int destiny_socket_close(int s)
{
    socket_internal_t *current_socket = get_socket(s);

    if (current_socket != NULL) {
        if (is_tcp_socket(s)) {
            /* Variables */
            msg_t m_recv;
            uint8_t send_buffer[BUFFER_SIZE];
            ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&send_buffer));
            tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t *)(&send_buffer[IPV6_HDR_LEN]));

            /* Check if socket exists and is TCP socket */
            if (!is_tcp_socket(s)) {
                return -1;
            }

            /* Check for TCP_ESTABLISHED STATE */
            if (current_socket->socket_values.tcp_control.state != TCP_ESTABLISHED) {
                close_socket(current_socket);
                return 0;
            }

            current_socket->send_pid = thread_getpid();

            /* Refresh local TCP socket information */
            current_socket->socket_values.tcp_control.state = TCP_FIN_WAIT_1;
#ifdef TCP_HC
            current_socket->socket_values.tcp_control.tcp_context.hc_type =
                COMPRESSED_HEADER;
#endif

            send_tcp(current_socket, current_tcp_packet, temp_ipv6_header,
                     TCP_FIN_ACK, 0);
            msg_receive(&m_recv);
            close_socket(current_socket);
            return 1;
        }
        else if (isUDPSocket(s)) {
            close_socket(current_socket);
            return 0;
        }

        return -1;
    }
    else {
        return -1;
    }
}

int destiny_socket_bind(int s, sockaddr6_t *addr, int addrlen)
{
    if (exists_socket(s)) {
        socket_t *current_socket = &get_socket(s)->socket_values;

        switch (current_socket->domain) {
            case (PF_INET): {
                /* Not provided */
                return -1;
                break;
            }

            case (PF_INET6): {
                switch (current_socket->type) {
                        /* TCP */
                    case (SOCK_STREAM): {
                        if ((current_socket->protocol == 0) ||
                            (current_socket->protocol == IPPROTO_TCP)) {
                            return bind_tcp_socket(s, addr, addrlen,
                                                   thread_getpid());
                            break;
                        }
                        else {
                            return -1;
                            break;
                        }

                        break;
                    }

                    /* UDP */
                    case (SOCK_DGRAM): {
                        if ((current_socket->protocol == 0) ||
                            (current_socket->protocol == IPPROTO_UDP)) {
                            return bind_udp_socket(s, addr, addrlen,
                                                   thread_getpid());
                            break;
                        }
                        else {
                            return -1;
                            break;
                        }

                        break;
                    }

                    case (SOCK_SEQPACKET): {
                        /* not provided */
                        return -1;
                        break;
                    }

                    case (SOCK_RAW): {
                        /* not provided */
                        return -1;
                        break;
                    }

                    default: {
                        return -1;
                        break;
                    }
                }

                break;
            }

            case (PF_UNIX): {
                /* Not provided */
                return -1;
                break;
            }
        }
    }
    else {
        printf("SOCKET DOES NOT EXIST!\n");
        return -1;
    }

    return -1;
}

int destiny_socket_listen(int s, int backlog)
{
    (void) backlog;

    if (is_tcp_socket(s) && get_socket(s)->socket_values.tcp_control.state == TCP_CLOSED) {
        socket_internal_t *current_socket = get_socket(s);
        current_socket->socket_values.tcp_control.state = TCP_LISTEN;
        return 0;
    }
    else {
        return -1;
    }
}

socket_internal_t *get_waiting_connection_socket(int socket,
        ipv6_hdr_t *ipv6_header,
        tcp_hdr_t *tcp_header)
{
    int i;
    socket_internal_t *current_socket, *listening_socket = get_socket(socket);

    for (i = 1; i < MAX_SOCKETS + 1; i++) {
        current_socket = get_socket(i);

        if (!current_socket) {
            continue;
        }

        /* Connection establishment ACK, Check for 4 touple and state */
        if ((ipv6_header != NULL) && (tcp_header != NULL)) {
            if (is_four_touple(current_socket, ipv6_header, tcp_header) &&
                (current_socket->socket_values.tcp_control.state == TCP_SYN_RCVD)) {
                return current_socket;
            }
        }
        /* Connection establishment SYN ACK, check only for port and state */
        else {
            if ((current_socket->socket_values.tcp_control.state == TCP_SYN_RCVD) &&
                (current_socket->socket_values.local_address.sin6_port ==
                 listening_socket->socket_values.local_address.sin6_port)) {
                return current_socket;
            }
        }
    }

    return NULL;
}

int handle_new_tcp_connection(socket_internal_t *current_queued_int_socket,
                              socket_internal_t *server_socket, uint8_t pid)
{
    (void) pid;

    msg_t msg_recv_client_ack, msg_send_client_ack;
    socket_t *current_queued_socket = &current_queued_int_socket->socket_values;
    uint8_t send_buffer[BUFFER_SIZE];
    ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&send_buffer));
    tcp_hdr_t *syn_ack_packet = ((tcp_hdr_t *)(&send_buffer[IPV6_HDR_LEN]));

    current_queued_int_socket->recv_pid = thread_getpid();
#ifdef TCP_HC
    current_queued_int_socket->socket_values.tcp_control.tcp_context.hc_type =
        FULL_HEADER;
    memcpy(&current_queued_int_socket->socket_values.tcp_control.tcp_context.context_id,
           &server_socket->socket_values.tcp_control.tcp_context.context_id,
           sizeof(server_socket->socket_values.tcp_control.tcp_context.context_id));
#endif
    /* Remember current time */
    timex_t now;
    vtimer_now(&now);
    current_queued_int_socket->socket_values.tcp_control.last_packet_time = now;

    current_queued_int_socket->socket_values.tcp_control.no_of_retries = 0;

    /* Set message type to Retry for while loop */
    msg_recv_client_ack.type = TCP_RETRY;

    while (msg_recv_client_ack.type == TCP_RETRY) {
        /* Send packet */
        send_tcp(current_queued_int_socket, syn_ack_packet, temp_ipv6_header,
                 TCP_SYN_ACK, 0);

        /* wait for ACK from Client */
        msg_receive(&msg_recv_client_ack);

        if (msg_recv_client_ack.type == TCP_TIMEOUT) {
            /* Set status of internal socket back to TCP_LISTEN */
            server_socket->socket_values.tcp_control.state = TCP_LISTEN;

            close_socket(current_queued_int_socket);
            return -1;
        }
    }

    tcp_hdr_t *tcp_header;

    tcp_header = ((tcp_hdr_t *)(msg_recv_client_ack.content.ptr));

    /* Check for consistency */
    if (tcp_header->ack_nr != current_queued_socket->tcp_control.send_nxt) {
        printf("TCP packets not consistent!\n");
    }

    /* Got ack, connection established, refresh local and foreign tcp socket
     * status */
    set_tcp_cb(&current_queued_socket->tcp_control, tcp_header->seq_nr,
               current_queued_socket->tcp_control.rcv_wnd, tcp_header->ack_nr,
               tcp_header->ack_nr, tcp_header->window);

#ifdef TCP_HC
    /* Copy TCP context information into new socket */
    memset(&server_socket->socket_values.tcp_control.tcp_context, 0,
           sizeof(tcp_hc_context_t));
#endif

    /* Update connection status information */
    current_queued_socket->tcp_control.state = TCP_ESTABLISHED;

    /* Set status of internal socket back to TCP_LISTEN */
    server_socket->socket_values.tcp_control.state = TCP_LISTEN;

    /* send a reply to the TCP handler after processing every information from
     * the TCP ACK packet */
    msg_reply(&msg_recv_client_ack, &msg_send_client_ack);

    /* Reset PID to an unlikely value */
    current_queued_int_socket->recv_pid = 255;

    /* Waiting for Clients ACK waiting period to time out */
    vtimer_usleep(TCP_SYN_INITIAL_TIMEOUT / 2);

    destiny_socket_print_sockets();

    return current_queued_int_socket->socket_id;
}

int destiny_socket_accept(int s, sockaddr6_t *addr, uint32_t *addrlen)
{
    (void) addr;
    (void) addrlen;

    socket_internal_t *server_socket = get_socket(s);

    if (is_tcp_socket(s) && (server_socket->socket_values.tcp_control.state == TCP_LISTEN)) {
        socket_internal_t *current_queued_socket =
            get_waiting_connection_socket(s, NULL, NULL);

        if (current_queued_socket != NULL) {
            return handle_new_tcp_connection(current_queued_socket,
                                             server_socket, thread_getpid());
        }
        else {
            /* No waiting connections, waiting for message from TCP Layer */
            msg_t msg_recv_client_syn;
            msg_recv_client_syn.type = UNDEFINED;

            while (msg_recv_client_syn.type != TCP_SYN) {
                msg_receive(&msg_recv_client_syn);
            }

            current_queued_socket = get_waiting_connection_socket(s, NULL, NULL);

            return handle_new_tcp_connection(current_queued_socket,
                                             server_socket, thread_getpid());
        }
    }
    else {
        return -1;
    }
}

socket_internal_t *new_tcp_queued_socket(ipv6_hdr_t *ipv6_header,
        tcp_hdr_t *tcp_header)
{
    int queued_socket_id;

    queued_socket_id = destiny_socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
    socket_internal_t *current_queued_socket = get_socket(queued_socket_id);

    /* Foreign address */
    set_socket_address(&current_queued_socket->socket_values.foreign_address,
                       AF_INET6, tcp_header->src_port, ipv6_header->flowlabel,
                       &ipv6_header->srcaddr);

    /* Local address */
    set_socket_address(&current_queued_socket->socket_values.local_address,
                       AF_INET6, tcp_header->dst_port, 0,
                       &ipv6_header->destaddr);

    /* Foreign TCP information */
    if ((tcp_header->data_offset * 4 > TCP_HDR_LEN) &&
        (*(((uint8_t *)tcp_header) + TCP_HDR_LEN) == TCP_MSS_OPTION)) {
        current_queued_socket->socket_values.tcp_control.mss =
            *((uint16_t *)(((uint8_t *)tcp_header) + TCP_HDR_LEN + 2));
    }
    else {
        current_queued_socket->socket_values.tcp_control.mss = DESTINY_SOCKET_STATIC_MSS;
    }

    current_queued_socket->socket_values.tcp_control.rcv_irs =
        tcp_header->seq_nr;
    mutex_lock(&global_sequence_counter_mutex);
    current_queued_socket->socket_values.tcp_control.send_iss =
        global_sequence_counter;
    mutex_unlock(&global_sequence_counter_mutex);
    current_queued_socket->socket_values.tcp_control.state = TCP_SYN_RCVD;
    set_tcp_cb(&current_queued_socket->socket_values.tcp_control,
               tcp_header->seq_nr + 1, DESTINY_SOCKET_STATIC_WINDOW,
               current_queued_socket->socket_values.tcp_control.send_iss + 1,
               current_queued_socket->socket_values.tcp_control.send_iss,
               tcp_header->window);

    return current_queued_socket;
}
