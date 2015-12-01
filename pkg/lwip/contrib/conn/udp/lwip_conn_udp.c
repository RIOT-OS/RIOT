/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>

#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "net/conn/udp.h"

#include "lwip/api.h"
#include "lwip/conn.h"

int conn_udp_create(conn_udp_t *conn, const void *addr, size_t addr_len, int family, uint16_t port)
{
    struct netconn *tmp;
    int res;

    res = lwip_conn_create(&tmp, addr, addr_len, family, NETCONN_UDP, 0, port);
    if (res < 0) {
        return res;
    }
    conn->lwip_conn = tmp;

    return res;
}

void conn_udp_close(conn_udp_t *conn)
{
    assert(conn != NULL);
    netconn_delete(conn->lwip_conn);
}

int conn_udp_getlocaladdr(conn_udp_t *conn, void *addr, uint16_t *port)
{
    assert(conn != NULL);
    return lwip_conn_getlocaladdr(conn->lwip_conn, addr, port);
}

int conn_udp_recvfrom(conn_udp_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len,
                      uint16_t *port)
{
    assert(conn != NULL);
    return lwip_conn_recvfrom(conn->lwip_conn, data, max_len, addr, addr_len, port);
}

int conn_udp_sendto(const void *data, size_t len, const void *src, size_t src_len,
                    const void *dst, size_t dst_len, int family, uint16_t sport,
                    uint16_t dport)
{
    struct netconn *tmp;
    int res;

    res = lwip_conn_create(&tmp, src, src_len, family, NETCONN_UDP, 0, sport);
    if (res < 0) {
        return res;
    }
    res = lwip_conn_sendto(tmp, data, len, dst, dst_len, dport);
    netconn_delete(tmp);
    return res;
}

/** @} */
