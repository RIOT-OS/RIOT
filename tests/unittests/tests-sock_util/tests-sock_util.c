/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Unit tests for sock_util module
 *
 * @author     Koen Zandberg <koen@bergzand.net>
 */

#include "embUnit.h"
#include "net/sock/util.h"
#include "stdio.h"

#define TEST_IPV6_ADDR      { 0x20, 0x01, 0x0d, 0xb8, \
                              0x00, 0x00, 0x00, 0x00, \
                              0x00, 0x00, 0x00, 0x00, \
                              0x00, 0x00, 0x00, 0x01 }
#define TEST_IPV6_NETIF     60000

#define TEST_IPV6_ADDR_STR          "2001:db8::1"
#define TEST_IPV6_ADDR_NETIF_STR    "2001:db8::1%60000"

#define TEST_IPV6_FMT_UDP_EP        {                               \
                                        .family = AF_INET6,         \
                                        .addr = {                   \
                                            .ipv6 = TEST_IPV6_ADDR, \
                                        },                          \
                                        .port = 53,                 \
                                    }


#define TEST_URL                    "http://[2001:db8::1]:80/local"
#define TEST_URL_HOSTPART           "[2001:db8::1]:80"
#define TEST_URL_LOCALPART          "/local"
#define TEST_URL_NOLOCAL            "coap://[2001:db8::1]"
#define TEST_URL_NOLOCAL_HOSTPART   "[2001:db8::1]"
#define TEST_URL_DNS                "http://test.local/local"
#define TEST_URL_DNS_HOSTPART       "test.local"
#define TEST_URL_INVALID            "[2001:db8::1]://local"
#define TEST_URL_INVALID2           "[2001:db8::1]/local"
#define TEST_URL_LONG_HOSTPORT      "http://veryveryvery.long.hostname.that." \
                                    "doesnt.fit.inside.sixtyfour.characters." \
                                    "of.buffer.space/localpart"
#define TEST_URL_LONG_URLPATH       "http://shorthostname/very/very/long/ " \
                                    "path/that/doesnt/fit/inside/sixtyfour/" \
                                    "chars/of/buffer/space"

#define TEST_STR2EP                 "[2001:db8::1]"
#define TEST_STR2EP_V4              "10.0.0.1"
#define TEST_STR2EP_V4_2            "10.0.0.1:53"
#define TEST_STR2EP_V4_INVALID      "[10.0.0.1]:53"
#define TEST_STR2EP_INVALID         "[2001:db8:a:b:c:d:e:f:1]"
#define TEST_STR2EP_INVALID2        "[2001:db8:a:b:c:d:e:f]:66000"

static char addr[SOCK_URLPATH_MAXLEN];
static char urlpath[SOCK_URLPATH_MAXLEN];


static void setup(void)
{
    /* Force both arrays to contain nonzero content to detect missing null
     * terminator */
    memset(addr, 1, sizeof(addr));
    memset(urlpath, 1, sizeof(urlpath));
}

static void test_sock_util_fmt__netif_unset(void)
{
    sock_udp_ep_t ep = TEST_IPV6_FMT_UDP_EP;
    uint16_t port;
    TEST_ASSERT_EQUAL_INT(strlen(TEST_IPV6_ADDR_STR),
            sock_udp_ep_fmt(&ep, addr, &port));
    TEST_ASSERT_EQUAL_INT(ep.port, port);
    TEST_ASSERT_EQUAL_STRING(TEST_IPV6_ADDR_STR, (char *)addr);
}

static void test_sock_util_fmt__netif_set(void)
{
    sock_udp_ep_t ep = TEST_IPV6_FMT_UDP_EP;
    uint16_t port;
    ep.netif = TEST_IPV6_NETIF;
    TEST_ASSERT_EQUAL_INT(strlen(TEST_IPV6_ADDR_NETIF_STR),
                sock_udp_ep_fmt(&ep, addr, &port));
    TEST_ASSERT_EQUAL_STRING(TEST_IPV6_ADDR_NETIF_STR, (char *)addr);
}

static void test_sock_util_fmt__unsupported(void)
{
    sock_udp_ep_t ep = TEST_IPV6_FMT_UDP_EP;
    uint16_t port;
    ep.family = AF_UNIX; /* Intentionally chosen for testing an unsupported
                            protocol */
    TEST_ASSERT_EQUAL_INT(sock_udp_ep_fmt(&ep, addr, &port), -ENOTSUP);
    TEST_ASSERT_EQUAL_STRING("", (char *)addr);
}

static void test_sock_util_urlsplit__host_path(void)
{
    TEST_ASSERT_EQUAL_INT(0,
            sock_urlsplit(TEST_URL, addr, urlpath));
    TEST_ASSERT_EQUAL_STRING(TEST_URL_HOSTPART, (char*)addr);
    TEST_ASSERT_EQUAL_STRING(TEST_URL_LOCALPART, (char*)urlpath);
}

