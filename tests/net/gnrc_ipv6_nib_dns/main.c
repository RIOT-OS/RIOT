/*
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       sock DNS client test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>

#include "net/sock/dns.h"
#include "shell.h"

#define MAIN_QUEUE_SIZE     (8)
static int _dns(int argc, char **argv);

static const shell_command_t _shell_commands[] = {
    { "dns", "configures and requests a DNS server", _dns },
    { NULL, NULL, NULL },
};
static char _shell_buffer[SHELL_DEFAULT_BUFSIZE];

static void _usage(char *cmd)
{
    printf("usage: %s server\n", cmd);
}

static int _dns_server(int argc, char **argv)
{
    int res = (argc < 2);

    if (!res) {
        if (sock_dns_server.port > 0) {
            char addrstr[INET6_ADDRSTRLEN];

            inet_ntop(AF_INET6, sock_dns_server.addr.ipv6, addrstr,
                      sizeof(addrstr));
            printf("DNS server: [%s%%%d]:%u\n", addrstr, sock_dns_server.netif,
                   sock_dns_server.port);
        }
        else {
            puts("DNS server: -");
        }
    }
    else {
        _usage(argv[0]);
    }
    return res;
}

static int _dns(int argc, char **argv)
{
    if ((argc > 1) && (strcmp(argv[1], "server") == 0)) {
        return _dns_server(argc, argv);
    }
    else {
        _usage(argv[0]);
        return 1;
    }
}

int main(void)
{
    /* start shell */
    shell_run(_shell_commands, _shell_buffer, sizeof(_shell_buffer));
    return 0;
}
