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

#include <assert.h>
#include <errno.h>

#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/sock/ip.h"
#include "timex.h"

#include "lwip/api.h"
#include "lwip/ip4.h"
#include "lwip/ip6.h"
#include "lwip/netif.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/sock_internal.h"

#ifdef SOCK_HAS_ASYNC_CTX
#include "net/sock/async/event.h"
#endif

int sock_ip_create(sock_ip_t *sock, const sock_ip_ep_t *local,
                   const sock_ip_ep_t *remote, uint8_t proto, uint16_t flags)
{
    assert(sock != NULL);

    int res;
    struct netconn *tmp = NULL;

    /* we pay attention in lwip_sock_create that _sock_tl_ep::port is not
     * touched for RAW */
    if ((res = lwip_sock_create(&tmp, (struct _sock_tl_ep *)local,
                                (struct _sock_tl_ep *)remote, proto, flags,
                                NETCONN_RAW)) == 0) {
        sock->base.conn = tmp;
#if IS_ACTIVE(SOCK_HAS_ASYNC)
        sock->base.async_cb.gen = NULL;
        netconn_set_callback_arg(sock->base.conn, &sock->base);
#endif
    }

    return res;
}

void sock_ip_close(sock_ip_t *sock)
{
    assert(sock != NULL);
    if (sock->base.conn != NULL) {
        netconn_delete(sock->base.conn);
        sock->base.conn = NULL;
    }
#ifdef SOCK_HAS_ASYNC_CTX
    sock_event_close(sock_ip_get_async_ctx(sock));
#endif
}

int sock_ip_get_local(sock_ip_t *sock, sock_ip_ep_t *ep)
{
    assert(sock != NULL);
    return (lwip_sock_get_addr(sock->base.conn, (struct _sock_tl_ep *)ep,
                               1)) ? -EADDRNOTAVAIL : 0;
}

int sock_ip_get_remote(sock_ip_t *sock, sock_ip_ep_t *ep)
{
    assert(sock != NULL);
    return (lwip_sock_get_addr(sock->base.conn, (struct _sock_tl_ep *)ep,
                               0)) ? -ENOTCONN : 0;
}

#if LWIP_IPV4
static uint16_t _ip4_addr_to_netif(const ip4_addr_p_t *addr)
{
    assert(addr != NULL);

    if (!ip4_addr_isany(addr)) {
        struct netif *netif;
        /* cppcheck-suppress uninitvar ; assigned by macro */
        NETIF_FOREACH(netif) {
            if (netif_ip4_addr(netif)->addr == addr->addr) {
                return (int)netif->num + 1;
            }
        }
    }

    return SOCK_ADDR_ANY_NETIF;
}
#endif

#if LWIP_IPV6
static uint16_t _ip6_addr_to_netif(const ip6_addr_p_t *_addr)
{
    ip6_addr_t addr;

    assert(_addr != NULL);
    ip6_addr_copy_from_packed(addr, *_addr);
    if (!ip6_addr_isany_val(addr)) {
        struct netif *netif;
        LOCK_TCPIP_CORE();
        /* cppcheck-suppress uninitvar ; assigned by macro */
        NETIF_FOREACH(netif) {
            if (netif_get_ip6_addr_match(netif, &addr) >= 0) {
                UNLOCK_TCPIP_CORE();
                return (int)netif->num + 1;
            }
        }
        UNLOCK_TCPIP_CORE();
    }

    return SOCK_ADDR_ANY_NETIF;
}
#endif

