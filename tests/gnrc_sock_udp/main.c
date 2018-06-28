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
 * @brief       Test for UDP socks
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 * @}
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "net/sock/udp.h"
#include "xtimer.h"

#include "constants.h"
#include "stack.h"

#define _TEST_BUFFER_SIZE   (128)

static uint8_t _test_buffer[_TEST_BUFFER_SIZE];
static sock_udp_t _sock, _sock2;

#define CALL(fn)            puts("Calling " # fn); fn; tear_down()

static void tear_down(void)
{
    sock_udp_close(&_sock);
    memset(&_sock, 0, sizeof(_sock));
}

#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
static void test_sock_udp_create__EADDRINUSE(void)
{
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };

    assert(0 == sock_udp_create(&_sock, &local, NULL, 0));
    assert(-EADDRINUSE == sock_udp_create(&_sock2, &local, NULL, 0));
}
#endif

static void test_sock_udp_create__EAFNOSUPPORT(void)
{
    /* port may not be NULL according to doc */
    static const sock_udp_ep_t local = { .family = AF_UNSPEC,
                                         .port = _TEST_PORT_LOCAL };
    /* port may not be NULL according to doc */
    static const sock_udp_ep_t remote = { .family = AF_UNSPEC,
                                          .port = _TEST_PORT_REMOTE };

    assert(-EAFNOSUPPORT == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(-EAFNOSUPPORT == sock_udp_create(&_sock, NULL, &remote, SOCK_FLAGS_REUSE_EP));
}

static void test_sock_udp_create__EINVAL_addr(void)
{
    /* port may not be NULL according to doc */
    static const sock_udp_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF,
                                         .port = _TEST_PORT_LOCAL };
    /* port may not be NULL according to doc */
    static const sock_udp_ep_t remote = { .family = AF_INET6,
                                          .netif = _TEST_NETIF,
                                          .port = _TEST_PORT_REMOTE };

    assert(-EINVAL == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
}

static void test_sock_udp_create__EINVAL_netif(void)
{
    /* port may not be NULL according to doc */
    static const sock_udp_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF,
                                         .port = _TEST_PORT_LOCAL };
    /* port may not be NULL according to doc */
    static const sock_udp_ep_t remote = { .family = AF_INET6,
                                          .netif = (_TEST_NETIF + 1),
                                          .port = _TEST_PORT_REMOTE,
                                          .addr = { .ipv6 = _TEST_ADDR_REMOTE } };

    assert(-EINVAL == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
}

static void test_sock_udp_create__no_endpoints(void)
{
    sock_udp_ep_t ep;

    assert(0 == sock_udp_create(&_sock, NULL, NULL, SOCK_FLAGS_REUSE_EP));
    assert(-EADDRNOTAVAIL == sock_udp_get_local(&_sock, &ep));
    assert(-ENOTCONN == sock_udp_get_remote(&_sock, &ep));
}

static void test_sock_udp_create__only_local(void)
{
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    sock_udp_ep_t ep;

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(0 == sock_udp_get_local(&_sock, &ep));
    assert(AF_INET6 == ep.family);
    assert(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    assert(SOCK_ADDR_ANY_NETIF == ep.netif);
    assert(_TEST_PORT_LOCAL == ep.port);
    assert(-ENOTCONN == sock_udp_get_remote(&_sock, &ep));
}

static void test_sock_udp_create__only_local_port0(void)
{
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = 0U };
    sock_udp_ep_t ep;

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(0 == sock_udp_get_local(&_sock, &ep));
    assert(AF_INET6 == ep.family);
    assert(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    assert(SOCK_ADDR_ANY_NETIF == ep.netif);
    assert(0U != ep.port);
    assert(-ENOTCONN == sock_udp_get_remote(&_sock, &ep));
}

static void test_sock_udp_create__only_local_reuse_ep(void)
{
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    sock_udp_ep_t ep, ep2;

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(0 == sock_udp_create(&_sock2, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(0 == sock_udp_get_local(&_sock, &ep));
    assert(0 == sock_udp_get_local(&_sock2, &ep2));
    assert(AF_INET6 == ep.family);
    assert(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    assert(SOCK_ADDR_ANY_NETIF == ep.netif);
    assert(_TEST_PORT_LOCAL == ep.port);
    assert(-ENOTCONN == sock_udp_get_remote(&_sock, &ep));
    assert(AF_INET6 == ep2.family);
    assert(memcmp(&ipv6_addr_unspecified, &ep2.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    assert(SOCK_ADDR_ANY_NETIF == ep2.netif);
    assert(_TEST_PORT_LOCAL == ep2.port);
    assert(-ENOTCONN == sock_udp_get_remote(&_sock, &ep2));
    sock_udp_close(&_sock2);
}

static void test_sock_udp_create__only_remote(void)
{
    static const ipv6_addr_t remote_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .addr = { .ipv6 = _TEST_ADDR_REMOTE } };
    sock_udp_ep_t ep;

    assert(0 == sock_udp_create(&_sock, NULL, &remote, SOCK_FLAGS_REUSE_EP));
    assert(-EADDRNOTAVAIL == sock_udp_get_local(&_sock, &ep));
    assert(0 == sock_udp_get_remote(&_sock, &ep));
    assert(AF_INET6 == ep.family);
    assert(memcmp(&remote_addr, &ep.addr.ipv6, sizeof(ipv6_addr_t)) == 0);
    assert(SOCK_ADDR_ANY_NETIF == ep.netif);
    assert(_TEST_PORT_REMOTE == ep.port);
}

static void test_sock_udp_create__full(void)
{
    static const ipv6_addr_t remote_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF,
                                         .port = _TEST_PORT_LOCAL };
    static const sock_udp_ep_t remote = { .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .addr = { .ipv6 = _TEST_ADDR_REMOTE } };
    sock_udp_ep_t ep;

    assert(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    assert(0 == sock_udp_get_local(&_sock, &ep));
    assert(AF_INET6 == ep.family);
    assert(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    assert(_TEST_NETIF == ep.netif);
    assert(_TEST_PORT_LOCAL == ep.port);
    assert(0 == sock_udp_get_remote(&_sock, &ep));
    assert(AF_INET6 == ep.family);
    assert(memcmp(&remote_addr, &ep.addr.ipv6, sizeof(ipv6_addr_t)) == 0);
    assert(SOCK_ADDR_ANY_NETIF == ep.netif);
    assert(_TEST_PORT_REMOTE == ep.port);
}

static void test_sock_udp_recv__EADDRNOTAVAIL(void)
{
    assert(0 == sock_udp_create(&_sock, NULL, NULL, SOCK_FLAGS_REUSE_EP));

    assert(-EADDRNOTAVAIL == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer),
                                           SOCK_NO_TIMEOUT, NULL));
}

static void test_sock_udp_recv__EAGAIN(void)
{
    static const sock_udp_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF,
                                         .port = _TEST_PORT_LOCAL };

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));

    assert(-EAGAIN == sock_udp_recv(&_sock, _test_buffer, sizeof(_test_buffer),
                                    0, NULL));
}

static void test_sock_udp_recv__ENOBUFS(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"), _TEST_NETIF));
    assert(-ENOBUFS == sock_udp_recv(&_sock, _test_buffer, 2, SOCK_NO_TIMEOUT,
                                     NULL));
    assert(_check_net());
}

static void test_sock_udp_recv__EPROTO(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_WRONG };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    assert(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    assert(-EPROTO == sock_udp_recv(&_sock, _test_buffer, sizeof(_test_buffer),
                                    SOCK_NO_TIMEOUT, NULL));
    assert(_check_net());
}

static void test_sock_udp_recv__ETIMEDOUT(void)
{
    static const sock_udp_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF,
                                         .port = _TEST_PORT_LOCAL };

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));

    puts(" * Calling sock_udp_recv()");
    assert(-ETIMEDOUT == sock_udp_recv(&_sock, _test_buffer,
                                       sizeof(_test_buffer), _TEST_TIMEOUT,
                                       NULL));
    printf(" * (timed out with timeout %lu)\n", (long unsigned)_TEST_TIMEOUT);
}

