/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test for raw IP socks
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "net/sock/ip.h"
#include "test_utils/expect.h"
#include "xtimer.h"

#include "constants.h"
#include "stack.h"

#define _TEST_BUFFER_SIZE   (128)

#define CALL(fn)            puts("Calling " # fn); fn; tear_down()

static sock_ip_t _sock, _sock2;
static uint8_t _test_buffer[_TEST_BUFFER_SIZE];

static void tear_down(void)
{
    sock_ip_close(&_sock);
    memset(&_sock, 0, sizeof(_sock));
}

#ifdef MODULE_LWIP_IPV4
static void test_sock_ip_create4__EAFNOSUPPORT(void)
{
    static const sock_ip_ep_t local = { .family = AF_UNSPEC };
    static const sock_ip_ep_t remote = { .family = AF_UNSPEC };

    expect(-EAFNOSUPPORT == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                                           SOCK_FLAGS_REUSE_EP));
    expect(-EAFNOSUPPORT == sock_ip_create(&_sock, NULL, &remote, _TEST_PROTO,
                                           SOCK_FLAGS_REUSE_EP));
}

static void test_sock_ip_create4__EINVAL_addr(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET, .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .family = AF_INET,
                                         .netif = _TEST_NETIF };

    expect(-EINVAL == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                                     SOCK_FLAGS_REUSE_EP));
}

static void test_sock_ip_create4__EINVAL_netif(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET, .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .family = AF_INET,
                                         .netif = (_TEST_NETIF + 1),
                                         .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE } };

    expect(-EINVAL == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                                     SOCK_FLAGS_REUSE_EP));
}

static void test_sock_ip_create4__no_endpoints(void)
{
    sock_ip_ep_t ep;

    expect(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(-EADDRNOTAVAIL == sock_ip_get_local(&_sock, &ep));
    expect(-ENOTCONN == sock_ip_get_remote(&_sock, &ep));
}

static void test_sock_ip_create4__only_local(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET };
    sock_ip_ep_t ep;

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_ip_get_local(&_sock, &ep));
    expect(AF_INET == ep.family);
    expect(0 == ep.addr.ipv4_u32);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(-ENOTCONN == sock_ip_get_remote(&_sock, &ep));
}

static void test_sock_ip_create4__only_local_reuse_ep(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET };
    sock_ip_ep_t ep, ep2;

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_ip_create(&_sock2, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_ip_get_local(&_sock, &ep));
    expect(0 == sock_ip_get_local(&_sock2, &ep2));
    expect(AF_INET == ep.family);
    expect(0 == ep.addr.ipv4_u32);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(-ENOTCONN == sock_ip_get_remote(&_sock, &ep));
    expect(AF_INET == ep2.family);
    expect(0 == ep2.addr.ipv4_u32);
    expect(SOCK_ADDR_ANY_NETIF == ep2.netif);
    expect(-ENOTCONN == sock_ip_get_remote(&_sock, &ep2));
    sock_ip_close(&_sock2);
}

static void test_sock_ip_create4__only_remote(void)
{
    static const sock_ip_ep_t remote = { .family = AF_INET,
                                         .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE } };
    sock_ip_ep_t ep;

    expect(0 == sock_ip_create(&_sock, NULL, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    /* lwIP binds connected sock implicitly */
    expect(0 == sock_ip_get_local(&_sock, &ep));
    expect(0 == sock_ip_get_remote(&_sock, &ep));
    expect(AF_INET == ep.family);
    expect(_TEST_ADDR4_REMOTE == ep.addr.ipv4_u32);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
}

static void test_sock_ip_create4__full(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET, .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .family = AF_INET,
                                         .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE } };
    sock_ip_ep_t ep;

    expect(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_ip_get_local(&_sock, &ep));
    expect(AF_INET == ep.family);
    /* this can't be guaranteed with lwIP */
    /* expect(0 == ep.addr.ipv4_u32); */
    expect(_TEST_NETIF == ep.netif);
    expect(0 == sock_ip_get_remote(&_sock, &ep));
    expect(AF_INET == ep.family);
    expect(_TEST_ADDR4_REMOTE == ep.addr.ipv4_u32);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
}

