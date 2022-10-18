/*
 * Copyright (C) 2021 Freie Universität Berlin
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
 * @author  Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>

#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "net/sock/udp.h"
#include "timex.h"

#include "lwip/api.h"
#include "lwip/opt.h"
#include "lwip/sock_internal.h"
#include "lwip/sys.h"
#include "lwip/udp.h"

int sock_udp_create(sock_udp_t *sock, const sock_udp_ep_t *local,
                    const sock_udp_ep_t *remote, uint16_t flags)
{
    assert(sock != NULL);
    assert(remote == NULL || remote->port != 0);

    int res;
    struct netconn *tmp = NULL;

    if ((res = lwip_sock_create(&tmp, (struct _sock_tl_ep *)local,
                                (struct _sock_tl_ep *)remote, 0, flags,
                                NETCONN_UDP)) == 0) {
        sock->base.conn = tmp;

        if (IS_USED(MODULE_SOCK_AUX_PEEK)) {
            sock->last_buf = NULL;
            sock->peek_buf_avail = false;
        }

#if IS_ACTIVE(SOCK_HAS_ASYNC)
        sock->base.async_cb.gen = NULL;
        netconn_set_callback_arg(sock->base.conn, &sock->base);
#endif
    }

    return res;
}

void sock_udp_close(sock_udp_t *sock)
{
    assert(sock != NULL);
    if (sock->base.conn != NULL) {
        netconn_delete(sock->base.conn);
        sock->base.conn = NULL;
    }
}

int sock_udp_get_local(sock_udp_t *sock, sock_udp_ep_t *ep)
{
    assert(sock != NULL);
    return (lwip_sock_get_addr(sock->base.conn, (struct _sock_tl_ep *)ep,
                               1)) ? -EADDRNOTAVAIL : 0;
}

int sock_udp_get_remote(sock_udp_t *sock, sock_udp_ep_t *ep)
{
    assert(sock != NULL);
    return (lwip_sock_get_addr(sock->base.conn, (struct _sock_tl_ep *)ep,
                               0)) ? -ENOTCONN : 0;
}

ssize_t sock_udp_recv_aux(sock_udp_t *sock, void *data, size_t max_len,
                          uint32_t timeout, sock_udp_ep_t *remote,
                          sock_udp_aux_rx_t *aux)
{
    void *pkt = NULL;
    struct netbuf *ctx = NULL;
    uint8_t *ptr = data;
    ssize_t res, ret = 0;
    bool nobufs = false;

    bool peek = false;
    if (aux != NULL) {
        if (IS_USED(MODULE_SOCK_AUX_PEEK)) {
            peek = aux->flags & SOCK_AUX_PEEK;
        }
    }

    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    while ((res = sock_udp_recv_buf_aux(sock, &pkt, (void**)&ctx, timeout,
                                        remote, aux)) > 0) {

        if (peek) {
            /* SOCK_AUX_PEEK was unset in previous call of sock_udp_recv_buf_aux */
            aux->flags |= SOCK_AUX_PEEK;
        }

        if (ctx->p->tot_len > (ssize_t)max_len) {
            /* copy everything that fits in to the buffer and return -ENOBUFS
             * (quite similar to the MSG_TRUNC flag) */
            memcpy(ptr, pkt, max_len);
            ret = max_len;
            nobufs = true;

            if (peek) {
                res = max_len;
                nobufs = false;
            }

            /* progress context to last element */
            while (netbuf_next(ctx) == 0) {}
            continue;
        }

        memcpy(ptr, pkt, res);
        ptr += res;
        ret += res;
    }

    if (aux != NULL) {
        if (IS_USED(MODULE_SOCK_AUX_PEEK)) {
            aux->flags &= ~(SOCK_AUX_PEEK);
        }
    }

    return (nobufs) ? -ENOBUFS : ((res < 0) ? res : ret);
}

