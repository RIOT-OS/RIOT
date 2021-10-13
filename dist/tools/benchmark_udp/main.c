/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     tools
 * @{
 *
 * @file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <sys/time.h>
#include <unistd.h>

#include "list.h"
#include "kernel_defines.h"
#include "test_utils/benchmark_udp.h"

#define US_PER_MS   (1000UL)
#define US_PER_SEC  (1000 * US_PER_MS)
#define MS_PER_SEC  (1000UL)

typedef struct {
    list_node_t node;
    struct sockaddr_in6 addr;
    struct timeval first_seen;
    uint32_t seq_no;
    uint32_t count_tx;
    uint32_t count_rx;
    uint32_t count_rt;
    uint32_t rtt_us;
    size_t packet_len;
} bench_client_t;

static bool one_way;
static uint32_t cookie;
static uint32_t delay_us    = 100 * US_PER_MS; /* 100 ms */
static uint16_t payload_len = 32;

static char addr_str[INET6_ADDRSTRLEN];

static bench_client_t *_find_or_add(list_node_t *head, struct sockaddr_in6 *addr,
                                    bool *new_node)
{
    for (list_node_t* n = head->next; n; n = n->next) {
        bench_client_t *node = container_of(n, bench_client_t, node);

        if (memcmp(&addr->sin6_addr, &node->addr.sin6_addr, sizeof(addr->sin6_addr)) == 0) {
            *new_node = false;
            return node;
        }
    }

    inet_ntop(AF_INET6, &addr->sin6_addr, addr_str, INET6_ADDRSTRLEN);
    printf("adding [%s]:%d\n", addr_str, ntohs(addr->sin6_port));
    bench_client_t *node = calloc(1, sizeof(bench_client_t));
    memcpy(&node->addr, addr, sizeof(*addr));
    list_add(head, &node->node);

    *new_node = true;
    return node;
}

static void clrscr(void)
{
    printf("\e[1;1H\e[2J");
}

static uint64_t _tv_diff_msec(struct timeval *a, struct timeval *b)
{
    return (a->tv_sec - b->tv_sec) * MS_PER_SEC
         + (a->tv_usec - b->tv_usec) / US_PER_MS;
}

static void _print_stats(list_node_t *head, struct timeval *now)
{
    static uint8_t max_addr_len;

    printf("host%*s\tbandwidth\tnum TX\tnum RX",  max_addr_len - 4, "");
    if (!one_way) {
        printf("\t\tnum RT\t\tRTT");
    }
    printf("\tpkg size\n");

    for (list_node_t* n = head->next; n; n = n->next) {
        bench_client_t *node = container_of(n, bench_client_t, node);
        uint8_t addr_len;

        inet_ntop(AF_INET6, &node->addr.sin6_addr, addr_str, INET6_ADDRSTRLEN);
        addr_len = printf("%s", addr_str);

        if (addr_len > max_addr_len) {
            max_addr_len = addr_len;
        }

        unsigned bw = (node->count_rx * node->packet_len)
                    / (1 + now->tv_sec - node->first_seen.tv_sec);

        unsigned success_rate = node->count_tx
                              ? (100 * node->count_rx) / node->count_tx
                              : 0;

        printf("%*s\t%4u b/s\t%u\t%u (%u%%)",
                                max_addr_len - addr_len, "",
                                bw,
                                node->count_tx, node->count_rx,
                                success_rate);
        if (!one_way) {
            unsigned success_rate_rt = node->count_tx
                                     ? (100 * node->count_rt) / node->count_tx
                                     : 0;
            printf("\t%u (%u%%)\t%u µs", node->count_rt, success_rate_rt, node->rtt_us);
        }
        printf("\t%zu\n", node->packet_len);
    }
}

static void dispatch_loop(int sock)
{
    list_node_t head = { .next = NULL };
    struct timeval tv_now, tv_last = { 0 };
    const size_t len_total = payload_len + sizeof(benchmark_msg_ping_t);
    uint8_t *buffer = malloc(len_total);

    puts("entering loop…");
    while (1) {
        struct sockaddr_in6 src_addr;
        socklen_t addr_len = sizeof(src_addr);

        /* receive incoming packet */
        ssize_t bytes_in = recvfrom(sock, buffer, len_total, 0,
                                    (struct sockaddr*)&src_addr, &addr_len);

        if (bytes_in <= 0 || addr_len != sizeof(src_addr)) {
            continue;
        }

        bool new_node;
        bench_client_t *node = _find_or_add(&head, &src_addr, &new_node);

        benchmark_msg_ping_t *ping = (void *)buffer;
        node->count_tx = ping->seq_no + 1;

        if (!one_way) {
            node->count_rt = ping->replies;
        }
        node->count_rx++;
        node->packet_len = bytes_in;

        if (new_node || (ping->flags & BENCH_MASK_COOKIE) != cookie) {
            benchmark_msg_cmd_t *cmd = (void *)buffer;
            cmd->flags       = BENCH_FLAG_CMD_PKT | cookie;
            cmd->delay_us    = delay_us;
            cmd->payload_len = payload_len;
            gettimeofday(&node->first_seen, NULL);
            node->count_rx   = 0;

            bytes_in = sizeof(*cmd);
            new_node = true;
        } else if (ping->rtt_last) {
            node->rtt_us = node->rtt_us
                         ? (node->rtt_us + ping->rtt_last) / 2
                         : ping->rtt_last;
        }

        /* send reply */
        if (!one_way || new_node) {
            sendto(sock, buffer, bytes_in, 0, (struct sockaddr*)&src_addr, addr_len);
        }

        gettimeofday(&tv_now, NULL);
        if (_tv_diff_msec(&tv_now, &tv_last) > 50) {
            tv_last = tv_now;
            clrscr();
            _print_stats(&head, &tv_now);
        }
    }
}

static void _print_help(const char *progname)
{
    fprintf(stderr, "usage: %s [-i send interval] [-s payload size] <address> <port>\n",
            progname);

    fprintf(stderr, "\npositional arguments:\n");
    fprintf(stderr, "\taddress\t\tlocal address to bind to\n");
    fprintf(stderr, "\tport\t\tlocal port to bind to\n");

    fprintf(stderr, "\noptional arguments:\n");
    fprintf(stderr, "\t-i <interval>\tsend interval in µs\n");
    fprintf(stderr, "\t-s <size>\tadded payload size\n");
    fprintf(stderr, "\t-o one-way mode, don't echo back packets\n");
}

int main(int argc, char **argv)
{
    const char *progname = argv[0];
    int c;

    while ((c = getopt(argc, argv, "i:s:o")) != -1) {
        switch (c) {
        case 'i':
            delay_us = atoi(optarg);
            break;
        case 's':
            payload_len = atoi(optarg);
            break;
        case 'o':
            one_way = true;
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

    while (getrandom(&cookie, sizeof(cookie), 0) != sizeof(cookie)) {}
    cookie &= BENCH_MASK_COOKIE;

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

    dispatch_loop(sock);

    close(sock);

    return 0;
}
/** @} */
