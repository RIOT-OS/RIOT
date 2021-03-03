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
#include "net/sock/udp.h"
#include "timex.h"

#include "lwip/api.h"
#include "lwip/opt.h"
#include "lwip/sock_internal.h"
#include "lwip/sys.h"
#include "lwip/udp.h"

#include "ringbuffer.h"

#define RECV_BUF_SIZE 32

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
#if IS_ACTIVE(SOCK_HAS_ASYNC)
        netconn_set_callback_arg(sock->base.conn, &sock->base);
#endif

        char *buf_ptr = malloc(RECV_BUF_SIZE * sizeof(char));
        if(buf_ptr == NULL) {
            errno = ENOMEM;
            res = -1;
        }
        else {
            ringbuffer_init(&(sock->recv_rb), buf_ptr, RECV_BUF_SIZE);
        }
    }
    return res;
}

void sock_udp_close(sock_udp_t *sock)
{
    assert(sock != NULL);
    if (sock->base.conn != NULL) {
        netconn_delete(sock->base.conn);
        sock->base.conn = NULL;

        free(sock->recv_rb.buf);
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
    void *ctx = NULL;
    uint8_t *ptr = data;
    ssize_t res, ret = 0;
    bool nobufs = false;

    size_t to_read = max_len;
    uint8_t *buf_index = ptr;

    while (to_read != 0 && !ringbuffer_empty(&(sock->recv_rb))) {
        *buf_index++ = ringbuffer_get_one(&(sock->recv_rb));
        to_read--;
    }

    if (to_read == 0) {
        /* read all bytes from ringbuffer */
        return max_len;
    }
    else {
        /* only a part of the bytes was read from peek buffer, need to get
         * the rest via socket calls
         */
        ptr += max_len - to_read;
        max_len = to_read;
    }

    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    while ((res = sock_udp_recv_buf_aux(sock, &pkt, &ctx, timeout,
                                        remote, aux)) > 0) {
        struct netbuf *buf = ctx;

        if (buf->p->tot_len > (ssize_t)max_len) {

            size_t leftover_count = res - max_len;
            ringbuffer_add(&(sock->recv_rb), (((char*) pkt) + max_len), leftover_count);

            memcpy(ptr, pkt, max_len);
            ptr += max_len;
            ret += max_len;

            //nobufs = true;
            /* progress context to last element */
            // while (netbuf_next(ctx) == 0) {}
            // continue;
        }
        else {
            memcpy(ptr, pkt, res);
            ptr += res;
            ret += res;
        }
    }
    return (nobufs) ? -ENOBUFS : ((res < 0) ? res : ret);
}

ssize_t sock_udp_recv_buf_aux(sock_udp_t *sock, void **data, void **ctx,
                              uint32_t timeout, sock_udp_ep_t *remote,
                              sock_udp_aux_rx_t *aux)
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
    *data = buf->ptr->payload;
    *ctx = buf;
    return (ssize_t)buf->ptr->len;
}

ssize_t sock_udp_send_aux(sock_udp_t *sock, const void *data, size_t len,
                          const sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux)
{
    (void)aux;
    assert((sock != NULL) || (remote != NULL));
    assert((len == 0) || (data != NULL)); /* (len != 0) => (data != NULL) */

    if ((remote != NULL) && (remote->port == 0)) {
        return -EINVAL;
    }
    return lwip_sock_send((sock) ? sock->base.conn : NULL, data, len, 0,
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
