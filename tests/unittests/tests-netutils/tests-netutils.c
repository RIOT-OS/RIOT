/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdint.h>

#include "embUnit/embUnit.h"
#include "net/utils.h"

#include "tests-netutils.h"

static netif_t dummy_netif[2];

static void test_ipv6_addr_from_str__one_colon_start(void)
{
    ipv6_addr_t address;
    netif_t *netif;

    TEST_ASSERT_EQUAL_INT(netutils_get_ipv6(&address, &netif, ":ff::1"), -EINVAL);
}

static void test_ipv6_addr_from_str__three_colons(void)
{
    ipv6_addr_t address;
    netif_t *netif;

    TEST_ASSERT_EQUAL_INT(netutils_get_ipv6(&address, &netif, "ff02:::1"), -EINVAL);
}

static void test_ipv6_addr_from_str__illegal_chars(void)
{
    ipv6_addr_t address;
    netif_t *netif;

    TEST_ASSERT_EQUAL_INT(netutils_get_ipv6(&address, &netif, ":-D"), -EINVAL);
}

static void test_ipv6_addr_from_str__addr_NULL(void)
{
    ipv6_addr_t address;
    netif_t *netif;

    TEST_ASSERT_EQUAL_INT(netutils_get_ipv6(&address, &netif, NULL), -EINVAL);
}

static void test_ipv6_addr_from_str__address_NULL(void)
{
    TEST_ASSERT_NULL(ipv6_addr_from_str(NULL, "::"));
}

static void test_ipv6_addr_from_str__success(void)
{
    const ipv6_addr_t a = { {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        }
    };
    ipv6_addr_t address;
    netif_t *netif;

    TEST_ASSERT_EQUAL_INT(netutils_get_ipv6(&address, &netif, "1:203:405:607:809:a0b:c0d:e0f"), 0);
    TEST_ASSERT(ipv6_addr_equal(&a, &address));
    TEST_ASSERT_NULL(netif);
}

static void test_ipv6_addr_from_str__success2(void)
{
    const ipv6_addr_t a = { {
            0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
        }
    };
    ipv6_addr_t address;
    netif_t *netif;

    TEST_ASSERT_EQUAL_INT(netutils_get_ipv6(&address, &netif, "fe80::f8f9:fafb:fcfd:feff%1"), 0);
    TEST_ASSERT_EQUAL_INT((uintptr_t)netif, (uintptr_t)&dummy_netif[1]);
    TEST_ASSERT(ipv6_addr_equal(&a, &address));
}

static void test_ipv6_addr_from_str__invalid_interface(void)
{
    ipv6_addr_t address;
    netif_t *netif;

    TEST_ASSERT_EQUAL_INT(netutils_get_ipv6(&address, &netif, "fe80::f8f9:fafb:fcfd:feff%3"), -EINVAL);
}

static void test_ipv6_addr_from_str__success4(void)
{
    const ipv6_addr_t a = { {
            0x26, 0x06, 0x28, 0x00, 0x02, 0x20, 0x00, 0x01,
            0x02, 0x48, 0x18, 0x93, 0x25, 0xc8, 0x19, 0x46
        }
    };
    ipv6_addr_t address;
    netif_t *netif;

    TEST_ASSERT_EQUAL_INT(netutils_get_ipv6(&address, &netif, "example.com"), 0);
    TEST_ASSERT(ipv6_addr_equal(&a, &address));
}

static void test_ipv6_addr_from_str__success5(void)
{
    const ipv6_addr_t a = IPV6_ADDR_LOOPBACK;
    ipv6_addr_t address;

    TEST_ASSERT_NOT_NULL(ipv6_addr_from_str(&address, "::1"));
    TEST_ASSERT(ipv6_addr_equal(&a, &address));
}

Test *tests_netutils_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ipv6_addr_from_str__one_colon_start),
        new_TestFixture(test_ipv6_addr_from_str__three_colons),
        new_TestFixture(test_ipv6_addr_from_str__illegal_chars),
        new_TestFixture(test_ipv6_addr_from_str__addr_NULL),
        new_TestFixture(test_ipv6_addr_from_str__address_NULL),
        new_TestFixture(test_ipv6_addr_from_str__success),
        new_TestFixture(test_ipv6_addr_from_str__success2),
        new_TestFixture(test_ipv6_addr_from_str__invalid_interface),
        new_TestFixture(test_ipv6_addr_from_str__success4),
        new_TestFixture(test_ipv6_addr_from_str__success5),
    };

    EMB_UNIT_TESTCALLER(ipv6_addr_tests, NULL, NULL, fixtures);

    return (Test *)&ipv6_addr_tests;
}

void tests_netutils(void)
{
    unsigned i = ARRAY_SIZE(dummy_netif);
    while (i--) {
        netif_register(&dummy_netif[i]);
    }

    TESTS_RUN(tests_netutils_tests());
}
/** @} */
