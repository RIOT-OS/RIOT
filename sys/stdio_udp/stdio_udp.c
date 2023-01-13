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

#include "macros/utils.h"
#include "net/sock/udp.h"

#ifndef CONFIG_STDIO_UDP_PORT
#define CONFIG_STDIO_UDP_PORT       2323
#endif

#ifndef CONFIG_STDIO_UDP_RX_BUF_LEN
#define CONFIG_STDIO_UDP_RX_BUF_LEN 64
#endif

static sock_udp_t sock;
static sock_udp_ep_t remote;

void stdio_init(void)
{
    const sock_udp_ep_t local = {
        .family = AF_INET6,
        .netif = SOCK_ADDR_ANY_NETIF,
        .port = CONFIG_STDIO_UDP_PORT,
    };

    sock_udp_create(&sock, &local, NULL, 0);
}

ssize_t stdio_read(void* buffer, size_t count)
{
    static uint8_t rx_buf[CONFIG_STDIO_UDP_RX_BUF_LEN];
    static uint8_t *pos;
    static size_t left;

    /* shell will only read one byte at a time, so we buffer the input */
    if (left == 0) {
        int res = sock_udp_recv(&sock, rx_buf, sizeof(rx_buf),
                                SOCK_NO_TIMEOUT, &remote);
        if (res > 0) {
            left = res;
            pos = rx_buf;
        } else {
            return res;
        }
    }

    count = MIN(left, count);
    memcpy(buffer, pos, count);

    left -= count;
    pos  += count;

    return count;
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    if (remote.port == 0) {
        return -ENOTCONN;
    }
    if (len == 0) {
        return 0;
    }

    return sock_udp_send(&sock, buffer, len, &remote);
}
