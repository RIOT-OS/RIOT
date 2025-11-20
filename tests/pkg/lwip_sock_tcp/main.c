/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test for UDP socks
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/uio.h>

#include "net/ipv6/addr.h"
#include "net/sock/tcp.h"
#include "sched.h"
#include "test_utils/expect.h"
#include "thread.h"
#include "ztimer.h"
#include "timex.h"

#include "constants.h"
#include "stack.h"

#define _TEST_BUFFER_SIZE   (128)
#define _QUEUE_SIZE         (1)

#define _MSG_QUEUE_SIZE     (4)
#define _CLIENT_BUF_SIZE    (128)
#define _SERVER_BUF_SIZE    (128)
#define _SERVER_QUEUE_SIZE  (1)
#define _CLIENT_MSG_START   (0xe307)
#define _CLIENT_MSG_READ    (0xe308)
#define _CLIENT_MSG_WRITE   (0xe309)
#define _CLIENT_MSG_STOP    (0xe30a)
#define _SERVER_MSG_START   (0xe30b)
#define _SERVER_MSG_ACCEPT  (0xe30c)
#define _SERVER_MSG_READ    (0xe30d)
#define _SERVER_MSG_WRITE   (0xe30e)
#define _SERVER_MSG_CLOSE   (0xe30f)
#define _SERVER_MSG_STOP    (0xe310)

static uint8_t _test_buffer[_TEST_BUFFER_SIZE];

static char _client_stack[THREAD_STACKSIZE_DEFAULT];
static char _server_stack[THREAD_STACKSIZE_DEFAULT];
static uint8_t _client_buf[_CLIENT_BUF_SIZE];
static uint8_t _server_buf[_SERVER_BUF_SIZE];
static msg_t _client_msg_queue[_MSG_QUEUE_SIZE];
static msg_t _server_msg_queue[_MSG_QUEUE_SIZE];
static sock_tcp_t _sock, _client_sock;
static sock_tcp_t _queue_array[_QUEUE_SIZE];
static sock_tcp_t _server_queue_array[_SERVER_QUEUE_SIZE];
static sock_tcp_queue_t _queue, _server_queue;
static sock_tcp_ep_t _server_addr;
static kernel_pid_t _server, _client;

#define CALL(fn)            puts("Calling " # fn); fn; tear_down()

static void *_server_func(void *arg);
static void *_client_func(void *arg);

static void tear_down(void)
{
    msg_t msg = { .type = _CLIENT_MSG_STOP };
    msg_send(&msg, _client);
    msg.type = _SERVER_MSG_STOP;
    msg_send(&msg, _server);
    sock_tcp_disconnect(&_sock);
    sock_tcp_stop_listen(&_queue);
    memset(&_sock, 0, sizeof(_sock));
    memset(&_queue, 0, sizeof(_queue));
    memset(&_server_addr, 0, sizeof(_server_addr));
}

#ifdef MODULE_LWIP_IPV4
#ifdef SO_REUSE
static void test_tcp_connect4__EADDRINUSE(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    static const uint16_t local_port = _TEST_PORT_REMOTE;

    _server_addr.family = AF_INET;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);    /* start server on _TEST_PORT_REMOTE */

    expect(-EADDRINUSE == sock_tcp_connect(&_sock, &remote, local_port, 0));
}
#endif

static void test_tcp_connect4__EAFNOSUPPORT(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };

    expect(-EAFNOSUPPORT == sock_tcp_connect(&_sock, &remote, 0,
                                             SOCK_FLAGS_REUSE_EP));
}

/* ECONNREFUSED does not apply for lwIP; netconn_connect does not wait for
 * connection build-up */

static void test_tcp_connect4__EINVAL_addr(void)
{
    static const sock_tcp_ep_t remote = { .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };

    expect(-EINVAL == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
}

static void test_tcp_connect4__EINVAL_netif(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = (_TEST_NETIF + 1) };

    expect(-EINVAL == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
}

/* ENETUNREACH not testable in given loopback setup */
/* ETIMEDOUT doesn't work because lwIP's connect doesn't timeout */

static void test_tcp_connect4__success_without_port(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = _TEST_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    sock_tcp_ep_t ep;

    _server_addr.family = AF_INET;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);    /* start server on _TEST_PORT_REMOTE */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_tcp_get_remote(&_sock, &ep));
    expect(AF_INET == ep.family);
    expect(_TEST_ADDR4_REMOTE == ep.addr.ipv4_u32);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(_TEST_PORT_REMOTE == ep.port);
}
static void test_tcp_connect4__success_local_port(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    static const uint16_t local_port = _TEST_PORT_LOCAL;
    sock_tcp_ep_t ep;

    _server_addr.family = AF_INET;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);    /* start server on _TEST_PORT_REMOTE */

    expect(0 == sock_tcp_connect(&_sock, &remote, local_port, SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_tcp_get_local(&_sock, &ep));
    expect(AF_INET == ep.family);
    expect(_TEST_PORT_LOCAL == ep.port);
    expect(0 == sock_tcp_get_remote(&_sock, &ep));
    expect(AF_INET == ep.family);
    expect(_TEST_ADDR4_REMOTE == ep.addr.ipv4_u32);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(_TEST_PORT_REMOTE == ep.port);
}