static void test_sock_udp_recv__socketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    assert(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    assert(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer),
                                           SOCK_NO_TIMEOUT, NULL));
    assert(_check_net());
}

static void test_sock_udp_recv__socketed_with_remote(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };
    sock_udp_ep_t result;

    assert(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    assert(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer),
                                           SOCK_NO_TIMEOUT, &result));
    assert(AF_INET6 == result.family);
    assert(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    assert(_TEST_PORT_REMOTE == result.port);
    assert(_TEST_NETIF == result.netif);
    assert(_check_net());
}

static void test_sock_udp_recv__socketed_with_port0(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static sock_udp_ep_t local = { .family = AF_INET6, .port = 0 };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };
    sock_udp_ep_t result;

    assert(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    assert(0 == sock_udp_get_local(&_sock, &local));
    assert(0 != local.port);
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          local.port, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    assert(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer),
                                           SOCK_NO_TIMEOUT, &result));
    assert(AF_INET6 == result.family);
    assert(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    assert(_TEST_PORT_REMOTE == result.port);
    assert(_TEST_NETIF == result.netif);
    assert(_check_net());
}

static void test_sock_udp_recv__unsocketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                         .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    assert(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer),
                                           SOCK_NO_TIMEOUT, NULL));
    assert(_check_net());
}

