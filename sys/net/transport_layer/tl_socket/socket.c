/**
 * Transport layer socket API
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup tl_socket
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

#include "socket_internal.h"
#include "msg_help.h"
#include "tcp.h"
#include "tcp_hc.h"
#include "tcp_timer.h"
#include "udp.h"


#define EPHEMERAL_PORTS 	49152

static socket_internal_t sockets[MAX_SOCKETS];

static void print_socket(socket_t *current_socket);
static void print_internal_socket(socket_internal_t *current_socket_internal);

static void print_socket(socket_t *current_socket)
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

static void print_internal_socket(socket_internal_t *current_socket_internal)
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
    if (socket_exist(s)) {
        return &(sockets[s - 1]);
    }
    else {
        return NULL;
    }
}

void tl_socket_print_sockets(void)
{
    int i;
    printf("\n---   Socket list:   ---\n");

    for (i = 1; i < MAX_SOCKETS + 1; i++) {
        if (get_socket(i) != NULL) {
            print_internal_socket(get_socket(i));
        }
    }
}

int socket_exist(int socket)
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

bool socket_is_udp(int s)
{
    if ((socket_exist(s)) &&
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

bool socket_is_tcp(int s)
{
    if ((socket_exist(s)) &&
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

int tl_socket(int domain, int type, int protocol)
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

uint16_t socket_get_free_source_port(uint8_t protocol)
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

int32_t tl_socket_recvfrom(int s, void *buf, uint32_t len, int flags,
                                sockaddr6_t *from, uint32_t *fromlen)
{
    (void) flags;

    if (socket_is_udp(s)) {
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
#ifdef MODULE_TCP
    else if (socket_is_tcp(s)) {
        return tl_socket_recv(s, buf, len, flags);
    }
#endif
    else {
        printf("Socket Type not supported!\n");
        return -1;
    }
}

int32_t tl_socket_sendto(int s, const void *buf, uint32_t len, int flags,
                              sockaddr6_t *to, uint32_t tolen)
{
    (void) flags;
    (void) tolen;

    if (socket_is_udp(s) &&
        (get_socket(s)->socket_values.foreign_address.sin6_port == 0)) {
        uint8_t send_buffer[BUFFER_SIZE];

        ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&send_buffer));
        udp_hdr_t *current_udp_packet = ((udp_hdr_t *)(&send_buffer[IPV6_HDR_LEN]));
        uint8_t *payload = &send_buffer[IPV6_HDR_LEN + UDP_HDR_LEN];

        memcpy(&(temp_ipv6_header->destaddr), &to->sin6_addr, 16);
        ipv6_net_if_get_best_src_addr(&(temp_ipv6_header->srcaddr), &(temp_ipv6_header->destaddr));

        current_udp_packet->src_port = socket_get_free_source_port(IPPROTO_UDP);
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

int tl_socket_close(int s)
{
    socket_internal_t *current_socket = get_socket(s);

    if (current_socket != NULL) {
        if (socket_is_udp(s)) {
            close_socket(current_socket);
            return 0;
        }
#ifdef MODULE_TCP
        else if (socket_is_tcp(s)) {
            /* Variables */
            msg_t m_recv;
            uint8_t send_buffer[BUFFER_SIZE];
            ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&send_buffer));
            tcp_hdr_t *current_tcp_packet = ((tcp_hdr_t *)(&send_buffer[IPV6_HDR_LEN]));

            /* Check if socket exists and is TCP socket */
            if (!socket_is_tcp(s)) {
                return -1;
            }

            /* Check for ESTABLISHED STATE */
            if (current_socket->socket_values.tcp_control.state != ESTABLISHED) {
                close_socket(current_socket);
                return 0;
            }

            current_socket->send_pid = thread_getpid();

            /* Refresh local TCP socket information */
            current_socket->socket_values.tcp_control.send_una++;
            current_socket->socket_values.tcp_control.state = FIN_WAIT_1;
#ifdef TCP_HC
            current_socket->socket_values.tcp_control.tcp_context.hc_type =
                COMPRESSED_HEADER;
#endif

            tcp_send(current_socket, current_tcp_packet, temp_ipv6_header,
                     TCP_FIN, 0);
            msg_receive(&m_recv);
            close_socket(current_socket);
            return 1;
        }
#endif

        return -1;
    }
    else {
        return -1;
    }
}

int tl_socket_bind(int s, sockaddr6_t *addr, int addrlen)
{
    if (socket_exist(s)) {
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
#ifdef MODULE_TCP
                        if ((current_socket->protocol == 0) ||
                            (current_socket->protocol == IPPROTO_TCP)) {
                            return tcp_bind_socket(s, addr, addrlen,
                                                   thread_getpid());
                        }
#endif
                        return -1;
                    }

                    /* UDP */
                    case (SOCK_DGRAM): {
                        if ((current_socket->protocol == 0) ||
                            (current_socket->protocol == IPPROTO_UDP)) {
                            return udp_bind_socket(s, addr, addrlen,
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

int tl_socket_listen(int s, int backlog)
{
    (void) backlog;

    if (socket_is_tcp(s) && get_socket(s)->socket_values.tcp_control.state == TCP_CLOSED) {
        socket_internal_t *current_socket = get_socket(s);
        current_socket->socket_values.tcp_control.state = TCP_LISTEN;
        return 0;
    }
    else {
        return -1;
    }
}

int tl_socket_accept(int s, sockaddr6_t *addr, uint32_t *addrlen)
{
    (void) addr;
    (void) addrlen;

#ifdef MODULE_TCP
    return tcp_accept(s);
#else /* No TCP, no accept */
    (void) s;
    return -1;
#endif
}
