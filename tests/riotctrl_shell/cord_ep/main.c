/*
 * SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Tests the basic shell commands always provided.
 *
 * @author      Martine S. Lenders <mlenders@riot-os.org>
 *
 * @note        IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
 *              YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
 *              See https://github.com/RIOT-OS/Release-Specs/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmt.h"
#include "od.h"
#include "net/coap.h"
#include "net/gnrc/netif.h"
#include "net/sock/async.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"
#include "shell.h"
#include "test_utils/expect.h"
#include "uri_parser.h"

#define NETIF_ID   (5U)

static uint8_t _response_buf[64];
static size_t _response_len = 0;
static sock_udp_ep_t _response_remote;
static sock_udp_cb_t _udp_cb;
static void *_udp_cb_arg;

int sock_udp_create(sock_udp_t *sock, const sock_udp_ep_t *local,
                    const sock_udp_ep_t *remote, uint16_t flags)
{
    (void)sock;
    (void)local;
    (void)remote;
    (void)flags;
    return 0;
}

ssize_t sock_udp_recv_buf_aux(sock_udp_t *sock, void **data, void **buf_ctx,
                              uint32_t timeout, sock_udp_ep_t *remote,
                              sock_udp_aux_rx_t *aux)
{
    (void)sock;
    (void)timeout;

    memcpy(remote, &_response_remote, sizeof(_response_remote));
    if (*buf_ctx == NULL) {
        ssize_t res = (ssize_t)_response_len;
        *data = &_response_buf[0];
        aux->local.family = AF_INET6;
        _response_len = 0;
        return res;
    }

    return 0;
}

ssize_t sock_udp_sendv_aux(sock_udp_t *sock, const iolist_t *snips,
                           const sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux)
{
    (void)remote;
    (void)aux;

    ssize_t res = 0;

    for (const iolist_t *ptr = snips; ptr != NULL; ptr = snips->iol_next) {
        uint8_t *buf = ptr->iol_base;

        /* the test only works if there is only one snip */
        expect(ptr->iol_next == NULL);
        od_hex_dump(ptr->iol_base, ptr->iol_len, OD_WIDTH_DEFAULT);
        res += ptr->iol_len;
        /* expect token length 2 */
        expect((buf[0] & 0xf) == 2);
        /* copy message ID to mocked response */
        _response_buf[2] = buf[2];
        _response_buf[3] = buf[3];
        /* copy token to mocked response */
        _response_buf[4] = buf[4];
        _response_buf[5] = buf[5];
    }

    _udp_cb(sock, SOCK_ASYNC_MSG_SENT, _udp_cb_arg);
    _udp_cb(sock, SOCK_ASYNC_MSG_RECV, _udp_cb_arg);
    return res;
}

void sock_udp_set_cb(sock_udp_t *sock, sock_udp_cb_t cb, void *cb_arg)
{
    (void)sock;
    _udp_cb = cb;
    _udp_cb_arg = cb_arg;
}

sock_async_ctx_t *sock_udp_get_async_ctx(sock_udp_t *sock)
{
    return &sock->async_ctx;
}

/* TODO: the module should have a generic `netif_numof()` dependency here */
unsigned gnrc_netif_numof(void)
{
    return 1U;
}

/* TODO: the module should have a generic `netif_iter()` dependency here */
gnrc_netif_t *gnrc_netif_iter(const gnrc_netif_t *prev)
{
    static gnrc_netif_t _netif = { .pid = NETIF_ID };

    if (prev) {
        return NULL;
    }
    else {
        return &_netif;
    }
}

static int _preload_response(int argc, char **argv)
{
    ssize_t data_len;

    if (argc < 2) {
        printf("usage: %s <data hex>\n", argv[0]);
        return 1;
    }

    if ((_response_len + (strlen(argv[1]) / 2)) > sizeof(_response_buf)) {
        printf("error: \"%s\" does not fit into remaining buffer "
               "(buffer size: %lu).\n", argv[1],
               (unsigned long)sizeof(_response_buf));
        return 1;
    }

    if ((data_len = scn_buf_hex(
        &_response_buf[_response_len], sizeof(_response_buf) - _response_len,
        argv[1], strlen(argv[1])
    )) < 0) {
        printf("error: Can not parse \"%s\" as hex string.\n", argv[1]);
        return 1;
    }
    _response_len += data_len;

    od_hex_dump(_response_buf, _response_len, OD_WIDTH_DEFAULT);

    return 0;
}

static int _set_response_remote(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <coap uri>\n", argv[0]);
        return 1;
    }

    uri_parser_result_t uri_result;
    if (uri_parser_process_string(&uri_result, argv[1]) != 0) {
        puts("error: unable to parse URI");
        return 1;
    }
    _response_remote.family = AF_INET6;
    _response_remote.netif = NETIF_ID;
    _response_remote.port = uri_result.port;

    if ((uri_result.scheme_len == 4) && (strncmp(uri_result.scheme, "coap", 4) == 0)) {
        if (uri_result.port == 0) {
            _response_remote.port = COAP_PORT;
        }
    }
    else {
        puts("error: Only coap schemes are supported");
        return 1;
    }

    if (uri_result.ipv6addr == NULL) {
        puts("error: Only IPv6 addresses are supported");
        return 1;
    }

    if (!ipv6_addr_from_buf((ipv6_addr_t *)&_response_remote.addr.ipv6, uri_result.ipv6addr,
                            uri_result.ipv6addr_len)) {
        puts("error: IPv6 address malformed");
        return 1;
    }
    static char addr_str[IPV6_ADDR_MAX_STR_LEN];
    uint16_t port;
    if (sock_udp_ep_fmt(&_response_remote, addr_str, &port) > 0) {
        printf("success: set response remote to [%s]:%u\n", addr_str, port);
    }
    else {
        puts("error: can not format response remote");
        return 1;
    }
    return 0;
}

SHELL_COMMAND(preload_response, "Preload CoAP response", _preload_response);
SHELL_COMMAND(response_remote, "Prepare remote of CoAP response", _set_response_remote);

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    puts("Starting riotctrl_shell.cord_ep test");

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
