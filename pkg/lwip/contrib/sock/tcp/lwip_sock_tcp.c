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

#include "mutex.h"

#include "net/sock/tcp.h"
#include "timex.h"

#include "lwip/sock_internal.h"
#include "lwip/api.h"
#include "lwip/opt.h"

static inline void _tcp_sock_init(sock_tcp_t *sock, struct netconn *conn,
                                  sock_tcp_queue_t *queue)
{
    mutex_init(&sock->mutex);
    mutex_lock(&sock->mutex);
    sock->base.conn = conn;
    netconn_set_callback_arg(sock->base.conn, &sock->base);
    sock->queue = queue;
    sock->last_buf = NULL;
    sock->last_offset = 0;
    mutex_unlock(&sock->mutex);
}

int sock_tcp_connect(sock_tcp_t *sock, const sock_tcp_ep_t *remote,
                     uint16_t local_port, uint16_t flags)
{
    assert(sock != NULL);
    assert((remote != NULL) && (remote->port != 0));

    int res;
    struct netconn *tmp = NULL;
    struct _sock_tl_ep local = { .family = remote->family,
                                 .netif = remote->netif,
                                 .port = local_port };

    if ((res = lwip_sock_create(&tmp, &local, (struct _sock_tl_ep *)remote, 0,
                                flags, NETCONN_TCP)) == 0) {
        _tcp_sock_init(sock, tmp, NULL);
    }
    return res;
}

int sock_tcp_listen(sock_tcp_queue_t *queue, const sock_tcp_ep_t *local,
                    sock_tcp_t *queue_array, unsigned queue_len,
                    uint16_t flags)
{
    assert(queue != NULL);
    assert((local != NULL) && (local->port != 0));
    assert((queue_array != NULL) && (queue_len != 0));

    int res;
    struct netconn *tmp = NULL;

    if (queue_len > USHRT_MAX) {
        return -EFAULT;
    }
    if ((res = lwip_sock_create(&tmp, (struct _sock_tl_ep *)local, NULL, 0,
                                flags, NETCONN_TCP)) < 0) {
        return res;
    }
    assert(tmp != NULL); /* just in case lwIP is trolling */
    mutex_init(&queue->mutex);
    mutex_lock(&queue->mutex);
    queue->base.conn = tmp;
    netconn_set_callback_arg(queue->base.conn, &queue->base);
    queue->array = queue_array;
    queue->len = queue_len;
    queue->used = 0;
    memset(queue->array, 0, sizeof(sock_tcp_t) * queue_len);
    mutex_unlock(&queue->mutex);
    switch (netconn_listen_with_backlog(queue->base.conn, queue->len)) {
        case ERR_OK:
            break;
        case ERR_MEM:
            return -ENOMEM;
        case ERR_USE:
            return -EADDRINUSE;
        case ERR_VAL:
            return -EINVAL;
        default:
            assert(false); /* should not happen since queue->base.conn is not
                            * closed and we have a TCP conn */
            break;
    }
    return 0;
}

void sock_tcp_disconnect(sock_tcp_t *sock)
{
    assert(sock != NULL);
    mutex_lock(&sock->mutex);
    if (sock->base.conn != NULL) {
        netconn_close(sock->base.conn);
        netconn_delete(sock->base.conn);
        sock->base.conn = NULL;
        /* if sock came from a sock_tcp_queue_t: since sock is a pointer in it's
         * array it is also deleted from there, but we need to decrement the used
         * counter */
        if (sock->queue != NULL) {
            assert(sock->queue->used > 0);
            sock->queue->used--;
            sock->queue = NULL;
        }
    }
    mutex_unlock(&sock->mutex);
    memset(&sock->mutex, 0, sizeof(mutex_t));
}

void sock_tcp_stop_listen(sock_tcp_queue_t *queue)
{
    assert(queue != NULL);
    mutex_lock(&queue->mutex);
    if (queue->base.conn != NULL) {
        netconn_close(queue->base.conn);
        netconn_delete(queue->base.conn);
        queue->base.conn = NULL;
        /* sever connections established through this queue */
        for (unsigned i = 0; i < queue->len; i++) {
            sock_tcp_disconnect(&queue->array[i]);
        }
        queue->array = NULL;
        queue->len = 0;
        queue->used = 0;
    }
    mutex_unlock(&queue->mutex);
    memset(&queue->mutex, 0, sizeof(mutex_t));
}