static void test_sock_ip_recv4__EADDRNOTAVAIL(void)
{
    expect(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));

    expect(-EADDRNOTAVAIL == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, NULL));
}

static void test_sock_ip_recv4__ENOBUFS(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_4packet(_TEST_ADDR4_REMOTE, _TEST_ADDR4_LOCAL, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(-ENOBUFS == sock_ip_recv(&_sock, _test_buffer, 2, 0, NULL));
    expect(_check_net());
}

static void test_sock_ip_recv4__EAGAIN(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET, .netif = _TEST_NETIF };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));

    expect(-EAGAIN == sock_ip_recv(&_sock, _test_buffer, sizeof(_test_buffer),
                                   0, NULL));
}

static void test_sock_ip_recv4__ETIMEDOUT(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET, .netif = _TEST_NETIF };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));

    puts(" * Calling sock_ip_recv()");
    expect(-ETIMEDOUT == sock_ip_recv(&_sock, _test_buffer,
                                      sizeof(_test_buffer), _TEST_TIMEOUT,
                                      NULL));
    printf(" * (timed out with timeout %u)\n", _TEST_TIMEOUT);
}

static void test_sock_ip_recv4__socketed(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET };
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET };

    expect(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_4packet(_TEST_ADDR4_REMOTE, _TEST_ADDR4_LOCAL, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, NULL));
    expect(_check_net());
}

static void test_sock_ip_recv4__socketed_with_remote(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET };
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET };
    sock_ip_ep_t result;

    expect(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_4packet(_TEST_ADDR4_REMOTE, _TEST_ADDR4_LOCAL, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, &result));
    expect(AF_INET == result.family);
    expect(_TEST_ADDR4_REMOTE == result.addr.ipv4_u32);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
}

static void test_sock_ip_recv4__unsocketed(void)
{
    static const sock_ip_ep_t local = { .addr = { .ipv4_u32 = _TEST_ADDR4_LOCAL },
                                        .family = AF_INET };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_4packet(_TEST_ADDR4_REMOTE, _TEST_ADDR4_LOCAL, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, NULL));
    expect(_check_net());
}

static void test_sock_ip_recv4__unsocketed_with_remote(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET };
    sock_ip_ep_t result;

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_4packet(_TEST_ADDR4_REMOTE, _TEST_ADDR4_LOCAL, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, &result));
    expect(AF_INET == result.family);
    expect(_TEST_ADDR4_REMOTE == result.addr.ipv4_u32);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
}

static void test_sock_ip_recv4__with_timeout(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET };
    sock_ip_ep_t result;

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_4packet(_TEST_ADDR4_REMOTE, _TEST_ADDR4_LOCAL, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), _TEST_TIMEOUT,
                                          &result));
    expect(AF_INET == result.family);
    expect(_TEST_ADDR4_REMOTE == result.addr.ipv4_u32);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
}

static void test_sock_ip_recv4__non_blocking(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET };
    sock_ip_ep_t result;

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_4packet(_TEST_ADDR4_REMOTE, _TEST_ADDR4_LOCAL, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, &result));
    expect(AF_INET == result.family);
    expect(_TEST_ADDR4_REMOTE == result.addr.ipv4_u32);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
}

static void test_sock_ip_recv_buf4__success(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET };
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET };
    void *data = NULL, *ctx = NULL;

    expect(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_4packet(_TEST_ADDR4_REMOTE, _TEST_ADDR4_LOCAL, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    assert(sizeof("ABCD") == sock_ip_recv_buf(&_sock, &data, &ctx,
                                              SOCK_NO_TIMEOUT, NULL));
    assert(data != NULL);
    assert(ctx != NULL);
    assert(0 == sock_ip_recv_buf(&_sock, &data, &ctx, SOCK_NO_TIMEOUT, NULL));
    assert(data == NULL);
    assert(ctx == NULL);
    assert(_check_net());
}

static void test_sock_ip_send4__EAFNOSUPPORT(void)
{
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_UNSPEC };

    expect(-EAFNOSUPPORT == sock_ip_send(NULL, "ABCD", sizeof("ABCD"),
                                         _TEST_PROTO, &remote));
    expect(_check_net());
}

