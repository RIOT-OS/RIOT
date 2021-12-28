/*
 * Copyright (C) 2021 Freie Universität Berlin
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
 * @brief       sock DNS client test application for LwIP
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
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

static void _usage(void)
{
    printf("usage: dns server <4 or 6 (IPv4/IPv6)> <DNS server addr> [DNS server port]\n");
    printf("       dns request <0, 4 or 6 (Unspecified/IPv4/IPv6)> <name>\n");
}

static int _dns_server(int argc, char **argv)
{
    if (argc == 2) { /* only print the currently set up dns server */
        char addrstr[INET6_ADDRSTRLEN];

        switch (sock_dns_server.family) {
            case AF_UNSPEC:
                puts("DNS server: -");
                break;

        #if IS_USED(MODULE_LWIP_IPV4)
            case AF_INET:
                inet_ntop(AF_INET, sock_dns_server.addr.ipv4, addrstr, sizeof(addrstr));
                printf("DNS server: %s:%u\n", addrstr, sock_dns_server.port);
                break;
        #endif
        #if IS_USED(MODULE_LWIP_IPV6)
            case AF_INET6:
                inet_ntop(AF_INET6, sock_dns_server.addr.ipv6, addrstr, sizeof(addrstr));
                printf("DNS server: [%s]:%u\n", addrstr, sock_dns_server.port);
                break;
        #endif
            default:
                puts("DNS server: -");
                return 1;
        }

        return 0;
    }
    else if (argc < 4) {
        _usage();
        return 1;
    }

    int ip_version = atoi(argv[2]);
    switch (ip_version) {
    #if IS_USED(MODULE_LWIP_IPV4)
        case 4:
            if (!inet_pton(AF_INET, argv[3], sock_dns_server.addr.ipv4)) {
                printf("invalid IPv4 address\n");
                _usage();
                return 1;
            }
            sock_dns_server.family = AF_INET;
            break;
    #endif
    #if IS_USED(MODULE_LWIP_IPV6)
        case 6:
            if (!inet_pton(AF_INET6, argv[3], sock_dns_server.addr.ipv6)) {
                printf("invalid IPv6 address\n");
                _usage();
                return 1;
            }
            sock_dns_server.family = AF_INET6;
            break;
    #endif
        default:
            printf("invalid IP version %d\n", ip_version);
            _usage();
            return 1;
    }

    int port = SOCK_DNS_PORT;
    if (argc == 5) {
        port = atoi(argv[4]);
        if (port == 0) {
            printf("invalid port\n");
            _usage();
            return 1;
        }
    }

    sock_dns_server.port = port;

    return 0;
}

static int _dns_request(int argc, char **argv)
{
    if (argc < 4) {
        _usage();
        return 1;
    }

    int ip_version = atoi(argv[2]);
    int family;
    switch (ip_version) {
        case 6:
            family = AF_INET6;
            break;
        case 4:
            family = AF_INET;
            break;
        default:
            family = AF_UNSPEC;
    }

    uint8_t addr[16] = {0};
    int res = sock_dns_query(argv[3], addr, family);

    if (res > 0) {
        char addrstr[INET6_ADDRSTRLEN];

        if (inet_ntop(res == 4 ? AF_INET : AF_INET6, addr, addrstr,
                      sizeof(addrstr)) == NULL) {
            printf("error converting ip to str\n");
            return 1;
        }
        printf("%s resolves to %s\n", argv[3], addrstr);
    }
    else {
        printf("error resolving %s\n", argv[3]);
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
        return _dns_request(argc, argv);
    }
    else {
        _usage();
        return 1;
    }
}

int main(void)
{
    sock_dns_server.family = AF_UNSPEC;

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    /* start shell */
    shell_run(_shell_commands, _shell_buffer, sizeof(_shell_buffer));

    return 0;
}
