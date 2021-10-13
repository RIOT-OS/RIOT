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

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "net/sock/udp.h"
#include "test_utils/expect.h"
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

    expect(0 == sock_udp_create(&_sock, &local, NULL, 0));
    expect(-EADDRINUSE == sock_udp_create(&_sock2, &local, NULL, 0));
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

    expect(-EAFNOSUPPORT == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(-EAFNOSUPPORT == sock_udp_create(&_sock, NULL, &remote, SOCK_FLAGS_REUSE_EP));
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

    expect(-EINVAL == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
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

    expect(-EINVAL == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
}

static void test_sock_udp_create__no_endpoints(void)
{
    sock_udp_ep_t ep;

    expect(0 == sock_udp_create(&_sock, NULL, NULL, SOCK_FLAGS_REUSE_EP));
    expect(-EADDRNOTAVAIL == sock_udp_get_local(&_sock, &ep));
    expect(-ENOTCONN == sock_udp_get_remote(&_sock, &ep));
}

static void test_sock_udp_create__only_local(void)
{
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    sock_udp_ep_t ep;

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_udp_get_local(&_sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(_TEST_PORT_LOCAL == ep.port);
    expect(-ENOTCONN == sock_udp_get_remote(&_sock, &ep));
}

static void test_sock_udp_create__only_local_port0(void)
{
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = 0U };
    sock_udp_ep_t ep;

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_udp_get_local(&_sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(0U != ep.port);
    expect(-ENOTCONN == sock_udp_get_remote(&_sock, &ep));
}

static void test_sock_udp_create__only_local_reuse_ep(void)
{
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    sock_udp_ep_t ep, ep2;

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_udp_create(&_sock2, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_udp_get_local(&_sock, &ep));
    expect(0 == sock_udp_get_local(&_sock2, &ep2));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(_TEST_PORT_LOCAL == ep.port);
    expect(-ENOTCONN == sock_udp_get_remote(&_sock, &ep));
    expect(AF_INET6 == ep2.family);
    expect(memcmp(&ipv6_addr_unspecified, &ep2.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep2.netif);
    expect(_TEST_PORT_LOCAL == ep2.port);
    expect(-ENOTCONN == sock_udp_get_remote(&_sock, &ep2));
    sock_udp_close(&_sock2);
}

static void test_sock_udp_create__only_remote(void)
{
    static const ipv6_addr_t remote_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .addr = { .ipv6 = _TEST_ADDR_REMOTE } };
    sock_udp_ep_t ep;

    expect(0 == sock_udp_create(&_sock, NULL, &remote, SOCK_FLAGS_REUSE_EP));
    expect(-EADDRNOTAVAIL == sock_udp_get_local(&_sock, &ep));
    expect(0 == sock_udp_get_remote(&_sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&remote_addr, &ep.addr.ipv6, sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(_TEST_PORT_REMOTE == ep.port);
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

    expect(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_udp_get_local(&_sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    expect(_TEST_NETIF == ep.netif);
    expect(_TEST_PORT_LOCAL == ep.port);
    expect(0 == sock_udp_get_remote(&_sock, &ep));
    expect(AF_INET6 == ep.family);
    expect(memcmp(&remote_addr, &ep.addr.ipv6, sizeof(ipv6_addr_t)) == 0);
    expect(SOCK_ADDR_ANY_NETIF == ep.netif);
    expect(_TEST_PORT_REMOTE == ep.port);
}

static void test_sock_udp_recv__EADDRNOTAVAIL(void)
{
    expect(0 == sock_udp_create(&_sock, NULL, NULL, SOCK_FLAGS_REUSE_EP));

    expect(-EADDRNOTAVAIL == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer),
                                           SOCK_NO_TIMEOUT, NULL));
}

static void test_sock_udp_recv__EAGAIN(void)
{
    static const sock_udp_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF,
                                         .port = _TEST_PORT_LOCAL };

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));

    expect(-EAGAIN == sock_udp_recv(&_sock, _test_buffer, sizeof(_test_buffer),
                                    0, NULL));
}

static void test_sock_udp_recv__ENOBUFS(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"), _TEST_NETIF));
    expect(-ENOBUFS == sock_udp_recv(&_sock, _test_buffer, 2, SOCK_NO_TIMEOUT,
                                     NULL));
    expect(_check_net());
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

    expect(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    expect(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    expect(-EPROTO == sock_udp_recv(&_sock, _test_buffer, sizeof(_test_buffer),
                                    SOCK_NO_TIMEOUT, NULL));
    expect(_check_net());
}

static void test_sock_udp_recv__ETIMEDOUT(void)
{
    static const sock_udp_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF,
                                         .port = _TEST_PORT_LOCAL };

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));

    puts(" * Calling sock_udp_recv()");
    expect(-ETIMEDOUT == sock_udp_recv(&_sock, _test_buffer,
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

    expect(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    expect(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    expect(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer),
                                           SOCK_NO_TIMEOUT, NULL));
    expect(_check_net());
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

    expect(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    expect(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    expect(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer),
                                           SOCK_NO_TIMEOUT, &result));
    expect(AF_INET6 == result.family);
    expect(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    expect(_TEST_PORT_REMOTE == result.port);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
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

    expect(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    expect(0 == sock_udp_get_local(&_sock, &local));
    expect(0 != local.port);
    expect(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          local.port, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    expect(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer),
                                           SOCK_NO_TIMEOUT, &result));
    expect(AF_INET6 == result.family);
    expect(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    expect(_TEST_PORT_REMOTE == result.port);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
}

static void test_sock_udp_recv__unsocketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                         .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    expect(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer),
                                           SOCK_NO_TIMEOUT, NULL));
    expect(_check_net());
}

static void test_sock_udp_recv__unsocketed_with_remote(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    sock_udp_ep_t result;

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    expect(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer),
                                           SOCK_NO_TIMEOUT, &result));
    expect(AF_INET6 == result.family);
    expect(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    expect(_TEST_PORT_REMOTE == result.port);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
}

static void test_sock_udp_recv__with_timeout(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    sock_udp_ep_t result;

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    expect(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer), _TEST_TIMEOUT,
                                           &result));
    expect(AF_INET6 == result.family);
    expect(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    expect(_TEST_PORT_REMOTE == result.port);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
}

