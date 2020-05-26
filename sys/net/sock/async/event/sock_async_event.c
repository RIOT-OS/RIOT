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
#endif /* MODULE_SOCK_UDP */

/** @} */
