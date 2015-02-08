/*
 * Copyright (C) 2014 Martine Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>

#include "embUnit.h"

#include "atomic.h"

#include "tests-core.h"

static void test_atomic_set_return_null_null(void)
{
    unsigned int res = 0;

    TEST_ASSERT_EQUAL_INT(0, atomic_set_return(&res, 0));
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_atomic_set_return_one_null(void)
{
    unsigned int res = 1;

    TEST_ASSERT_EQUAL_INT(1, atomic_set_return(&res, 0));
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_atomic_set_return_null_one(void)
{
    unsigned int res = 0;

    TEST_ASSERT_EQUAL_INT(0, atomic_set_return(&res, 1));
    TEST_ASSERT_EQUAL_INT(1, res);
}

static void test_atomic_set_return_limit_null(void)
{
    unsigned int res = UINT_MAX;

    TEST_ASSERT_EQUAL_INT(UINT_MAX, atomic_set_return(&res, 0));
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_atomic_set_return_null_limit(void)
{
    unsigned int res = 0;

    TEST_ASSERT_EQUAL_INT(0, atomic_set_return(&res, UINT_MAX));
    TEST_ASSERT_EQUAL_INT(UINT_MAX, res);
}

static void test_atomic_set_return_null_random(void)
{
    unsigned int res = 0;
    unsigned int r = 45;    /* XXX: decided by fair dice-roll ;-) */

    TEST_ASSERT_EQUAL_INT(0, atomic_set_return(&res, r));
    TEST_ASSERT_EQUAL_INT(r, res);
}

Test *tests_core_atomic_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_atomic_set_return_null_null),
        new_TestFixture(test_atomic_set_return_one_null),
        new_TestFixture(test_atomic_set_return_null_one),
        new_TestFixture(test_atomic_set_return_limit_null),
        new_TestFixture(test_atomic_set_return_null_limit),
        new_TestFixture(test_atomic_set_return_null_random),
    };

    EMB_UNIT_TESTCALLER(core_atomic_tests, NULL, NULL,
                        fixtures);

    return (Test *)&core_atomic_tests;
}
