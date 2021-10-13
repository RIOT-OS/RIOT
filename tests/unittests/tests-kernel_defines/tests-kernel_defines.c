/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
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
#include "embUnit.h"
#include "kernel_defines.h"

static void kernel_version_parse(uint64_t version, uint16_t *mayor,
                                 uint16_t *minor, uint16_t *patch, uint16_t *extra)
{
    *mayor = version >> 48;
    *minor = (version >> 32) & 0xFFFF;
    *patch = (version >> 16) & 0xFFFF;
    *extra = (version >> 0) & 0xFFFF;
}

static void test_kernel_version(void)
{
    uint16_t mayor, minor, patch, extra;

    TEST_ASSERT(RIOT_VERSION_CODE);
    TEST_ASSERT(RIOT_VERSION_CODE > RIOT_VERSION_NUM(2017, 1, 3, 0));
    TEST_ASSERT(RIOT_VERSION_CODE < RIOT_VERSION_NUM(2080, 10, 2, 0));

    kernel_version_parse(RIOT_VERSION_NUM(2020, 7, 1, 0),
                         &mayor, &minor, &patch, &extra);

    TEST_ASSERT_EQUAL_INT(2020, mayor);
    TEST_ASSERT_EQUAL_INT(7, minor);
    TEST_ASSERT_EQUAL_INT(1, patch);
    TEST_ASSERT_EQUAL_INT(0, extra);
#if RIOT_VERSION_CODE < RIOT_VERSION_NUM(2000, 1, 1, 0)
    TEST_ASSERT(0);
#endif
#if RIOT_VERSION_CODE > RIOT_VERSION_NUM(2100, 1, 1, 1)
    TEST_ASSERT(0);
#endif
}

Test *tests_kernel_defines_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_kernel_version),
    };

    EMB_UNIT_TESTCALLER(kernel_defines_tests, NULL, NULL, fixtures);

    return (Test *)&kernel_defines_tests;
}

void tests_kernel_defines(void)
{
    TESTS_RUN(tests_kernel_defines_tests());
}
/** @} */
