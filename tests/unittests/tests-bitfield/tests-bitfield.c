/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <string.h>

#include "embUnit.h"

#include "bitfield.h"

static void test_bf_get_unset_empty(void)
{
    int res = 0;
    uint8_t field[5];

    memset(field, 0, sizeof(field));
    res = bf_get_unset(field, 0);
    TEST_ASSERT_EQUAL_INT(-1, res);

    memset(field, 0, sizeof(field));
    res = bf_get_unset(field, 1);
    TEST_ASSERT_EQUAL_INT(0, res);

    memset(field, 0, sizeof(field));
    res = bf_get_unset(field, 3);
    TEST_ASSERT_EQUAL_INT(0, res);

    memset(field, 0, sizeof(field));
    res = bf_get_unset(field, 8);
    TEST_ASSERT_EQUAL_INT(0, res);

    memset(field, 0, sizeof(field));
    res = bf_get_unset(field, 9);
    TEST_ASSERT_EQUAL_INT(0, res);

    memset(field, 0, sizeof(field));
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(0, res);
}

static void test_bf_get_unset_firstbyte(void)
{
    int res = 0;
    uint8_t field[5];
    memset(field, 0xff, sizeof(field));

    field[0] = 0x00;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(0, res);

    field[0] = 0x01;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(1, res);

    field[0] = 0x0f;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(4, res);

    field[0] = 0x7f;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(7, res);

    field[0] = 0xff;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(-1, res);
}

static void test_bf_get_unset_middle(void)
{
    int res = 0;
    uint8_t field[5];
    memset(field, 0xff, sizeof(field));

    field[2] = 0x00;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(16, res);

    field[2] = 0x01;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(17, res);

    field[2] = 0x0f;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(20, res);

    field[2] = 0x7f;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(23, res);
}

static void test_bf_get_unset_lastbyte(void)
{
    int res = 0;
    uint8_t field[5];
    memset(field, 0xff, sizeof(field));

    field[4] = 0x00;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(32, res);

    field[4] = 0x01;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(33, res);

    field[4] = 0x0f;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(36, res);

    field[4] = 0x7f;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(39, res);
}

Test *tests_bitfield_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_bf_get_unset_empty),
        new_TestFixture(test_bf_get_unset_firstbyte),
        new_TestFixture(test_bf_get_unset_middle),
        new_TestFixture(test_bf_get_unset_lastbyte),
    };

    EMB_UNIT_TESTCALLER(bitfield_tests, NULL, NULL, fixtures);

    return (Test *)&bitfield_tests;
}


void tests_bitfield(void)
{
    TESTS_RUN(tests_bitfield_tests());
}
