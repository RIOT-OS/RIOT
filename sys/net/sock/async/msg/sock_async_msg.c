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

#include "net/sock/async/msg.h"

static void _cb(void *sock, sock_async_flags_t type, kernel_pid_t *pid_ptr)
{
    msg_t msg = {
        .type = SOCK_ASYNC_MSG_TYPE_MASK | type,
        .content = { .ptr = sock },
    };
    msg_try_send(&msg, *pid_ptr);
}

#ifdef MODULE_SOCK_DTLS
static void _dtls_cb(sock_dtls_t *sock, sock_async_flags_t type)
{
    _cb(sock, type, (kernel_pid_t *)sock_dtls_get_async_ctx(sock));
}

void sock_dtls_msg_init(sock_dtls_t *sock, kernel_pid_t target)
{
    kernel_pid_t *target_ptr = (kernel_pid_t *)sock_dtls_get_async_ctx(sock);

    *target_ptr = target;
    sock_dtls_set_cb(sock, _dtls_cb);
}
#endif /* MODULE_SOCK_DTLS */

#ifdef MODULE_SOCK_IP
static void _ip_cb(sock_ip_t *sock, sock_async_flags_t type)
{
    _cb(sock, type, (kernel_pid_t *)sock_ip_get_async_ctx(sock));
}

void sock_ip_msg_init(sock_ip_t *sock, kernel_pid_t target)
{
    kernel_pid_t *target_ptr = (kernel_pid_t *)sock_ip_get_async_ctx(sock);

    *target_ptr = target;
    sock_ip_set_cb(sock, _ip_cb);
}
#endif  /* MODULE_SOCK_IP */

#ifdef MODULE_SOCK_TCP
static void _tcp_cb(sock_tcp_t *sock, sock_async_flags_t type)
{
    _cb(sock, type, (kernel_pid_t *)sock_tcp_get_async_ctx(sock));
}

void sock_tcp_msg_init(sock_tcp_t *sock, kernel_pid_t target)
{
    kernel_pid_t *target_ptr = (kernel_pid_t *)sock_tcp_get_async_ctx(sock);

    *target_ptr = target;
    sock_tcp_set_cb(sock, _tcp_cb);
}

static void _tcp_queue_cb(sock_tcp_queue_t *queue, sock_async_flags_t type)
{
    _cb(queue, type, (kernel_pid_t *)sock_tcp_queue_get_async_ctx(queue));
}

void sock_tcp_queue_msg_init(sock_tcp_queue_t *queue, kernel_pid_t target)
{
    kernel_pid_t *target_ptr = (kernel_pid_t *)sock_tcp_queue_get_async_ctx(queue);

    *target_ptr = target;
    sock_tcp_queue_set_cb(queue, _tcp_queue_cb);
}
#endif /* MODULE_SOCK_TCP */

#ifdef MODULE_SOCK_UDP
static void _udp_cb(sock_udp_t *sock, sock_async_flags_t type)
{
    _cb(sock, type, (kernel_pid_t *)sock_udp_get_async_ctx(sock));
}

void sock_udp_msg_init(sock_udp_t *sock, kernel_pid_t target)
{
    kernel_pid_t *target_ptr = (kernel_pid_t *)sock_udp_get_async_ctx(sock);

    *target_ptr = target;
    sock_udp_set_cb(sock, _udp_cb);
}
#endif /* MODULE_SOCK_UDP */

/** @} */
