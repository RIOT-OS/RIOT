/*
 * Copyright (C) 2013, 2014 INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup socket_base
 * @{
 * @file    socket.c
 * @brief   functions for BSD socket API, methods return default values and 
 *          will be overwritten by appropriate transport layer protocols.
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Cenk Gündoğan <cnkgndgn@gmail.com>
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

#include "socket.h"

#define EPHEMERAL_PORTS     49152

socket_internal_t socket_base_sockets[MAX_SOCKETS];

int __attribute__((weak)) tcp_connect(int socket, sockaddr6_t *addr, uint32_t addrlen)
{
    (void) socket;
    (void) addr;
    (void) addrlen;

    return -1;
}

int __attribute__((weak)) tcp_listen(int s, int backlog)
{
    (void) s;
    (void) backlog;

    return -1;
}

int __attribute__((weak)) tcp_accept(int s, sockaddr6_t *addr, uint32_t *addrlen)
{
    (void) s;
    (void) addr;
    (void) addrlen;

    return -1;
}

int32_t __attribute__((weak)) tcp_send(int s, const void *buf, uint32_t len, int flags)
{
    (void) s;
    (void) buf;
    (void) len;
    (void) flags;

    return -1;
}

int32_t __attribute__((weak)) tcp_recv(int s, void *buf, uint32_t len, int flags)
{
    (void) s;
    (void) buf;
    (void) len;
    (void) flags;

    return -1;
}

int32_t __attribute__((weak)) udp_recvfrom(int s, void *buf, uint32_t len, int flags,
                                sockaddr6_t *from, uint32_t *fromlen)
{
    (void) s;
    (void) buf;
    (void) len;
    (void) flags;
    (void) from;
    (void) fromlen;

    return -1;
}

int32_t __attribute__((weak)) udp_sendto(int s, const void *buf, uint32_t len, int flags,
                              sockaddr6_t *to, uint32_t tolen)
{
    (void) s;
    (void) buf;
    (void) len;
    (void) flags;
    (void) to;
    (void) tolen;

    return -1;
}

int __attribute__((weak)) tcp_bind_socket(int s, sockaddr6_t *name, int namelen, uint8_t pid)
{
    (void) s;
    (void) name;
    (void) namelen;
    (void) pid;

    return -1;
}

int __attribute__((weak)) udp_bind_socket(int s, sockaddr6_t *name, int namelen, uint8_t pid)
{
    (void) s;
    (void) name;
    (void) namelen;
    (void) pid;

    return -1;
}

bool __attribute__((weak)) tcp_socket_compliancy(int s)
{
    (void) s;

    return false;
}

bool __attribute__((weak)) udp_socket_compliancy(int s)
{
    (void) s;

    return false;
}

int __attribute__((weak)) tcp_teardown(socket_internal_t *current_socket)
{
    (void) current_socket;

    return -1;
}

void socket_base_print_socket(socket_t *current_socket)
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

void socket_base_print_internal_socket(socket_internal_t *current_socket_internal)
{
    socket_t *current_socket = &current_socket_internal->socket_values;
    printf("\n--------------------------\n");
    printf("ID: %i, RECV PID: %i SEND PID: %i\n",
           current_socket_internal->socket_id, current_socket_internal->recv_pid,
           current_socket_internal->send_pid);
    socket_base_print_socket(current_socket);
    printf("\n--------------------------\n");
}

int socket_base_exists_socket(int socket)
{
    if (socket_base_sockets[socket - 1].socket_id == 0) {
        return false;
    }
    else {
        return true;
    }
}

socket_internal_t *socket_base_get_socket(int s)
{
    if (socket_base_exists_socket(s)) {
        return &(socket_base_sockets[s - 1]);
    }
    else {
        return NULL;
    }
}

void socket_base_print_sockets(void)
{
    int i;
    printf("\n---   Socket list:   ---\n");

    for (i = 1; i < MAX_SOCKETS + 1; i++) {
        if (socket_base_get_socket(i) != NULL) {
            socket_base_print_internal_socket(socket_base_get_socket(i));
        }
    }
}

int socket_base_close(int s)
{
    socket_internal_t *current_socket = socket_base_get_socket(s);

    if (udp_socket_compliancy(s)) {
        memset(current_socket, 0, sizeof(socket_internal_t));
        return 0;
    }
    else if (tcp_socket_compliancy(s)) {
        return tcp_teardown(current_socket);
    }

    printf("Socket Type not supported!\n");
    return -1;
}

uint16_t socket_base_get_free_source_port(uint8_t protocol)
{
    int i;
    uint16_t biggest_port = EPHEMERAL_PORTS - 1;

    /* Remember biggest ephemeral port number used so far and add 1 */
    for (i = 0; i < MAX_SOCKETS; i++) {
        if ((socket_base_sockets[i].socket_values.protocol == protocol) &&
            (socket_base_sockets[i].socket_values.local_address.sin6_port > biggest_port)) {
            biggest_port = socket_base_sockets[i].socket_values.local_address.sin6_port;
        }
    }

    return biggest_port + 1;
}

