/*
 * Copyright (C) 2019 Freie Universität Berlin
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

#include "irq.h"
#include "net/sock/async/event.h"

#ifdef MODULE_FUZZING
extern gnrc_pktsnip_t *gnrc_pktbuf_fuzzptr;
extern gnrc_pktsnip_t *gnrc_sock_prevpkt;
#endif

static void _event_handler(event_t *ev)
{
    sock_event_t *event = (sock_event_t *)ev;
    unsigned state = irq_disable();
    sock_async_flags_t _type = event->type;

    event->type = 0;
    irq_restore(state);
    if (_type) {
        event->cb.generic(event->sock, _type, event->cb_arg);
    }
}

static inline void _cb(void *sock, sock_async_flags_t type, void *arg,
                       sock_async_ctx_t *ctx)
{
    ctx->event.sock = sock;
    ctx->event.cb_arg = arg;
    ctx->event.type |= type;
    event_post(ctx->queue, &ctx->event.super);

    /* The fuzzing module is only enabled when building a fuzzing
     * application from the fuzzing/ subdirectory. The fuzzing setup
     * assumes that gnrc_sock_recv is called by the event callback. If
     * the value returned by gnrc_sock_recv was the fuzzing packet, the
     * fuzzing application is terminated as input processing finished. */
#ifdef MODULE_FUZZING
    if (gnrc_sock_prevpkt && gnrc_sock_prevpkt == gnrc_pktbuf_fuzzptr) {
        exit(EXIT_SUCCESS);
    }
#endif
}

static void _set_ctx(sock_async_ctx_t *ctx, event_queue_t *ev_queue)
{
    ctx->event.type = 0;
    ctx->event.super.list_node.next = NULL;
    ctx->event.super.handler = _event_handler;
    ctx->queue = ev_queue;
}

void sock_event_close(sock_async_ctx_t *async_ctx)
{
    event_queue_t *queue = async_ctx->queue;
    if (!queue) {
        /* no callback registered */
        return;
    }

    /* RIOTs socket API is not thread safe so we assume that wherever this is
     * called from it's not racing against some other socket usage.
     *
     * But we have to go stricter and assume this is called from the same thread
     * that processes the events. There is no way of preventing the networking
     * subsystem from posting socket events until the socket has been
     * unregistered, which is only happening when closing the socket and the
     * reason we're here in the first place. */
    assume(!queue->waiter || thread_getpid_of(queue->waiter) == thread_getpid());

    event_cancel(async_ctx->queue, &async_ctx->event.super);
}

typedef enum {
#if defined(MODULE_SOCK_UDP) || defined(DOXYGEN)
    SOCK_ASYNC_UDP,
#endif
#if defined(MODULE_SOCK_TCP) || defined(DOXYGEN)
    SOCK_ASYNC_TCP,
#endif
#if defined(MODULE_SOCK_IP) || defined(DOXYGEN)
    SOCK_ASYNC_IP,
#endif
#if defined(MODULE_SOCK_DTLS) || defined(DOXYGEN)
    SOCK_ASYNC_DTLS,
#endif
} sock_async_t;

typedef struct {
    event_t super;
    sock_async_t type;
    void *sock;
} _sock_close_ev;

static void _sock_event_close_cb(event_t *ev)
{
    _sock_close_ev *close_ev = container_of(ev, _sock_close_ev, super);

    switch (close_ev->type) {
#if defined(MODULE_SOCK_UDP) || defined(DOXYGEN)
        case SOCK_ASYNC_UDP:
        sock_udp_close(close_ev->sock);
        break;
#endif
#if defined(MODULE_SOCK_TCP) || defined(DOXYGEN)
        case SOCK_ASYNC_TCP:
        sock_tcp_disconnect(close_ev->sock);
        break;
#endif
#if defined(MODULE_SOCK_IP) || defined(DOXYGEN)
        case SOCK_ASYNC_IP:
        sock_ip_close(close_ev->sock);
        break;
#endif
#if defined(MODULE_SOCK_DTLS) || defined(DOXYGEN)
        case SOCK_ASYNC_DTLS:
        sock_dtls_close(close_ev->sock);
        break;
#endif
    }
}