int sock_tcp_get_local(sock_tcp_t *sock, sock_tcp_ep_t *ep)
{
    int res = 0;
    assert(sock != NULL);
    mutex_lock(&sock->mutex);
    if ((sock->base.conn == NULL) || lwip_sock_get_addr(sock->base.conn,
                                                   (struct _sock_tl_ep *)ep,
                                                   1)) {
        res = -EADDRNOTAVAIL;
    }
    mutex_unlock(&sock->mutex);
    return res;
}

int sock_tcp_get_remote(sock_tcp_t *sock, sock_tcp_ep_t *ep)
{
    int res = 0;
    assert(sock != NULL);
    mutex_lock(&sock->mutex);
    if ((sock->base.conn == NULL) || lwip_sock_get_addr(sock->base.conn,
                                                   (struct _sock_tl_ep *)ep,
                                                   0)) {
        res = -ENOTCONN;
    }
    mutex_unlock(&sock->mutex);
    return res;
}

int sock_tcp_queue_get_local(sock_tcp_queue_t *queue, sock_tcp_ep_t *ep)
{
    int res = 0;

    assert(queue != NULL);
    mutex_lock(&queue->mutex);
    if ((queue->base.conn == NULL) || lwip_sock_get_addr(queue->base.conn,
                                                    (struct _sock_tl_ep *)ep,
                                                    1)) {
        res = -EADDRNOTAVAIL;
    }
    mutex_unlock(&queue->mutex);
    return res;
}

int sock_tcp_accept(sock_tcp_queue_t *queue, sock_tcp_t **sock,
                    uint32_t timeout)
{
    struct netconn *tmp = NULL;
    int res = 0;

    assert((queue != NULL) && (sock != NULL));
    if (queue->base.conn == NULL) {
        return -EINVAL;
    }
    if (timeout == 0) {
        if (!mutex_trylock(&queue->mutex)) {
            return -EAGAIN;
        }
    }
    else if (timeout != 0) {
        mutex_lock(&queue->mutex);
    }
    if (queue->used < queue->len) {
#if LWIP_SO_RCVTIMEO
        if ((timeout != 0) && (timeout != SOCK_NO_TIMEOUT)) {
            netconn_set_recvtimeout(queue->base.conn, timeout / US_PER_MS);
        }
        else
#endif
        if ((timeout == 0) && !mbox_avail(&queue->base.conn->acceptmbox.mbox)) {
            mutex_unlock(&queue->mutex);
            return -EAGAIN;
        }
        switch (netconn_accept(queue->base.conn, &tmp)) {
            case ERR_OK:
                for (unsigned short i = 0; i < queue->len; i++) {
                    sock_tcp_t *s = &queue->array[i];
                    if (s->base.conn == NULL) {
                        _tcp_sock_init(s, tmp, queue);
                        queue->used++;
                        assert(queue->used > 0);
                        *sock = s;
                        break;
                    }
                }
                break;
            case ERR_ABRT:
                res = -ECONNABORTED;
                break;
            case ERR_MEM:
                res = -ENOMEM;
                break;
#if LWIP_SO_RCVTIMEO
            case ERR_TIMEOUT:
                res = -ETIMEDOUT;
                break;
#endif
            default:
                assert(false);
                res = -1;
                break;
        }
    }
    else {
        while (mbox_avail(&queue->base.conn->acceptmbox.mbox)) {
            /* close connections potentially accepted by lwIP */
            if (netconn_accept(queue->base.conn, &tmp) == ERR_OK) {
                netconn_close(tmp);
                netconn_delete(tmp);
            }
        }
        res = -ENOMEM;
    }
#if LWIP_SO_RCVTIMEO
    netconn_set_recvtimeout(queue->base.conn, 0);
#endif
#if IS_ACTIVE(SOCK_HAS_ASYNC)
    if (queue->base.async_cb.gen &&
        mbox_avail(&queue->base.conn->acceptmbox.mbox)) {
        queue->base.async_cb.gen(&queue->base, SOCK_ASYNC_CONN_RECV,
                                 queue->base.async_cb_arg);
    }
#endif
    mutex_unlock(&queue->mutex);
    return res;
}

