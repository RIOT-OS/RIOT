/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Demonstrating the sending and receiving of UDP data over POSIX sockets.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "od.h"
#include "net/af.h"
#include "net/conn/ip.h"
#include "net/ipv6.h"
#include "thread.h"
#include "xtimer.h"

#define SERVER_MSG_QUEUE_SIZE   (8)
#define SERVER_BUFFER_SIZE      (64)

static bool server_running;
static conn_ip_t server_conn;
static char server_buffer[SERVER_BUFFER_SIZE];
static char server_stack[THREAD_STACKSIZE_DEFAULT];
static msg_t server_msg_queue[SERVER_MSG_QUEUE_SIZE];

static void *_server_thread(void *args)
{
    ipv6_addr_t server_addr = IPV6_ADDR_UNSPECIFIED;
    uint8_t protocol;
    msg_init_queue(server_msg_queue, SERVER_MSG_QUEUE_SIZE);
    /* parse protocol */
    protocol = (uint8_t)atoi((char *)args);
    if (conn_ip_create(&server_conn, &server_addr, sizeof(server_addr), AF_INET6, protocol) < 0) {
        return NULL;
    }
    server_running = true;
    printf("Success: started IP server on protocol %" PRIu8 "\n", protocol);
    while (1) {
        int res;
        ipv6_addr_t src;
        size_t src_len = sizeof(ipv6_addr_t);
        if ((res = conn_ip_recvfrom(&server_conn, server_buffer, sizeof(server_buffer), &src,
                                    &src_len)) < 0) {
            puts("Error on receive");
        }
        else if (res == 0) {
            puts("No data received");
        }
        else {
            od_hex_dump(server_buffer, res, 0);
        }
    }
    return NULL;
}


static size_t _parse_data(uint8_t *out, const char *data)
{
    bool upper = true;
    size_t out_size = 0;
    while (*data != '\0') {
        if ((*data >= '0') && (*data <= '9')) {
            if (upper) {
                *out = (char)(*data - '0') << 4;
            }
            else {
                *out |= (char)(*data - '0');
                out++;
                out_size++;
            }
            upper = !upper;
        }
        else if ((*data >= 'a') && (*data <= 'f')) {
            if (upper) {
                *out = (char)(*data - 'a' + 10) << 4;
            }
            else {
                *out |= (char)(*data - 'a' + 10);
                out++;
                out_size++;
            }
            upper = !upper;
        }
        else if ((*data >= 'A') && (*data <= 'F')) {
            if (upper) {
                *out = (char)(*data - 'A' + 10) << 4;
            }
            else {
                *out |= (char)(*data - 'A' + 10);
                out++;
                out_size++;
            }
            upper = !upper;
        }
        data++;
    }
    if (!upper) {
        out_size++;
    }
    return out_size;
}

static int ip_send(char *addr_str, char *port_str, char *data, unsigned int num,
                   unsigned int delay)
{
    ipv6_addr_t src = IPV6_ADDR_UNSPECIFIED, dst;
    uint8_t protocol;
    uint8_t byte_data[strlen(data) / 2];
    size_t data_len;
    /* parse destination address */
    if (ipv6_addr_from_str(&dst, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return 1;
    }
    /* parse protocol */
    protocol = (uint8_t)atoi(port_str);
    data_len = _parse_data(byte_data, data);
    for (unsigned int i = 0; i < num; i++) {
        if (conn_ip_sendto(byte_data, data_len, &src, sizeof(src), (struct sockaddr *)&dst,
                           sizeof(dst), AF_INET6, protocol) < 0) {
            puts("could not send");
        }
        else {
            printf("Success: send %u byte to %s (next header: %" PRIu8 ")\n",
                   (unsigned)data_len, addr_str, protocol);
        }
        xtimer_usleep(delay);
    }
    return 0;
}

static int ip_start_server(char *port_str)
{
    if (thread_create(server_stack, sizeof(server_stack), THREAD_PRIORITY_MAIN - 1,
                      THREAD_CREATE_STACKTEST, _server_thread, port_str, "IP server") <= KERNEL_PID_UNDEF) {
        return 1;
    }
    return 0;
}

int ip_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [send|server]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        uint32_t num = 1;
        uint32_t delay = 1000000;
        if (argc < 5) {
            printf("usage: %s send <addr> <protocol> <hex data> [<num> [<delay in us>]]\n",
                   argv[0]);
            return 1;
        }
        if (argc > 5) {
            num = (uint32_t)atoi(argv[5]);
        }
        if (argc > 6) {
            delay = (uint32_t)atoi(argv[6]);
        }
        return ip_send(argv[2], argv[3], argv[4], num, delay);
    }
    else if (strcmp(argv[1], "server") == 0) {
        if (argc < 3) {
            printf("usage: %s server [start|stop]\n", argv[0]);
            return 1;
        }
        if (strcmp(argv[2], "start") == 0) {
            if (argc < 4) {
                printf("usage %s server start <protocol>\n", argv[0]);
                return 1;
            }
            return ip_start_server(argv[3]);
        }
        else {
            puts("error: invalid command");
            return 1;
        }
    }
    else {
        puts("error: invalid command");
        return 1;
    }
}

/** @} */