static void test_sock_udp_recv__non_blocking(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    sock_udp_ep_t result;

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    expect(sizeof("ABCD") == sock_udp_recv(&_sock, _test_buffer,
                                           sizeof(_test_buffer), 0, &result));
    expect(AF_INET6 == result.family);
    expect(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    expect(_TEST_PORT_REMOTE == result.port);
    expect(_TEST_NETIF == result.netif);
    expect(_check_net());
}

static void test_sock_udp_recv__aux(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    static const inject_aux_t inject_aux = { .timestamp = 1337, .rssi = -11 };
    sock_udp_ep_t result;
    sock_udp_aux_rx_t aux = {
        .flags = SOCK_AUX_GET_LOCAL | SOCK_AUX_GET_TIMESTAMP | SOCK_AUX_GET_RSSI
    };

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(_inject_packet_aux(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                              _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                              _TEST_NETIF, &inject_aux));
    expect(sizeof("ABCD") == sock_udp_recv_aux(&_sock, _test_buffer,
                                               sizeof(_test_buffer), 0,
                                               &result, &aux));
    expect(AF_INET6 == result.family);
    expect(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    expect(_TEST_PORT_REMOTE == result.port);
    expect(_TEST_NETIF == result.netif);
#if IS_USED(MODULE_SOCK_AUX_LOCAL)
    expect(!(aux.flags & SOCK_AUX_GET_LOCAL));
    expect(memcmp(&aux.local.addr, &dst_addr, sizeof(dst_addr)) == 0);
    expect(_TEST_PORT_LOCAL == aux.local.port);
#else
    expect(aux.flags & SOCK_AUX_GET_LOCAL);
#endif
#if IS_USED(MODULE_SOCK_AUX_TIMESTAMP)
    expect(!(aux.flags & SOCK_AUX_GET_TIMESTAMP));
    expect(inject_aux.timestamp == aux.timestamp);
#else
    expect(aux.flags & SOCK_AUX_GET_TIMESTAMP);
#endif
#if IS_USED(MODULE_SOCK_AUX_RSSI)
    expect(!(aux.flags & SOCK_AUX_GET_RSSI));
    expect(inject_aux.rssi == aux.rssi);
#else
    expect(aux.flags & SOCK_AUX_GET_RSSI);
#endif
    expect(_check_net());
}

static void test_sock_udp_recv_buf__success(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };
    void *data = NULL, *ctx = NULL;

    assert(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PORT_REMOTE,
                          _TEST_PORT_LOCAL, "ABCD", sizeof("ABCD"),
                          _TEST_NETIF));
    assert(sizeof("ABCD") == sock_udp_recv_buf(&_sock, &data, &ctx,
                                               SOCK_NO_TIMEOUT, NULL));
    assert(data != NULL);
    assert(ctx != NULL);
    assert(0 == sock_udp_recv_buf(&_sock, &data, &ctx, SOCK_NO_TIMEOUT, NULL));
    assert(data == NULL);
    assert(ctx == NULL);
    assert(_check_net());
}

