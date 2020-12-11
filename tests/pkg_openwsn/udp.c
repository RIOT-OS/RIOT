/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 * Copyright (C) 2020 Inria
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
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ztimer.h"
#include "net/ipv6.h"
#include "net/sock/udp.h"
#ifdef SOCK_HAS_ASYNC
#include "net/sock/async.h"
#endif
#include "od.h"

#include "opendefs.h"
#include "02a-MAClow/IEEE802154E.h"
#include "03b-IPv6/icmpv6rpl.h"

#include "cross-layers/idmanager.h"
#include "cross-layers/packetfunctions.h"

extern idmanager_vars_t idmanager_vars;

static sock_udp_t _sock_udp;

void _sock_udp_handler(sock_udp_t *sock, sock_async_flags_t type, void *arg)
{
    if (type & SOCK_ASYNC_MSG_RECV) {
        char buf[50];
        sock_udp_ep_t remote;
        int16_t res;

        if ((res = sock_udp_recv(sock, buf, sizeof(buf), 0, &remote)) >= 0) {
            printf("Received %i bytes on port %i\n", res, remote.port);
            od_hex_dump(buf, res, OD_WIDTH_DEFAULT);
        }
    }

    if (type & SOCK_ASYNC_MSG_SENT) {
        if (*((uint8_t *)arg) == E_FAIL) {
            puts("Failed to Send");
        }
        else {
            puts("Send Success");
        }
    }
}

void udp_cli_init(void)
{
    memset(&_sock_udp, 0, sizeof(sock_udp_t));
    sock_udp_ep_t local;

    local.family = AF_INET6;
    local.netif = SOCK_ADDR_ANY_NETIF;
    local.port = WKP_UDP_ECHO;

    if (sock_udp_create(&_sock_udp, &local, NULL, 0) < 0) {
        puts("Could not create socket");
        return;
    }

    sock_udp_set_cb(&_sock_udp, _sock_udp_handler, NULL);
}

static int udp_send(char *addr_str, char *port_str, char *data,
                    unsigned int num, unsigned int delay)
{
    size_t data_len;
    open_addr_t parentNeighbor;
    ipv6_addr_t addr;

    data_len = strlen(data);

    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("Error: unable to parse destination address, exit");
        return 1;
    }

    for (unsigned int i = 0; i < num; i++) {

        printf("Send %u byte over UDP to [%s]:%s\n",
               (unsigned)data_len, addr_str, port_str);

        /* don't run if not in synch */
        if (ieee154e_isSynch() == FALSE) {
            puts("Error: node is not synchronized, exit");
            return 1;
        }

        /* don't run on dagroot */
        if (idmanager_getIsDAGroot()) {
            puts("Error: node is DAGROOT, exit");
            return 1;
        }

        bool foundNeighbor = icmpv6rpl_getPreferredParentEui64(&parentNeighbor);
        if (foundNeighbor == FALSE) {
            puts("Error: no preferred parent EUI64, exit");
            return 1;
        }

        sock_udp_ep_t remote;
        remote.family = AF_INET6;
        remote.netif = SOCK_ADDR_ANY_NETIF;
        remote.port = atoi(port_str);
        memcpy(&remote.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));

        int res = sock_udp_send(&_sock_udp, data, data_len, &remote);
        if (res == -EINVAL) {
            puts("Error: EINVAL");
        }
        else if (res == -EAFNOSUPPORT) {
            puts("Error: EAFNOSUPPORT");
        }
        else if (res == -ENOMEM) {
            puts("Error: ENOMEM");
        }
        else if (res == -ENOBUFS) {
            puts("Error: ENOBUFS");
        }

        ztimer_sleep(ZTIMER_USEC, delay);
    }
    return 0;
}

int udp_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [send|server]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "send") == 0) {
        uint32_t num = 1;
        uint32_t delay = 1000000LU;
        /* don't send as root */
        if (idmanager_vars.isDAGroot) {
            puts("Error: node is root, exit");
            return 1;
        }
        if (argc < 5) {
            printf(
                "Usage: %s send <addr> <port> <hex data> [<num> [<delay in us>]]\n",
                argv[0]);
            return 1;
        }
        if (argc > 5) {
            num = atoi(argv[5]);
        }
        if (argc > 6) {
            delay = atoi(argv[6]);
        }
        return udp_send(argv[2], argv[3], argv[4], num, delay);
    }
    else if (strcmp(argv[1], "server") == 0) {
        if (argc < 3) {
            printf("Usage: %s server [start|show]\n", argv[0]);
            return 1;
        }
        if (strcmp(argv[2], "start") == 0) {
            if (argc < 4) {
                printf("Usage %s server start <port>\n", argv[0]);
                return 1;
            }
            uint16_t port = atoi(argv[3]);
            sock_udp_ep_t local;
            sock_udp_get_local(&_sock_udp, &local);
            local.port = port;
            printf("Set UDP server port to %" PRIu16 "\n", port);
            return 0;
        }
        else if (strcmp(argv[2], "show") == 0) {
            sock_udp_ep_t local;
            sock_udp_get_local(&_sock_udp, &local);
            printf("Udp port: %i\n", local.port);
        }
        else {
            puts("Error: invalid command");
            return 1;
        }
    }
    else {
        puts("Error: invalid command");
        return 1;
    }

    return 1;
}
/** @} */
