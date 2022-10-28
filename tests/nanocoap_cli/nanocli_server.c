/*
 * Copyright (c) 2018 Ken Bannister. All rights reserved.
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
 * @brief       nanocoap test server
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "net/nanocoap_sock.h"
#include "net/sock/udp.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/*
 * Customized implementation of nanocoap_server() to ignore a count of
 * requests. Allows testing confirmable messaging.
 */
static int _nanocoap_server(sock_udp_ep_t *local, uint8_t *buf, size_t bufsize,
                            int ignore_count)
{
    sock_udp_t sock;
    sock_udp_ep_t remote;

    if (!local->port) {
        local->port = COAP_PORT;
    }

    ssize_t res = sock_udp_create(&sock, local, NULL, 0);
    if (res == -1) {
        return -1;
    }

    int recv_count = 0;
    while (1) {
        res = sock_udp_recv(&sock, buf, bufsize, -1, &remote);
        if (++recv_count <= ignore_count) {
            DEBUG("ignoring request\n");
            continue;
        }
        if (res == -1) {
            DEBUG("nanocoap: error receiving coap request, \n");
            return -1;
        }
        else {
            coap_pkt_t pkt;
            coap_request_ctx_t ctx = {
                .remote = &remote,
            };

            if (coap_parse(&pkt, (uint8_t *)buf, res) < 0) {
                DEBUG("nanocoap: error parsing packet\n");
                continue;
            }
            if ((res = coap_handle_req(&pkt, buf, bufsize, &ctx)) > 0) {
                res = sock_udp_send(&sock, buf, res, &remote);
            }
        }
    }

    return 0;
}

static void _start_server(uint16_t port, int ignore_count)
{
    uint8_t buf[128];
    sock_udp_ep_t local = { .port=port, .family=AF_INET6 };
    _nanocoap_server(&local, buf, sizeof(buf), ignore_count);
}

int nanotest_server_cmd(int argc, char **argv)
{
    if (argc < 2) {
        goto error;
    }

    if (strncmp("start", argv[1], 5) != 0) {
        goto error;
    }

    int arg_pos = 2;
    int ignore_count = 0;
    if ((argc >= (arg_pos+1)) && (strcmp(argv[arg_pos], "-i") == 0)) {
        /* need count of requests to ignore*/
        if (argc == 3) {
            goto error;
        }
        arg_pos++;

        ignore_count = atoi(argv[arg_pos]);
        if (ignore_count <= 0) {
            puts("nanocli: unable to parse ignore_count");
            goto error;
        }
        arg_pos++;
    }

    uint16_t port = COAP_PORT;
    if (argc == (arg_pos+1)) {
        port = atoi(argv[arg_pos]);
        if (port == 0) {
            puts("nanocli: unable to parse port");
            goto error;
        }
    }

    printf("starting server on port %u\n", port);
    _start_server(port, ignore_count);

    /* server executes run loop; never reaches this point*/
    return 0;

    error:
    printf("usage: %s start [-i ignore_count] [port]\n", argv[0]);
    printf("Options\n");
    printf("    -i  ignore a number of requests\n");
    printf("  port  defaults to %u\n", COAP_PORT);
    return 1;
}
