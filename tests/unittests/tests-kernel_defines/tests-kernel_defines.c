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
#include "assert.h"
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

static void test_index_of(void)
{
    unsigned foo[8];
    uint8_t bar[32];

    TEST_ASSERT_EQUAL_INT(5, index_of(foo, &foo[5]));
    TEST_ASSERT_EQUAL_INT(17, index_of(bar, &bar[17]));
}

static void test_declare_constant(void)
{
    /* the expression that is assigned to foo is not an integer constant expression,
     * but it is still constant.
     */
    DECLARE_CONSTANT(foo, (uintptr_t)((void *) 7) & 3);

    /* static_assert() only excepts integer constant expressions. If that
     * compiles, foo is an integer constant expression (even though the value
     * assigned to it was not an integer constant expression */
    static_assert(foo == 3, "ensure correct value of constant foo");

    /* temporarily add -Werror=vla to ensure that using foo as length in an
     * array does not create variable length arrays */
#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wvla"
    char test_array[foo];
#pragma GCC diagnostic pop
    static_assert(sizeof(test_array) == 3, "test_array should be 3 bytes long");
}

#ifdef BOARD_NATIVE
/* native compiles with -Og, which does not automatically inline functions.
 * We just turn the function into a macro to get the test also passing on
 * native */
#define magic_computation(...) (unsigned)(42U * 3.14159 / 1337U)
#else
static unsigned magic_computation(void)
{
    return (unsigned)(42U * 3.14159 / 1337U);
}
#endif

static void test_is_compile_time_constant(void)
{
    /* These test might fail on non-GCC-non-clang compilers. We don't support
     * any of those (yet), but this test might need adaption in the future */
    unsigned actual_constant = magic_computation();
    volatile unsigned not_a_constant = actual_constant;

    TEST_ASSERT(IS_CT_CONSTANT(actual_constant));
    TEST_ASSERT(!IS_CT_CONSTANT(not_a_constant));
}

Test *tests_kernel_defines_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_kernel_version),
        new_TestFixture(test_index_of),
        new_TestFixture(test_declare_constant),
        new_TestFixture(test_is_compile_time_constant),
    };

    EMB_UNIT_TESTCALLER(kernel_defines_tests, NULL, NULL, fixtures);

    return (Test *)&kernel_defines_tests;
}

void tests_kernel_defines(void)
{
    TESTS_RUN(tests_kernel_defines_tests());
}
/** @} */
