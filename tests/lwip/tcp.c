/*
 * Copyright (C) 2017 Freie Universität Berlin
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

#include "common.h"
#include "od.h"
#include "net/af.h"
#include "net/sock/tcp.h"
#include "net/ipv6.h"
#include "shell.h"
#include "thread.h"
#include "xtimer.h"

#ifdef MODULE_SOCK_TCP
static char sock_inbuf[SOCK_INBUF_SIZE];
static bool server_running = false, client_running = false;
static sock_tcp_t server_sock, client_sock;
static sock_tcp_queue_t server_queue;
static char server_stack[THREAD_STACKSIZE_DEFAULT];
static msg_t server_msg_queue[SERVER_MSG_QUEUE_SIZE];

static void *_server_thread(void *args)
{
    sock_tcp_ep_t server_addr = SOCK_IPV6_EP_ANY;
    int res;

    msg_init_queue(server_msg_queue, SERVER_MSG_QUEUE_SIZE);
    /* parse port */
    server_addr.port = atoi(args);
    if ((res = sock_tcp_listen(&server_queue, &server_addr, &server_sock, 1,
                               0)) < 0) {
        printf("Unable to open TCP server on port %" PRIu16 " (error code %d)\n",
               server_addr.port, -res);
        return NULL;
    }
    server_running = true;
    printf("Success: started TCP server on port %" PRIu16 "\n",
           server_addr.port);
    while (1) {
        char client_addr[IPV6_ADDR_MAX_STR_LEN];
        sock_tcp_t *sock = NULL;
        int res;
        unsigned client_port;

        if ((res = sock_tcp_accept(&server_queue, &sock, SOCK_NO_TIMEOUT)) < 0) {
            puts("Error on TCP accept");
            continue;
        }
        else {
            sock_tcp_ep_t client;

            sock_tcp_get_remote(sock, &client);
            ipv6_addr_to_str(client_addr, (ipv6_addr_t *)&client.addr.ipv6,
                             sizeof(client_addr));
            client_port = client.port;
            printf("TCP client [%s]:%u connected\n",
                   client_addr, client_port);
        }
        /* we don't use timeouts so all errors should be related to a lost
         * connection */
        while ((res = sock_tcp_read(sock, sock_inbuf, sizeof(sock_inbuf),
                                    SOCK_NO_TIMEOUT)) >= 0) {
            printf("Received TCP data from client [%s]:%u:\n",
                   client_addr, client_port);
            if (res > 0) {
                od_hex_dump(sock_inbuf, res, 0);
            }
            else {
                puts("(nul)");
            }
        }
        printf("TCP connection to [%s]:%u reset, starting to accept again\n",
               client_addr, client_port);
        sock_tcp_disconnect(sock);
    }
    return NULL;
}

static int tcp_connect(char *addr_str, char *port_str, char *local_port_str)
{
    sock_tcp_ep_t dst = SOCK_IPV6_EP_ANY;
    uint16_t local_port = 0;

    if (client_running) {
        puts("Cient already connected");
    }

    /* parse destination address */
    if (ipv6_addr_from_str((ipv6_addr_t *)&dst.addr.ipv6, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return 1;
    }
    /* parse port */
    dst.port = atoi(port_str);
    if (local_port_str != NULL) {
        local_port = atoi(port_str);
    }
    if (sock_tcp_connect(&client_sock, &dst, local_port, 0) < 0) {
        puts("Error: unable to connect");
        return 1;
    }
    client_running = true;
    return 0;
}

static int tcp_disconnect(void)
{
    sock_tcp_disconnect(&client_sock);
    client_running = false;
    return 0;
}

static int tcp_send(char *data, unsigned int num, unsigned int delay)
{
    uint8_t byte_data[SHELL_DEFAULT_BUFSIZE / 2];
    size_t data_len;

    data_len = hex2ints(byte_data, data);
    for (unsigned int i = 0; i < num; i++) {
        if (sock_tcp_write(&client_sock, byte_data, data_len) < 0) {
            puts("could not send");
        }
        else {
            printf("Success: send %u byte over TCP to server\n", (unsigned)data_len);
        }
        xtimer_usleep(delay);
    }
    return 0;
}

static int tcp_start_server(char *port_str)
{
    if (thread_create(server_stack, sizeof(server_stack), THREAD_PRIORITY_MAIN - 1,
                      THREAD_CREATE_STACKTEST, _server_thread, port_str,
                      "TCP server") <= KERNEL_PID_UNDEF) {
        return 1;
    }
    return 0;
}

int tcp_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [connect|disconnect|send|server]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "connect") == 0) {
        char *local_port = NULL;

        if (argc < 4) {
            printf("usage: %s connect <addr> <port> [local_port]\n",
                   argv[0]);
            return 1;
        }
        if (argc > 4) {
            local_port = argv[4];
        }
        return tcp_connect(argv[2], argv[3], local_port);
    }
    if (strcmp(argv[1], "disconnect") == 0) {
        return tcp_disconnect();
    }
    else if (strcmp(argv[1], "send") == 0) {
        uint32_t num = 1;
        uint32_t delay = 1000000UL;
        if (argc < 3) {
            printf("usage: %s send <hex data> [<num> [<delay in us>]]\n",
                   argv[0]);
            return 1;
        }
        if (argc > 3) {
            num = atoi(argv[3]);
        }
        if (argc > 4) {
            delay = atoi(argv[4]);
        }
        return tcp_send(argv[2], num, delay);
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
            return tcp_start_server(argv[3]);
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
