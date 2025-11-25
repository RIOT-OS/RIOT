/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "stdio_base.h"
#include "net/sock/async.h"
#include "net/sock/udp.h"
#include "net/utils.h"
#include "ztimer.h"

#ifndef CONFIG_STDIO_UDP_PORT
#define CONFIG_STDIO_UDP_PORT 2323
#endif

#ifndef EOT
#define EOT 0x4
#endif

extern int readline(char *buf, size_t size);

static void _sock_cb(sock_udp_t *sock, sock_async_flags_t flags, void *arg)
{
    (void)arg;
    if ((flags & SOCK_ASYNC_MSG_RECV) == 0) {
        return;
    }

    void *data, *ctx = NULL;

    int res;
    while ((res = sock_udp_recv_buf(sock, &data, &ctx, 0, NULL)) > 0) {
        stdio_write(data, res);
    }
}

static int _udptty_cmd(int argc, char **argv)
{
    if (argc < 2) {
        goto usage;
    }

    netif_t *netif = NULL;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
    sock_udp_t sock;
    int res;

    const sock_udp_ep_t local = {
        .family = AF_INET6,
        .netif = SOCK_ADDR_ANY_NETIF,
    };

    if (netutils_get_ipv6((ipv6_addr_t *)&remote.addr, &netif, argv[1])) {
        goto usage;
    }

    if (netif) {
        remote.netif = netif_get_id(netif);
    }

    if (argc > 2) {
        remote.port = atoi(argv[2]);
    } else {
        remote.port = CONFIG_STDIO_UDP_PORT;
    }

    res = sock_udp_create(&sock, &local, &remote, 0);
    if (res) {
        printf("can't create socket: %d\n", res);
        return res;
    }

    printf("connected to %s, press Ctrl+D to exit\n", argv[1]);
    sock_udp_set_cb(&sock, _sock_cb, NULL);

    do {
        char buf[64];
        res = readline(buf, sizeof(buf));

        if (res > 0) {
            buf[res] = '\n';
            sock_udp_send(&sock, buf, res + 1, NULL);
        }

#ifdef CPU_NATIVE
        /* readline() on native blocks all other threads - see #19002 */
        ztimer_sleep(ZTIMER_MSEC, 100);
#endif
    } while (res != EOF);

    /* send disconnect */
    const char eot = EOT;
    sock_udp_send(&sock, &eot, sizeof(eot), NULL);
    sock_udp_close(&sock);

    return 0;

usage:
    printf("usage: %s <server> <port>\n", argv[0]);
    return -1;
}

SHELL_COMMAND(udptty, "UDP remote shell", _udptty_cmd);
/** @} */