#ifdef SO_REUSE
static void test_tcp_listen4__EADDRINUSE(void)
{
    static const sock_tcp_ep_t local = { .addr = { .ipv4_u32 = _TEST_ADDR4_LOCAL },
                                         .family = AF_INET,
                                         .port = _TEST_PORT_LOCAL,
                                         .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };

    _server_addr.family = AF_INET;
    _server_addr.port = _TEST_PORT_LOCAL;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);    /* start server on _TEST_PORT_LOCAL */

    expect(-EADDRINUSE == sock_tcp_listen(&_queue, &local, _queue_array,
                                          _QUEUE_SIZE, 0));
}
#endif

static void test_tcp_listen4__EAFNOSUPPORT(void)
{
    static const sock_tcp_ep_t local = { .addr = { .ipv4_u32 = _TEST_ADDR4_LOCAL },
                                         .port = _TEST_PORT_LOCAL,
                                         .netif = SOCK_ADDR_ANY_NETIF };

    expect(-EAFNOSUPPORT == sock_tcp_listen(&_queue, &local, _queue_array,
                                            _QUEUE_SIZE, 0));
}

static void test_tcp_listen4__EINVAL(void)
{
    static const sock_tcp_ep_t local = { .addr = { .ipv4_u32 = _TEST_ADDR4_LOCAL },
                                         .family = AF_INET,
                                         .port = _TEST_PORT_LOCAL,
                                         .netif = (_TEST_NETIF + 1) };

    expect(-EINVAL == sock_tcp_listen(&_queue, &local, _queue_array,
                                      _QUEUE_SIZE, 0));
}

static void test_tcp_listen4__success_any_netif(void)
{
    static const sock_tcp_ep_t local = { .addr = { .ipv4_u32 = _TEST_ADDR4_LOCAL },
                                         .family = AF_INET,
                                         .port = _TEST_PORT_LOCAL,
                                         .netif = SOCK_ADDR_ANY_NETIF };
    sock_tcp_ep_t ep;

    expect(0 == sock_tcp_listen(&_queue, &local, _queue_array,
                                _QUEUE_SIZE, 0));
    expect(0 == sock_tcp_queue_get_local(&_queue, &ep));
    expect(AF_INET == ep.family);
    expect(_TEST_ADDR4_LOCAL == ep.addr.ipv4_u32);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(_TEST_PORT_LOCAL == ep.port);
}

static void test_tcp_listen4__success_spec_netif(void)
{
    static const sock_tcp_ep_t local = { .family = AF_INET,
                                         .port = _TEST_PORT_LOCAL,
                                         .netif = _TEST_NETIF };
    sock_tcp_ep_t ep;

    expect(0 == sock_tcp_listen(&_queue, &local, _queue_array,
                                _QUEUE_SIZE, 0));
    expect(0 == sock_tcp_queue_get_local(&_queue, &ep));
    expect(AF_INET == ep.family);
    expect(_TEST_NETIF == ep.netif);
    expect(_TEST_PORT_LOCAL == ep.port);
}

/* ECONNABORTED can't be tested in this setup */

static void test_tcp_accept4__EAGAIN(void)
{
    static const sock_tcp_ep_t local = { .family = AF_INET,
                                         .port = _TEST_PORT_LOCAL };
    sock_tcp_t *sock;

    expect(0 == sock_tcp_listen(&_queue, &local, _queue_array,
                                _QUEUE_SIZE, SOCK_FLAGS_REUSE_EP));
    expect(-EAGAIN == sock_tcp_accept(&_queue, &sock, 0));
}

static void test_tcp_accept4__EINVAL(void)
{
    sock_tcp_t *sock;

    expect(-EINVAL == sock_tcp_accept(&_queue, &sock, SOCK_NO_TIMEOUT));
}

