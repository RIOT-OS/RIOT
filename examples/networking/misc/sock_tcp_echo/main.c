/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdio.h>

#include "net/sock/tcp.h"
#include "net/ipv6/addr.h"
#include "net/utils.h"
#include "shell.h"

#define SOCK_QUEUE_LEN  1

static char _echo_server_stack[THREAD_STACKSIZE_DEFAULT];

static int _cmd_tcp_send(int argc, char **argv)
{
    int res;
    sock_tcp_t sock;
    sock_tcp_ep_t remote;
    netif_t *netif;
    char buf[128];

    if (argc < 4) {
        printf("usage: %s <addr> <port> <data>\n", argv[0]);
        return -1;
    }

    if (netutils_get_ipv6((ipv6_addr_t *)&remote.addr,
                          &netif, argv[1]) < 0) {
        printf("can't resolve %s\n", argv[1]);
        return -1;
    }

    remote.family = AF_INET6;
    remote.netif  = netif ? netif_get_id(netif) : 0;
    remote.port   = atoi(argv[2]);

    if (remote.port == 0) {
        printf("Invalid port: %s\n", argv[2]);
        return -1;
    }

    if ((res = sock_tcp_connect(&sock, &remote, 0, 0)) < 0) {
        printf("Error connecting sock: %s\n", strerror(-res));
        return -1;
    }

    if ((res = sock_tcp_write(&sock, argv[3], strlen(argv[3]))) < 0) {
        printf("Errored on write: %s\n", strerror(-res));
        goto error;
    }

    if ((res = sock_tcp_read(&sock, &buf, sizeof(buf), SOCK_NO_TIMEOUT)) <= 0) {
        printf("Disconnected: %s\n", strerror(-res));
        goto error;
    }

    buf[res] = 0;
    printf("Read: \"%s\"\n", buf);

error:
    sock_tcp_disconnect(&sock);

    return 0;
}
SHELL_COMMAND(send, "send data over TCP", _cmd_tcp_send);

static void *_run_echo_server(void *ctx)
{
    sock_tcp_t sock_queue[SOCK_QUEUE_LEN];
    char buf[128];
    uint16_t port = (uintptr_t)ctx;

    sock_tcp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_tcp_queue_t queue;
    local.port = port;
    if (sock_tcp_listen(&queue, &local, sock_queue, SOCK_QUEUE_LEN, 0) < 0) {
        puts("Error creating listening queue");
        return NULL;
    }
    printf("Listening on port %u\n", port);
    while (1) {
        sock_tcp_t *sock;
        if (sock_tcp_accept(&queue, &sock, SOCK_NO_TIMEOUT) < 0) {
            puts("Error accepting new sock");
            break;
        }

        int res = 0;
        puts("Reading data");
        while (res >= 0) {
            res = sock_tcp_read(sock, &buf, sizeof(buf), SOCK_NO_TIMEOUT);
            if (res <= 0) {
                printf("Disconnected: %s\n", strerror(-res));
                break;
            }
            buf[res] = 0;
            printf("Read: \"%s\"\n", buf);
            if ((res = sock_tcp_write(sock, &buf, res)) < 0) {
                printf("Errored on write: %s\n", strerror(-res));
            }
        }
        sock_tcp_disconnect(sock);
    }
    sock_tcp_stop_listen(&queue);
    return NULL;
}

static int _cmd_tcp_listen(int argc, char **argv)
{
    static kernel_pid_t pid;
    uint16_t port;

    if (argc < 2) {
        printf("usage: %s <port>\n", argv[0]);
        return -1;
    }

    if (pid) {
        puts("server already running");
        return -1;
    }

    port = atoi(argv[1]);

    if (port == 0) {
        printf("invalid port: %s\n", argv[1]);
        return -1;
    }

    pid = thread_create(_echo_server_stack, sizeof(_echo_server_stack),
                        THREAD_PRIORITY_MAIN - 1, 0,
                        _run_echo_server, (void*)(uintptr_t)port, "echo_server");
    return 0;
}
SHELL_COMMAND(listen, "start echo server", _cmd_tcp_listen);

int main(void)
{
    puts("RIOT TCP client example application");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
/** @} */
