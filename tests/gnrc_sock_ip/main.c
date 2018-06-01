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

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "net/sock/ip.h"
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

static void test_sock_ip_create__EAFNOSUPPORT(void)
{
    static const sock_ip_ep_t local = { .family = AF_UNSPEC };
    static const sock_ip_ep_t remote = { .family = AF_UNSPEC };

    assert(-EAFNOSUPPORT == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                                           SOCK_FLAGS_REUSE_EP));
    assert(-EAFNOSUPPORT == sock_ip_create(&_sock, NULL, &remote, _TEST_PROTO,
                                           SOCK_FLAGS_REUSE_EP));
}

static void test_sock_ip_create__EINVAL_addr(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .family = AF_INET6,
                                         .netif = _TEST_NETIF };

    assert(-EINVAL == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                                     SOCK_FLAGS_REUSE_EP));
}

static void test_sock_ip_create__EINVAL_netif(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .family = AF_INET6,
                                         .netif = (_TEST_NETIF + 1),
                                         .addr = { .ipv6 = _TEST_ADDR_REMOTE } };

    assert(-EINVAL == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                                     SOCK_FLAGS_REUSE_EP));
}

static void test_sock_ip_create__no_endpoints(void)
{
    sock_ip_ep_t ep;

    assert(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(-EADDRNOTAVAIL == sock_ip_get_local(&_sock, &ep));
    assert(-ENOTCONN == sock_ip_get_remote(&_sock, &ep));
}

static void test_sock_ip_create__only_local(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    sock_ip_ep_t ep;

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(0 == sock_ip_get_local(&_sock, &ep));
    assert(AF_INET6 == ep.family);
    assert(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    assert(SOCK_ADDR_ANY_NETIF == ep.netif);
    assert(-ENOTCONN == sock_ip_get_remote(&_sock, &ep));
}

static void test_sock_ip_create__only_local_reuse_ep(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    sock_ip_ep_t ep, ep2;

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(0 == sock_ip_create(&_sock2, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(0 == sock_ip_get_local(&_sock, &ep));
    assert(0 == sock_ip_get_local(&_sock2, &ep2));
    assert(AF_INET6 == ep.family);
    assert(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    assert(SOCK_ADDR_ANY_NETIF == ep.netif);
    assert(-ENOTCONN == sock_ip_get_remote(&_sock, &ep));
    assert(AF_INET6 == ep2.family);
    assert(memcmp(&ipv6_addr_unspecified, &ep2.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    assert(SOCK_ADDR_ANY_NETIF == ep2.netif);
    assert(-ENOTCONN == sock_ip_get_remote(&_sock, &ep2));
    sock_ip_close(&_sock2);
}

static void test_sock_ip_create__only_remote(void)
{
    static const ipv6_addr_t remote_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t remote = { .family = AF_INET6,
                                         .addr = { .ipv6 = _TEST_ADDR_REMOTE } };
    sock_ip_ep_t ep;

    assert(0 == sock_ip_create(&_sock, NULL, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(-EADDRNOTAVAIL == sock_ip_get_local(&_sock, &ep));
    assert(0 == sock_ip_get_remote(&_sock, &ep));
    assert(AF_INET6 == ep.family);
    assert(memcmp(&remote_addr, &ep.addr.ipv6, sizeof(ipv6_addr_t)) == 0);
    assert(SOCK_ADDR_ANY_NETIF == ep.netif);
}

static void test_sock_ip_create__full(void)
{
    static const ipv6_addr_t remote_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .family = AF_INET6,
                                         .addr = { .ipv6 = _TEST_ADDR_REMOTE } };
    sock_ip_ep_t ep;

    assert(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(0 == sock_ip_get_local(&_sock, &ep));
    assert(AF_INET6 == ep.family);
    assert(memcmp(&ipv6_addr_unspecified, &ep.addr.ipv6,
                  sizeof(ipv6_addr_t)) == 0);
    assert(_TEST_NETIF == ep.netif);
    assert(0 == sock_ip_get_remote(&_sock, &ep));
    assert(AF_INET6 == ep.family);
    assert(memcmp(&remote_addr, &ep.addr.ipv6, sizeof(ipv6_addr_t)) == 0);
    assert(SOCK_ADDR_ANY_NETIF == ep.netif);
}

static void test_sock_ip_recv__EADDRNOTAVAIL(void)
{
    assert(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));

    assert(-EADDRNOTAVAIL == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), SOCK_NO_TIMEOUT,
                                          NULL));
}

static void test_sock_ip_recv__EAGAIN(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF };

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));

    assert(-EAGAIN == sock_ip_recv(&_sock, _test_buffer, sizeof(_test_buffer),
                                   0, NULL));
}

static void test_sock_ip_recv__ENOBUFS(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    assert(-ENOBUFS == sock_ip_recv(&_sock, _test_buffer, 2, SOCK_NO_TIMEOUT,
                                    NULL));
    assert(_check_net());
}

static void test_sock_ip_recv__EPROTO(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_WRONG };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6 };

    assert(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    assert(-EPROTO == sock_ip_recv(&_sock, _test_buffer, sizeof(_test_buffer),
                                   SOCK_NO_TIMEOUT, NULL));
    assert(_check_net());
}

static void test_sock_ip_recv__ETIMEDOUT(void)
{
    static const sock_ip_ep_t local = { .family = AF_INET6, .netif = _TEST_NETIF };

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));

    puts(" * Calling sock_ip_recv()");
    assert(-ETIMEDOUT == sock_ip_recv(&_sock, _test_buffer,
                                      sizeof(_test_buffer), _TEST_TIMEOUT,
                                      NULL));
    printf(" * (timed out with timeout %lu)\n", (long unsigned)_TEST_TIMEOUT);
}