static void test_tcp_accept4__ETIMEDOUT(void)
{
    static const sock_tcp_ep_t local = { .family = AF_INET,
                                         .port = _TEST_PORT_LOCAL };
    sock_tcp_t *sock;

    expect(0 == sock_tcp_listen(&_queue, &local, _queue_array,
                                _QUEUE_SIZE, SOCK_FLAGS_REUSE_EP));
    puts(" * Calling sock_tcp_accept()");
    expect(-ETIMEDOUT == sock_tcp_accept(&_queue, &sock, _TEST_TIMEOUT));
    printf(" * (timed out with timeout %u)\n", _TEST_TIMEOUT);
}

static void test_tcp_accept4__success(void)
{
    static const sock_tcp_ep_t local = { .family = AF_INET,
                                         .port = _TEST_PORT_LOCAL };
    msg_t msg = { .type = _CLIENT_MSG_START,
                  .content = { .value = _TEST_PORT_REMOTE } };
    sock_tcp_ep_t ep;
    sock_tcp_t *sock;

    _server_addr.addr.ipv4_u32 = _TEST_ADDR4_REMOTE;  /* loopback */
    _server_addr.family = AF_INET;
    _server_addr.port = _TEST_PORT_LOCAL;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    expect(0 == sock_tcp_listen(&_queue, &local, _queue_array,
                                _QUEUE_SIZE, 0));
    msg_send(&msg, _client);    /* start client on _TEST_PORT_REMOTE, connecting
                                 * to _TEST_PORT_LOCAL */
    expect(0 == sock_tcp_accept(&_queue, &sock, SOCK_NO_TIMEOUT));
    expect(0 == sock_tcp_get_local(sock, &ep));
    expect(AF_INET == ep.family);
    expect(_TEST_PORT_LOCAL == ep.port);
    expect(0 == sock_tcp_get_remote(sock, &ep));
    expect(AF_INET == ep.family);
    expect(_TEST_ADDR4_REMOTE == ep.addr.ipv4_u32);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(_TEST_PORT_REMOTE == ep.port);
}

/* ECONNABORTED can't be tested in this setup */

static void test_tcp_read4__EAGAIN(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };

    _server_addr.family = AF_INET;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    expect(-EAGAIN == sock_tcp_read(&_sock, _test_buffer, sizeof(_test_buffer), 0));
}

static void test_tcp_read4__ECONNRESET(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };

    _server_addr.family = AF_INET;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    msg.type = _SERVER_MSG_CLOSE;
    msg_send(&msg, _server);        /* close connection at server side */
    expect(-ECONNRESET == sock_tcp_read(&_sock, _test_buffer,
                                        sizeof(_test_buffer),
                                        SOCK_NO_TIMEOUT));
}

static void test_tcp_read4__ENOTCONN(void)
{
    expect(-ENOTCONN == sock_tcp_read(&_sock, _test_buffer,
                                      sizeof(_test_buffer), SOCK_NO_TIMEOUT));
}

static void test_tcp_read4__ETIMEDOUT(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };

    _server_addr.family = AF_INET;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    puts(" * Calling sock_tcp_read()");
    expect(-ETIMEDOUT == sock_tcp_read(&_sock, _test_buffer, sizeof(_test_buffer),
                                       _TEST_TIMEOUT));
    printf(" * (timed out with timeout %u)\n", _TEST_TIMEOUT);
}

static void test_tcp_read4__success(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    static const struct iovec exp_data = { .iov_base = "Hello!",
                                           .iov_len = sizeof("Hello!") };

    _server_addr.family = AF_INET;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    msg.type = _SERVER_MSG_WRITE;
    msg.content.ptr = (void *)&exp_data;
    msg_send(&msg, _server);        /* write expected data at server */
    expect(((ssize_t)exp_data.iov_len) == sock_tcp_read(&_sock, _test_buffer,
                                                        sizeof(_test_buffer),
                                                        SOCK_NO_TIMEOUT));
    expect(memcmp(exp_data.iov_base, _test_buffer, exp_data.iov_len) == 0);
}

static void test_tcp_read4__success_with_timeout(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    static const struct iovec exp_data = { .iov_base = "Hello!",
                                           .iov_len = sizeof("Hello!") };

    _server_addr.family = AF_INET;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    msg.type = _SERVER_MSG_WRITE;
    msg.content.ptr = (void *)&exp_data;
    msg_send(&msg, _server);        /* write expected data at server */
    expect(((ssize_t)exp_data.iov_len) == sock_tcp_read(&_sock, _test_buffer,
                                                        sizeof(_test_buffer),
                                                        _TEST_TIMEOUT));
    expect(memcmp(exp_data.iov_base, _test_buffer, exp_data.iov_len) == 0);
}

