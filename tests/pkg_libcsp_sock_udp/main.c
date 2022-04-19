/*
 * Copyright (C) 2022 Inria
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
 * @brief       Test for UDP libCSP socket
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "net/sock/udp.h"
#include "net/ipv6/addr.h"

#include "test_utils/expect.h"

#include "constants.h"

#define _TEST_BUFFER_SIZE   (128)

static sock_udp_t _sock, _sock2;

#define CALL(fn)            puts("Calling " # fn); fn; tear_down()

static void tear_down(void)
{
    sock_udp_close(&_sock);
    memset(&_sock, 0, sizeof(_sock));
    memset(&_sock2, 0, sizeof(_sock2));
}

static void test_sock_udp_create__EADDRINUSE(void)
{
    static const sock_udp_ep_t local = { .family = AF_INET6,
                                         .port = _TEST_PORT_LOCAL };

    expect(0 == sock_udp_create(&_sock, &local, NULL, 0));
    expect(-EADDRINUSE == sock_udp_create(&_sock2, &local, NULL, 0));
}

static void test_sock_udp_create__EAFNOSUPPORT(void)
{
    /* port may not be NULL according to doc */
    static const sock_udp_ep_t local = { .family = AF_UNSPEC,
                                         .port = _TEST_PORT_LOCAL };
    /* port may not be NULL according to doc */
    static const sock_udp_ep_t remote = { .family = AF_UNSPEC,
                                          .port = _TEST_PORT_REMOTE };

    expect(-EAFNOSUPPORT ==
           sock_udp_create(&_sock, &local, NULL, SOCK_FLAGS_REUSE_EP));
    expect(-EAFNOSUPPORT ==
           sock_udp_create(&_sock, NULL, &remote, SOCK_FLAGS_REUSE_EP));
}

static void test_sock_udp_create__EINVAL_addr(void)
{
    /* port may not be NULL according to doc */
    static const sock_udp_ep_t local =
    { .family = AF_INET6, .netif = _TEST_NETIF,
      .port = _TEST_PORT_LOCAL };
    /* port may not be NULL according to doc */
    static const sock_udp_ep_t remote = { .family = AF_INET6,
                                          .netif = _TEST_NETIF,
                                          .port = _TEST_PORT_REMOTE };

    expect(-EINVAL ==
           sock_udp_create(&_sock, &local, &remote, SOCK_FLAGS_REUSE_EP));
}

static void test_sock_udp_create__EINVAL_netif(void)
{
    /* port may not be NULL according to doc */
    static const sock_udp_ep_t local_1 =
    { .family = AF_INET6, .netif = _TEST_NETIF,
      .port = _TEST_PORT_LOCAL };
    /* port may not be NULL according to doc */
    static const sock_udp_ep_t remote = { .family = AF_INET6,
                                          .netif = (_TEST_NETIF + 1),
                                          .port = _TEST_PORT_REMOTE,
                                          .addr =
                                          { .ipv6 = _TEST_ADDR_REMOTE } };

    expect(-EINVAL ==
           sock_udp_create(&_sock, &local_1, &remote, SOCK_FLAGS_REUSE_EP));
    static const sock_udp_ep_t local_2 = { .family = AF_INET6,
                                           .netif = _TEST_NETIF + 1,
                                           .port = _TEST_PORT_LOCAL };
    expect(-EINVAL ==
           sock_udp_create(&_sock, &local_2, &remote, SOCK_FLAGS_REUSE_EP));
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

static void test_sock_udp_create__only_remote(void)
{
    static const ipv6_addr_t remote_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_udp_ep_t remote = { .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .addr =
                                          { .ipv6 = _TEST_ADDR_REMOTE } };
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
    static const sock_udp_ep_t local =
    { .family = AF_INET6, .netif = _TEST_NETIF,
      .port = _TEST_PORT_LOCAL };
    static const sock_udp_ep_t remote = { .family = AF_INET6,
                                          .port = _TEST_PORT_REMOTE,
                                          .addr =
                                          { .ipv6 = _TEST_ADDR_REMOTE } };
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

int main(void)
{
    tear_down();

    CALL(test_sock_udp_create__EADDRINUSE());
    CALL(test_sock_udp_create__EAFNOSUPPORT());
    CALL(test_sock_udp_create__EINVAL_addr());
    CALL(test_sock_udp_create__EINVAL_netif());
    CALL(test_sock_udp_create__no_endpoints());
    CALL(test_sock_udp_create__only_local());
    CALL(test_sock_udp_create__only_local_port0());
    CALL(test_sock_udp_create__only_remote());
    CALL(test_sock_udp_create__full());
    /* sock_udp_close() is tested in tear_down() */

    puts("ALL TESTS SUCCESSFUL");

    return 0;
}