ssize_t sock_udp_recv_buf_aux(sock_udp_t *sock, void **data, void **ctx,
                              uint32_t timeout, sock_udp_ep_t *remote,
                              sock_udp_aux_rx_t *aux)
{
    struct netbuf *buf;
    int res;

    assert((sock != NULL) && (data != NULL) && (ctx != NULL));

    bool peek = false;
    if (aux != NULL) {
        if (IS_USED(MODULE_SOCK_AUX_PEEK)) {
            peek = aux->flags & SOCK_AUX_PEEK;
            aux->flags &= ~(SOCK_AUX_PEEK);
        }
    }

    buf = sock->last_buf;

    if (buf != NULL) {
        if (netbuf_next(buf) == -1) { /* check for next part in chain */
            /* this is the last part of the chain (and maybe also the first) */

            if (sock->peek_buf_avail && IS_USED(MODULE_SOCK_AUX_PEEK)) { /* first element in the chain */
                /* return data from buffer fetched from sock */
                sock->peek_buf_avail = false;
                *ctx = buf;
                *data = buf->ptr->payload;
                res = buf->ptr->len;

                return res;
            }
            else {
                if (peek) {
                    /* reset to the original starting point for later use and finish */
                    netbuf_first(buf);
                    sock->peek_buf_avail = true;

                    return 0;
                }
                else {
                    /* delete netbuf and finish */
                    netbuf_delete(buf);
                    sock->last_buf = NULL;
                    *data = NULL;
                    *ctx = NULL;

                    return 0;
                }
            }
        }
        else {
            /* next part in chain now available */
            *data = buf->ptr->payload;
            res = buf->ptr->len;

            return res;
        }
    }

    if ((res = lwip_sock_recv(sock->base.conn, timeout, &buf)) < 0) {
        return res;
    }

    if ((remote != NULL) ||
        ((aux != NULL) && IS_USED(MODULE_SOCK_AUX_LOCAL)
                       && IS_ACTIVE(LWIP_NETBUF_RECVINFO))) {
        /* convert remote */
        size_t addr_len = sizeof(ipv4_addr_t);
        int family = AF_INET;

        if (NETCONNTYPE_ISIPV6(sock->base.conn->type)) {
            addr_len = sizeof(ipv6_addr_t);
            family = AF_INET6;
        }
        else if (!IS_ACTIVE(LWIP_IPV4)) {
            netbuf_delete(buf);
            return -EPROTO;
        }

        if (remote != NULL) {
            remote->family = family;
#if LWIP_NETBUF_RECVINFO
            remote->netif = lwip_sock_bind_addr_to_netif(&buf->toaddr);
#else
            remote->netif = SOCK_ADDR_ANY_NETIF;
#endif
            /* copy address */
            memcpy(&remote->addr, &buf->addr, addr_len);
            remote->port = buf->port;
        }
#if IS_USED(MODULE_SOCK_AUX_LOCAL)
        static_assert(IS_ACTIVE(LWIP_NETBUF_RECVINFO),
                      "sock_aux_local depends on LWIP_NETBUF_RECVINFO");

        if ((aux != NULL) && (aux->flags & SOCK_AUX_GET_LOCAL)) {
            aux->flags &= ~(SOCK_AUX_GET_LOCAL);
            aux->local.family = family;
            memcpy(&aux->local.addr, &buf->toaddr, addr_len);
            aux->local.port = sock->base.conn->pcb.udp->local_port;
        }
#endif /* MODULE_SOCK_AUX_LOCAL */
    }

    *ctx = buf;
    sock->last_buf = buf;

    *data = buf->ptr->payload;
    res = (ssize_t)buf->ptr->len;


    return res;
}

ssize_t sock_udp_sendv_aux(sock_udp_t *sock, const iolist_t *snips,
                           const sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux)
{
    (void)aux;
    assert((sock != NULL) || (remote != NULL));

    if ((remote != NULL) && (remote->port == 0)) {
        return -EINVAL;
    }

    return lwip_sock_sendv((sock) ? sock->base.conn : NULL, snips, 0,
                           (struct _sock_tl_ep *)remote, NETCONN_UDP);
}

#ifdef SOCK_HAS_ASYNC
void sock_udp_set_cb(sock_udp_t *sock, sock_udp_cb_t cb, void *arg)
{
    sock->base.async_cb_arg = arg;
    sock->base.async_cb.udp = cb;
}

#ifdef SOCK_HAS_ASYNC_CTX
sock_async_ctx_t *sock_udp_get_async_ctx(sock_udp_t *sock)
{
    return &sock->base.async_ctx;
}
#endif  /* SOCK_HAS_ASYNC_CTX */
#endif  /* SOCK_HAS_ASYNC */

/** @} */
