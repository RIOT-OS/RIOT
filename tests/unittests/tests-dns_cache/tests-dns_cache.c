/*
 * SPDX-FileCopyrightText: 2022 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdint.h>
#include <string.h>
#include "net/af.h"
#include "net/ipv6.h"
#include "ztimer.h"

#include "net/dns/cache.h"

#include "tests-dns_cache.h"

static void test_dns_cache_add(void)
{
    ipv6_addr_t addr_in = IPV6_ADDR_ALL_NODES_IF_LOCAL;
    ipv6_addr_t addr_out;

    TEST_ASSERT_EQUAL_INT(0, dns_cache_query("example.com", &addr_out, AF_INET6));

    /* add DNS entry, set it to expire in 1s */
    dns_cache_add("example.com", &addr_in, sizeof(addr_in), 1);
    TEST_ASSERT_EQUAL_INT(sizeof(addr_out), dns_cache_query("example.com", &addr_out, AF_INET6));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&addr_in, &addr_out, sizeof(addr_in)));

    TEST_ASSERT_EQUAL_INT(0, dns_cache_query("example.com", &addr_out, AF_INET));
    TEST_ASSERT_EQUAL_INT(0, dns_cache_query("alt.example.com", &addr_out, AF_INET6));
    TEST_ASSERT_EQUAL_INT(0, dns_cache_query("example.comm", &addr_out, AF_INET6));
    TEST_ASSERT_EQUAL_INT(0, dns_cache_query("example.co", &addr_out, AF_INET6));

    ztimer_sleep(ZTIMER_USEC, 2000000);
    TEST_ASSERT_EQUAL_INT(0, dns_cache_query("example.com", &addr_out, AF_INET6));
}

static void test_dns_cache_add_ttl0(void)
{
    ipv6_addr_t addr_in = IPV6_ADDR_ALL_NODES_IF_LOCAL;
    ipv6_addr_t addr_out;

    TEST_ASSERT_EQUAL_INT(0, dns_cache_query("example.com", &addr_out, AF_INET6));
    dns_cache_add("example.com", &addr_in, sizeof(addr_in), 0);
    TEST_ASSERT_EQUAL_INT(0, dns_cache_query("example.com", &addr_out, AF_INET6));
    dns_cache_add("example.com", &addr_in, sizeof(addr_in), 1);
    TEST_ASSERT_EQUAL_INT(sizeof(addr_out), dns_cache_query("example.com", &addr_out, AF_INET6));
    dns_cache_add("example.com", &addr_in, sizeof(addr_in), 0);
    TEST_ASSERT_EQUAL_INT(0, dns_cache_query("example.com", &addr_out, AF_INET6));
}

Test *tests_dns_cache_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_dns_cache_add),
        new_TestFixture(test_dns_cache_add_ttl0),
    };

    EMB_UNIT_TESTCALLER(dns_cache_tests, NULL, NULL, fixtures);

    return (Test *)&dns_cache_tests;
}

void tests_dns_cache(void)
{
    TESTS_RUN(tests_dns_cache_tests());
}
