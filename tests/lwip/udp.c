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

#include "common.h"
#include "od.h"
#include "net/af.h"
#include "net/conn/udp.h"
#include "net/ipv6.h"
#include "thread.h"
#include "xtimer.h"

#ifdef MODULE_CONN_UDP
static char conn_inbuf[CONN_INBUF_SIZE];
static bool server_running;
static conn_udp_t server_conn;
static char server_stack[THREAD_STACKSIZE_DEFAULT];
static msg_t server_msg_queue[SERVER_MSG_QUEUE_SIZE];

static void *_server_thread(void *args)
{
    ipv6_addr_t server_addr = IPV6_ADDR_UNSPECIFIED;
    uint16_t port;
    int res;

    msg_init_queue(server_msg_queue, SERVER_MSG_QUEUE_SIZE);
    /* parse port */
    port = (uint16_t)atoi((char *)args);
    if ((res = conn_udp_create(&server_conn, &server_addr,
                               sizeof(server_addr), AF_INET6, port)) < 0) {
        printf("Unable to open UDP server on port %" PRIu16 " (error code %d)\n",
               port, -res);
        return NULL;
    }
    server_running = true;
    printf("Success: started UDP server on port %" PRIu16 "\n", port);
    while (1) {
        int res;
        ipv6_addr_t src;
        size_t src_len = sizeof(ipv6_addr_t);
        uint16_t sport;
        if ((res = conn_udp_recvfrom(&server_conn, conn_inbuf, sizeof(conn_inbuf), &src,
                                     &src_len, &sport)) < 0) {
            puts("Error on receive");
        }
        else if (res == 0) {
            puts("No data received");
        }
        else {
            char addrstr[IPV6_ADDR_MAX_STR_LEN];
            printf("Received from [%s]:%" PRIu16 ":\n", ipv6_addr_to_str(addrstr, &src,
                                                                         sizeof(addrstr)), sport);
            od_hex_dump(conn_inbuf, res, 0);
        }
    }
    return NULL;
}

static int udp_send(char *addr_str, char *port_str, char *data, unsigned int num,
                    unsigned int delay)
{
    ipv6_addr_t src = IPV6_ADDR_UNSPECIFIED, dst;
    uint16_t port;
    uint8_t byte_data[strlen(data) / 2];
    size_t data_len;

    /* parse destination address */
    if (ipv6_addr_from_str(&dst, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return 1;
    }
    /* parse port */
    port = (uint16_t)atoi(port_str);
    data_len = hex2ints(byte_data, data);
    for (unsigned int i = 0; i < num; i++) {
        if (conn_udp_sendto(byte_data, data_len, &src, sizeof(src), (struct sockaddr *)&dst,
                            sizeof(dst), AF_INET6, port, port) < 0) {
            puts("could not send");
        }
        else {
            printf("Success: send %u byte to [%s]:%" PRIu16 ")\n",
                   (unsigned)data_len, addr_str, port);
        }
        xtimer_usleep(delay);
    }
    return 0;
}

static int udp_start_server(char *port_str)
{
    if (thread_create(server_stack, sizeof(server_stack), THREAD_PRIORITY_MAIN - 1,
                      THREAD_CREATE_STACKTEST, _server_thread, port_str,
                      "UDP server") <= KERNEL_PID_UNDEF) {
        return 1;
    }
    return 0;
}

int udp_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [send|server]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        uint32_t num = 1;
        uint32_t delay = 1000000;
        if (argc < 5) {
            printf("usage: %s send <addr> <port> <hex data> [<num> [<delay in us>]]\n",
                   argv[0]);
            return 1;
        }
        if (argc > 5) {
            num = (uint32_t)atoi(argv[5]);
        }
        if (argc > 6) {
            delay = (uint32_t)atoi(argv[6]);
        }
        return udp_send(argv[2], argv[3], argv[4], num, delay);
    }
    else if (strcmp(argv[1], "server") == 0) {
        if (argc < 3) {
            printf("usage: %s server [start|stop]\n", argv[0]);
            return 1;
        }
        if (strcmp(argv[2], "start") == 0) {
            if (argc < 4) {
                printf("usage %s server start <port>\n", argv[0]);
                return 1;
            }
            return udp_start_server(argv[3]);
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
#else
typedef int dont_be_pedantic;
#endif

/** @} */