static void test_tcp_read4__success_non_blocking(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    static const struct iovec exp_data = { .iov_base = "Hello!",
                                           .iov_len = sizeof("Hello!") };

    _server_addr.family = AF_INET;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    msg.type = _SERVER_MSG_WRITE;
    msg.content.ptr = (void *)&exp_data;
    msg_send(&msg, _server);        /* write expected data at server */
    expect(((ssize_t)exp_data.iov_len) == sock_tcp_read(&_sock, _test_buffer,
                                                        sizeof(_test_buffer),
                                                        0));
    expect(memcmp(exp_data.iov_base, _test_buffer, exp_data.iov_len) == 0);
}

/* ENOTCONN not applicable since lwIP always tries to send */

static void test_tcp_write4__ENOTCONN(void)
{
    expect(-ENOTCONN == sock_tcp_write(&_sock, "Hello!", sizeof("Hello!")));
}

static void test_tcp_write4__success(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    static const struct iovec exp_data = { .iov_base = "Hello!",
                                           .iov_len = sizeof("Hello!") };

    _server_addr.family = AF_INET;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    msg.type = _SERVER_MSG_READ;
    msg.content.ptr = (void *)&exp_data;
    msg_send(&msg, _server);        /* write expected data at server */
    expect(((ssize_t)exp_data.iov_len) == sock_tcp_write(&_sock, "Hello!",
                                                        sizeof("Hello!")));
    expect(memcmp(exp_data.iov_base, _test_buffer, exp_data.iov_len) == 0);
    ztimer_sleep(ZTIMER_MSEC, 5);            /* wait for server */
}
#endif /* MODULE_LWIP_IPV4 */

#ifdef MODULE_LWIP_IPV6
#ifdef SO_REUSE
static void test_tcp_connect6__EADDRINUSE(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    static const uint16_t local_port = _TEST_PORT_REMOTE;

    _server_addr.family = AF_INET6;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);    /* start server on _TEST_PORT_REMOTE */

    expect(-EADDRINUSE == sock_tcp_connect(&_sock, &remote, local_port, 0));
}
#endif

static void test_tcp_connect6__EAFNOSUPPORT(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };

    expect(-EAFNOSUPPORT == sock_tcp_connect(&_sock, &remote, 0,
                                             SOCK_FLAGS_REUSE_EP));
}

/* ECONNREFUSED does not apply for lwIP; netconn_connect does not wait for
 * connection build-up */

static void test_tcp_connect6__EINVAL_addr(void)
{
    static const sock_tcp_ep_t remote = { .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };

    expect(-EINVAL == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
}

static void test_tcp_connect6__EINVAL_netif(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = (_TEST_NETIF + 1) };

    expect(-EINVAL == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
}

/* ENETUNREACH not testable in given loopback setup */
/* ETIMEDOUT doesn't work because lwIP's connect doesn't timeout */

static void test_tcp_connect6__success_without_port(void)
{
    static const ipv6_addr_t remote_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_tcp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = _TEST_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    sock_tcp_ep_t ep;

    _server_addr.family = AF_INET6;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);    /* start server on _TEST_PORT_REMOTE */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_tcp_get_remote(&_sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&remote_addr, &ep.addr.ipv6, sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(_TEST_PORT_REMOTE == ep.port);
}
static void test_tcp_connect6__success_local_port(void)
{
    static const ipv6_addr_t remote_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_tcp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    static const uint16_t local_port = _TEST_PORT_LOCAL;
    sock_tcp_ep_t ep;

    _server_addr.family = AF_INET6;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);    /* start server on _TEST_PORT_REMOTE */

    expect(0 == sock_tcp_connect(&_sock, &remote, local_port, SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_tcp_get_local(&_sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(_TEST_PORT_LOCAL == ep.port);
    expect(0 == sock_tcp_get_remote(&_sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&remote_addr, &ep.addr.ipv6, sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(_TEST_PORT_REMOTE == ep.port);
}

#ifdef SO_REUSE
static void test_tcp_listen6__EADDRINUSE(void)
{
    static sock_tcp_t queue_array2[_QUEUE_SIZE];
    static sock_tcp_queue_t queue2;
    static const sock_tcp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR6_LOCAL },
                                         .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL,
                                         .netif = SOCK_ADDR_ANY_NETIF };
    expect(0 == sock_tcp_listen(&_queue, &local, _queue_array, _QUEUE_SIZE, 0));
    expect(-EADDRINUSE == sock_tcp_listen(&queue2, &local, queue_array2,
                                          _QUEUE_SIZE, 0));
}
#endif

static void test_tcp_listen6__EAFNOSUPPORT(void)
{
    static const sock_tcp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR6_LOCAL },
                                         .port = _TEST_PORT_LOCAL,
                                         .netif = SOCK_ADDR_ANY_NETIF };

    expect(-EAFNOSUPPORT == sock_tcp_listen(&_queue, &local, _queue_array,
                                            _QUEUE_SIZE, 0));
}

static void test_tcp_listen6__EINVAL(void)
{
    static const sock_tcp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR6_LOCAL },
                                         .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL,
                                         .netif = (_TEST_NETIF + 1) };

    expect(-EINVAL == sock_tcp_listen(&_queue, &local, _queue_array,
                                      _QUEUE_SIZE, 0));
}

