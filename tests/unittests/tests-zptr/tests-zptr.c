/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
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
#include <string.h>

#include "embUnit/embUnit.h"

#include "zptr.h"
#include "tests-zptr.h"

static void test_zptr_basic(void)
{
    uint32_t val;
    TEST_ASSERT(&val == zptrd(zptrc(&val)));
}

Test *tests_zptr_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_zptr_basic),
    };

    EMB_UNIT_TESTCALLER(zptr_tests, NULL, NULL, fixtures);

    return (Test *)&zptr_tests;
}

void tests_zptr(void)
{
    TESTS_RUN(tests_zptr_tests());
}
/** @} */
