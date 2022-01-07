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

#include <stdio.h>

#include <arpa/inet.h>
#include <netdb.h>

#include "test_utils/expect.h"
#include "net/sock/dns.h"
#include "ztimer.h"

#define CALL(fn)    puts("Calling " # fn); fn;

#define HOST_NAME   "example.com"
#define HOST_IPV4 "93.184.216.34"
#define HOST_IPV6 "2606:2800:220:1:248:1893:25c8:1946"

static void test_posix_netdb_getaddrinfo__EAI_BADFLAGS(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    /* non-existing flag */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= 0xff;
    expect(EAI_BADFLAGS == getaddrinfo(HOST_NAME, NULL, &hints, &result));
    freeaddrinfo(result);

    /* invalid combination: AI_CANONNAME && nodename == NULL */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_CANONNAME;
    expect(EAI_BADFLAGS == getaddrinfo(NULL, "0", &hints, &result));
    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__EAI_FAIL(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints = { 0 };

    /* res = NULL */
    expect(EAI_FAIL == getaddrinfo(HOST_NAME, NULL, &hints, NULL));

    /* nodename too long */
    char nodename[2 * SOCK_DNS_MAX_NAME_LEN] = { 0 };
    for (size_t i = 0; i < sizeof(nodename) - 1; i++) {
        nodename[i] = 'a';
    }
    expect(EAI_FAIL == getaddrinfo(nodename, NULL, &hints, &result));
    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__EAI_FAMILY(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    /* non-supported address family */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC + AF_INET + AF_INET6;
    expect(EAI_FAMILY == getaddrinfo(HOST_NAME, NULL, &hints, &result));
    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__EAI_NONAME(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    /* neither nodename nor servname given */
    memset(&hints, 0, sizeof(struct addrinfo));
    expect(EAI_NONAME == getaddrinfo(NULL, NULL, &hints, &result));
    freeaddrinfo(result);

    /* nodename and servname are empty strings */
    memset(&hints, 0, sizeof(struct addrinfo));
    expect(EAI_NONAME == getaddrinfo("", "", &hints, &result));
    freeaddrinfo(result);

    /* invalid IPv4 address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_INET;
    expect(EAI_NONAME == getaddrinfo("1.2.3.4.5",
                                     NULL, &hints, &result));
    freeaddrinfo(result);

    /* invalid IPv6 address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_INET6;
    expect(EAI_NONAME == getaddrinfo("1:2:3:4:5:6:7:8:9",
                                     NULL, &hints, &result));
    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__EAI_SERVICE(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    /* non-numeric port */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICSERV;
    expect(EAI_SERVICE == getaddrinfo(NULL, "abc", &hints, &result));
    freeaddrinfo(result);

    /* invalid port ( port < 0 ) */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICSERV;
    expect(EAI_SERVICE == getaddrinfo(NULL, "-1", &hints, &result));
    freeaddrinfo(result);

    /* invalid port ( port > 65535 ) */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICSERV;
    expect(EAI_SERVICE == getaddrinfo(NULL, "70000", &hints, &result));
    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__AI_NUMERICHOST(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints;

#ifdef MODULE_IPV4_ADDR
    char* test_ipv4 = "1.2.3.4";
    char addr4_str[INET_ADDRSTRLEN];

    /* explicit IPv4 address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_INET;
    expect(0 == getaddrinfo(test_ipv4, NULL, &hints, &result));
    expect(result != NULL);
    expect(result->ai_family == AF_INET);
    expect(NULL != inet_ntop(AF_INET, &((struct sockaddr_in*)(result->ai_addr))->sin_addr,
                             addr4_str, sizeof(addr4_str)));
    expect(strcmp(addr4_str, test_ipv4) == 0);
    freeaddrinfo(result);

    /* auto-detect IPv4 address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    expect(0 == getaddrinfo(test_ipv4, NULL, &hints, &result));
    expect(result != NULL);
    expect(result->ai_family == AF_INET);
    expect(NULL != inet_ntop(AF_INET, &((struct sockaddr_in*)(result->ai_addr))->sin_addr,
                             addr4_str, sizeof(addr4_str)));
    expect(strcmp(addr4_str, test_ipv4) == 0);
    freeaddrinfo(result);
#endif

#ifdef MODULE_IPV6_ADDR
    char* test_ipv6 = "1:2:3:4:5:6:7:8";
    char addr6_str[INET6_ADDRSTRLEN];

    /* explicit IPv6 address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_INET6;
    expect(0 == getaddrinfo(test_ipv6, NULL, &hints, &result));
    expect(result != NULL);
    expect(result->ai_family == AF_INET6);
    expect(NULL != inet_ntop(AF_INET6, &((struct sockaddr_in6*)(result->ai_addr))->sin6_addr,
                             addr6_str, sizeof(addr6_str)));
    expect(strcmp(addr6_str, test_ipv6) == 0);
    freeaddrinfo(result);

    /* auto-detect IPv6 address */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    expect(0 == getaddrinfo(test_ipv6, NULL, &hints, &result));
    expect(result != NULL);
    expect(result->ai_family == AF_INET6);
    expect(NULL != inet_ntop(AF_INET6, &((struct sockaddr_in6*)(result->ai_addr))->sin6_addr,
                             addr6_str, sizeof(addr6_str)));
    expect(strcmp(addr6_str, test_ipv6) == 0);
    freeaddrinfo(result);
#endif
}

static void test_posix_netdb_getaddrinfo__AI_NUMERICSERV(void)
{
    struct addrinfo *result = NULL;
    struct addrinfo hints = { 0 };
    hints.ai_flags |= AI_NUMERICHOST;
    hints.ai_flags |= AI_NUMERICSERV;

#if defined(MODULE_IPV4_ADDR)
    char* ip_addr = "1.2.3.4";
    hints.ai_family = AF_INET;
#elif defined(MODULE_IPV6_ADDR)
    char* ip_addr = "1:2:3:4:5:6:7:8";
    hints.ai_family = AF_INET6;
#endif

    expect(0 == getaddrinfo(ip_addr, "12345", &hints, &result));
    expect(result != NULL);
    /* sockaddr_in or sockaddr_in6 does not matter if we only access the port */
    expect(ntohs(((struct sockaddr_in *)result->ai_addr)->sin_port) == 12345);
    freeaddrinfo(result);
}

static void _test_posix_netdb_getaddrinfo__ai_family(int family)
{
    expect(family == AF_UNSPEC
        || family == AF_INET
        || family == AF_INET6);

    struct addrinfo *result = NULL;
    struct addrinfo hints = { 0 };
    hints.ai_family = family;

    expect(0 == getaddrinfo(HOST_NAME, NULL, &hints, &result));
    expect(result != NULL);

    if (hints.ai_family != AF_UNSPEC) {
        expect(result->ai_family == hints.ai_family);
    }

    char addr_str[INET6_ADDRSTRLEN];

    switch (result->ai_family) {
    #ifdef MODULE_IPV4_ADDR
        case AF_INET:
            inet_ntop(AF_INET, &((struct sockaddr_in*)(result->ai_addr))->sin_addr,
                    addr_str, sizeof(addr_str));

            expect(strcmp(addr_str, HOST_IPV4) == 0);
            break;
    #endif
    #ifdef MODULE_IPV6_ADDR
        case AF_INET6:
            inet_ntop(AF_INET6, &((struct sockaddr_in6*)(result->ai_addr))->sin6_addr,
                    addr_str, sizeof(addr_str));

            expect(strcmp(addr_str, HOST_IPV6) == 0);
            break;
    #endif
        default:
            expect(false);
    }

    freeaddrinfo(result);
}

static void test_posix_netdb_getaddrinfo__AF_UNSPEC(void)
{
#if defined(MODULE_IPV4_ADDR) && defined(MODULE_IPV6_ADDR)
    _test_posix_netdb_getaddrinfo__ai_family(AF_UNSPEC);
#endif
}

static void test_posix_netdb_getaddrinfo__AF_INET(void)
{
#ifdef MODULE_IPV4_ADDR
    _test_posix_netdb_getaddrinfo__ai_family(AF_INET);
#endif
}

static void test_posix_netdb_getaddrinfo__AF_INET6(void)
{
#ifdef MODULE_IPV6_ADDR
    _test_posix_netdb_getaddrinfo__ai_family(AF_INET6);
#endif
}

int main(void)
{
    /* wait until connection has been established */
    ztimer_sleep(ZTIMER_SEC, 5);

    /* test error return values */
    CALL(test_posix_netdb_getaddrinfo__EAI_BADFLAGS());
    CALL(test_posix_netdb_getaddrinfo__EAI_FAIL());
    CALL(test_posix_netdb_getaddrinfo__EAI_FAMILY());
    CALL(test_posix_netdb_getaddrinfo__EAI_NONAME());
    CALL(test_posix_netdb_getaddrinfo__EAI_SERVICE());

    /* test input flags */
    CALL(test_posix_netdb_getaddrinfo__AI_NUMERICHOST());
    CALL(test_posix_netdb_getaddrinfo__AI_NUMERICSERV());
    /* If the requirements are not met (enabled IPv4, IPv6 or both) some of
     * these tests will be simply skipped such that the python test script does
     * not need to be adjusted. */
    CALL(test_posix_netdb_getaddrinfo__AF_UNSPEC());
    CALL(test_posix_netdb_getaddrinfo__AF_INET());
    CALL(test_posix_netdb_getaddrinfo__AF_INET6());

    puts("ALL TESTS SUCCESSFUL");

    return 0;
}