static void test_tcp_listen6__success_any_netif(void)
{
    static const ipv6_addr_t local_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const sock_tcp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR6_LOCAL },
                                         .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL,
                                         .netif = SOCK_ADDR_ANY_NETIF };
    sock_tcp_ep_t ep;

    expect(0 == sock_tcp_listen(&_queue, &local, _queue_array,
                                _QUEUE_SIZE, 0));
    expect(0 == sock_tcp_queue_get_local(&_queue, &ep));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&local_addr, &ep.addr.ipv6, sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(_TEST_PORT_LOCAL == ep.port);
}

static void test_tcp_listen6__success_spec_netif(void)
{
    static const sock_tcp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL,
                                         .netif = _TEST_NETIF };
    sock_tcp_ep_t ep;

    expect(0 == sock_tcp_listen(&_queue, &local, _queue_array,
                                _QUEUE_SIZE, 0));
    expect(0 == sock_tcp_queue_get_local(&_queue, &ep));
    expect(AF_INET6 == ep.family);
    expect(_TEST_NETIF == ep.netif);
    expect(_TEST_PORT_LOCAL == ep.port);
}

/* ECONNABORTED can't be tested in this setup */

static void test_tcp_accept6__EAGAIN(void)
{
    static const sock_tcp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    sock_tcp_t *sock;

    expect(0 == sock_tcp_listen(&_queue, &local, _queue_array,
                                _QUEUE_SIZE, 0));
    expect(-EAGAIN == sock_tcp_accept(&_queue, &sock, 0));
}

static void test_tcp_accept6__EINVAL(void)
{
    sock_tcp_t *sock;

    expect(-EINVAL == sock_tcp_accept(&_queue, &sock, SOCK_NO_TIMEOUT));
}

static void test_tcp_accept6__ETIMEDOUT(void)
{
    static const sock_tcp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    sock_tcp_t *sock;

    expect(0 == sock_tcp_listen(&_queue, &local, _queue_array,
                                _QUEUE_SIZE, 0));
    puts(" * Calling sock_tcp_accept()");
    expect(-ETIMEDOUT == sock_tcp_accept(&_queue, &sock, _TEST_TIMEOUT));
    printf(" * (timed out with timeout %u)\n", _TEST_TIMEOUT);
}

