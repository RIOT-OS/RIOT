/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>

#include "embUnit.h"

#include "cib.h"

#include "tests-core.h"

#define TEST_CIB_SIZE  (4)

static cib_t cib;

static void set_up(void)
{
    cib_init(&cib, TEST_CIB_SIZE);
}

static void test_cib_put(void)
{
    for (unsigned i = 0; i < TEST_CIB_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(i, cib_put(&cib));
    }
    TEST_ASSERT_EQUAL_INT(-1, cib_put(&cib));
}

static void test_cib_get(void)
{
    TEST_ASSERT_EQUAL_INT(-1, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_get(&cib));
}

static void test_cib_get__overflow(void)
{
    /* XXX this is hacky, but bare with me ;-) */
    cib.read_count = UINT_MAX;
    cib.write_count = UINT_MAX;

    TEST_ASSERT_EQUAL_INT(3, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(3, cib_get(&cib));
}

static void test_cib_peek(void)
{
    cib_init(&cib, TEST_CIB_SIZE);
    TEST_ASSERT_EQUAL_INT(-1, cib_peek(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_peek(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_peek(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_peek(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_peek(&cib));
}

static void test_cib_peek__overflow(void)
{
    /* XXX this is hacky, but bare with me ;-) */
    cib.read_count = UINT_MAX;
    cib.write_count = UINT_MAX;

    TEST_ASSERT_EQUAL_INT(3, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(3, cib_peek(&cib));
}

static void test_cib_avail(void)
{
    TEST_ASSERT_EQUAL_INT(0, cib_avail(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_avail(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(2, cib_avail(&cib));
}

static void test_empty_cib(void)
{
    cib_init(&cib, 0);
    TEST_ASSERT_EQUAL_INT(0, cib_avail(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_put(&cib));
}

static void test_singleton_cib(void)
{
    cib_init(&cib, 1);
    TEST_ASSERT_EQUAL_INT(0, cib_avail(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_avail(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_avail(&cib));
}

Test *tests_core_cib_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_cib_put),
        new_TestFixture(test_cib_get),
        new_TestFixture(test_cib_get__overflow),
        new_TestFixture(test_cib_avail),
        new_TestFixture(test_empty_cib),
        new_TestFixture(test_singleton_cib),
        new_TestFixture(test_cib_peek),
        new_TestFixture(test_cib_peek__overflow),
    };

    EMB_UNIT_TESTCALLER(core_cib_tests, set_up, NULL, fixtures);

    return (Test *)&core_cib_tests;
}