static void test_sock_util_urlsplit__no_path(void)
{
    TEST_ASSERT_EQUAL_INT(0,
            sock_urlsplit(TEST_URL_NOLOCAL, addr, urlpath));
    TEST_ASSERT_EQUAL_STRING(TEST_URL_NOLOCAL_HOSTPART, (char*)addr);
    TEST_ASSERT_EQUAL_INT(0, strlen(urlpath));
}

static void test_sock_util_urlsplit__dnsname(void)
{
    TEST_ASSERT_EQUAL_INT(0,
            sock_urlsplit(TEST_URL_DNS, addr, urlpath));
    TEST_ASSERT_EQUAL_STRING(TEST_URL_DNS_HOSTPART, (char*)addr);
    TEST_ASSERT_EQUAL_STRING(TEST_URL_LOCALPART, (char*)urlpath);
}

static void test_sock_util_urlsplit__invalid_sep(void)
{
    TEST_ASSERT_EQUAL_INT(-EINVAL,
            sock_urlsplit(TEST_URL_INVALID, addr, urlpath));
}

static void test_sock_util_urlsplit__no_schema(void)
{
    TEST_ASSERT_EQUAL_INT(-EINVAL,
            sock_urlsplit(TEST_URL_INVALID2, addr, urlpath));
}

static void test_sock_util_urlsplit__hostport_too_long(void)
{
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW,
            sock_urlsplit(TEST_URL_LONG_HOSTPORT, addr, urlpath));
}

static void test_sock_util_urlsplit__urlpath_too_long(void)
{
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW,
            sock_urlsplit(TEST_URL_LONG_URLPATH, addr, urlpath));
}

static void test_sock_util_str2ep__ipv6_noport(void)
{
    sock_udp_ep_t ep;
    ep.port = 0;
    TEST_ASSERT_EQUAL_INT(0, sock_udp_str2ep(&ep, TEST_STR2EP));
    TEST_ASSERT_EQUAL_INT(0, ep.port);
    TEST_ASSERT_EQUAL_INT(AF_INET6, ep.family);
}

static void test_sock_util_str2ep__ipv4_noport(void)
{
    sock_udp_ep_t ep;
    ep.port = 0;
    TEST_ASSERT_EQUAL_INT(0, sock_udp_str2ep(&ep, TEST_STR2EP_V4));
    TEST_ASSERT_EQUAL_INT(0, ep.port);
    TEST_ASSERT_EQUAL_INT(AF_INET, ep.family);
}

static void test_sock_util_str2ep__ipv4_port(void)
{
    sock_udp_ep_t ep;
    TEST_ASSERT_EQUAL_INT(0, sock_udp_str2ep(&ep, TEST_STR2EP_V4_2));
    TEST_ASSERT_EQUAL_INT(53, ep.port);
    TEST_ASSERT_EQUAL_INT(AF_INET, ep.family);
}

static void test_sock_util_str2ep__ipv4_bracketed(void)
{
    sock_udp_ep_t ep;
    TEST_ASSERT_EQUAL_INT(-EINVAL, sock_udp_str2ep(&ep,
                                                   TEST_STR2EP_V4_INVALID));
}

static void test_sock_util_str2ep__invalid_ipv6(void)
{
    sock_udp_ep_t ep;
    TEST_ASSERT_EQUAL_INT(-EINVAL, sock_udp_str2ep(&ep, TEST_STR2EP_INVALID));
}

static void test_sock_util_str2ep__invalid_port(void)
{
    sock_udp_ep_t ep;
    TEST_ASSERT_EQUAL_INT(-EINVAL, sock_udp_str2ep(&ep, TEST_STR2EP_INVALID2));
}

Test *tests_sock_util_all(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_sock_util_fmt__netif_unset),
        new_TestFixture(test_sock_util_fmt__netif_set),
        new_TestFixture(test_sock_util_fmt__unsupported),
        new_TestFixture(test_sock_util_urlsplit__host_path),
        new_TestFixture(test_sock_util_urlsplit__no_path),
        new_TestFixture(test_sock_util_urlsplit__dnsname),
        new_TestFixture(test_sock_util_urlsplit__invalid_sep),
        new_TestFixture(test_sock_util_urlsplit__no_schema),
        new_TestFixture(test_sock_util_urlsplit__hostport_too_long),
        new_TestFixture(test_sock_util_urlsplit__urlpath_too_long),
        new_TestFixture(test_sock_util_str2ep__ipv6_noport),
        new_TestFixture(test_sock_util_str2ep__ipv4_noport),
        new_TestFixture(test_sock_util_str2ep__ipv4_port),
        new_TestFixture(test_sock_util_str2ep__ipv4_bracketed),
        new_TestFixture(test_sock_util_str2ep__invalid_ipv6),
        new_TestFixture(test_sock_util_str2ep__invalid_port),
    };

    EMB_UNIT_TESTCALLER(sockutil_tests, setup, NULL, fixtures);
    return (Test *)&sockutil_tests;
}

void tests_sock_util(void)
{
    TESTS_RUN(tests_sock_util_all());
}