static void test_tcp_accept6__success(void)
{
    static const ipv6_addr_t remote_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_tcp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    msg_t msg = { .type = _CLIENT_MSG_START,
                  .content = { .value = _TEST_PORT_REMOTE } };
    sock_tcp_ep_t ep;
    sock_tcp_t *sock;

    _server_addr.addr.ipv6[15] = 1; /* make unspecified address to loopback */
    _server_addr.family = AF_INET6;
    _server_addr.port = _TEST_PORT_LOCAL;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    expect(0 == sock_tcp_listen(&_queue, &local, _queue_array,
                                _QUEUE_SIZE, 0));
    msg_send(&msg, _client);    /* start client on _TEST_PORT_REMOTE, connecting
                                 * to _TEST_PORT_LOCAL */
    expect(0 == sock_tcp_accept(&_queue, &sock, SOCK_NO_TIMEOUT));
    expect(0 == sock_tcp_get_local(sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(_TEST_PORT_LOCAL == ep.port);
    expect(0 == sock_tcp_get_remote(sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&remote_addr, &ep.addr.ipv6, sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(_TEST_PORT_REMOTE == ep.port);
}

/* ECONNABORTED can't be tested in this setup */

static void test_tcp_read6__EAGAIN(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };

    _server_addr.family = AF_INET6;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    expect(-EAGAIN == sock_tcp_read(&_sock, _test_buffer, sizeof(_test_buffer), 0));
}

static void test_tcp_read6__ECONNRESET(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };

    _server_addr.family = AF_INET6;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    msg.type = _SERVER_MSG_CLOSE;
    msg_send(&msg, _server);        /* close connection at server side */
    expect(-ECONNRESET == sock_tcp_read(&_sock, _test_buffer,
                                        sizeof(_test_buffer), SOCK_NO_TIMEOUT));
}

static void test_tcp_read6__ENOTCONN(void)
{
    expect(-ENOTCONN == sock_tcp_read(&_sock, _test_buffer,
                                      sizeof(_test_buffer), SOCK_NO_TIMEOUT));
}

static void test_tcp_read6__ETIMEDOUT(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };

    _server_addr.family = AF_INET6;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    puts(" * Calling sock_tcp_read()");
    expect(-ETIMEDOUT == sock_tcp_read(&_sock, _test_buffer,
                                       sizeof(_test_buffer), _TEST_TIMEOUT));
    printf(" * (timed out with timeout %u)\n", _TEST_TIMEOUT);
}
static void test_tcp_read6__success(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    static const struct iovec exp_data = { .iov_base = "Hello!",
                                           .iov_len = sizeof("Hello!") };

    _server_addr.family = AF_INET6;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    msg.type = _SERVER_MSG_WRITE;
    msg.content.ptr = (void *)&exp_data;
    msg_send(&msg, _server);        /* write expected data at server */
    expect(((ssize_t)exp_data.iov_len) == sock_tcp_read(&_sock, _test_buffer,
                                                        sizeof(_test_buffer),
                                                        SOCK_NO_TIMEOUT));
    expect(memcmp(exp_data.iov_base, _test_buffer, exp_data.iov_len) == 0);
}

static void test_tcp_read6__success_with_timeout(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    static const struct iovec exp_data = { .iov_base = "Hello!",
                                           .iov_len = sizeof("Hello!") };

    _server_addr.family = AF_INET6;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    msg.type = _SERVER_MSG_WRITE;
    msg.content.ptr = (void *)&exp_data;
    msg_send(&msg, _server);        /* write expected data at server */
    expect(((ssize_t)exp_data.iov_len) == sock_tcp_read(&_sock, _test_buffer,
                                                        sizeof(_test_buffer),
                                                        _TEST_TIMEOUT));
    expect(memcmp(exp_data.iov_base, _test_buffer, exp_data.iov_len) == 0);
}

static void test_tcp_read6__success_non_blocking(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    static const struct iovec exp_data = { .iov_base = "Hello!",
                                           .iov_len = sizeof("Hello!") };

    _server_addr.family = AF_INET6;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    msg.type = _SERVER_MSG_WRITE;
    msg.content.ptr = (void *)&exp_data;
    msg_send(&msg, _server);        /* write expected data at server */
    expect(((ssize_t)exp_data.iov_len) == sock_tcp_read(&_sock, _test_buffer,
                                                        sizeof(_test_buffer),
                                                        0));
    expect(memcmp(exp_data.iov_base, _test_buffer, exp_data.iov_len) == 0);
}

/* ENOTCONN not applicable since lwIP always tries to send */

static void test_tcp_write6__ENOTCONN(void)
{
    expect(-ENOTCONN == sock_tcp_write(&_sock, "Hello!", sizeof("Hello!")));
}

static void test_tcp_write6__success(void)
{
    static const sock_tcp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = SOCK_ADDR_ANY_NETIF };
    msg_t msg = { .type = _SERVER_MSG_START };
    static const struct iovec exp_data = { .iov_base = "Hello!",
                                           .iov_len = sizeof("Hello!") };

    _server_addr.family = AF_INET6;
    _server_addr.port = _TEST_PORT_REMOTE;
    _server_addr.netif = SOCK_ADDR_ANY_NETIF;

    msg_send(&msg, _server);        /* start server on _TEST_PORT_LOCAL */
    msg.type = _SERVER_MSG_ACCEPT;
    msg_send(&msg, _server);        /* let server accept */

    expect(0 == sock_tcp_connect(&_sock, &remote, 0, SOCK_FLAGS_REUSE_EP));
    msg.type = _SERVER_MSG_READ;
    msg.content.ptr = (void *)&exp_data;
    msg_send(&msg, _server);        /* write expected data at server */
    expect(((ssize_t)exp_data.iov_len) == sock_tcp_write(&_sock, "Hello!",
                                                        sizeof("Hello!")));
    expect(memcmp(exp_data.iov_base, _test_buffer, exp_data.iov_len) == 0);
    ztimer_sleep(ZTIMER_MSEC, 5);            /* wait for server */
}
#endif /* MODULE_LWIP_IPV6 */

