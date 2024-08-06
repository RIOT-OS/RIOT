/*
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Posix netdb test application
 *
 * @author  Hendrik van Essen <hendrik.ve@fu-berlin.de>
 *
 * @}
 */

#include "kernel_defines.h"

#include <stdio.h>

#include <arpa/inet.h>
#include <netdb.h>

#include "net/dns_mock.h"
#include "net/sock/dns.h"

#include "embUnit.h"

static void test_posix_netdb_getaddrinfo__EAI_BADFLAGS(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    /* non-existing flag */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= 0xff;
    TEST_ASSERT_EQUAL_INT(EAI_BADFLAGS,
                          getaddrinfo(SOCK_DNS_MOCK_EXAMPLE_COM_HOSTNAME, NULL, &hints, &result));
    freeaddrinfo(result);

    /* invalid combination: AI_CANONNAME && nodename == NULL */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_CANONNAME;
    TEST_ASSERT_EQUAL_INT(EAI_BADFLAGS, getaddrinfo(NULL, "0", &hints, &result));
    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__EAI_FAIL(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints = { 0 };

    /* res = NULL */
    TEST_ASSERT_EQUAL_INT(EAI_FAIL,
                          getaddrinfo(SOCK_DNS_MOCK_EXAMPLE_COM_HOSTNAME, NULL, &hints, NULL));

    /* nodename too long */
    char nodename[2 * SOCK_DNS_MAX_NAME_LEN] = { 0 };
    for (size_t i = 0; i < sizeof(nodename) - 1; i++) {
        nodename[i] = 'a';
    }
    TEST_ASSERT_EQUAL_INT(EAI_FAIL, getaddrinfo(nodename, NULL, &hints, &result));
    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__EAI_FAMILY(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    /* non-supported address family */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC + AF_INET + AF_INET6;
    TEST_ASSERT_EQUAL_INT(EAI_FAMILY,
                          getaddrinfo(SOCK_DNS_MOCK_EXAMPLE_COM_HOSTNAME, NULL, &hints, &result));
    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__EAI_NONAME(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    /* neither nodename nor servname given */
    memset(&hints, 0, sizeof(struct addrinfo));
    TEST_ASSERT_EQUAL_INT(EAI_NONAME, getaddrinfo(NULL, NULL, &hints, &result));
    freeaddrinfo(result);

    /* nodename and servname are empty strings */
    memset(&hints, 0, sizeof(struct addrinfo));
    TEST_ASSERT_EQUAL_INT(EAI_NONAME, getaddrinfo("", "", &hints, &result));
    freeaddrinfo(result);

    /* invalid IPv4 address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_INET;
    TEST_ASSERT_EQUAL_INT(EAI_NONAME, getaddrinfo("1.2.3.4.5",NULL, &hints, &result));
    freeaddrinfo(result);

    /* invalid IPv6 address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_INET6;
    TEST_ASSERT_EQUAL_INT(EAI_NONAME, getaddrinfo("1:2:3:4:5:6:7:8:9",NULL, &hints, &result));
    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__EAI_SERVICE(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    /* non-numeric port */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICSERV;
    TEST_ASSERT_EQUAL_INT(EAI_SERVICE, getaddrinfo(NULL, "abc", &hints, &result));
    freeaddrinfo(result);

    /* invalid port ( port < 0 ) */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICSERV;
    TEST_ASSERT_EQUAL_INT(EAI_SERVICE, getaddrinfo(NULL, "-1", &hints, &result));
    freeaddrinfo(result);

    /* invalid port ( port > 65535 ) */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICSERV;
    TEST_ASSERT_EQUAL_INT(EAI_SERVICE, getaddrinfo(NULL, "70000", &hints, &result));
    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__AI_NUMERICHOST(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    char* test_ipv4 = "1.2.3.4";
    char addr4_str[INET_ADDRSTRLEN];

    /* explicit IPv4 address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_INET;
    TEST_ASSERT_EQUAL_INT(0, getaddrinfo(test_ipv4, NULL, &hints, &result));
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(AF_INET, result->ai_family);
    TEST_ASSERT_NOT_NULL(inet_ntop(AF_INET, &((struct sockaddr_in*)(result->ai_addr))->sin_addr,
                                   addr4_str, sizeof(addr4_str)));
    TEST_ASSERT_EQUAL_INT(0, strcmp(addr4_str, test_ipv4));
    freeaddrinfo(result);

    /* auto-detect IPv4 address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    TEST_ASSERT_EQUAL_INT(0, getaddrinfo(test_ipv4, NULL, &hints, &result));
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(AF_INET, result->ai_family);
    TEST_ASSERT_NOT_NULL(inet_ntop(AF_INET, &((struct sockaddr_in*)(result->ai_addr))->sin_addr,
                                   addr4_str, sizeof(addr4_str)));
    TEST_ASSERT_EQUAL_INT(0, strcmp(addr4_str, test_ipv4));
    freeaddrinfo(result);


    char* test_ipv6 = "1:2:3:4:5:6:7:8";
    char addr6_str[INET6_ADDRSTRLEN];

    /* explicit IPv6 address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_INET6;
    TEST_ASSERT_EQUAL_INT(0, getaddrinfo(test_ipv6, NULL, &hints, &result));
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(AF_INET6, result->ai_family);
    TEST_ASSERT_NOT_NULL(inet_ntop(AF_INET6, &((struct sockaddr_in6*)(result->ai_addr))->sin6_addr,
                                   addr6_str, sizeof(addr6_str)));
    TEST_ASSERT_EQUAL_INT(0, strcmp(addr6_str, test_ipv6));
    freeaddrinfo(result);

    /* auto-detect IPv6 address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    TEST_ASSERT_EQUAL_INT(0, getaddrinfo(test_ipv6, NULL, &hints, &result));
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(AF_INET6, result->ai_family);
    TEST_ASSERT_NOT_NULL(inet_ntop(AF_INET6, &((struct sockaddr_in6*)(result->ai_addr))->sin6_addr,
                                   addr6_str, sizeof(addr6_str)));
    TEST_ASSERT_EQUAL_INT(0, strcmp(addr6_str, test_ipv6));
    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__AI_NUMERICSERV(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints = { 0 };
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_flags |= AI_NUMERICSERV;

    char* ip_addr = "1.2.3.4";
    hints.ai_family = AF_INET;

    TEST_ASSERT_EQUAL_INT(0, getaddrinfo(ip_addr, "12345", &hints, &result));
    TEST_ASSERT_NOT_NULL(result);
    /* sockaddr_in or sockaddr_in6 does not matter if we only access the port */
    TEST_ASSERT_EQUAL_INT(12345, ntohs(((struct sockaddr_in *)result->ai_addr)->sin_port));
    freeaddrinfo(result);
}

static void _test_posix_netdb_getaddrinfo__ai_family(int family)
{
    TEST_ASSERT(family == AF_UNSPEC || family == AF_INET || family == AF_INET6);

    struct addrinfo *result = NULL;
    struct addrinfo hints = { 0 };
    hints.ai_family = family;

    int rc = getaddrinfo(SOCK_DNS_MOCK_EXAMPLE_COM_HOSTNAME, NULL, &hints, &result);
    TEST_ASSERT_EQUAL_INT(0, rc);
    TEST_ASSERT_NOT_NULL(result);

    if (hints.ai_family != AF_UNSPEC) {
        TEST_ASSERT_EQUAL_INT(result->ai_family, hints.ai_family);
    }

    ipv4_addr_t addr_ipv4;
    ipv6_addr_t addr_ipv6;

    switch (result->ai_family) {
    case AF_INET:
        memcpy(&addr_ipv4, &((struct sockaddr_in*)(result->ai_addr))->sin_addr.s_addr, sizeof(ipv4_addr_t));
        TEST_ASSERT(ipv4_addr_equal(&sock_dns_mock_example_com_addr_ipv4, &addr_ipv4));
        break;
    case AF_INET6:
        memcpy(&addr_ipv6, &((struct sockaddr_in6*)(result->ai_addr))->sin6_addr.s6_addr, sizeof(ipv6_addr_t));
        TEST_ASSERT(ipv6_addr_equal(&sock_dns_mock_example_com_addr_ipv6, &addr_ipv6));
        break;
    default:
        TEST_ASSERT(false);
    }

    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__AF_UNSPEC(void)
{
    _test_posix_netdb_getaddrinfo__ai_family(AF_UNSPEC);
}

static void test_posix_netdb_getaddrinfo__AF_INET(void)
{
    _test_posix_netdb_getaddrinfo__ai_family(AF_INET);
}

static void test_posix_netdb_getaddrinfo__AF_INET6(void)
{
    _test_posix_netdb_getaddrinfo__ai_family(AF_INET6);
}

Test *tests_posix_netdb(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        /* test error codes */
        new_TestFixture(test_posix_netdb_getaddrinfo__EAI_BADFLAGS),
        new_TestFixture(test_posix_netdb_getaddrinfo__EAI_FAIL),
        new_TestFixture(test_posix_netdb_getaddrinfo__EAI_FAMILY),
        new_TestFixture(test_posix_netdb_getaddrinfo__EAI_NONAME),
        new_TestFixture(test_posix_netdb_getaddrinfo__EAI_SERVICE),
        /* test input flags */
        new_TestFixture(test_posix_netdb_getaddrinfo__AI_NUMERICHOST),
        new_TestFixture(test_posix_netdb_getaddrinfo__AI_NUMERICSERV),
        new_TestFixture(test_posix_netdb_getaddrinfo__AF_UNSPEC),
        new_TestFixture(test_posix_netdb_getaddrinfo__AF_INET),
        new_TestFixture(test_posix_netdb_getaddrinfo__AF_INET6),
    };

    EMB_UNIT_TESTCALLER(posix_netdb_tests, NULL, NULL, fixtures);

    return (Test *)&posix_netdb_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_posix_netdb());
    TESTS_END();

    return 0;
}
