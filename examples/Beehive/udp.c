/*
 * Copyright (C) 2013, 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include "thread.h"
#include "socket_base/socket.h"
#include "net_help.h"
#include "beehive.h"


#define UDP_BUFFER_SIZE     (128)

uint16_t sixlowapp_netcat_listen_port;

/* UDP server thread */
void *sixlowapp_udp_server_loop(void *arg)
{
    (void) arg;

    sockaddr6_t sa;
    char buffer_main[UDP_BUFFER_SIZE];
    uint32_t fromlen;
    int sock;
    fromlen = sizeof(sa);

    while (1) {
        while(!sixlowapp_netcat_listen_port) {
            thread_sleep();
        }

        sock = socket_base_socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);

        memset(&sa, 0, sizeof(sa));
        sa.sin6_family = AF_INET;
        sa.sin6_port = HTONS(sixlowapp_netcat_listen_port);

        if (-1 == socket_base_bind(sock, &sa, sizeof(sa))) {
            printf("Error bind failed!\n");
            socket_base_close(sock);
            sixlowapp_netcat_listen_port = 0;
            continue;
        }

        printf("Listening for incoming UDP connection at port %" PRIu16 "\n", sixlowapp_netcat_listen_port);
        int32_t recsize = socket_base_recvfrom(sock, (void *)buffer_main, UDP_BUFFER_SIZE, 0, &sa, &fromlen);

        if (recsize < 0) {
            printf("ERROR: recsize < 0!\n");
        }

        printf("UDP packet received, payload: %s\n", buffer_main);

        socket_base_close(sock);
        sixlowapp_netcat_listen_port = 0;
    }

    return NULL;
}

/* UDP send command */
void sixlowapp_udp_send(ipv6_addr_t *dest, uint16_t port, char *payload, size_t len)
{
    int sock;
    sockaddr6_t sa;
    int bytes_sent;

    sock = socket_base_socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    if (-1 == sock) {
        printf("Error Creating Socket!");
        return;
    }
    memset(&sa, 0, sizeof(sa));
    sa.sin6_family = AF_INET;
    memcpy(&sa.sin6_addr, dest, 16);
    sa.sin6_port = HTONS(port);

    printf("Trying to send %i bytes to %s:%" PRIu16 "\n", len,
           ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, dest), port);
    bytes_sent = socket_base_sendto(sock, payload, len, 0, &sa, sizeof(sa));

    if (bytes_sent < 0) {
        printf("Error sending packet!\n");
    }
    else {
        printf("Successful deliverd %i bytes over UDP to %s to 6LoWPAN\n",
               bytes_sent, ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                                            dest));
    }

    socket_base_close(sock);
}