int main(void)
{
    uint8_t code = 0;

#ifdef SO_REUSE
    code |= 1;
#endif
#ifdef MODULE_LWIP_IPV4
    code |= (1 << 4);
#endif
#ifdef MODULE_LWIP_IPV6
    code |= (1 << 6);
#endif
    printf("code 0x%02x\n", code);
    _net_init();
    expect(0 < thread_create(_client_stack, sizeof(_client_stack),
                             THREAD_PRIORITY_MAIN - 1, 0,
                             _client_func, NULL, "tcp_client"));
    expect(0 < thread_create(_server_stack, sizeof(_server_stack),
                             THREAD_PRIORITY_MAIN - 2, 0,
                             _server_func, NULL, "tcp_server"));
    tear_down();
#ifdef MODULE_LWIP_IPV4
#ifdef SO_REUSE
    CALL(test_tcp_connect4__EADDRINUSE());
#endif
    CALL(test_tcp_connect4__EAFNOSUPPORT());
    /* ECONNREFUSED does not apply for lwIP; netconn_connect does not wait for
     * connection build-up */
    CALL(test_tcp_connect4__EINVAL_addr());
    CALL(test_tcp_connect4__EINVAL_netif());
    /* ENETUNREACH not testable in given loopback setup */
    /* ETIMEDOUT doesn't work because lwIP's connect doesn't timeout */
    CALL(test_tcp_connect4__success_without_port());
    CALL(test_tcp_connect4__success_local_port());
#ifdef SO_REUSE
    CALL(test_tcp_listen4__EADDRINUSE());
#endif
    CALL(test_tcp_listen4__EAFNOSUPPORT());
    CALL(test_tcp_listen4__EINVAL());
    CALL(test_tcp_listen4__success_any_netif());
    CALL(test_tcp_listen4__success_spec_netif());
    /* sock_tcp_disconnect() is tested in tear_down() */
    /* sock_tcp_stop_listen() is tested in tear_down() */
    /* sock_tcp_get_local() is tested in sock_tcp_connect() tests */
    /* sock_tcp_get_remote() is tested in sock_tcp_connect() tests */
    /* sock_tcp_queue_get_local() is tested in sock_tcp_listen() tests */
    /* ECONNABORTED can't be tested in this setup */
    CALL(test_tcp_accept4__EAGAIN());
    CALL(test_tcp_accept4__EINVAL());
    CALL(test_tcp_accept4__ETIMEDOUT());
    CALL(test_tcp_accept4__success());
    /* ECONNABORTED can't be tested in this setup */
    CALL(test_tcp_read4__EAGAIN());
    CALL(test_tcp_read4__ECONNRESET());
    CALL(test_tcp_read4__ENOTCONN());
    CALL(test_tcp_read4__ETIMEDOUT());
    CALL(test_tcp_read4__success());
    CALL(test_tcp_read4__success_with_timeout());
    CALL(test_tcp_read4__success_non_blocking());
    /* ECONNABORTED can't be tested in this setup */
    /* ENOTCONN not applicable since lwIP always tries to send */
    CALL(test_tcp_write4__ENOTCONN());
    CALL(test_tcp_write4__success());
#endif /* MODULE_LWIP_IPV4 */
#ifdef MODULE_LWIP_IPV6
#ifdef SO_REUSE
    CALL(test_tcp_connect6__EADDRINUSE());
#endif
    CALL(test_tcp_connect6__EAFNOSUPPORT());
    /* ECONNREFUSED does not apply for lwIP; netconn_connect does not wait for
     * connection build-up */
    CALL(test_tcp_connect6__EINVAL_addr());
    CALL(test_tcp_connect6__EINVAL_netif());
    /* ENETUNREACH not testable in given loopback setup */
    /* ETIMEDOUT doesn't work because lwIP's connect doesn't timeout */
    CALL(test_tcp_connect6__success_without_port());
    CALL(test_tcp_connect6__success_local_port());
#ifdef SO_REUSE
    CALL(test_tcp_listen6__EADDRINUSE());
#endif
    CALL(test_tcp_listen6__EAFNOSUPPORT());
    CALL(test_tcp_listen6__EINVAL());
    CALL(test_tcp_listen6__success_any_netif());
    CALL(test_tcp_listen6__success_spec_netif());
    /* sock_tcp_disconnect() is tested in tear_down() */
    /* sock_tcp_stop_listen() is tested in tear_down() */
    /* sock_tcp_get_local() is tested in sock_tcp_connect() tests */
    /* sock_tcp_get_remote() is tested in sock_tcp_connect() tests */
    /* sock_tcp_queue_get_local() is tested in sock_tcp_listen() tests */
    /* ECONNABORTED can't be tested in this setup */
    CALL(test_tcp_accept6__EAGAIN());
    CALL(test_tcp_accept6__EINVAL());
    CALL(test_tcp_accept6__ETIMEDOUT());
    CALL(test_tcp_accept6__success());
    /* ECONNABORTED can't be tested in this setup */
    CALL(test_tcp_read6__EAGAIN());
    CALL(test_tcp_read6__ECONNRESET());
    CALL(test_tcp_read6__ENOTCONN());
    CALL(test_tcp_read6__ETIMEDOUT());
    CALL(test_tcp_read6__success());
    CALL(test_tcp_read6__success_with_timeout());
    CALL(test_tcp_read6__success_non_blocking());
    /* ECONNABORTED can't be tested in this setup */
    /* ENOTCONN not applicable since lwIP always tries to send */
    CALL(test_tcp_write6__ENOTCONN());
    CALL(test_tcp_write6__success());
#endif /* MODULE_LWIP_IPV6 */

    puts("ALL TESTS SUCCESSFUL");

    return 0;
}