static void test_sock_ip_send4__EINVAL_addr(void)
{
    static const sock_ip_ep_t local = { .addr = { .ipv4_u32 = _TEST_ADDR4_LOCAL },
                                        .family = AF_INET,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .family = AF_INET,
                                         .netif = _TEST_NETIF };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(-EINVAL == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"), _TEST_PROTO,
                                   &remote));
    expect(_check_net());
}

static void test_sock_ip_send4__EINVAL_netif(void)
{
    static const sock_ip_ep_t local = { .addr = { .ipv4_u32 = _TEST_ADDR4_LOCAL },
                                        .family = AF_INET,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET,
                                         .netif = _TEST_NETIF + 1 };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(-EINVAL == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"), _TEST_PROTO,
                                   &remote));
    expect(_check_net());
}

static void test_sock_ip_send4__EHOSTUNREACH(void)
{
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_WRONG },
                                         .family = AF_INET };

    expect(-EHOSTUNREACH == sock_ip_send(NULL, "ABCD", sizeof("ABCD"), _TEST_PROTO,
                                         &remote));
}

static void test_sock_ip_send4__ENOTCONN(void)
{
    expect(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(-ENOTCONN == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                     _TEST_PROTO, NULL));
    expect(_check_net());
}

static void test_sock_ip_send4__socketed_no_local_no_netif(void)
{
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET };

    expect(0 == sock_ip_create(&_sock, NULL, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, NULL));
    expect(_check_4packet(0, _TEST_ADDR4_REMOTE, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send4__socketed_no_netif(void)
{
    static const sock_ip_ep_t local = { .addr = { .ipv4_u32 = _TEST_ADDR4_LOCAL },
                                        .family = AF_INET };
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET };

    expect(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, NULL));
    expect(_check_4packet(_TEST_ADDR4_LOCAL, _TEST_ADDR4_REMOTE, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send4__socketed_no_local(void)
{
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET,
                                         .netif = _TEST_NETIF };

    expect(0 == sock_ip_create(&_sock, NULL, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, NULL));
    expect(_check_4packet(0, _TEST_ADDR4_REMOTE, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send4__socketed(void)
{
    static const sock_ip_ep_t local = { .addr = { .ipv4_u32 = _TEST_ADDR4_LOCAL },
                                        .family = AF_INET,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET };

    expect(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, NULL));
    expect(_check_4packet(_TEST_ADDR4_LOCAL, _TEST_ADDR4_REMOTE, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send4__socketed_other_remote(void)
{
    static const sock_ip_ep_t local = { .addr = { .ipv4_u32 = _TEST_ADDR4_LOCAL },
                                        .family = AF_INET,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t sock_remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_WRONG },
                                              .family = AF_INET };
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET };

    expect(0 == sock_ip_create(&_sock, &local, &sock_remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_4packet(_TEST_ADDR4_LOCAL, _TEST_ADDR4_REMOTE, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send4__unsocketed_no_local_no_netif(void)
{
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET };

    expect(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_4packet(0, _TEST_ADDR4_REMOTE, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send4__unsocketed_no_netif(void)
{
    static const sock_ip_ep_t local = { .addr = { .ipv4_u32 = _TEST_ADDR4_LOCAL },
                                        .family = AF_INET };
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_4packet(_TEST_ADDR4_LOCAL, _TEST_ADDR4_REMOTE, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send4__unsocketed_no_local(void)
{
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET,
                                         .netif = _TEST_NETIF };

    expect(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_4packet(0, _TEST_ADDR4_REMOTE, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send4__unsocketed(void)
{
    static const sock_ip_ep_t local = { .addr = { .ipv4_u32 = _TEST_ADDR4_LOCAL },
                                        .family = AF_INET,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_4packet(_TEST_ADDR4_LOCAL, _TEST_ADDR4_REMOTE, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send4__no_sock_no_netif(void)
{
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET };

    expect(sizeof("ABCD") == sock_ip_send(NULL, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_4packet(0, _TEST_ADDR4_REMOTE, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send4__no_sock(void)
{
    static const sock_ip_ep_t remote = { .addr = { .ipv4_u32 = _TEST_ADDR4_REMOTE },
                                         .family = AF_INET,
                                         .netif = _TEST_NETIF };

    expect(sizeof("ABCD") == sock_ip_send(NULL, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_4packet(0, _TEST_ADDR4_REMOTE, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}
#endif  /* MODULE_LWIP_IPV4 */

#ifdef MODULE_LWIP_IPV6
static void test_sock_ip_create6__EAFNOSUPPORT(void)
{
    static const sock_ip_ep_t local = { .family = AF_UNSPEC };
    static const sock_ip_ep_t remote = { .family = AF_UNSPEC };

    expect(-EAFNOSUPPORT == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                                           SOCK_FLAGS_REUSE_EP));
    expect(-EAFNOSUPPORT == sock_ip_create(&_sock, NULL, &remote, _TEST_PROTO,
                                           SOCK_FLAGS_REUSE_EP));
}

static void test_sock_ip_create6__EINVAL_addr(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .family = AF_INET6,
                                         .netif = _TEST_NETIF };

    expect(-EINVAL == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                                     SOCK_FLAGS_REUSE_EP));
}

static void test_sock_ip_create6__EINVAL_netif(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .family = AF_INET6,
                                         .netif = (_TEST_NETIF + 1),
                                         .addr = { .ipv6 = _TEST_ADDR6_REMOTE } };

    expect(-EINVAL == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                                     SOCK_FLAGS_REUSE_EP));
}

static void test_sock_ip_create6__no_endpoints(void)
{
    sock_ip_ep_t ep;

    expect(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(-EADDRNOTAVAIL == sock_ip_get_local(&_sock, &ep));
    expect(-ENOTCONN == sock_ip_get_remote(&_sock, &ep));
}

static void test_sock_ip_create6__only_local(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    sock_ip_ep_t ep;

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_ip_get_local(&_sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(-ENOTCONN == sock_ip_get_remote(&_sock, &ep));
}

static void test_sock_ip_create6__only_local_reuse_ep(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    sock_ip_ep_t ep, ep2;

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_ip_create(&_sock2, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_ip_get_local(&_sock, &ep));
    expect(0 == sock_ip_get_local(&_sock2, &ep2));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(-ENOTCONN == sock_ip_get_remote(&_sock, &ep));
    expect(AF_INET6 == ep2.family);
    expect(memcmp(&ipv6_addr_unspecified, &ep2.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep2.netif);
    expect(-ENOTCONN == sock_ip_get_remote(&_sock, &ep2));
    sock_ip_close(&_sock2);
}

static void test_sock_ip_create6__only_remote(void)
{
    static const ipv6_addr_t remote_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t remote = { .family = AF_INET6,
                                         .addr = { .ipv6 = _TEST_ADDR6_REMOTE } };
    sock_ip_ep_t ep;

    expect(0 == sock_ip_create(&_sock, NULL, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    /* lwIP binds connected sock implicitly */
    expect(0 == sock_ip_get_local(&_sock, &ep));
    expect(0 == sock_ip_get_remote(&_sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&remote_addr, &ep.addr.ipv6, sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
}

static void test_sock_ip_create6__full(void)
{
    static const ipv6_addr_t remote_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .family = AF_INET6,
                                         .addr = { .ipv6 = _TEST_ADDR6_REMOTE } };
    sock_ip_ep_t ep;

    expect(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_ip_get_local(&_sock, &ep));
    expect(AF_INET6 == ep.family);
    /* this can't be guaranteed with lwIP */
    /* expect(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6, */
    /*               sizeof(ipv6_addr_t)) == 0); */
    expect(_TEST_NETIF == ep.netif);
    expect(0 == sock_ip_get_remote(&_sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&remote_addr, &ep.addr.ipv6, sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
}

static void test_sock_ip_recv6__EADDRNOTAVAIL(void)
{
    expect(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));

    expect(-EADDRNOTAVAIL == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, NULL));
}

static void test_sock_ip_recv6__EAGAIN(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));

    expect(-EAGAIN == sock_ip_recv(&_sock, _test_buffer, sizeof(_test_buffer),
                                   0, NULL));
}

static void test_sock_ip_recv6__ENOBUFS(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(-ENOBUFS == sock_ip_recv(&_sock, _test_buffer, 2, 0, NULL));
    expect(_check_net());
}

static void test_sock_ip_recv6__ETIMEDOUT(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));

    puts(" * Calling sock_ip_recv()");
    expect(-ETIMEDOUT == sock_ip_recv(&_sock, _test_buffer,
                                      sizeof(_test_buffer), _TEST_TIMEOUT,
                                      NULL));
    printf(" * (timed out with timeout %u)\n", _TEST_TIMEOUT);
}

static void test_sock_ip_recv6__socketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6 };

    expect(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, NULL));
    expect(_check_net());
}

static void test_sock_ip_recv6__socketed_with_remote(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6 };
    sock_ip_ep_t result;

    expect(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, &result));
    expect(AF_INET6 == result.family);
    expect(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
}

static void test_sock_ip_recv6__unsocketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR6_LOCAL },
                                        .family = AF_INET6 };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, NULL));
    expect(_check_net());
}

static void test_sock_ip_recv6__unsocketed_with_remote(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    sock_ip_ep_t result;

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, &result));
    expect(AF_INET6 == result.family);
    expect(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
}

static void test_sock_ip_recv6__with_timeout(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    sock_ip_ep_t result;

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), _TEST_TIMEOUT,
                                          &result));
    expect(AF_INET6 == result.family);
    expect(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
}

static void test_sock_ip_recv6__non_blocking(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    sock_ip_ep_t result;

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(_inject_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    expect(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, &result));
    expect(AF_INET6 == result.family);
    expect(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
}

static void test_sock_ip_recv_buf6__success(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6 };
    void *data = NULL, *ctx = NULL;

    assert(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(_inject_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                           sizeof("ABCD"), _TEST_NETIF));
    assert(sizeof("ABCD") == sock_ip_recv_buf(&_sock, &data, &ctx, SOCK_NO_TIMEOUT,
                                              NULL));
    assert(data != NULL);
    assert(ctx != NULL);
    assert(0 == sock_ip_recv_buf(&_sock, &data, &ctx, SOCK_NO_TIMEOUT, NULL));
    assert(data == NULL);
    assert(ctx == NULL);
    assert(_check_net());
}

static void test_sock_ip_send6__EAFNOSUPPORT(void)
{
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_UNSPEC };

    expect(-EAFNOSUPPORT == sock_ip_send(NULL, "ABCD", sizeof("ABCD"),
                                         _TEST_PROTO, &remote));
    expect(_check_net());
}

static void test_sock_ip_send6__EINVAL_addr(void)
{
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR6_LOCAL },
                                        .family = AF_INET6,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .family = AF_INET6,
                                         .netif = _TEST_NETIF };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(-EINVAL == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"), _TEST_PROTO,
                                   &remote));
    expect(_check_net());
}

static void test_sock_ip_send6__EINVAL_netif(void)
{
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR6_LOCAL },
                                        .family = AF_INET6,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6,
                                         .netif = _TEST_NETIF + 1 };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(-EINVAL == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"), _TEST_PROTO,
                                   &remote));
    expect(_check_net());
}

static void test_sock_ip_send6__EHOSTUNREACH(void)
{
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_WRONG },
                                         .family = AF_INET6 };

    expect(-EHOSTUNREACH == sock_ip_send(NULL, "ABCD", sizeof("ABCD"), _TEST_PROTO,
                                         &remote));
}