static void test_sock_ip_recv__socketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6 };

    assert(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    assert(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), SOCK_NO_TIMEOUT,
                                          NULL));
    assert(_check_net());
}

static void test_sock_ip_recv__socketed_with_remote(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6 };
    sock_ip_ep_t result;

    assert(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    assert(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), SOCK_NO_TIMEOUT,
                                          &result));
    assert(AF_INET6 == result.family);
    assert(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    assert(_TEST_NETIF == result.netif);
    assert(_check_net());
}

static void test_sock_ip_recv__unsocketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                        .family = AF_INET6 };

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    assert(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), SOCK_NO_TIMEOUT,
                                          NULL));
    assert(_check_net());
}

static void test_sock_ip_recv__unsocketed_with_remote(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    sock_ip_ep_t result;

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    assert(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), SOCK_NO_TIMEOUT,
                                          &result));
    assert(AF_INET6 == result.family);
    assert(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    assert(_TEST_NETIF == result.netif);
    assert(_check_net());
}

static void test_sock_ip_recv__with_timeout(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    sock_ip_ep_t result;

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    assert(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), _TEST_TIMEOUT,
                                          &result));
    assert(AF_INET6 == result.family);
    assert(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    assert(_TEST_NETIF == result.netif);
    assert(_check_net());
}

static void test_sock_ip_recv__non_blocking(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const sock_ip_ep_t local = { .family = AF_INET6 };
    sock_ip_ep_t result;

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(_inject_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                          sizeof("ABCD"), _TEST_NETIF));
    assert(sizeof("ABCD") == sock_ip_recv(&_sock, _test_buffer,
                                          sizeof(_test_buffer), 0, &result));
    assert(AF_INET6 == result.family);
    assert(memcmp(&result.addr, &src_addr, sizeof(result.addr)) == 0);
    assert(_TEST_NETIF == result.netif);
    assert(_check_net());
}

static void test_sock_ip_send__EAFNOSUPPORT(void)
{
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET };

    assert(-EAFNOSUPPORT == sock_ip_send(NULL, "ABCD", sizeof("ABCD"),
                                         _TEST_PROTO, &remote));
    assert(_check_net());
}

static void test_sock_ip_send__EINVAL_addr(void)
{
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                        .family = AF_INET6,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .family = AF_INET6,
                                         .netif = _TEST_NETIF };

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(-EINVAL == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"), _TEST_PROTO,
                                   &remote));
    assert(_check_net());
}

static void test_sock_ip_send__EINVAL_netif(void)
{
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                        .family = AF_INET6,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6,
                                         .netif = _TEST_NETIF + 1 };

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(-EINVAL == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"), _TEST_PROTO,
                                   &remote));
    assert(_check_net());
}

static void test_sock_ip_send__ENOTCONN(void)
{
    assert(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(-ENOTCONN == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                     _TEST_PROTO, NULL));
    assert(_check_net());
}