static void test_sock_udp_recv__unsocketed_with_remote(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    sock_udp_ep_t result;

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    assert(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer),
                                           SOCK_NO_TIMEOUT, &result));
    assert(AF_INET6 == result.family);
    assert(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    assert(_TEST_PORT_REMOTE == result.port);
    assert(_TEST_NETIF == result.netif);
    assert(_check_net());
}

static void test_sock_udp_recv__with_timeout(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    sock_udp_ep_t result;

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    assert(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer), _TEST_TIMEOUT,
                                           &result));
    assert(AF_INET6 == result.family);
    assert(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    assert(_TEST_PORT_REMOTE == result.port);
    assert(_TEST_NETIF == result.netif);
    assert(_check_net());
}

static void test_sock_udp_recv__non_blocking(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    sock_udp_ep_t result;

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    assert(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer), 0, &result));
    assert(AF_INET6 == result.family);
    assert(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    assert(_TEST_PORT_REMOTE == result.port);
    assert(_TEST_NETIF == result.netif);
    assert(_check_net());
}

static void test_sock_udp_send__EAFNOSUPPORT(void)
{
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE };

    assert(-EAFNOSUPPORT == sock_udp_send(NULL, "ABCD", sizeof("ABCD"),
                                          &remote));
    assert(_check_net());
}

static void test_sock_udp_send__EINVAL_addr(void)
{
    static const sock_udp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                         .family = AF_INET6,
                                         .port = _TEST_PORT_REMOTE,
                                         .netif = _TEST_NETIF };
    static const sock_udp_ep_t remote = { .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = _TEST_NETIF };

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(-EINVAL == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"), &remote));
    assert(_check_net());
}

static void test_sock_udp_send__EINVAL_netif(void)
{
    static const sock_udp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                         .family = AF_INET6,
                                         .port = _TEST_PORT_REMOTE,
                                         .netif = _TEST_NETIF };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .netif = _TEST_NETIF + 1 };

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(-EINVAL == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"), &remote));
    assert(_check_net());
}

static void test_sock_udp_send__EINVAL_port(void)
{
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6 };

    assert(-EINVAL == sock_udp_send(NULL, "ABCD", sizeof("ABCD"), &remote));
    assert(_check_net());
}

