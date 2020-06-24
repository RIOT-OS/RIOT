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

#include <assert.h>

#include "net/sock/async/msg.h"

void sock_async_msg_handler(msg_t *msg)
{
    sock_async_ctx_t *ctx = msg->content.ptr;

    assert(sock_async_msg_is(msg));
    ctx->cb.generic(ctx->sock, sock_async_msg_flags(msg), ctx->cb_arg);
}

static void _cb(sock_async_ctx_t *ctx, sock_async_flags_t flags, void *arg)
{
    msg_t msg = {
        .type = SOCK_ASYNC_MSG_TYPE_ID | flags,
        .content = { .ptr = ctx },
    };
    ctx->cb_arg = arg;
    msg_try_send(&msg, ctx->pid);
}

#ifdef MODULE_SOCK_DTLS
static void _dtls_cb(sock_dtls_t *sock, sock_async_flags_t flags, void *arg)
{
    _cb(sock_dtls_get_async_ctx(sock), flags, arg);
}

void sock_dtls_msg_init(sock_dtls_t *sock, kernel_pid_t target,
                        sock_dtls_cb_t handler, void *handler_arg)
{
    sock_async_ctx_t *ctx = sock_dtls_get_async_ctx(sock);

    ctx->sock = sock;
    ctx->cb.dtls = handler;
    ctx->pid = target;
    sock_dtls_set_cb(sock, _dtls_cb, handler_arg);
}
#endif /* MODULE_SOCK_DTLS */

#ifdef MODULE_SOCK_IP
static void _ip_cb(sock_ip_t *sock, sock_async_flags_t flags, void *arg)
{
    _cb(sock_ip_get_async_ctx(sock), flags, arg);
}

void sock_ip_msg_init(sock_ip_t *sock, kernel_pid_t target,
                      sock_ip_cb_t handler, void *handler_arg)
{
    sock_async_ctx_t *ctx = sock_ip_get_async_ctx(sock);

    ctx->sock = sock;
    ctx->cb.ip = handler;
    ctx->pid = target;
    sock_ip_set_cb(sock, _ip_cb, handler_arg);
}
#endif  /* MODULE_SOCK_IP */

#ifdef MODULE_SOCK_TCP
static void _tcp_cb(sock_tcp_t *sock, sock_async_flags_t flags, void *arg)
{
    _cb(sock_tcp_get_async_ctx(sock), flags, arg);
}

void sock_tcp_msg_init(sock_tcp_t *sock, kernel_pid_t target,
                       sock_tcp_cb_t handler, void *handler_arg)
{
    sock_async_ctx_t *ctx = sock_tcp_get_async_ctx(sock);

    ctx->sock = sock;
    ctx->cb.tcp = handler;
    ctx->pid = target;
    sock_tcp_set_cb(sock, _tcp_cb, handler_arg);
}

static void _tcp_queue_cb(sock_tcp_queue_t *queue, sock_async_flags_t flags,
                          void *arg)
{
    _cb(sock_tcp_queue_get_async_ctx(queue), flags, arg);
}

void sock_tcp_queue_msg_init(sock_tcp_queue_t *queue, kernel_pid_t target,
                             sock_tcp_queue_cb_t handler, void *handler_arg)
{
    sock_async_ctx_t *ctx = sock_tcp_queue_get_async_ctx(queue);

    ctx->sock = sock;
    ctx->cb.tcp_queue = handler;
    ctx->pid = target;
    sock_tcp_queue_set_cb(queue, _tcp_queue_cb, handler_arg);
}
#endif /* MODULE_SOCK_TCP */

#ifdef MODULE_SOCK_UDP
static void _udp_cb(sock_udp_t *sock, sock_async_flags_t flags, void *arg)
{
    _cb(sock_udp_get_async_ctx(sock), flags, arg);
}

void sock_udp_msg_init(sock_udp_t *sock, kernel_pid_t target,
                       sock_udp_cb_t handler, void *handler_arg)
{
    sock_async_ctx_t *ctx = sock_udp_get_async_ctx(sock);

    ctx->sock = sock;
    ctx->cb.udp = handler;
    ctx->pid = target;
    sock_udp_set_cb(sock, _udp_cb, handler_arg);
}
#endif /* MODULE_SOCK_UDP */

/** @} */