static void test_sock_ip_send__socketed_no_local_no_netif(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6 };

    assert(0 == sock_ip_create(&_sock, NULL, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, NULL));
    assert(_check_packet(&ipv6_addr_unspecified, &dst_addr, _TEST_PROTO, "ABCD",
                         sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_ip_send__socketed_no_netif(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                        .family = AF_INET6 };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6 };

    assert(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, NULL));
    assert(_check_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                         sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_ip_send__socketed_no_local(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6,
                                         .netif = _TEST_NETIF };

    assert(0 == sock_ip_create(&_sock, NULL, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, NULL));
    assert(_check_packet(&ipv6_addr_unspecified, &dst_addr, _TEST_PROTO, "ABCD",
                         sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_ip_send__socketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                        .family = AF_INET6,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6 };

    assert(0 == sock_ip_create(&_sock, &local, &remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, NULL));
    assert(_check_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                         sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_ip_send__socketed_other_remote(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                        .family = AF_INET6,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t sock_remote = { .addr = { .ipv6 = _TEST_ADDR_WRONG },
                                              .family = AF_INET6 };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6 };

    assert(0 == sock_ip_create(&_sock, &local, &sock_remote, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    assert(_check_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                         sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_ip_send__unsocketed_no_local_no_netif(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6 };

    assert(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    assert(_check_packet(&ipv6_addr_unspecified, &dst_addr, _TEST_PROTO, "ABCD",
                         sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_ip_send__unsocketed_no_netif(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                        .family = AF_INET6 };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6 };

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    assert(_check_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                         sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_ip_send__unsocketed_no_local(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6,
                                         .netif = _TEST_NETIF };

    assert(0 == sock_ip_create(&_sock, NULL, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    assert(_check_packet(&ipv6_addr_unspecified, &dst_addr, _TEST_PROTO, "ABCD",
                         sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_ip_send__unsocketed(void)
{
    static const ipv6_addr_t src_addr = { .u8 = _TEST_ADDR_LOCAL };
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t local = { .addr = { .ipv6 = _TEST_ADDR_LOCAL },
                                        .family = AF_INET6,
                                        .netif = _TEST_NETIF };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6 };

    assert(0 == sock_ip_create(&_sock, &local, NULL, _TEST_PROTO,
                               SOCK_FLAGS_REUSE_EP));
    assert(sizeof("ABCD") == sock_ip_send(&_sock, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    assert(_check_packet(&src_addr, &dst_addr, _TEST_PROTO, "ABCD",
                         sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_ip_send__no_sock_no_netif(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6 };

    assert(sizeof("ABCD") == sock_ip_send(NULL, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    assert(_check_packet(&ipv6_addr_unspecified, &dst_addr, _TEST_PROTO, "ABCD",
                         sizeof("ABCD"), SOCK_ADDR_ANY_NETIF));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

static void test_sock_ip_send__no_sock(void)
{
    static const ipv6_addr_t dst_addr = { .u8 = _TEST_ADDR_REMOTE };
    static const sock_ip_ep_t remote = { .addr = { .ipv6 = _TEST_ADDR_REMOTE },
                                         .family = AF_INET6,
                                         .netif = _TEST_NETIF };

    assert(sizeof("ABCD") == sock_ip_send(NULL, "ABCD", sizeof("ABCD"),
                                          _TEST_PROTO, &remote));
    assert(_check_packet(&ipv6_addr_unspecified, &dst_addr, _TEST_PROTO, "ABCD",
                         sizeof("ABCD"), _TEST_NETIF));
    xtimer_usleep(1000);    /* let GNRC stack finish */
    assert(_check_net());
}

int main(void)
{
    _net_init();
    tear_down();
    /* EADDRINUSE does not apply for GNRC */
    CALL(test_sock_ip_create__EAFNOSUPPORT());
    CALL(test_sock_ip_create__EINVAL_addr());
    CALL(test_sock_ip_create__EINVAL_netif());
    /* EPROTONOSUPPORT does not apply for gnrc_ipv6 */
    CALL(test_sock_ip_create__no_endpoints());
    CALL(test_sock_ip_create__only_local());
    CALL(test_sock_ip_create__only_local_reuse_ep());
    CALL(test_sock_ip_create__only_remote());
    CALL(test_sock_ip_create__full());
    /* sock_ip_close() is tested in tear_down() */
    /* sock_ip_get_local() is tested in sock_ip_create() tests */
    /* sock_ip_get_remote() is tested in sock_ip_create() tests */
    CALL(test_sock_ip_recv__EADDRNOTAVAIL());
    CALL(test_sock_ip_recv__EAGAIN());
    CALL(test_sock_ip_recv__ENOBUFS());
    CALL(test_sock_ip_recv__EPROTO());
    CALL(test_sock_ip_recv__ETIMEDOUT());
    CALL(test_sock_ip_recv__socketed());
    CALL(test_sock_ip_recv__socketed_with_remote());
    CALL(test_sock_ip_recv__unsocketed());
    CALL(test_sock_ip_recv__unsocketed_with_remote());
    CALL(test_sock_ip_recv__with_timeout());
    CALL(test_sock_ip_recv__non_blocking());
    _prepare_send_checks();
    CALL(test_sock_ip_send__EAFNOSUPPORT());
    CALL(test_sock_ip_send__EINVAL_addr());
    CALL(test_sock_ip_send__EINVAL_netif());
    CALL(test_sock_ip_send__ENOTCONN());
    /* EPROTOTYPE does not apply for gnrc_ipv6 */
    CALL(test_sock_ip_send__socketed_no_local_no_netif());
    CALL(test_sock_ip_send__socketed_no_netif());
    CALL(test_sock_ip_send__socketed_no_local());
    CALL(test_sock_ip_send__socketed());
    CALL(test_sock_ip_send__socketed_other_remote());
    CALL(test_sock_ip_send__unsocketed_no_local_no_netif());
    CALL(test_sock_ip_send__unsocketed_no_netif());
    CALL(test_sock_ip_send__unsocketed_no_local());
    CALL(test_sock_ip_send__unsocketed());
    CALL(test_sock_ip_send__no_sock_no_netif());
    CALL(test_sock_ip_send__no_sock());

    puts("ALL TESTS SUCCESSFUL");

    return 0;
}
