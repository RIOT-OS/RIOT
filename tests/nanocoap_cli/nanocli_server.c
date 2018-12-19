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

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _start_server(uint16_t port)
{
    uint8_t buf[128];
    sock_udp_ep_t local = { .port=port, .family=AF_INET6 };
    nanocoap_server(&local, buf, sizeof(buf));
}

int nanotest_server_cmd(int argc, char **argv)
{
    uint16_t port = COAP_PORT;
    if (argc >= 2) {
        if (strncmp("start", argv[1], 5) != 0) {
            goto end;
        }

        if (argc == 3) {
            port = atoi(argv[2]);
            if (port == 0) {
                puts("nanocli: unable to parse port");
                return 1;
            }
        }

        puts("starting server\n");
        _start_server(port);

        /* server executes run loop; never reaches this point*/
        return 0;
    }

    end:
    printf("usage: %s start [port]\n", argv[0]);
    printf("       Port defaults to %u\n", COAP_PORT);
    return 1;
}
