/*
 * SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       sock DNS over DTLS client test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>

#include "net/credman.h"
#include "net/sock/dodtls.h"
#include "net/sock/util.h"
#include "shell.h"

#define MAIN_QUEUE_SIZE     8U
#define PSK_ID_LEN          32U
#define PSK_LEN             32U

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static int _dodtls(int argc, char **argv);

static const shell_command_t _shell_commands[] = {
    { "dodtls", "configures and requests a DNS server", _dodtls },
    { NULL, NULL, NULL },
};
static char _shell_buffer[SHELL_DEFAULT_BUFSIZE];
static char _psk_id[PSK_ID_LEN];
static char _psk[PSK_LEN];
static credman_credential_t _credential = {
    .type = CREDMAN_TYPE_PSK,
    .params = {
        .psk = {
            .id = { .s = _psk_id, .len = 0U, },
            .key = { .s = _psk, .len = 0U, },
        }
    }
};

static void _usage(char *cmd)
{
    printf("usage: %s server <DNS server addr> <DNS server port> <cred tag> "
           "<PSK id> <PSK key>\n", cmd);
    printf("usage: %s server -d\n", cmd);
    printf("       %s request <name> [<family>]\n", cmd);
}

static int _dodtls_server(int argc, char **argv)
{
    sock_udp_ep_t server;
    int res;

    if ((argc == 3) && (strcmp(argv[2], "-d") == 0)) {
        sock_dodtls_set_server(NULL, NULL);
    }
    else if ((argc > 1) && (argc < 6)) {
        _usage(argv[0]);
        return 1;
    }
    else {
        if (sock_udp_str2ep(&server, argv[2]) < 0) {
            _usage(argv[0]);
            return 1;
        }
        if (server.port == 0) {
            server.port = SOCK_DODTLS_PORT;
        }
        if (server.netif == SOCK_ADDR_ANY_NETIF) {
            netif_t *netif = netif_iter(NULL);
            /* we only have one interface so take that one, otherwise
             * TinyDTLS is not able to identify the peer */
            server.netif = netif_get_id(netif);
        }
        if ((_credential.tag = atoi(argv[3])) == 0) {
            _usage(argv[0]);
            return 1;
        }
        if ((_credential.params.psk.id.len = strlen(argv[4])) >= PSK_ID_LEN) {
            printf("PSK ID too long (max. %u bytes allowed)", PSK_ID_LEN);
            return 1;
        }
        if ((_credential.params.psk.key.len = strlen(argv[5])) >= PSK_LEN) {
            printf("PSK too long (max. %u bytes allowed)", PSK_LEN);
            return 1;
        }
        strcpy((char *)_credential.params.psk.id.s, argv[4]);
        strcpy((char *)_credential.params.psk.key.s, argv[5]);
        if ((res = sock_dodtls_set_server(&server, &_credential)) < 0) {
            errno = -res;
            perror("Unable to establish session with server");
        }
    }
    if (sock_dodtls_get_server(&server) == 0) {
        char addrstr[INET6_ADDRSTRLEN + 8U];    /* + 8 for port + colons + [] */
        uint16_t port;

        sock_udp_ep_fmt(&server, addrstr, &port);
        printf("DNS server: [%s]:%u\n", addrstr, port);
    }
    else {
        puts("DNS server: -");
    }
    return 0;
}

static int _dodtls_request(int argc, char **argv)
{
    uint8_t addr[16] = {0};
    int res, family = AF_UNSPEC;

    if (argc > 3) {
        if (strcmp(argv[3], "inet") == 0) {
            family = AF_INET;
        }
        else if (strcmp(argv[3], "inet6") == 0) {
            family = AF_INET6;
        }
        else {
            _usage(argv[0]);
        }
    }

    res = sock_dodtls_query(argv[2], addr, family);

    if (res > 0) {
        char addrstr[INET6_ADDRSTRLEN];

        if (inet_ntop(res == 4 ? AF_INET : AF_INET6, addr, addrstr,
                      sizeof(addrstr))) {
            printf("%s resolves to %s\n", argv[2], addrstr);
        }
        else {
            printf("unable to print resolved address for %s.\n", argv[2]);
            printf("Maybe address module is missing\n");
            return 1;
        }
    }
    else {
        printf("error resolving %s: ", argv[2]);
        errno = -res;
        perror("");
        return 1;
    }
    return 0;
}

static int _dodtls(int argc, char **argv)
{
    if ((argc > 1) && (strcmp(argv[1], "server") == 0)) {
        return _dodtls_server(argc, argv);
    }
    else if ((argc > 2) && (strcmp(argv[1], "request") == 0)) {
        return _dodtls_request(argc, argv);
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
