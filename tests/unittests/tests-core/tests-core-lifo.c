/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "embUnit.h"

#include "lifo.h"

#include "tests-core.h"

#define TEST_LIFO_MAX_ELEM   2

int lifo[TEST_LIFO_MAX_ELEM + 1];

static void set_up(void)
{
    lifo_init(lifo, TEST_LIFO_MAX_ELEM);
}

static void test_lifo_empty(void)
{
    TEST_ASSERT(lifo_empty(lifo));
}

static void test_lifo_insert(void)
{
    lifo_insert(lifo, 0);
    TEST_ASSERT(!lifo_empty(lifo));
}

static void test_lifo_get_one(void)
{
    lifo_insert(lifo, 0);
    TEST_ASSERT_EQUAL_INT(0, lifo_get(lifo));
}

static void test_lifo_get_two(void)
{
    lifo_insert(lifo, 0);
    lifo_insert(lifo, 1);
    TEST_ASSERT_EQUAL_INT(1, lifo_get(lifo));
    TEST_ASSERT_EQUAL_INT(0, lifo_get(lifo));
}

Test *tests_core_lifo_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_lifo_empty),
        new_TestFixture(test_lifo_insert),
        new_TestFixture(test_lifo_get_one),
        new_TestFixture(test_lifo_get_two),
    };

    EMB_UNIT_TESTCALLER(core_lifo_tests, set_up, NULL, fixtures);

    return (Test *)&core_lifo_tests;
}
