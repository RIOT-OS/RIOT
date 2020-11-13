/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#ifndef ZEP_DISPATCH_PDU
#define ZEP_DISPATCH_PDU    256
#endif

#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "list.h"
#include "kernel_defines.h"

typedef struct {
    list_node_t node;
    struct sockaddr_in6 addr;
} zep_client_t;

static void dispatch_loop(int sock)
{
    list_node_t head = { .next = NULL };

    puts("entering loop…");
    while (1) {
        char addr_str[INET6_ADDRSTRLEN];
        uint8_t buffer[ZEP_DISPATCH_PDU];
        struct sockaddr_in6 src_addr;
        socklen_t addr_len = sizeof(src_addr);

        /* receive incoming packet */
        ssize_t bytes_in = recvfrom(sock, buffer, sizeof(buffer), 0,
                               (struct sockaddr*)&src_addr, &addr_len);

        if (bytes_in <= 0 || addr_len != sizeof(src_addr)) {
            continue;
        }

        /* send packet to all other clients */
        bool known_node = false;
        list_node_t *prev = &head;
        for (list_node_t* n = head.next; n; n = n->next) {
            struct sockaddr_in6 *addr = &container_of(n, zep_client_t, node)->addr;

            /* don't echo packet back to sender */
            if (memcmp(&src_addr, addr, addr_len) == 0) {
                known_node = true;
            /* remove client if sending fails */
            } else if (sendto(sock, buffer, bytes_in, 0, (struct sockaddr*)addr, addr_len) < 0) {
                inet_ntop(AF_INET6, &addr->sin6_addr, addr_str, INET6_ADDRSTRLEN);
                printf("removing [%s]:%d\n", addr_str, ntohs(addr->sin6_port));
                prev->next = n->next;
                free(n);
                continue;
            }

            prev = n;
        }

        /* if the client new, add it to the broadcast list */
        if (!known_node) {
            inet_ntop(AF_INET6, &src_addr.sin6_addr, addr_str, INET6_ADDRSTRLEN);
            printf("adding [%s]:%d\n", addr_str, ntohs(src_addr.sin6_port));
            zep_client_t *client = malloc(sizeof(zep_client_t));
            memcpy(&client->addr, &src_addr, addr_len);
            list_add(&head, &client->node);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s <address> <port>\n",
                argv[0]);
        exit(1);
    }

    struct addrinfo hint = {
        .ai_family   = AF_INET6,
        .ai_socktype = SOCK_DGRAM,
        .ai_protocol = IPPROTO_UDP,
        .ai_flags    = AI_NUMERICHOST,
    };

    struct addrinfo *server_addr;
    int res = getaddrinfo(argv[1], argv[2],
                          &hint, &server_addr);
    if (res != 0) {
        perror("getaddrinfo()");
        exit(1);
    }

    int sock = socket(server_addr->ai_family, server_addr->ai_socktype,
                      server_addr->ai_protocol);
    if (sock < 0) {
        perror("socket() failed");
        exit(1);
    }

    if (bind(sock, server_addr->ai_addr, server_addr->ai_addrlen) < 0) {
        perror("bind() failed");
        exit(1);
    }

    freeaddrinfo(server_addr);

    dispatch_loop(sock);

    close(sock);

    return 0;
}
