/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_stdio_udp
 * @{
 *
 * @file
 * @brief       STDIO over UDP implementation
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "auto_init.h"
#include "auto_init_utils.h"

#include "stdio_base.h"
#include "macros/utils.h"
#include "net/sock/async.h"
#include "net/sock/udp.h"
#include "net/gnrc/netif.h"
#include "net/netdev.h"

/**
 * We can't use NETDEV_ANY since it's defined in an enum
 */
#define NETDEV_INVALID                  -1

#ifndef CONFIG_STDIO_UDP_PORT
#define CONFIG_STDIO_UDP_PORT           2323
#endif

#ifndef CONFIG_STDIO_UDP_NETIF_TYPE
#define CONFIG_STDIO_UDP_NETIF_TYPE     NETDEV_INVALID
#endif

#ifndef CONFIG_STDIO_UDP_NETIF_IDX
#define CONFIG_STDIO_UDP_NETIF_IDX      0
#endif

#ifndef AUTO_INIT_PRIO_MOD_STDIO_UDP
#define AUTO_INIT_PRIO_MOD_STDIO_UDP    2000
#endif

#ifndef EOT
#define EOT 0x4
#endif

static sock_udp_t sock;
static sock_udp_ep_t remote;

static void _sock_cb(sock_udp_t *sock, sock_async_flags_t flags, void *arg)
{
    (void)arg;

    if ((flags & SOCK_ASYNC_MSG_RECV) == 0) {
        return;
    }

    void *data, *ctx = NULL;

    int res;
    while ((res = sock_udp_recv_buf(sock, &data, &ctx, 0, &remote)) > 0) {
        isrpipe_write(&stdin_isrpipe, data, res);

        /* detach remote */
        if (res == 1 && *(int8_t *)data == EOT) {
            const char msg[] = "\nremote detached\n";
            sock_udp_send(sock, msg, sizeof(msg), &remote);
            memset(&remote, 0, sizeof(remote));
        }
    }
}

static void _init(void)
{
    sock_udp_ep_t local = {
        .family = AF_INET6,
        .netif = SOCK_ADDR_ANY_NETIF,
        .port = CONFIG_STDIO_UDP_PORT,
    };

    if (CONFIG_STDIO_UDP_NETIF_TYPE != NETDEV_INVALID) {
        gnrc_netif_t *netif = gnrc_netif_get_by_type(CONFIG_STDIO_UDP_NETIF_TYPE,
                                                     CONFIG_STDIO_UDP_NETIF_IDX);
        /* this will always fail when called in early init */
        if (netif == NULL) {
            return;
        }
        local.netif = netif->pid;
    }

    sock_udp_create(&sock, &local, NULL, 0);
    sock_udp_set_cb(&sock, _sock_cb, NULL);
}

static ssize_t _write(const void* buffer, size_t len)
{
    if (remote.port == 0) {
        return -ENOTCONN;
    }
    if (len == 0) {
        return 0;
    }

    return sock_udp_send(&sock, buffer, len, &remote);
}

static void _detach(void)
{
    sock_udp_close(&sock);
    memset(&remote, 0, sizeof(remote));
}
STDIO_PROVIDER(STDIO_UDP, _init, _detach, _write)

#if CONFIG_STDIO_UDP_NETIF_TYPE != NETDEV_INVALID
/* call _init() again when network interfaces have been brought up */
AUTO_INIT(_init, AUTO_INIT_PRIO_MOD_STDIO_UDP);
#endif