ssize_t sock_tcp_read(sock_tcp_t *sock, void *data, size_t max_len,
                      uint32_t timeout)
{
    uint8_t *data_ptr = data;
    struct pbuf *buf;
    ssize_t offset = 0, res = 0;
    bool done = false;

    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    if (sock->base.conn == NULL) {
        return -ENOTCONN;
    }
    if (timeout == 0) {
        if (!mutex_trylock(&sock->mutex)) {
            return -EAGAIN;
        }
    }
    else {
        mutex_lock(&sock->mutex);
    }
#if LWIP_SO_RCVTIMEO
    if ((timeout != 0) && (timeout != SOCK_NO_TIMEOUT)) {
        netconn_set_recvtimeout(sock->base.conn, timeout / US_PER_MS);
    }
    else
#endif
    if ((timeout == 0) && !mbox_avail(&sock->base.conn->recvmbox.mbox)) {
        mutex_unlock(&sock->mutex);
        return -EAGAIN;
    }
    while (!done) {
        uint16_t copylen, buf_len;
        if (sock->last_buf != NULL) {
            buf = sock->last_buf;
        }
        else {
            err_t err;
            if ((err = netconn_recv_tcp_pbuf(sock->base.conn, &buf)) < 0) {
                switch (err) {
                    case ERR_ABRT:
                        res = -ECONNABORTED;
                        break;
                    case ERR_CONN:
                        res = -EADDRNOTAVAIL;
                        break;
                    case ERR_RST:
                    case ERR_CLSD:
                        res = -ECONNRESET;
                        break;
                    case ERR_MEM:
                        res = -ENOMEM;
                        break;
#if LWIP_SO_RCVTIMEO
                    case ERR_TIMEOUT:
                        res = -ETIMEDOUT;
                        break;
#endif
                    default:
                        /* no applicable error */
                        res = -1;
                        break;
                }
                break;
            }
            sock->last_buf = buf;
        }
        buf_len = buf->tot_len - sock->last_offset;
        copylen = (buf_len > max_len) ? (uint16_t)max_len : buf_len;
        pbuf_copy_partial(buf, data_ptr + offset, copylen, sock->last_offset);
        offset += copylen;
        max_len -= copylen; /* should be 0 at minimum due to copylen setting above */
        if (max_len == 0) {
            done = true;
            res = offset;   /* in case offset == 0 */
        }
        /* post-process buf */
        if (buf_len > copylen) {
            /* there is still data in the buffer */
            sock->last_buf = buf;
            sock->last_offset += copylen;
        }
        else {
            sock->last_buf = NULL;
            sock->last_offset = 0;
            pbuf_free(buf);
            break;
        }
    }
    if (offset > 0) {
        res = offset;   /* we received data so return it */
    }
    /* unset flags */
#if LWIP_SO_RCVTIMEO
    netconn_set_recvtimeout(sock->base.conn, 0);
#endif
    netconn_set_nonblocking(sock->base.conn, false);
    mutex_unlock(&sock->mutex);
    return res;
}

ssize_t sock_tcp_write(sock_tcp_t *sock, const void *data, size_t len)
{
    struct netconn *conn;
    int res = 0;

    assert(sock != NULL);
    assert((len == 0) || (data != NULL)); /* (len != 0) => (data != NULL) */
    mutex_lock(&sock->mutex);
    if (sock->base.conn == NULL) {
        mutex_unlock(&sock->mutex);
        return -ENOTCONN;
    }
    conn = sock->base.conn;
    mutex_unlock(&sock->mutex); /* we won't change anything to sock here
                                   (lwip_sock_send neither, since it remote is
                                   NULL) so we can leave the mutex */
    res = lwip_sock_send(conn, data, len, 0, NULL, NETCONN_TCP);
    return res;
}

#ifdef SOCK_HAS_ASYNC
void sock_tcp_set_cb(sock_tcp_t *sock, sock_tcp_cb_t cb, void *arg)
{
    sock->base.async_cb_arg = arg;
    sock->base.async_cb.tcp = cb;
}

void sock_tcp_queue_set_cb(sock_tcp_queue_t *queue, sock_tcp_queue_cb_t cb,
                           void *arg)
{
    queue->base.async_cb_arg = arg;
    queue->base.async_cb.tcp_queue = cb;
}

#ifdef SOCK_HAS_ASYNC_CTX
sock_async_ctx_t *sock_tcp_get_async_ctx(sock_tcp_t *sock)
{
    return &sock->base.async_ctx;
}

sock_async_ctx_t *sock_tcp_queue_get_async_ctx(sock_tcp_queue_t *queue)
{
    return &queue->base.async_ctx;
}
#endif  /* SOCK_HAS_ASYNC_CTX */
#endif  /* SOCK_HAS_ASYNC */

/** @} */