static void test_sock_udp_send__EAFNOSUPPORT(void)
{
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET,
                                          .port = _TEST_PORT_REMOTE };

    expect(-EAFNOSUPPORT == sock_udp_send(NULL, "ABCD", sizeof("ABCD"),
                                          &remote));
    expect(_check_net());
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

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(-EINVAL == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"), &remote));
    expect(_check_net());
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

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(-EINVAL == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"), &remote));
    expect(_check_net());
}

static void test_sock_udp_send__EINVAL_port(void)
{
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6 };

    expect(-EINVAL == sock_udp_send(NULL, "ABCD", sizeof("ABCD"), &remote));
    expect(_check_net());
}

static void test_sock_udp_send__ENOTCONN(void)
{
    expect(0 == sock_udp_create(&_sock, NULL, NULL, SOCK_FLAGS_REUSE_EP));
    expect(-ENOTCONN == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"), NULL));
    expect(_check_net());
}

static void test_sock_udp_send__socketed_no_local_no_netif(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    expect(0 == sock_udp_create(&_sock, NULL, &remote, SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           NULL));
    expect(_check_packet(&ipv6_addr_unspecified, &dst_addr, 0,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         SOCK_ADDR_ANY_NETIF, true));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    expect(_check_net());
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

    expect(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           NULL));
    expect(_check_packet(&src_addr, &dst_addr, _TEST_PORT_LOCAL,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         SOCK_ADDR_ANY_NETIF, false));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    expect(_check_net());
}

static void test_sock_udp_send__socketed_no_local(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .netif = _TEST_NETIF,
                                          .port = _TEST_PORT_REMOTE };

    expect(0 == sock_udp_create(&_sock, NULL, &remote, SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           NULL));
    expect(_check_packet(&ipv6_addr_unspecified, &dst_addr, 0,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"), _TEST_NETIF,
                         true));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    expect(_check_net());
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

    expect(0 == sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           NULL));
    expect(_check_packet(&src_addr, &dst_addr, _TEST_PORT_LOCAL,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         _TEST_NETIF, false));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    expect(_check_net());
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

    expect(0 == sock_udp_create(&_sock, &local, &sock_remote, SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           &remote));
    expect(_check_packet(&src_addr, &dst_addr, _TEST_PORT_LOCAL,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         _TEST_NETIF, false));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    expect(_check_net());
}

static void test_sock_udp_send__unsocketed_no_local_no_netif(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    expect(0 == sock_udp_create(&_sock, NULL, NULL, SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           &remote));
    expect(_check_packet(&ipv6_addr_unspecified, &dst_addr, 0,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         SOCK_ADDR_ANY_NETIF, true));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    expect(_check_net());
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

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           &remote));
    expect(_check_packet(&src_addr, &dst_addr, _TEST_PORT_LOCAL,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         SOCK_ADDR_ANY_NETIF, false));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    expect(_check_net());
}

static void test_sock_udp_send__unsocketed_no_local(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .netif = _TEST_NETIF,
                                          .port = _TEST_PORT_REMOTE };

    expect(0 == sock_udp_create(&_sock, NULL, NULL, SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           &remote));
    expect(_check_packet(&ipv6_addr_unspecified, &dst_addr, 0,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"), _TEST_NETIF,
                         true));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    expect(_check_net());
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

    expect(0 == sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(sizeof("ABCD") == sock_udp_send(&_sock, "ABCD", sizeof("ABCD"),
                                           &remote));
    expect(_check_packet(&src_addr, &dst_addr, _TEST_PORT_LOCAL,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         _TEST_NETIF, false));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    expect(_check_net());
}

static void test_sock_udp_send__no_sock_no_netif(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE };

    expect(sizeof("ABCD") == sock_udp_send(NULL, "ABCD", sizeof("ABCD"),
                                           &remote));
    expect(_check_packet(&ipv6_addr_unspecified, &dst_addr, 0,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         SOCK_ADDR_ANY_NETIF, true));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    expect(_check_net());
}

static void test_sock_udp_send__no_sock(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                          .family = AF_INET6,
                                          .netif = _TEST_NETIF,
                                          .port = _TEST_PORT_REMOTE };

    expect(sizeof("ABCD") == sock_udp_send(NULL, "ABCD", sizeof("ABCD"),
                                           &remote));
    expect(_check_packet(&ipv6_addr_unspecified, &dst_addr, 0,
                         _TEST_PORT_REMOTE, "ABCD", sizeof("ABCD"),
                         _TEST_NETIF, true));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    expect(_check_net());
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
    CALL(test_sock_udp_recv__aux());
    CALL(test_sock_udp_recv_buf__success());
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
