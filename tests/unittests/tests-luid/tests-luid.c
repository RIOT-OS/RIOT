/*
 * SPDX-FileCopyrightText: 2019 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdint.h>
#include <string.h>
#include <luid.h>

#include "tests-luid.h"

static void test_luid_uniqe_eui64(void)
{
    eui64_t mac[3];

    luid_get_eui64(&mac[0]);
    luid_get_eui64(&mac[1]);
    luid_get_eui64(&mac[2]);
    TEST_ASSERT_EQUAL_INT(0, !memcmp(&mac[0], &mac[1], sizeof(mac[0])));
    TEST_ASSERT_EQUAL_INT(0, !memcmp(&mac[1], &mac[2], sizeof(mac[1])));
}

static void test_luid_uniqe_eui48(void)
{
    eui48_t mac[3];

    luid_get_eui48(&mac[0]);
    luid_get_eui48(&mac[1]);
    luid_get_eui48(&mac[2]);
    TEST_ASSERT_EQUAL_INT(0, !memcmp(&mac[0], &mac[1], sizeof(mac[0])));
    TEST_ASSERT_EQUAL_INT(0, !memcmp(&mac[1], &mac[2], sizeof(mac[1])));
}

static void test_luid_custom(void)
{
    uint8_t a[2][8];
    uint8_t b[2][8];

    luid_custom(a[0], sizeof(a[0]), 0xfefe);
    luid_custom(a[1], sizeof(a[1]), 0xfefe);
    luid_custom(b[0], sizeof(b[0]), 0xbeef);
    luid_custom(b[1], sizeof(b[1]), 0xbeef);

    TEST_ASSERT_EQUAL_INT(0, !memcmp(a[0], b[0], sizeof(a[0])));
    TEST_ASSERT_EQUAL_INT(0, !memcmp(a[1], b[1], sizeof(a[1])));
    TEST_ASSERT_EQUAL_INT(0, memcmp(a[0], a[0], sizeof(a[0])));
    TEST_ASSERT_EQUAL_INT(0, memcmp(b[1], b[1], sizeof(b[0])));
}

Test *tests_luid_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_luid_uniqe_eui48),
        new_TestFixture(test_luid_uniqe_eui64),
        new_TestFixture(test_luid_custom),
    };

    EMB_UNIT_TESTCALLER(luid_tests, NULL, NULL, fixtures);

    return (Test *)&luid_tests;
}

void tests_luid(void)
{
    TESTS_RUN(tests_luid_tests());
}