static void *_server_func(void *arg)
{
    bool server_started = false;
    sock_tcp_t *sock = NULL;

    (void)arg;
    msg_init_queue(_server_msg_queue, _MSG_QUEUE_SIZE);
    _server = thread_getpid();
    while (1) {
        msg_t msg;

        msg_receive(&msg);
        switch (msg.type) {
            case _SERVER_MSG_START:
                if (!server_started) {
                    expect(0 == sock_tcp_listen(&_server_queue, &_server_addr,
                                                _server_queue_array,
                                                _SERVER_QUEUE_SIZE,
                                                SOCK_FLAGS_REUSE_EP));
                    server_started = true;
                }
                break;
            case _SERVER_MSG_ACCEPT:
                if (server_started) {
                    expect(0 == sock_tcp_accept(&_server_queue, &sock,
                                                SOCK_NO_TIMEOUT));
                }
                break;
            case _SERVER_MSG_READ:
                if (sock != NULL) {
                    const struct iovec *exp = msg.content.ptr;

                    expect(((ssize_t)exp->iov_len) ==
                           sock_tcp_read(sock, _server_buf, sizeof(_server_buf),
                                         SOCK_NO_TIMEOUT));
                    expect(memcmp(exp->iov_base, _server_buf, exp->iov_len) == 0);
                }
                break;
            case _SERVER_MSG_WRITE:
                if (sock != NULL) {
                    const struct iovec *data = msg.content.ptr;

                    expect(((ssize_t)data->iov_len) ==
                           sock_tcp_write(sock, data->iov_base, data->iov_len));
                }
                break;
            case _SERVER_MSG_CLOSE:
                if (sock != NULL) {
                    sock_tcp_disconnect(sock);
                    sock = NULL;
                }
                break;
            case _SERVER_MSG_STOP:
                if (server_started) {
                    sock_tcp_stop_listen(&_server_queue);
                    server_started = false;
                    /* sock_tcp_stop_listen is also supposed to close sock */
                    sock = NULL;
                }
                break;
            default:
                break;
        }
    }
    return NULL;
}

static void *_client_func(void *arg)
{
    bool client_started = false;

    (void)arg;
    msg_init_queue(_client_msg_queue, _MSG_QUEUE_SIZE);
    _client = thread_getpid();
    while (1) {
        msg_t msg;

        msg_receive(&msg);
        switch (msg.type) {
            case _CLIENT_MSG_START:
                if (!client_started) {
                    const uint16_t local_port = (uint16_t)msg.content.value;
                    expect(0 == sock_tcp_connect(&_client_sock, &_server_addr,
                                                 local_port, SOCK_FLAGS_REUSE_EP));
                    client_started = true;
                }
                break;
            case _CLIENT_MSG_READ:
                if (client_started) {
                    const struct iovec *exp = msg.content.ptr;

                    expect(((ssize_t)exp->iov_len) ==
                           sock_tcp_read(&_client_sock, _client_buf,
                                         sizeof(_client_buf), SOCK_NO_TIMEOUT));
                    expect(memcmp(exp->iov_base, _client_buf, exp->iov_len) == 0);
                }
                break;
            case _CLIENT_MSG_WRITE:
                if (client_started) {
                    const struct iovec *data = msg.content.ptr;

                    expect(((ssize_t)data->iov_len) ==
                           sock_tcp_write(&_client_sock, data->iov_base,
                                          data->iov_len));
                }
                break;
            case _CLIENT_MSG_STOP:
                if (client_started) {
                    sock_tcp_disconnect(&_client_sock);
                    memset(&_client_sock, 0, sizeof(sock_tcp_t));
                    client_started = false;
                }
                break;
            default:
                break;
        }
    }
    return NULL;
}
