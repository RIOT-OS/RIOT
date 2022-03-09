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

#include "byteorder.h"
#include "net/ipv4/addr.h"

#include "tests-ipv4_addr.h"

static void test_ipv4_addr_equal__unequal(void)
{
    ipv4_addr_t a = { { 1, 1, 1, 0 } };
    ipv4_addr_t b = { { 1, 1, 1, 1 } };

    TEST_ASSERT(!ipv4_addr_equal(&a, &b));
}

static void test_ipv4_addr_equal__equal(void)
{
    ipv4_addr_t a = { { 1, 1, 1, 1 } };
    ipv4_addr_t b = { { 1, 1, 1, 1 } };

    TEST_ASSERT(ipv4_addr_equal(&a, &b));
}

static void test_ipv4_addr_to_str__string_too_short(void)
{
    ipv4_addr_t a = { { 1, 1, 1, 1 } };
    char result[1];

    TEST_ASSERT_NULL(ipv4_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv4_addr_to_str__addr_NULL(void)
{
    char result[IPV4_ADDR_MAX_STR_LEN];

    TEST_ASSERT_NULL(ipv4_addr_to_str(result, NULL, sizeof(result)));
}

static void test_ipv4_addr_to_str__result_NULL(void)
{
    ipv4_addr_t a = {0};

    TEST_ASSERT_NULL(ipv4_addr_to_str(NULL, &a, IPV4_ADDR_MAX_STR_LEN));
}

static void test_ipv4_addr_to_str__success(void)
{
    ipv4_addr_t a = { { 1, 1, 1, 1 } };
    char result[IPV4_ADDR_MAX_STR_LEN];

    TEST_ASSERT_EQUAL_STRING("1.1.1.1", ipv4_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv4_addr_to_str__success2(void)
{
    ipv4_addr_t a = { { 0, 1, 12, 123 } };
    char result[IPV4_ADDR_MAX_STR_LEN];

    TEST_ASSERT_EQUAL_STRING("0.1.12.123", ipv4_addr_to_str(result, &a, sizeof(result)));
}

static void test_ipv4_addr_from_str__dot_start(void)
{
    ipv4_addr_t result;

    TEST_ASSERT_NULL(ipv4_addr_from_str(&result, ".1.12.123"));
}

static void test_ipv4_addr_from_str__double_dot(void)
{
    ipv4_addr_t result;

    TEST_ASSERT_NULL(ipv4_addr_from_str(&result, "0..12.123"));
}

static void test_ipv4_addr_from_str__string_too_long(void)
{
    ipv4_addr_t result;

    TEST_ASSERT_NULL(ipv4_addr_from_str(&result, "255.255.255.255.255"));
}

static void test_ipv4_addr_from_str__illegal_chars(void)
{
    ipv4_addr_t result;

    TEST_ASSERT_NULL(ipv4_addr_from_str(&result, "0.::ab-)"));
}

static void test_ipv4_addr_from_str__addr_NULL(void)
{
    ipv4_addr_t result;

    TEST_ASSERT_NULL(ipv4_addr_from_str(&result, NULL));
}

static void test_ipv4_addr_from_str__result_NULL(void)
{
    TEST_ASSERT_NULL(ipv4_addr_from_str(NULL, "::"));
}

static void test_ipv4_addr_from_str__success(void)
{
    ipv4_addr_t a = { { 1, 1, 1, 1 } };
    ipv4_addr_t result;

    TEST_ASSERT_NOT_NULL(ipv4_addr_from_str(&result, "1.1.1.1"));
    TEST_ASSERT(ipv4_addr_equal(&a, &result));
}

static void test_ipv4_addr_from_str__success2(void)
{
    ipv4_addr_t a = { { 0, 1, 12, 123 } }, result;

    TEST_ASSERT_NOT_NULL(ipv4_addr_from_str(&result, "0.1.12.123"));
    TEST_ASSERT(ipv4_addr_equal(&a, &result));
}

static void test_ipv4_addr_from_buf__success(void)
{
    ipv4_addr_t a = { { 1, 1, 1, 1 } };
    ipv4_addr_t result;

    TEST_ASSERT_NOT_NULL(ipv4_addr_from_buf(&result, "1.1.1.1%tap0", 7));
    TEST_ASSERT(ipv4_addr_equal(&a, &result));
}

static void test_ipv4_addr_from_buf__result_NULL(void)
{
    TEST_ASSERT_NULL(ipv4_addr_from_buf(NULL, "::", 2));
}

static void test_ipv4_addr_from_buf__illegal_chars(void)
{
    ipv4_addr_t result;

    TEST_ASSERT_NULL(ipv4_addr_from_buf(&result, "1.1.1.1%tap0", 13));
}

static void test_ipv4_addr_from_buf__too_long_len(void)
{
    ipv4_addr_t result;

    TEST_ASSERT_NULL(ipv4_addr_from_buf(&result, "1.1.1.1", IPV4_ADDR_MAX_STR_LEN + 1));
}

Test *tests_ipv4_addr_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ipv4_addr_equal__unequal),
        new_TestFixture(test_ipv4_addr_equal__equal),
        new_TestFixture(test_ipv4_addr_to_str__string_too_short),
        new_TestFixture(test_ipv4_addr_to_str__addr_NULL),
        new_TestFixture(test_ipv4_addr_to_str__result_NULL),
        new_TestFixture(test_ipv4_addr_to_str__success),
        new_TestFixture(test_ipv4_addr_to_str__success2),
        new_TestFixture(test_ipv4_addr_from_str__dot_start),
        new_TestFixture(test_ipv4_addr_from_str__double_dot),
        new_TestFixture(test_ipv4_addr_from_str__string_too_long),
        new_TestFixture(test_ipv4_addr_from_str__illegal_chars),
        new_TestFixture(test_ipv4_addr_from_str__addr_NULL),
        new_TestFixture(test_ipv4_addr_from_str__result_NULL),
        new_TestFixture(test_ipv4_addr_from_str__success),
        new_TestFixture(test_ipv4_addr_from_str__success2),
        new_TestFixture(test_ipv4_addr_from_buf__success),
        new_TestFixture(test_ipv4_addr_from_buf__result_NULL),
        new_TestFixture(test_ipv4_addr_from_buf__illegal_chars),
        new_TestFixture(test_ipv4_addr_from_buf__too_long_len),
    };

    EMB_UNIT_TESTCALLER(ipv4_addr_tests, NULL, NULL, fixtures);

    return (Test *)&ipv4_addr_tests;
}

void tests_ipv4_addr(void)
{
    TESTS_RUN(tests_ipv4_addr_tests());
}
/** @} */
