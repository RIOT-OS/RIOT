/*
 * Copyright (C) 2025 Krzysztof Cabaj <kcabaj@gmail.com>
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
 * @brief       lwip_ipv4 - sample application for demonstrating basic LWIP
 *              IPv4 client/server functions.
 *
 * @author      Krzysztof Cabaj <kcabaj@gmail.com>
 *
 * @}
 */
#include "stdio.h"
#include "stdlib.h"
#include "shell.h"
#include "net/sock/udp.h"
#include <arpa/inet.h>

static int _client_cmd(int argc, char **argv)
{
    uint32_t dest_ip;

    if (argc < 3) {
        printf("usage: %s <destination IP> <text>\n", argv[0]);
        return -1;
    }

    if (inet_pton(AF_INET, argv[1], &dest_ip) != 1) {
        printf("\"%s\" - is not a valid IPv4 address!\n", argv[1]);
        return -1;
    }
    sock_udp_t sock;
    sock_udp_ep_t remote = { .addr = { .ipv4_u32 = dest_ip },
                             .family = AF_INET,
                             .port = 4444 };
    int result;

    if ((result = sock_udp_create(&sock, NULL, &remote, SOCK_FLAGS_REUSE_EP)) != 0) {
        printf("Error sock_udp_create - result %d!\n", result);
        return -1;
    }
    if (strlen(argv[2]) != (size_t)sock_udp_send(&sock, argv[2], strlen(argv[2]), NULL)) {
        printf("Error sock_udp_send!\n");
        return -1;
    }

    sock_udp_close(&sock);

    return 0;
}

SHELL_COMMAND(client, "Send UDP datagram", _client_cmd);

static int _server_cmd(int argc, char **argv)
{
    sock_udp_t sock;
    sock_udp_ep_t local = { .family = AF_INET,
                            .port = 4444 };
    int result;
    char buffer[128] = { 0 };
    int i, n;

    if (argc < 2) {
        printf("usage: %s <number of received packets>\n", argv[0]);
        return -1;
    }

    n = atoi(argv[1]);

    if (sock_udp_create(&sock, &local, NULL, SOCK_FLAGS_REUSE_EP) != 0) {
        printf("Sock_udp_create error!\n");
        return -1;
    }

    printf("Server started.\n");

    for (i = 0; i < n; i++) {
        if ((result = sock_udp_recv(&sock, buffer, sizeof(buffer), SOCK_NO_TIMEOUT, NULL)) < 0) {
            printf("Sock_udp_recv error!\n");
            return -1;
        }

        printf("Received %d bytes - %s\n", result, buffer);
    }

    sock_udp_close(&sock);

    return 0;
}

SHELL_COMMAND(server, "Starts server which receives UDP datagrams", _server_cmd);

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
