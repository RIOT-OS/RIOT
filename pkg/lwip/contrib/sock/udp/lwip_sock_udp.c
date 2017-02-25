/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <errno.h>

#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "net/sock/udp.h"
#include "timex.h"

#include "lwip/api.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/sock_internal.h"

int sock_udp_create(sock_udp_t *sock, const sock_udp_ep_t *local,
                    const sock_udp_ep_t *remote, uint16_t flags)
{
    assert(sock != NULL);
    assert(local == NULL || local->port != 0);
    assert(remote == NULL || remote->port != 0);

    int res;
    struct netconn *tmp = NULL;

    if ((res = lwip_sock_create(&tmp, (struct _sock_tl_ep *)local,
                                (struct _sock_tl_ep *)remote, 0, flags,
                                NETCONN_UDP)) == 0) {
        sock->conn = tmp;
    }
    return res;
}

void sock_udp_close(sock_udp_t *sock)
{
    assert(sock != NULL);
    if (sock->conn != NULL) {
        netconn_delete(sock->conn);
        sock->conn = NULL;
    }
}

int sock_udp_get_local(sock_udp_t *sock, sock_udp_ep_t *ep)
{
    assert(sock != NULL);
    return (lwip_sock_get_addr(sock->conn, (struct _sock_tl_ep *)ep,
                               1)) ? -EADDRNOTAVAIL : 0;
}

int sock_udp_get_remote(sock_udp_t *sock, sock_udp_ep_t *ep)
{
    assert(sock != NULL);
    return (lwip_sock_get_addr(sock->conn, (struct _sock_tl_ep *)ep,
                               0)) ? -ENOTCONN : 0;
}

ssize_t sock_udp_recv(sock_udp_t *sock, void *data, size_t max_len,
                      uint32_t timeout, sock_udp_ep_t *remote)
{
    uint8_t *data_ptr = data;
    struct netbuf *buf;
    int res;

    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    if ((res = lwip_sock_recv(sock->conn, timeout, &buf)) < 0) {
        return res;
    }
    res = buf->p->tot_len;
    if ((unsigned)res > max_len) {
        netbuf_delete(buf);
        return -ENOBUFS;
    }
    if (remote != NULL) {
        /* convert remote */
        size_t addr_len;
#if LWIP_IPV6
        if (sock->conn->type & NETCONN_TYPE_IPV6) {
            addr_len = sizeof(ipv6_addr_t);
            remote->family = AF_INET6;
        }
        else {
#endif
#if LWIP_IPV4
            addr_len = sizeof(ipv4_addr_t);
            remote->family = AF_INET;
#else
            netbuf_delete(buf);
            return -EPROTO;
#endif
#if LWIP_IPV6
        }
#endif
#if LWIP_NETBUF_RECVINFO
        remote->netif = lwip_sock_bind_addr_to_netif(&buf->toaddr);
#else
        remote->netif = SOCK_ADDR_ANY_NETIF;
#endif
        /* copy address */
        memcpy(&remote->addr, &buf->addr, addr_len);
        remote->port = buf->port;
    }
    /* copy data */
    for (struct pbuf *q = buf->p; q != NULL; q = q->next) {
        memcpy(data_ptr, q->payload, q->len);
        data_ptr += q->len;
    }
    netbuf_delete(buf);
    return (ssize_t)res;
}

ssize_t sock_udp_send(sock_udp_t *sock, const void *data, size_t len,
                      const sock_udp_ep_t *remote)
{
    assert((sock != NULL) || (remote != NULL));
    assert((len == 0) || (data != NULL)); /* (len != 0) => (data != NULL) */

    if ((remote != NULL) && (remote->port == 0)) {
        return -EINVAL;
    }
    return lwip_sock_send(&sock->conn, data, len, 0, (struct _sock_tl_ep *)remote,
                          NETCONN_UDP);
}

/** @} */