int socket_base_socket(int domain, int type, int protocol)
{
    int i = 1;

    while (socket_base_get_socket(i) != NULL) {
        i++;
    }

    if (i > MAX_SOCKETS + 1) {
        return -1;
    }
    else {
        socket_t *current_socket = &socket_base_sockets[i - 1].socket_values;
        socket_base_sockets[i - 1].socket_id = i;
        current_socket->domain = domain;
        current_socket->type = type;
        current_socket->protocol = protocol;
#ifdef MODULE_TCP
        current_socket->tcp_control.state = 0;
#endif
        return socket_base_sockets[i - 1].socket_id;
    }
}

int socket_base_connect(int socket, sockaddr6_t *addr, uint32_t addrlen)
{
    if (tcp_socket_compliancy(socket)) {
        return tcp_connect(socket, addr, addrlen);
    }
    else {
        printf("Socket Type not supported!\n");
    }

    return -1;
}

int socket_base_accept(int s, sockaddr6_t *addr, uint32_t *addrlen)
{
    if (tcp_socket_compliancy(s)) {
        return tcp_accept(s, addr, addrlen);
    }

    printf("Socket Type not supported!\n");
    return -1;
}

int socket_base_listen(int s, int backlog)
{
    if (tcp_socket_compliancy(s)) {
        return tcp_listen(s, backlog);
    }

    printf("Socket Type not supported!\n");
    return -1;
}

int32_t socket_base_recv(int s, void *buf, uint32_t len, int flags)
{
    if (tcp_socket_compliancy(s)) {
        return tcp_recv(s, buf, len, flags);
    }

    printf("Socket Type not supported!\n");
    return -1;
}

int32_t socket_base_recvfrom(int s, void *buf, uint32_t len, int flags,
                                sockaddr6_t *from, uint32_t *fromlen)
{
    if (udp_socket_compliancy(s)) {
        return udp_recvfrom(s, buf, len, flags, from, fromlen);
    }
    else if (tcp_socket_compliancy(s)) {
        return tcp_recv(s, buf, len, flags);
    }

    printf("Socket Type not supported!\n");
    return -1;
}

int32_t socket_base_sendto(int s, const void *buf, uint32_t len, int flags,
                              sockaddr6_t *to, uint32_t tolen)
{
    if (udp_socket_compliancy(s)) {
        return udp_sendto(s, buf, len, flags, to, tolen);
    }
    else if (tcp_socket_compliancy(s)) {
        return tcp_send(s, buf, len, flags);
    }
    else {
        printf("Socket Type not supported!\n");
        return -1;
    }

    return -1;
}

int32_t socket_base_send(int s, const void *buf, uint32_t len, int flags)
{
    if (tcp_socket_compliancy(s)) {
        return tcp_send(s, buf, len, flags);
    }
    else {
        printf("Socket Type not supported!\n");
        return -1;
    }

    return -1;
}

int socket_base_bind(int s, sockaddr6_t *addr, int addrlen)
{
    if (socket_base_exists_socket(s)) {
        socket_t *current_socket = &socket_base_get_socket(s)->socket_values;

        switch (current_socket->domain) {
            case (PF_INET): {
                /* Not provided */
                return -1;
            }

            case (PF_INET6): {
                switch (current_socket->type) {
                        /* TCP */
                    case (SOCK_STREAM): {
                        if ((current_socket->protocol == 0) ||
                            (current_socket->protocol == IPPROTO_TCP)) {
                            return tcp_bind_socket(s, addr, addrlen,
                                                   sched_active_pid);
                        }
                        else {
                            return -1;
                        }
                    }

                    /* UDP */
                    case (SOCK_DGRAM): {
                        if ((current_socket->protocol == 0) ||
                            (current_socket->protocol == IPPROTO_UDP)) {
                            return udp_bind_socket(s, addr, addrlen,
                                                   sched_active_pid);
                        }
                        else {
                            return -1;
                        }
                    }

                    case (SOCK_SEQPACKET): {
                        /* not provided */
                        return -1;
                    }

                    case (SOCK_RAW): {
                        /* not provided */
                        return -1;
                    }

                    default: {
                        return -1;
                    }
                }

                break;
            }

            case (PF_UNIX): {
                /* Not provided */
                return -1;
            }
        }
    }
    else {
        printf("SOCKET DOES NOT EXIST!\n");
        return -1;
    }

    return -1;
}
