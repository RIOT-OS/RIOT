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
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "kernel_defines.h"
#include "topology.h"
#include "zep_parser.h"

typedef struct {
    list_node_t node;
    struct sockaddr_in6 addr;
} zep_client_t;

typedef void (*dispatch_cb_t)(void *ctx, void *buffer, size_t len,
                              int sock, struct sockaddr_in6 *src_addr);

/* all nodes are directly connected */
static void _send_flat(void *ctx, void *buffer, size_t len,
                       int sock, struct sockaddr_in6 *src_addr)
{
    list_node_t *head = ctx;
    char addr_str[INET6_ADDRSTRLEN];

    /* send packet to all other clients */
    bool known_node = false;
    list_node_t *prev = head;

    for (list_node_t *n = head->next; n; n = n->next) {
        struct sockaddr_in6 *addr = &container_of(n, zep_client_t, node)->addr;

        /* don't echo packet back to sender */
        if (memcmp(src_addr, addr, sizeof(*addr)) == 0) {
            known_node = true;
            /* remove client if sending fails */
        }
        else if (sendto(sock, buffer, len, 0, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
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
        inet_ntop(AF_INET6, &src_addr->sin6_addr, addr_str, INET6_ADDRSTRLEN);
        printf("adding [%s]:%d\n", addr_str, ntohs(src_addr->sin6_port));
        zep_client_t *client = malloc(sizeof(zep_client_t));
        memcpy(&client->addr, src_addr, sizeof(*src_addr));
        list_add(head, &client->node);
    }
}

/* nodes are connected as described by topology */
static void _send_topology(void *ctx, void *buffer, size_t len,
                           int sock, struct sockaddr_in6 *src_addr)
{
    uint8_t mac_src[8];
    uint8_t mac_src_len;

    if (zep_parse_mac(buffer, len, mac_src, &mac_src_len) &&
        topology_add(ctx, mac_src, mac_src_len, src_addr)) {
        topology_send(ctx, sock, mac_src, mac_src_len, buffer, len);
    }
}

static void dispatch_loop(int sock, dispatch_cb_t dispatch, void *ctx)
{
    puts("entering loop…");
    while (1) {
        uint8_t buffer[ZEP_DISPATCH_PDU];
        struct sockaddr_in6 src_addr;
        socklen_t addr_len = sizeof(src_addr);

        /* receive incoming packet */
        ssize_t bytes_in = recvfrom(sock, buffer, sizeof(buffer), 0,
                                    (struct sockaddr *)&src_addr, &addr_len);

        if (bytes_in <= 0 || addr_len != sizeof(src_addr)) {
            continue;
        }

        dispatch(ctx, buffer, bytes_in, sock, &src_addr);
    }
}

static topology_t topology;
static const char *graphviz_file;
static void _info_handler(int signal)
{
    if (signal != SIGUSR1) {
        return;
    }

    if (topology_print(graphviz_file, &topology)) {
        fprintf(stderr, "can't open %s\n", graphviz_file);
    }
    else {
        printf("graph written to %s\n", graphviz_file);
    }
}

static void _print_help(const char *progname)
{
    fprintf(stderr, "usage: %s [-t topology] [-s seed] [-g graphviz_out] <address> <port>\n",
            progname);

    fprintf(stderr, "\npositional arguments:\n");
    fprintf(stderr, "\taddress\t\tlocal address to bind to\n");
    fprintf(stderr, "\tport\t\tlocal port to bind to\n");

    fprintf(stderr, "\noptional arguments:\n");
    fprintf(stderr, "\t-t <file>\tLoad toplogy from file\n");
    fprintf(stderr, "\t-s <seed>\tRandom seed used to simulate packet loss\n");
    fprintf(stderr, "\t-g <file>\tFile to dump topology as Graphviz visualisation on SIGUSR1\n");
}

int main(int argc, char **argv)
{
    int c;
    unsigned int seed = time(NULL);
    const char *topo_file = NULL;
    const char *progname = argv[0];

    while ((c = getopt(argc, argv, "t:s:g:")) != -1) {
        switch (c) {
        case 't':
            topo_file = optarg;
            break;
        case 's':
            seed = atoi(optarg);
            break;
        case 'g':
            graphviz_file = optarg;
            break;
        default:
            _print_help(progname);
            exit(1);
        }
    }

    argc -= optind;
    argv += optind;

    if (argc != 2) {
        _print_help(progname);
        exit(1);
    }

    srand(seed);

    if (topo_file) {
        if (topology_parse(topo_file, &topology)) {
            fprintf(stderr, "can't open '%s'\n", topo_file);
            return -1;
        }
        topology.flat = false;
    }
    else {
        topology.flat = true;
    }

    if (graphviz_file) {
        signal(SIGUSR1, _info_handler);
    }

    struct addrinfo hint = {
        .ai_family   = AF_INET6,
        .ai_socktype = SOCK_DGRAM,
        .ai_protocol = IPPROTO_UDP,
        .ai_flags    = AI_NUMERICHOST,
    };

    struct addrinfo *server_addr;
    int res = getaddrinfo(argv[0], argv[1],
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

    if (topology.flat) {
        dispatch_loop(sock, _send_flat, &topology.nodes);
    }
    else {
        dispatch_loop(sock, _send_topology, &topology);
    }

    close(sock);

    return 0;
}
