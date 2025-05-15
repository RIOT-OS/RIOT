/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @}
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "cli.h"
#include "od.h"
#include "net/af.h"
#include "net/sock/async/event.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"
#include "shell.h"
#include "test_utils/expect.h"
#include "thread.h"
#include "utilities.h"
#include "ztimer.h"

#ifdef MODULE_SOCK_UDP
static char sock_inbuf[SOCK_INBUF_SIZE];
static bool server_running;
static sock_udp_t server_sock;
static char server_stack[THREAD_STACKSIZE_DEFAULT];
static msg_t server_msg_queue[SERVER_MSG_QUEUE_SIZE];

static void _udp_recv(sock_udp_t *sock, sock_async_flags_t flags, void *arg)
{
    expect(strcmp(arg, "test") == 0);
    if (flags & SOCK_ASYNC_MSG_RECV) {
        sock_udp_ep_t src;
        int res;

        if ((res = sock_udp_recv(sock, sock_inbuf, sizeof(sock_inbuf),
                                 0, &src)) < 0) {
            puts("Error on receive");
        }
        else if (res == 0) {
            puts("No data received");
        }
        else {
            char addrstr[IPV6_ADDR_MAX_STR_LEN];
            uint16_t port;

            printf("Received UDP data from ");
            if (sock_udp_ep_fmt(&src, addrstr, &port) >= 0) {
                printf("[%s]:%u\n", addrstr, port);
            }
            else {
                printf("unspecified source\n");
            }
            od_hex_dump(sock_inbuf, res, 0);
        }
    }
}

static void *_server_thread(void *args)
{
    event_queue_t queue;
    sock_udp_ep_t server_addr = SOCK_IP_EP_ANY;
    int res;

    msg_init_queue(server_msg_queue, SERVER_MSG_QUEUE_SIZE);
    /* parse port */
    server_addr.port = atoi(args);
    if ((res = sock_udp_create(&server_sock, &server_addr, NULL, 0)) < 0) {
        printf("Unable to open UDP server on port %" PRIu16 " (error code %d)\n",
               server_addr.port, -res);
        return NULL;
    }
    server_running = true;
    printf("Success: started UDP server on port %" PRIu16 "\n",
           server_addr.port);
    event_queue_init(&queue);
    sock_udp_event_init(&server_sock, &queue, _udp_recv, "test");
    event_loop(&queue);
    return NULL;
}

static int udp_send(char *addr_str, char *data, unsigned int num,
                    unsigned int delay)
{
    sock_udp_ep_t dst = SOCK_IP_EP_ANY;
    uint8_t byte_data[SHELL_DEFAULT_BUFSIZE / 2];
    size_t data_len;

    /* parse destination address */
    if (sock_udp_name2ep(&dst, addr_str) < 0) {
        puts("Error: unable to parse destination address");
        return 1;
    }
    if (dst.port == 0) {
        puts("Error: no port or illegal port value provided");
        return 1;
    }
    data_len = hex2ints(byte_data, data);
    for (unsigned int i = 0; i < num; i++) {
        sock_udp_t *sock = NULL;

        if (server_running) {
            sock = &server_sock;
        }
        if (sock_udp_send(sock, byte_data, data_len, &dst) < 0) {
            puts("could not send");
        }
        else {
            printf("Success: send %" PRIuSIZE " byte over UDP to %s\n",
                   data_len, addr_str);
        }
        ztimer_sleep(ZTIMER_USEC, delay);
    }
    return 0;
}

static int udp_start_server(char *port_str)
{
    if (thread_create(server_stack, sizeof(server_stack), THREAD_PRIORITY_MAIN - 1,
                      0, _server_thread, port_str,
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
        if (argc < 4) {
            printf("usage: %s send <addr>:<port> <hex data> [<num> [<delay in us>]]\n",
                   argv[0]);
            return 1;
        }
        if (argc > 4) {
            num = atoi(argv[4]);
        }
        if (argc > 5) {
            delay = atoi(argv[5]);
        }
        return udp_send(argv[2], argv[3], num, delay);
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
