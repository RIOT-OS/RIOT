/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

#include <limits.h>

#include "embUnit/embUnit.h"

#include "bitarithm.h"

#include "tests-core.h"

static void test_SETBIT_null_null(void)
{
    int res = 0x00;

    SETBIT(res, 0x00);

    TEST_ASSERT_EQUAL_INT(0x00, res);
}

static void test_SETBIT_null_limit(void)
{
    unsigned int res = 0x00;

    SETBIT(res, UINT_MAX);

    TEST_ASSERT_EQUAL_INT(UINT_MAX, res);
}

static void test_SETBIT_limit_null(void)
{
    unsigned int res = UINT_MAX;

    SETBIT(res, 0x00);

    TEST_ASSERT_EQUAL_INT(UINT_MAX, res);
}

static void test_SETBIT_limit_limit(void)
{
    unsigned int res = UINT_MAX;

    SETBIT(res, UINT_MAX);

    TEST_ASSERT_EQUAL_INT(UINT_MAX, res);
}

static void test_SETBIT_null_one(void)
{
    unsigned int res = 0x00;

    SETBIT(res, 0x01);

    TEST_ASSERT_EQUAL_INT(0x01, res);
}

static void test_SETBIT_one_null(void)
{
    unsigned int res = 0x01;

    SETBIT(res, 0x00);

    TEST_ASSERT_EQUAL_INT(0x01, res);
}

static void test_SETBIT_one_random(void)
{
    unsigned int res = 0x01;

    SETBIT(res, 0x06);  /* randomized by fair dice roll ;-) */

    TEST_ASSERT_EQUAL_INT(0x07, res);
}

static void test_CLRBIT_null_null(void)
{
    int res = 0x00;

    CLRBIT(res, 0x00);

    TEST_ASSERT_EQUAL_INT(0x00, res);
}

static void test_CLRBIT_null_limit(void)
{
    unsigned int res = 0x00;

    CLRBIT(res, UINT_MAX);

    TEST_ASSERT_EQUAL_INT(0x00, res);
}

static void test_CLRBIT_limit_null(void)
{
    unsigned int res = UINT_MAX;

    CLRBIT(res, 0x00);

    TEST_ASSERT_EQUAL_INT(UINT_MAX, res);
}

static void test_CLRBIT_limit_limit(void)
{
    unsigned int res = UINT_MAX;

    CLRBIT(res, UINT_MAX);

    TEST_ASSERT_EQUAL_INT(0x00, res);
}

static void test_CLRBIT_null_one(void)
{
    unsigned int res = 0x00;

    CLRBIT(res, 0x01);

    TEST_ASSERT_EQUAL_INT(0x00, res);
}

static void test_CLRBIT_one_null(void)
{
    unsigned int res = 0x01;

    CLRBIT(res, 0x00);

    TEST_ASSERT_EQUAL_INT(0x01, res);
}

static void test_CLRBIT_one_random(void)
{
    unsigned int res = 0x01;

    CLRBIT(res, 0x05);  /* randomized by fair dice roll ;-) */

    TEST_ASSERT_EQUAL_INT(0x00, res);
}

static void test_number_of_highest_bit_one(void)
{
    TEST_ASSERT_EQUAL_INT(0, number_of_highest_bit(1));
}

static void test_number_of_highest_bit_limit(void)
{
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned) * 8 - 1,
                          number_of_highest_bit(UINT_MAX));
}

static void test_number_of_highest_bit_random(void)
{
    TEST_ASSERT_EQUAL_INT(2, number_of_highest_bit(4)); /* randomized by fair
                                                           dice roll ;-) */
}

static void test_number_of_lowest_bit_one(void)
{
    TEST_ASSERT_EQUAL_INT(0, number_of_lowest_bit(1));
}

static void test_number_of_lowest_bit_limit(void)
{
    TEST_ASSERT_EQUAL_INT(0, number_of_lowest_bit(UINT_MAX));
}

static void test_number_of_lowest_bit_random(void)
{
    TEST_ASSERT_EQUAL_INT(3, number_of_lowest_bit(8)); /* randomized by fair
                                                          dice roll ;-) */
}

static void test_number_of_bits_set_null(void)
{
    TEST_ASSERT_EQUAL_INT(0, number_of_bits_set(0));
}

static void test_number_of_bits_set_one(void)
{
    TEST_ASSERT_EQUAL_INT(1, number_of_bits_set(1));
}

static void test_number_of_bits_set_limit(void)
{
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned) * 8,
                          number_of_bits_set(UINT_MAX));
}

static void test_number_of_bits_set_random(void)
{
    TEST_ASSERT_EQUAL_INT(3, number_of_bits_set(7)); /* randomized by fair
                                                        dice roll ;-) */
}

Test *tests_core_bitarithm_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_SETBIT_null_null),
        new_TestFixture(test_SETBIT_null_limit),
        new_TestFixture(test_SETBIT_limit_null),
        new_TestFixture(test_SETBIT_limit_limit),
        new_TestFixture(test_SETBIT_null_one),
        new_TestFixture(test_SETBIT_one_null),
        new_TestFixture(test_SETBIT_one_random),
        new_TestFixture(test_CLRBIT_null_null),
        new_TestFixture(test_CLRBIT_null_limit),
        new_TestFixture(test_CLRBIT_limit_null),
        new_TestFixture(test_CLRBIT_limit_limit),
        new_TestFixture(test_CLRBIT_null_one),
        new_TestFixture(test_CLRBIT_one_null),
        new_TestFixture(test_CLRBIT_one_random),
        new_TestFixture(test_number_of_highest_bit_one),
        new_TestFixture(test_number_of_highest_bit_limit),
        new_TestFixture(test_number_of_highest_bit_random),
        new_TestFixture(test_number_of_lowest_bit_one),
        new_TestFixture(test_number_of_lowest_bit_limit),
        new_TestFixture(test_number_of_lowest_bit_random),
        new_TestFixture(test_number_of_bits_set_null),
        new_TestFixture(test_number_of_bits_set_one),
        new_TestFixture(test_number_of_bits_set_limit),
        new_TestFixture(test_number_of_bits_set_random),
    };

    EMB_UNIT_TESTCALLER(core_bitarithm_tests, NULL, NULL, fixtures);

    return (Test *)&core_bitarithm_tests;
}
