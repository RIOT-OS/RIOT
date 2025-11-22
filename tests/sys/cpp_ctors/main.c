/*
 * SPDX-FileCopyrightText: 2016-2017 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "embUnit.h"
#include "tests-cpp_ctors.h"
#include "thread.h" /* For thread_getpid() */

long tests_cpp_ctors_global_value(void);
long tests_cpp_ctors_static_value(void);
long tests_cpp_ctors_local_value(long number);

extern volatile long tests_cpp_ctors_magic1;
extern volatile long tests_cpp_ctors_magic2;
extern void *tests_cpp_ctors_order[8];

static void tests_cpp_global_ctors(void)
{
    long expected = tests_cpp_ctors_magic1;
    long actual = tests_cpp_ctors_global_value();
    /* Test to ensure that global constructors have executed */
    TEST_ASSERT_EQUAL_INT(expected, actual);
}

static void tests_cpp_static_ctors(void)
{
    for (long i = 1; i < 10; ++i) {
        long expected = tests_cpp_ctors_magic2 + i;
        long actual = tests_cpp_ctors_static_value();
        TEST_ASSERT_EQUAL_INT(expected, actual);
    }
}

static void tests_cpp_local_ctors(void)
{
    /* Test to ensure that local constructors are executed properly */
    long expected = thread_getpid() + 1;
    long actual = tests_cpp_ctors_local_value(thread_getpid());
    TEST_ASSERT_EQUAL_INT(expected, actual);
}

Test *tests_cpp_ctors_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(tests_cpp_local_ctors),
        new_TestFixture(tests_cpp_global_ctors),
        new_TestFixture(tests_cpp_static_ctors),
    };

    EMB_UNIT_TESTCALLER(cpp_tests, NULL, NULL, fixtures);

    return (Test *)&cpp_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_cpp_ctors_tests());
    TESTS_END();
    return 0;
}
