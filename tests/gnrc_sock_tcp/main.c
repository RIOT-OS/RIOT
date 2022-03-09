/*
 * Copyright (C) 2021   Simon Brummer <simon.brummer@posteo.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "msg.h"
#include "net/sock/tcp.h"
#include "net/gnrc/tcp.h"

#define MAIN_QUEUE_SIZE (8)
#define SOCK_TCP_QUEUE_SIZE (1)
#define BUFFER_SIZE (1024)

static msg_t main_msg_queue[MAIN_QUEUE_SIZE];
static sock_tcp_t socks[SOCK_TCP_QUEUE_SIZE];
static sock_tcp_t *sock = socks;
static sock_tcp_queue_t sock_queue;
static char buffer[BUFFER_SIZE];

void dump_args(int argc, char **argv)
{
    printf("%s: ", argv[0]);
    printf("argc=%d", argc);
    for (int i = 0; i < argc; ++i) {
        printf(", argv[%d] = %s", i, argv[i]);
    }
    printf("\n");
}

void print_result(const char *name, int err)
{
    if (err) {
        printf("%s: returns %s\n", name, strerror(-err));
    } else {
        printf("%s: returns %d\n", name, err);
    }
}

int sock_tcp_connect_cmd(int argc, char **argv)
{
    dump_args(argc, argv);

    sock_tcp_ep_t ep = SOCK_IPV6_EP_ANY;
    gnrc_tcp_ep_from_str((gnrc_tcp_ep_t *) &ep, argv[1]);
    uint16_t local_port = atol(argv[2]);
    uint16_t flags = 0;

    int err = sock_tcp_connect(sock, &ep, local_port, flags);
    print_result(argv[0], err);
    return 0;
}

int sock_tcp_disconnect_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    sock_tcp_disconnect(sock);
    printf("%s: returns\n", argv[0]);
    return 0;
}

int sock_tcp_listen_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    sock_tcp_ep_t ep = SOCK_IPV6_EP_ANY;
    gnrc_tcp_ep_from_str((gnrc_tcp_ep_t *) &ep, argv[1]);
    uint16_t flags = 0;

    int err = sock_tcp_listen(&sock_queue, &ep, socks, SOCK_TCP_QUEUE_SIZE, flags);
    print_result(argv[0], err);
    return 0;
}

int sock_tcp_stop_listen_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    sock_tcp_stop_listen(&sock_queue);
    printf("%s: returns\n", argv[0]);
    return 0;
}

int sock_tcp_accept_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    sock_tcp_t *tmp = NULL;
    uint16_t timeout = atol(argv[1]);

    int err = sock_tcp_accept(&sock_queue, &tmp, timeout);
    print_result(argv[0], err);

    if (tmp) {
        sock = tmp;
    }
    return 0;
}

int sock_tcp_read_cmd(int argc, char **argv)
{
    dump_args(argc, argv);

    unsigned to_receive = atol(argv[1]);
    int timeout = (argc > 2) ? atol(argv[1]) : 0;
    unsigned rcvd = 0;

    while (rcvd < to_receive) {
        int ret = sock_tcp_read(sock, buffer + rcvd, to_receive - rcvd, timeout);
        if (ret > 0) {
            rcvd += ret;
        } else {
            print_result(argv[0], ret);
            if (ret == -EAGAIN) {
                continue;
            } else {
                return ret;
            }
        }
    }
    buffer[to_receive] = '\0';
    printf("%s: received %u %s\n", argv[0], rcvd, buffer);
    return 0;
}

int sock_tcp_write_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    unsigned payload_size = strlen(argv[1]);
    char *payload = argv[1];

    unsigned sent = 0;
    while (sent < payload_size)
    {
        int ret = sock_tcp_write(sock, payload + sent, payload_size - sent);
        if (ret >= 0) {
            sent += ret;
        } else {
            print_result(argv[0], ret);
            return ret;
        }
    }
    printf("%s: sent %u\n", argv[0], sent);
    return 0;
}

int sock_tcp_get_local_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    sock_tcp_ep_t ep = SOCK_IPV6_EP_ANY;
    int err = sock_tcp_get_local(sock, &ep);

    print_result(argv[0], err);
    if (err == 0) {
        printf("Endpoint: addr.ipv6=");
        ipv6_addr_print((ipv6_addr_t *) ep.addr.ipv6);
        printf(" netif=%u port=%u\n", ep.netif, ep.port);
    }
    return 0;
}

int sock_tcp_queue_get_local_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    sock_tcp_ep_t ep = SOCK_IPV6_EP_ANY;
    int err = sock_tcp_queue_get_local(&sock_queue, &ep);

    print_result(argv[0], err);
    if (err == 0) {
        printf("Endpoint: addr.ipv6=");
        ipv6_addr_print((ipv6_addr_t *) ep.addr.ipv6);
        printf(" netif=%u port=%u\n", ep.netif, ep.port);
    }
    return 0;
}

int sock_tcp_get_remote_cmd(int argc, char **argv)
{
    dump_args(argc, argv);
    sock_tcp_ep_t ep = SOCK_IPV6_EP_ANY;
    int err = sock_tcp_get_remote(sock, &ep);

    print_result(argv[0], err);
    if (err == 0) {
        printf("Endpoint: addr.ipv6=");
        ipv6_addr_print((ipv6_addr_t *) ep.addr.ipv6);
        printf(" netif=%u port=%u\n", ep.netif, ep.port);
    }
    return 0;
}

/* Exporting GNRC SOCK TCP Api to for shell usage */
static const shell_command_t shell_commands[] = {
    { "sock_tcp_connect", "connect", sock_tcp_connect_cmd },
    { "sock_tcp_disconnect", "disconnect", sock_tcp_disconnect_cmd },
    { "sock_tcp_listen", "listen", sock_tcp_listen_cmd },
    { "sock_tcp_stop_listen", "stop_listen", sock_tcp_stop_listen_cmd },
    { "sock_tcp_accept", "accept", sock_tcp_accept_cmd },
    { "sock_tcp_read", "read", sock_tcp_read_cmd },
    { "sock_tcp_write", "write", sock_tcp_write_cmd },
    { "sock_tcp_get_local", "get_local", sock_tcp_get_local_cmd },
    { "sock_tcp_queue_get_local", "queue_get_local", sock_tcp_queue_get_local_cmd },
    { "sock_tcp_get_remote", "get_remote", sock_tcp_get_remote_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(main_msg_queue, MAIN_QUEUE_SIZE);
    printf("RIOT GNRC_TCP test application\n");

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