static void test_sock_ip_send6__ENOTCONN(void)
{
    expect(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(-ENOTCONN == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                     _TEST_PROTO, NULL));
    expect(_check_net());
}

static void test_sock_ip_send6__socketed_no_local_no_netif(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6 };

    expect(0 == sock_ip_create(&_sock, NULL, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, NULL));
    expect(_check_6packet(&ipv6_addr_unspecified, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send6__socketed_no_netif(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR6_LOCAL },
                                        .family = AF_INET6 };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6 };

    expect(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, NULL));
    expect(_check_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send6__socketed_no_local(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6,
                                         .netif = _TEST_NETIF };

    expect(0 == sock_ip_create(&_sock, NULL, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, NULL));
    expect(_check_6packet(&ipv6_addr_unspecified, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send6__socketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR6_LOCAL },
                                        .family = AF_INET6,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6 };

    expect(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, NULL));
    expect(_check_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send6__socketed_other_remote(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR6_LOCAL },
                                        .family = AF_INET6,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t sock_remote = { .addr = { .ipv6 = _TEST_ADDR6_WRONG },
                                              .family = AF_INET6 };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6 };

    expect(0 == sock_ip_create(&_sock, &local, &sock_remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send6__unsocketed_no_local_no_netif(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6 };

    expect(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_6packet(&ipv6_addr_unspecified, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send6__unsocketed_no_netif(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR6_LOCAL },
                                        .family = AF_INET6 };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6 };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send6__unsocketed_no_local(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6,
                                         .netif = _TEST_NETIF };

    expect(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_6packet(&ipv6_addr_unspecified, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send6__unsocketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR6_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR6_LOCAL },
                                        .family = AF_INET6,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6 };

    expect(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_6packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send6__no_sock_no_netif(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6 };

    expect(sizeof("ABCD") == sock_ip_send(NULL, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_6packet(&ipv6_addr_unspecified, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}

static void test_sock_ip_send6__no_sock(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR6_REMOTE };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR6_REMOTE },
                                         .family = AF_INET6,
                                         .netif = _TEST_NETIF };

    expect(sizeof("ABCD") == sock_ip_send(NULL, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    expect(_check_6packet(&ipv6_addr_unspecified, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let lwIP stack finish */
    expect(_check_net());
}
#endif  /* MODULE_LWIP_IPV6 */

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
    tear_down();
#ifdef MODULE_LWIP_IPV4
    /* EADDRINUSE does not apply for lwIP */
    CALL(test_sock_ip_create4__EAFNOSUPPORT());
    CALL(test_sock_ip_create4__EINVAL_addr());
    CALL(test_sock_ip_create4__EINVAL_netif());
    /* EPROTONOSUPPORT does not apply for lwIP */
    CALL(test_sock_ip_create4__no_endpoints());
    CALL(test_sock_ip_create4__only_local());
    CALL(test_sock_ip_create4__only_local_reuse_ep());
    CALL(test_sock_ip_create4__only_remote());
    CALL(test_sock_ip_create4__full());
    /* sock_ip_close() is tested in tear_down() */
    /* sock_ip_get_local() is tested in sock_ip_create() tests */
    /* sock_ip_get_remote() is tested in sock_ip_create() tests */
    CALL(test_sock_ip_recv4__EADDRNOTAVAIL());
    CALL(test_sock_ip_recv4__EAGAIN());
    CALL(test_sock_ip_recv4__ENOBUFS());
    /* EPROTO not applicable for lwIP */
    CALL(test_sock_ip_recv4__ETIMEDOUT());
    CALL(test_sock_ip_recv4__socketed());
    CALL(test_sock_ip_recv4__socketed_with_remote());
    CALL(test_sock_ip_recv4__unsocketed());
    CALL(test_sock_ip_recv4__unsocketed_with_remote());
    CALL(test_sock_ip_recv4__with_timeout());
    CALL(test_sock_ip_recv4__non_blocking());
    CALL(test_sock_ip_recv_buf4__success());
    _prepare_send_checks();
    CALL(test_sock_ip_send4__EAFNOSUPPORT());
    CALL(test_sock_ip_send4__EINVAL_addr());
    CALL(test_sock_ip_send4__EINVAL_netif());
    CALL(test_sock_ip_send4__EHOSTUNREACH());
    CALL(test_sock_ip_send4__ENOTCONN());
    /* EPROTOTYPE does not apply for lwIP */
    CALL(test_sock_ip_send4__socketed_no_local_no_netif());
    CALL(test_sock_ip_send4__socketed_no_netif());
    CALL(test_sock_ip_send4__socketed_no_local());
    CALL(test_sock_ip_send4__socketed());
    CALL(test_sock_ip_send4__socketed_other_remote());
    CALL(test_sock_ip_send4__unsocketed_no_local_no_netif());
    CALL(test_sock_ip_send4__unsocketed_no_netif());
    CALL(test_sock_ip_send4__unsocketed_no_local());
    CALL(test_sock_ip_send4__unsocketed());
    CALL(test_sock_ip_send4__no_sock_no_netif());
    CALL(test_sock_ip_send4__no_sock());
#endif  /* MODULE_LWIP_IPV4 */
#ifdef MODULE_LWIP_IPV6
    /* EADDRINUSE does not apply for lwIP */
    CALL(test_sock_ip_create6__EAFNOSUPPORT());
    CALL(test_sock_ip_create6__EINVAL_addr());
    CALL(test_sock_ip_create6__EINVAL_netif());
    /* EPROTONOSUPPORT does not apply for lwIP */
    CALL(test_sock_ip_create6__no_endpoints());
    CALL(test_sock_ip_create6__only_local());
    CALL(test_sock_ip_create6__only_local_reuse_ep());
    CALL(test_sock_ip_create6__only_remote());
    CALL(test_sock_ip_create6__full());
    /* sock_ip_close() is tested in tear_down() */
    /* sock_ip_get_local() is tested in sock_ip_create() tests */
    /* sock_ip_get_remote() is tested in sock_ip_create() tests */
    CALL(test_sock_ip_recv6__EADDRNOTAVAIL());
    CALL(test_sock_ip_recv6__EAGAIN());
    CALL(test_sock_ip_recv6__ENOBUFS());
    /* EPROTO not applicable for lwIP */
    CALL(test_sock_ip_recv6__ETIMEDOUT());
    CALL(test_sock_ip_recv6__socketed());
    CALL(test_sock_ip_recv6__socketed_with_remote());
    CALL(test_sock_ip_recv6__unsocketed());
    CALL(test_sock_ip_recv6__unsocketed_with_remote());
    CALL(test_sock_ip_recv6__with_timeout());
    CALL(test_sock_ip_recv6__non_blocking());
    CALL(test_sock_ip_recv_buf6__success());
    _prepare_send_checks();
    CALL(test_sock_ip_send6__EAFNOSUPPORT());
    CALL(test_sock_ip_send6__EINVAL_addr());
    CALL(test_sock_ip_send6__EINVAL_netif());
    CALL(test_sock_ip_send6__EHOSTUNREACH());
    CALL(test_sock_ip_send6__ENOTCONN());
    /* EPROTOTYPE does not apply for lwIP */
    CALL(test_sock_ip_send6__socketed_no_local_no_netif());
    CALL(test_sock_ip_send6__socketed_no_netif());
    CALL(test_sock_ip_send6__socketed_no_local());
    CALL(test_sock_ip_send6__socketed());
    CALL(test_sock_ip_send6__socketed_other_remote());
    CALL(test_sock_ip_send6__unsocketed_no_local_no_netif());
    CALL(test_sock_ip_send6__unsocketed_no_netif());
    CALL(test_sock_ip_send6__unsocketed_no_local());
    CALL(test_sock_ip_send6__unsocketed());
    CALL(test_sock_ip_send6__no_sock_no_netif());
    CALL(test_sock_ip_send6__no_sock());
#endif /* MODULE_LWIP_IPV6 */

    puts("ALL TESTS SUCCESSFUL");

    return 0;
}