static void _sock_event_close_common(void *sock, sock_async_ctx_t *async_ctx,
                                     sock_async_t type)
{
    _sock_close_ev ev = {
        .super.handler = _sock_event_close_cb,
        .type = type,
        .sock = sock,
    };

    if (!async_ctx->queue ||
        !async_ctx->queue->waiter ||
        thread_getpid_of(async_ctx->queue->waiter) == thread_getpid()) {
        /* - this socket is not async OR
         * - there is no thread processing the event queue (in which case we
         *   might race against @ref event_queue_claim() but so is life)
         *   OR we are on the event queue thread. In the first case we might
         *   block forever, in the second we surely will, so do it now. */
        _sock_event_close_cb(&ev.super);
        return;
    }

    event_post(async_ctx->queue, &ev.super);
    event_sync(async_ctx->queue);
}
#ifdef MODULE_SOCK_DTLS
static void _dtls_cb(sock_dtls_t *sock, sock_async_flags_t type, void *arg)
{
    _cb(sock, type, arg, sock_dtls_get_async_ctx(sock));
}

void sock_dtls_event_init(sock_dtls_t *sock, event_queue_t *ev_queue,
                         sock_dtls_cb_t handler, void *handler_arg)
{
    sock_async_ctx_t *ctx = sock_dtls_get_async_ctx(sock);

    _set_ctx(ctx, ev_queue);
    ctx->event.cb.dtls = handler;
    sock_dtls_set_cb(sock, _dtls_cb, handler_arg);
}

void sock_dtls_event_close(sock_dtls_t *sock)
{
    _sock_event_close_common(sock, sock_dtls_get_async_ctx(sock), SOCK_ASYNC_DTLS);
}
#endif /* MODULE_SOCK_DTLS */

#ifdef MODULE_SOCK_IP
static void _ip_cb(sock_ip_t *sock, sock_async_flags_t type, void *arg)
{
    _cb(sock, type, arg, sock_ip_get_async_ctx(sock));
}

void sock_ip_event_init(sock_ip_t *sock, event_queue_t *ev_queue,
                        sock_ip_cb_t handler, void *handler_arg)
{
    sock_async_ctx_t *ctx = sock_ip_get_async_ctx(sock);

    _set_ctx(ctx, ev_queue);
    ctx->event.cb.ip = handler;
    sock_ip_set_cb(sock, _ip_cb, handler_arg);
}

void sock_ip_event_close(sock_ip_t *sock)
{
    _sock_event_close_common(sock, sock_ip_get_async_ctx(sock), SOCK_ASYNC_IP);
}
#endif  /* MODULE_SOCK_IP */

#ifdef MODULE_SOCK_TCP
static void _tcp_cb(sock_tcp_t *sock, sock_async_flags_t type, void *arg)
{
    _cb(sock, type, arg, sock_tcp_get_async_ctx(sock));
}

void sock_tcp_event_init(sock_tcp_t *sock, event_queue_t *ev_queue,
                         sock_tcp_cb_t handler, void *handler_arg)
{
    sock_async_ctx_t *ctx = sock_tcp_get_async_ctx(sock);

    _set_ctx(ctx, ev_queue);
    ctx->event.cb.tcp = handler;
    sock_tcp_set_cb(sock, _tcp_cb, handler_arg);
}

static void _tcp_queue_cb(sock_tcp_queue_t *queue, sock_async_flags_t type,
                          void *arg)
{
    _cb(queue, type, arg, sock_tcp_queue_get_async_ctx(queue));
}

void sock_tcp_queue_event_init(sock_tcp_queue_t *queue, event_queue_t *ev_queue,
                               sock_tcp_queue_cb_t handler, void *handler_arg)
{
    sock_async_ctx_t *ctx = sock_tcp_queue_get_async_ctx(queue);

    _set_ctx(ctx, ev_queue);
    ctx->event.cb.tcp_queue = handler;
    sock_tcp_queue_set_cb(queue, _tcp_queue_cb, handler_arg);
}

void sock_tcp_event_close(sock_tcp_t *sock)
{
    _sock_event_close_common(sock, sock_tcp_get_async_ctx(sock), SOCK_ASYNC_TCP);
}
#endif /* MODULE_SOCK_TCP */

#ifdef MODULE_SOCK_UDP
static void _udp_cb(sock_udp_t *sock, sock_async_flags_t type, void *arg)
{
    _cb(sock, type, arg, sock_udp_get_async_ctx(sock));
}

void sock_udp_event_init(sock_udp_t *sock, event_queue_t *ev_queue,
                         sock_udp_cb_t handler, void *handler_arg)
{
    sock_async_ctx_t *ctx = sock_udp_get_async_ctx(sock);

    _set_ctx(ctx, ev_queue);
    ctx->event.cb.udp = handler;
    sock_udp_set_cb(sock, _udp_cb, handler_arg);
}

void sock_udp_event_close(sock_udp_t *sock)
{
    _sock_event_close_common(sock, sock_udp_get_async_ctx(sock), SOCK_ASYNC_UDP);
}

#endif /* MODULE_SOCK_UDP */

/** @} */
