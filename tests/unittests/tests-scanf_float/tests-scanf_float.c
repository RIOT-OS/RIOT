/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       Test scanf with floating-point numbers (scanf_float).
 *
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "embUnit/embUnit.h"

#include "tests-scanf_float.h"

#define TEST_EASY(name, format, constant) static void test_ ## name (void) \
{\
    int items;\
    float x = 0;\
    items = sscanf(#constant, format, &x);\
    TEST_ASSERT_EQUAL_INT(items, 1); \
    TEST_ASSERT(x == constant##f);\
}

TEST_EASY(f, "%f", 2.71828)
TEST_EASY(e, "%e", 2.71828)
TEST_EASY(E, "%E", 2.71828)
TEST_EASY(g, "%g", 2.71828)

/* This does not seem to be supported in newlib-nano. */
/*TEST_EASY(a, "%a", 2.71828)*/

TEST_EASY(sign, "%f", -.785398)

static void test_scientific(void)
{
    int items;
    float x = 0;

    items = sscanf("-3.21e2", "%f", &x);
    TEST_ASSERT_EQUAL_INT(items, 1);
    TEST_ASSERT(x == -321.0f);
}
/* This does not seem to be supported in newlib-nano. */
/*
static void test_hexa(void)
{
    int items;
    float x = 0;

    items = sscanf("0xA.A", "%f", &x);
    TEST_ASSERT_EQUAL_INT(items, 1);
    TEST_ASSERT(x == 0xA.Ap0f);
}
*/
Test *tests_scanf_float_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_f),
        new_TestFixture(test_e),
        new_TestFixture(test_E),
        new_TestFixture(test_g),
    /*  new_TestFixture(test_a),    */
        new_TestFixture(test_sign),
        new_TestFixture(test_scientific),
    /*  new_TestFixture(test_hexa), */
    };

    EMB_UNIT_TESTCALLER(scanf_float_tests,
                        NULL, NULL,
                        fixtures);

    return (Test *)&scanf_float_tests;
}

void tests_scanf_float(void)
{
    TESTS_RUN(tests_scanf_float_tests());
}
