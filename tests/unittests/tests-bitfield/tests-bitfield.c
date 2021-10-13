/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <string.h>

#include "embUnit.h"

#include "bitfield.h"

static void test_bf_set(void)
{
    BITFIELD(field, 32U);

    memset(field, 0x00, sizeof(field));
    bf_set(field, 3);
    /* bit idx:  3/8 +   01234567
     *      0x10    == 0b00010000 */
    TEST_ASSERT_EQUAL_INT(0x10, field[3 / 8]);
    bf_set(field, 1);
    /* bit idx:  1/8 +   01234567
     *      0x50    == 0b01010000 */
    TEST_ASSERT_EQUAL_INT(0x50, field[1 / 8]);
    bf_set(field, 25);
    /* bit idx: 25/8 +   01234567
     *      0x40    == 0b01000000 */
    TEST_ASSERT_EQUAL_INT(0x40, field[25 / 8]);
}

static void test_bf_unset(void)
{
    BITFIELD(field, 32U);

    memset(field, 0xff, sizeof(field));
    bf_unset(field, 5);
    /* bit idx:  5/8 +   01234567
     *      0xfb    == 0b11111011 */
    TEST_ASSERT_EQUAL_INT(0xfb, field[5 / 8]);
    bf_unset(field, 8);
    /* cppcheck-suppress duplicateExpression
     * reason: intentionally dividing 8 by itself to make test more readable
     * bit idx:  8/8 +   01234567
     *      0x7f    == 0b01111111 */
    TEST_ASSERT_EQUAL_INT(0x7f, field[8 / 8]);
    bf_unset(field, 10);
    /* bit idx: 10/8 +   01234567
     *      0x5f    == 0b01011111 */
    TEST_ASSERT_EQUAL_INT(0x5f, field[10 / 8]);
}

static void test_bf_toggle(void)
{
    BITFIELD(field, 32U);

    memset(field, 0xff, sizeof(field));
    bf_toggle(field, 7);
    /* bit idx:  7/8 +   01234567
     *      0xfe    == 0b11111110 */
    TEST_ASSERT_EQUAL_INT(0xfe, field[7 / 8]);
    bf_toggle(field, 0);
    /* bit idx:  0/8 +   01234567
     *      0x7e    == 0b01111110 */
    TEST_ASSERT_EQUAL_INT(0x7e, field[0 / 8]);
    bf_toggle(field, 7);
    /* bit idx:  7/8 +   01234567
     *      0x7f    == 0b01111111 */
    TEST_ASSERT_EQUAL_INT(0x7f, field[7 / 8]);
    bf_toggle(field, 0);
    /* bit idx:  0/8 +   01234567
     *      0xff    == 0b11111111 */
    TEST_ASSERT_EQUAL_INT(0xff, field[0 / 8]);
    bf_toggle(field, 28);
    /* bit idx: 28/8 +   01234567
     *      0xf7    == 0b11110111 */
    TEST_ASSERT_EQUAL_INT(0xf7, field[28 / 8]);
}

static void test_bf_isset(void)
{
    BITFIELD(field, 32U);

    /* bf_set / bf_unset tested above */
    memset(field, 0x00, sizeof(field));
    TEST_ASSERT(!bf_isset(field, 25));
    bf_set(field, 25);
    TEST_ASSERT(bf_isset(field, 25));
    bf_unset(field, 25);
    TEST_ASSERT(!bf_isset(field, 25));
}

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

    field[0] = 0x80;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(1, res);

    field[0] = 0xf0;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(4, res);

    field[0] = 0xfe;
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

    field[2] = 0x80;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(17, res);

    field[2] = 0xf0;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(20, res);

    field[2] = 0xfe;
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

    field[4] = 0x80;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(33, res);

    field[4] = 0xf0;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(36, res);

    field[4] = 0xfe;
    res = bf_get_unset(field, 40);
    TEST_ASSERT_EQUAL_INT(39, res);
}

Test *tests_bitfield_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_bf_set),
        new_TestFixture(test_bf_unset),
        new_TestFixture(test_bf_toggle),
        new_TestFixture(test_bf_isset),
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