static void test_sock_udp_send__ENOTCONN(void)
{
    assert(0 == sock_udp_create(&_sock, NULL, NULL, SOCK_FLAGS_REUSE_EP));
    assert(-ENOTCONN == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"), NULL));
    assert(_check_net());
}

static void test_sock_udp_send__socketed_no_local_no_netif(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    assert(0 == sock_udp_create(&_sock, NULL, &remote, SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           NULL));
    assert(_check_packet(&ipv6_addr_unspecified, &dst_addr, 0,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         SOCK_ADDR_ANY_NETIF, true));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_udp_send__socketed_no_netif(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                         .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    assert(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           NULL));
    assert(_check_packet(&src_addr, &dst_addr, _TEST_PORT_LOCAL,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         SOCK_ADDR_ANY_NETIF, false));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_udp_send__socketed_no_local(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .netif = _TEST_NETIF,
                                          .port = _TEST_PORT_REMOTE };

    assert(0 == sock_udp_create(&_sock, NULL, &remote, SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           NULL));
    assert(_check_packet(&ipv6_addr_unspecified, &dst_addr, 0,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"), _TEST_NETIF,
                         true));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_udp_send__socketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                         .family = AF_INET6,
                                         .netif = _TEST_NETIF,
                                         .port = _TEST_PORT_LOCAL };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    assert(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           NULL));
    assert(_check_packet(&src_addr, &dst_addr, _TEST_PORT_LOCAL,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         _TEST_NETIF, false));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_udp_send__socketed_other_remote(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                         .family = AF_INET6,
                                         .netif = _TEST_NETIF,
                                         .port = _TEST_PORT_LOCAL };
    static const sock_udp_ep_t sock_remote = { .addr = { .ipv6 = _TEST_ADDR_WRONG },
                                               .family = AF_INET6,
                                               .port = _TEST_PORT_REMOTE + _TEST_PORT_LOCAL };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    assert(0 == sock_udp_create(&_sock, &local, &sock_remote, SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           &remote));
    assert(_check_packet(&src_addr, &dst_addr, _TEST_PORT_LOCAL,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         _TEST_NETIF, false));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_udp_send__unsocketed_no_local_no_netif(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    assert(0 == sock_udp_create(&_sock, NULL, NULL, SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           &remote));
    assert(_check_packet(&ipv6_addr_unspecified, &dst_addr, 0,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         SOCK_ADDR_ANY_NETIF, true));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_udp_send__unsocketed_no_netif(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                         .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           &remote));
    assert(_check_packet(&src_addr, &dst_addr, _TEST_PORT_LOCAL,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         SOCK_ADDR_ANY_NETIF, false));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_udp_send__unsocketed_no_local(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .netif = _TEST_NETIF,
                                          .port = _TEST_PORT_REMOTE };

    assert(0 == sock_udp_create(&_sock, NULL, NULL, SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           &remote));
    assert(_check_packet(&ipv6_addr_unspecified, &dst_addr, 0,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"), _TEST_NETIF,
                         true));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_udp_send__unsocketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                         .family = AF_INET6,
                                         .netif = _TEST_NETIF,
                                         .port = _TEST_PORT_LOCAL };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    assert(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           &remote));
    assert(_check_packet(&src_addr, &dst_addr, _TEST_PORT_LOCAL,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         _TEST_NETIF, false));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_udp_send__no_sock_no_netif(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    assert(sizeof("ABCD") == sock_udp_send(NULL, "ABCD", sizeof("ABCD"),
                                           &remote));
    assert(_check_packet(&ipv6_addr_unspecified, &dst_addr, 0,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         SOCK_ADDR_ANY_NETIF, true));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_udp_send__no_sock(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .netif = _TEST_NETIF,
                                          .port = _TEST_PORT_REMOTE };

    assert(sizeof("ABCD") == sock_udp_send(NULL, "ABCD", sizeof("ABCD"),
                                           &remote));
    assert(_check_packet(&ipv6_addr_unspecified, &dst_addr, 0,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         _TEST_NETIF, true));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

int main(void)
{
    _net_init();
    tear_down();
#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
    CALL(test_sock_udp_create__EADDRINUSE());
#endif
    CALL(test_sock_udp_create__EAFNOSUPPORT());
    CALL(test_sock_udp_create__EINVAL_addr());
    CALL(test_sock_udp_create__EINVAL_netif());
    CALL(test_sock_udp_create__no_endpoints());
    CALL(test_sock_udp_create__only_local());
    CALL(test_sock_udp_create__only_local_port0());
    CALL(test_sock_udp_create__only_local_reuse_ep());
    CALL(test_sock_udp_create__only_remote());
    CALL(test_sock_udp_create__full());
    /* sock_udp_close() is tested in tear_down() */
    /* sock_udp_get_local() is tested in sock_udp_create() tests */
    /* sock_udp_get_remote() is tested in sock_udp_create() tests */
    CALL(test_sock_udp_recv__EADDRNOTAVAIL());
    CALL(test_sock_udp_recv__EAGAIN());
    CALL(test_sock_udp_recv__ENOBUFS());
    CALL(test_sock_udp_recv__EPROTO());
    CALL(test_sock_udp_recv__ETIMEDOUT());
    CALL(test_sock_udp_recv__socketed());
    CALL(test_sock_udp_recv__socketed_with_remote());
    CALL(test_sock_udp_recv__socketed_with_port0());
    CALL(test_sock_udp_recv__unsocketed());
    CALL(test_sock_udp_recv__unsocketed_with_remote());
    CALL(test_sock_udp_recv__with_timeout());
    CALL(test_sock_udp_recv__non_blocking());
    _prepare_send_checks();
    CALL(test_sock_udp_send__EAFNOSUPPORT());
    CALL(test_sock_udp_send__EINVAL_addr());
    CALL(test_sock_udp_send__EINVAL_netif());
    CALL(test_sock_udp_send__EINVAL_port());
    CALL(test_sock_udp_send__ENOTCONN());
    CALL(test_sock_udp_send__socketed_no_local_no_netif());
    CALL(test_sock_udp_send__socketed_no_netif());
    CALL(test_sock_udp_send__socketed_no_local());
    CALL(test_sock_udp_send__socketed());
    CALL(test_sock_udp_send__socketed_other_remote());
    CALL(test_sock_udp_send__unsocketed_no_local_no_netif());
    CALL(test_sock_udp_send__unsocketed_no_netif());
    CALL(test_sock_udp_send__unsocketed_no_local());
    CALL(test_sock_udp_send__unsocketed());
    CALL(test_sock_udp_send__no_sock_no_netif());
    CALL(test_sock_udp_send__no_sock());

    puts("ALL TESTS SUCCESSFUL");

    return 0;
}
