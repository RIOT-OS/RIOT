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

#include <errno.h>
#include <stdbool.h>

#include "byteorder.h"
#include "net/af.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "net/conn.h"

#include "lwip/api.h"
#include "lwip/opt.h"

int lwip_conn_create(struct netconn **netconn, const void *addr, size_t addr_len, int family,
                     int type, int proto, uint16_t port)
{
    struct netconn *tmp;
    int res = 0;

    switch (family) {
#if LWIP_IPV4
        case AF_INET:
            if (addr_len != sizeof(ipv4_addr_t)) {
                return -EINVAL;
            }
            break;
#endif
#if LWIP_IPV6
        case AF_INET6:
            if (addr_len != sizeof(ipv6_addr_t)) {
                return -EINVAL;
            }
            type |= NETCONN_TYPE_IPV6;
            break;
#endif
        default:
            return -EAFNOSUPPORT;
    }
    if ((tmp = netconn_new_with_proto_and_callback(type, proto, NULL)) == NULL) {
        return -ENOMEM;
    }
    switch (netconn_bind(tmp, (ip_addr_t *)addr, port)) {
        case ERR_USE:
            netconn_delete(tmp);
            res = -EADDRINUSE;
            break;
        case ERR_VAL:
            netconn_delete(tmp);
            res = -EINVAL;
            break;
        default:
            break;
    }
    *netconn = tmp;
    return res;
}

int lwip_conn_getlocaladdr(struct netconn *netconn, void *addr, uint16_t *port)
{
    uint16_t tmp;

    if (netconn_getaddr(netconn, addr, &tmp, 1) != ERR_OK) {
        return -EOPNOTSUPP;
    }
    if (port != NULL) {
        *port = tmp;
    }
#if LWIP_IPV6
    if (netconn->type & NETCONN_TYPE_IPV6) {
        return sizeof(ipv6_addr_t);
    }
#endif
#if LWIP_IPV4
    return sizeof(ipv4_addr_t);
#else
    return -EOPNOTSUPP;
#endif
}

int lwip_conn_recvfrom(struct netconn *netconn, void *data, size_t max_len, void *addr,
                       size_t *addr_len, uint16_t *port)
{
    struct netbuf *buf;
    size_t len = 0;
    err_t res = 0;
    uint8_t *data_ptr = data;

    if (netconn == NULL) {
        return -ENOTSOCK;
    }
    if ((res = netconn_recv(netconn, &buf))) {
        switch (res) {
#if LWIP_SO_RCVTIMEO
            case ERR_TIMEOUT:
                return -ETIMEDOUT;
#endif
            case ERR_MEM:
                return -ENOMEM;
            default:
                return -EIO;
        }
    }
    len = buf->p->tot_len;
    if (len > max_len) {
        netbuf_delete(buf);
        return -ENOBUFS;
    }
#if LWIP_IPV6
    if (netconn->type & NETCONN_TYPE_IPV6) {
        *addr_len = sizeof(ipv6_addr_t);
    }
    else {
#endif
#if LWIP_IPV4
        *addr_len = sizeof(ipv4_addr_t);
#else
        netbuf_delete(buf);
        return -EOPNOTSUPP;
#endif
#if LWIP_IPV6
    }
#endif
    /* copy address */
    memcpy(addr, &buf->addr, *addr_len);
    /* copy port */
    if (port != NULL) {
        *port = buf->port;
    }
    /* copy data */
    for (struct pbuf *q = buf->p; q != NULL; q = q->next) {
        memcpy(data_ptr, q->payload, q->len);
        data_ptr += q->len;
    }

    netbuf_delete(buf);

    return (int)len;
}

int lwip_conn_sendto(struct netconn *netconn, const void *data, size_t len,
                     const void *addr, size_t addr_len, uint16_t port)
{
    struct netbuf *buf;
    int res;

#if LWIP_IPV6
    if (netconn->type & NETCONN_TYPE_IPV6) {
        if (addr_len != sizeof(ipv6_addr_t)) {
            return -EINVAL;
        }
    }
    else {
#endif
#if LWIP_IPV4
        if (addr_len != sizeof(ipv4_addr_t)) {
            return -EINVAL;
        }
#endif
#if LWIP_IPV6
    }
#endif
    buf = netbuf_new();
    if ((buf == NULL) || (netbuf_alloc(buf, len) == NULL)) {
        netbuf_delete(buf);
        return -ENOMEM;
    }
    if (netbuf_take(buf, data, len) != ERR_OK) {
        netbuf_delete(buf);
        return -ENOBUFS;
    }
    switch ((res = netconn_sendto(netconn, buf, addr, port))) {
        case ERR_OK:
            res = len;
            break;
        case ERR_RTE:
            res = -EHOSTUNREACH;
            break;
        case ERR_VAL:
            res = -EINVAL;
            break;
        case ERR_IF:
            res = -EAFNOSUPPORT;
            break;
        default:
            res = -EIO;
            break;
    }
    netbuf_delete(buf);
    return res;
}

/** @} */