static int _parse_iphdr(struct netbuf *buf, void **data, void **ctx,
                        sock_ip_ep_t *remote, sock_ip_ep_t *local)
{
    uint8_t *data_ptr = buf->ptr->payload;
    size_t data_len = buf->ptr->len;

    switch (data_ptr[0] >> 4) {
#if LWIP_IPV4
    case 4:
        if (remote != NULL) {
            struct ip_hdr *iphdr = (struct ip_hdr *)data_ptr;

            assert(buf->p->len > sizeof(struct ip_hdr));
            remote->family = AF_INET;
            memcpy(&remote->addr, &iphdr->src, sizeof(ip4_addr_t));
            remote->netif = _ip4_addr_to_netif(&iphdr->dest);
        }
        if (local != NULL) {
            struct ip_hdr *iphdr = (struct ip_hdr *)data_ptr;

            assert(buf->p->len > sizeof(struct ip_hdr));
            local->family = AF_INET;
            memcpy(&local->addr, &iphdr->dest, sizeof(ip4_addr_t));
        }
        data_ptr += sizeof(struct ip_hdr);
        data_len -= sizeof(struct ip_hdr);
        break;
#endif
#if LWIP_IPV6
    case 6:
        if (remote != NULL) {
            struct ip6_hdr *iphdr = (struct ip6_hdr *)data_ptr;

            assert(buf->p->len > sizeof(struct ip6_hdr));
            remote->family = AF_INET6;
            memcpy(&remote->addr, &iphdr->src, sizeof(ip6_addr_t));
            remote->netif = _ip6_addr_to_netif(&iphdr->dest);
        }
        if (local != NULL) {
            struct ip6_hdr *iphdr = (struct ip6_hdr *)data_ptr;

            assert(buf->p->len > sizeof(struct ip6_hdr));
            local->family = AF_INET6;
            memcpy(&local->addr, &iphdr->dest, sizeof(ip6_addr_t));
        }
        data_ptr += sizeof(struct ip6_hdr);
        data_len -= sizeof(struct ip6_hdr);
        break;
#endif
    default:
        netbuf_delete(buf);
        return -EPROTO;
    }

    *data = data_ptr;
    *ctx = buf;

    return (ssize_t)data_len;
}

ssize_t sock_ip_recv_aux(sock_ip_t *sock, void *data, size_t max_len,
                         uint32_t timeout, sock_ip_ep_t *remote,
                         sock_ip_aux_rx_t *aux)
{
    void *pkt = NULL;
    struct netbuf *ctx = NULL;
    uint8_t *ptr = data;
    ssize_t res, ret = 0;
    bool nobufs = false;

    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    while ((res = sock_ip_recv_buf_aux(sock, &pkt, (void **)&ctx, timeout,
                                       remote, aux)) > 0) {
        if (ctx->p->tot_len > (ssize_t)max_len) {
            nobufs = true;
            /* progress context to last element */
            while (netbuf_next(ctx) == 0) {}
            continue;
        }
        memcpy(ptr, pkt, res);
        ptr += res;
        ret += res;
    }

    return (nobufs) ? -ENOBUFS : ((res < 0) ? res : ret);
}

ssize_t sock_ip_recv_buf_aux(sock_ip_t *sock, void **data, void **ctx,
                             uint32_t timeout, sock_ip_ep_t *remote,
                             sock_ip_aux_rx_t *aux)
{
    (void)aux;
    struct netbuf *buf;
    int res;

    assert((sock != NULL) && (data != NULL) && (ctx != NULL));
    buf = *ctx;

    if (buf != NULL) {
        if (netbuf_next(buf) == -1) {
            *data = NULL;
            netbuf_delete(buf);
            *ctx = NULL;
            return 0;
        }
        else {
            *data = buf->ptr->payload;
            return buf->ptr->len;
        }
    }

    if ((res = lwip_sock_recv(sock->base.conn, timeout, &buf)) < 0) {
        return res;
    }

    sock_ip_ep_t *local = NULL;

#if IS_USED(MODULE_SOCK_AUX_LOCAL)
    if (aux != NULL) {
        local = &aux->local;
        aux->flags &= ~(SOCK_AUX_GET_LOCAL);
    }
#endif

    res = _parse_iphdr(buf, data, ctx, remote, local);

    return res;
}

ssize_t sock_ip_send_aux(sock_ip_t *sock, const void *data, size_t len,
                         uint8_t proto, const sock_ip_ep_t *remote,
                         sock_ip_aux_tx_t *aux)
{
    (void)aux;
    assert((sock != NULL) || (remote != NULL));
    assert((len == 0) || (data != NULL)); /* (len != 0) => (data != NULL) */

    return lwip_sock_send(sock ? sock->base.conn : NULL, data, len, proto,
                          (struct _sock_tl_ep *)remote, NETCONN_RAW);
}

#ifdef SOCK_HAS_ASYNC
void sock_ip_set_cb(sock_ip_t *sock, sock_ip_cb_t cb, void *arg)
{
    sock->base.async_cb_arg = arg;
    sock->base.async_cb.ip = cb;
}

#ifdef SOCK_HAS_ASYNC_CTX
sock_async_ctx_t *sock_ip_get_async_ctx(sock_ip_t *sock)
{
    return &sock->base.async_ctx;
}
#endif  /* SOCK_HAS_ASYNC_CTX */
#endif  /* SOCK_HAS_ASYNC */

/** @} */
