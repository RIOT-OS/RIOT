/*
 * SPDX-FileCopyrightText: 2026 Christopher Büchse
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_epson_escpos
 * @{
 *
 * @file
 * @brief       TCP transport of the ESC/POS driver
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "epson_escpos.h"
#include "net/ipv4/addr.h"
#include "time_units.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int epson_escpos_tcp_setup(epson_escpos_tcp_t *tcp,
                           const epson_escpos_tcp_params_t *params)
{
    assert(tcp && params && params->host);

    memset(&tcp->remote, 0, sizeof(tcp->remote));
    tcp->remote.family = AF_INET;
    tcp->remote.netif = SOCK_ADDR_ANY_NETIF;
    tcp->remote.port = params->port;

    if (!ipv4_addr_from_str((ipv4_addr_t *)&tcp->remote.addr.ipv4,
                            params->host)) {
        DEBUG("[epson_escpos_tcp] invalid host: %s\n", params->host);
        return -EINVAL;
    }
    return 0;
}

static int _open(void *ctx)
{
    epson_escpos_tcp_t *tcp = ctx;
    return sock_tcp_connect(&tcp->sock, &tcp->remote, 0, 0);
}

static void _close(void *ctx)
{
    epson_escpos_tcp_t *tcp = ctx;
    sock_tcp_disconnect(&tcp->sock);
}

static int _write(void *ctx, const void *data, size_t len)
{
    epson_escpos_tcp_t *tcp = ctx;
    const uint8_t *pos = data;

    while (len) {
        ssize_t res = sock_tcp_write(&tcp->sock, pos, len);
        if (res < 0) {
            DEBUG("[epson_escpos_tcp] write failed: %d\n", (int)res);
            return res;
        }
        pos += res;
        len -= res;
    }
    return 0;
}

static ssize_t _read(void *ctx, void *data, size_t len, uint32_t timeout_ms)
{
    epson_escpos_tcp_t *tcp = ctx;
    return sock_tcp_read(&tcp->sock, data, len, timeout_ms * US_PER_MS);
}

const epson_escpos_transport_t epson_escpos_tcp_transport = {
    .open = _open,
    .close = _close,
    .write = _write,
    .read = _read,
};
