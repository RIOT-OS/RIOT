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
#include "net/conn/ip.h"

#include "lwip/api.h"
#include "lwip/conn.h"

int conn_ip_create(conn_ip_t *conn, const void *addr, size_t addr_len, int family, int proto)
{
    struct netconn *tmp;
    int res;

    res = lwip_conn_create(&tmp, addr, addr_len, family, NETCONN_RAW, proto, 0);
    if (res < 0) {
        return res;
    }
    conn->lwip_conn = tmp;

    return res;
}

void conn_ip_close(conn_ip_t *conn)
{
    assert(conn != NULL);
    netconn_delete(conn->lwip_conn);
}

int conn_ip_getlocaladdr(conn_ip_t *conn, void *addr)
{
    assert(conn != NULL);
    return lwip_conn_getlocaladdr(conn->lwip_conn, addr, NULL);
}

int conn_ip_recvfrom(conn_ip_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len)
{
    assert(conn != NULL);
    return lwip_conn_recvfrom(conn->lwip_conn, data, max_len, addr, addr_len, NULL);
}

int conn_ip_sendto(const void *data, size_t len, const void *src, size_t src_len,
                   void *dst, size_t dst_len, int family, int proto)
{
    struct netconn *tmp;
    int res;

    res = lwip_conn_create(&tmp, src, src_len, family, NETCONN_RAW, proto, 0);
    if (res < 0) {
        return res;
    }
    res = lwip_conn_sendto(tmp, data, len, dst, dst_len, 0);
    netconn_delete(tmp);
    return res;
}

/** @} */
