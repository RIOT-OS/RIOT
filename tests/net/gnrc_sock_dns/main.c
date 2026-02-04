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
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static int _dns(int argc, char **argv);

static const shell_command_t _shell_commands[] = {
    { "dns", "configures and requests a DNS server", _dns },
    { NULL, NULL, NULL },
};
static char _shell_buffer[SHELL_DEFAULT_BUFSIZE];

static void _usage(char *cmd)
{
    printf("usage: %s server <DNS server addr> <DNS server port>\n", cmd);
    printf("       %s request <name>\n", cmd);
}

static int _dns_server(int argc, char **argv)
{
    if (((argc > 2) && !inet_pton(AF_INET6, argv[2],
                                  sock_dns_server.addr.ipv6)) ||
        ((argc > 3) && ((sock_dns_server.port = atoi(argv[3])) == 0))) {
        _usage(argv[1]);
        return 1;
    }
    else if (argc == 3) {
        sock_dns_server.port = SOCK_DNS_PORT;
    }
    else if (argc == 2) {
        if (sock_dns_server.port > 0) {
            char addrstr[INET6_ADDRSTRLEN];

            inet_ntop(AF_INET6, sock_dns_server.addr.ipv6, addrstr,
                      sizeof(addrstr));
            printf("DNS server: [%s]:%u\n", addrstr, sock_dns_server.port);
        }
        else {
            puts("DNS server: -");
        }
    }
    sock_dns_server.family = AF_INET6;
    return 0;
}

static int _dns_request(char **argv)
{
    uint8_t addr[16] = {0};
    int res = sock_dns_query(argv[2], addr, AF_UNSPEC);

    if (res > 0) {
        char addrstr[INET6_ADDRSTRLEN];

        inet_ntop(res == 4 ? AF_INET : AF_INET6, addr, addrstr,
                  sizeof(addrstr));
        printf("%s resolves to %s\n", argv[2], addrstr);
    }
    else {
        printf("error resolving %s\n", argv[2]);
        return 1;
    }
    return 0;
}

static int _dns(int argc, char **argv)
{
    if ((argc > 1) && (strcmp(argv[1], "server") == 0)) {
        return _dns_server(argc, argv);
    }
    else if ((argc > 2) && (strcmp(argv[1], "request") == 0)) {
        return _dns_request(argv);
    }
    else {
        _usage(argv[0]);
        return 1;
    }
}

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    /* start shell */
    shell_run(_shell_commands, _shell_buffer, sizeof(_shell_buffer));
    return 0;
}
