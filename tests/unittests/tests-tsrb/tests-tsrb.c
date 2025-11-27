/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */
#include <stdint.h>
#include <string.h>

#include "embUnit/embUnit.h"

#include "unittests-constants.h"
#include "tsrb.h"
#include "tests-tsrb.h"

#define TEST_INPUT          (0xdb)
#define TEST_DROP_NUM       (4U)
#define BUFFER_SIZE         (16)    /* intentionally not unsigned to easier
                                     * check for implicit casting problems */
#define IO_BUFFER_CANARY    (0xb8)

static uint8_t _tsrb_buffer[BUFFER_SIZE];
static uint8_t _io_buffer[BUFFER_SIZE * 2];
static tsrb_t _tsrb = TSRB_INIT(_tsrb_buffer);

static void tear_down(void)
{
    memset(_io_buffer, IO_BUFFER_CANARY, sizeof(_io_buffer));
    memset(_tsrb_buffer, 0, sizeof(_tsrb_buffer));
    tsrb_init(&_tsrb, _tsrb_buffer, BUFFER_SIZE);
}

static void test_clear(void)
{
    TEST_ASSERT_EQUAL_INT(0, tsrb_avail(&_tsrb));

    for (int i = 0; i < BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, TEST_INPUT));
        TEST_ASSERT_EQUAL_INT(i + 1, tsrb_avail(&_tsrb));
    }

    tsrb_clear(&_tsrb);

    TEST_ASSERT_EQUAL_INT(0, tsrb_avail(&_tsrb));
}

static void test_empty(void)
{
    TEST_ASSERT_EQUAL_INT(1, tsrb_empty(&_tsrb));

    TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, TEST_INPUT));
    TEST_ASSERT_EQUAL_INT(0, tsrb_empty(&_tsrb));
}

static void test_avail(void)
{
    TEST_ASSERT_EQUAL_INT(0, tsrb_avail(&_tsrb));

    for (int i = 0; i < BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, TEST_INPUT));
        TEST_ASSERT_EQUAL_INT(i + 1, tsrb_avail(&_tsrb));
    }
}

static void test_full(void)
{
    TEST_ASSERT_EQUAL_INT(0, tsrb_full(&_tsrb));

    for (int i = 0; i < (BUFFER_SIZE - 1); i++) {
        TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, TEST_INPUT));
        TEST_ASSERT_EQUAL_INT(0, tsrb_full(&_tsrb));
    }
    TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, TEST_INPUT));
    TEST_ASSERT_EQUAL_INT(1, tsrb_full(&_tsrb));
}

static void test_free(void)
{
    TEST_ASSERT_EQUAL_INT(BUFFER_SIZE, tsrb_free(&_tsrb));

    for (int i = 0; i < BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, TEST_INPUT));
        TEST_ASSERT_EQUAL_INT(BUFFER_SIZE - (i + 1), tsrb_free(&_tsrb));
    }
}

static void test_get_one(void)
{
    int res;

    TEST_ASSERT_EQUAL_INT(-1, tsrb_get_one(&_tsrb));
    TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, TEST_INPUT));
    TEST_ASSERT_EQUAL_INT(TEST_INPUT, tsrb_get_one(&_tsrb));
    TEST_ASSERT_EQUAL_INT(-1, tsrb_get_one(&_tsrb));
    TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, 0x0));
    TEST_ASSERT_EQUAL_INT(0x0, tsrb_get_one(&_tsrb));
    TEST_ASSERT_EQUAL_INT(-1, tsrb_get_one(&_tsrb));
    TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, 0xff));
    res = tsrb_get_one(&_tsrb);
    TEST_ASSERT_EQUAL_INT(0xff, res);
    /* 0xff is -1 in signed int8_t */
    TEST_ASSERT(-1 != res);
    TEST_ASSERT_EQUAL_INT(-1, tsrb_get_one(&_tsrb));
}

static void test_get(void)
{
    TEST_ASSERT(BUFFER_SIZE < sizeof(_io_buffer));
    TEST_ASSERT_EQUAL_INT(0, tsrb_get(&_tsrb, _io_buffer,
                                      sizeof(_io_buffer)));

    for (int i = 0; i < BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, TEST_INPUT + i));
    }
    TEST_ASSERT_EQUAL_INT(BUFFER_SIZE, tsrb_get(&_tsrb, _io_buffer,
                                                sizeof(_io_buffer)));
    for (int i = 0; i < BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT((TEST_INPUT + i), _io_buffer[i]);
    }
    for (int i = BUFFER_SIZE; i < (int)sizeof(_io_buffer); i++) {
        TEST_ASSERT_EQUAL_INT(IO_BUFFER_CANARY, _io_buffer[i]);
    }
}

static void test_peek_one(void)
{
    int res;

    TEST_ASSERT_EQUAL_INT(-1, tsrb_peek_one(&_tsrb));
    TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, TEST_INPUT));
    TEST_ASSERT_EQUAL_INT(TEST_INPUT, tsrb_peek_one(&_tsrb));
    TEST_ASSERT_EQUAL_INT(TEST_INPUT, tsrb_peek_one(&_tsrb));
    TEST_ASSERT_EQUAL_INT(TEST_INPUT, tsrb_get_one(&_tsrb));
    TEST_ASSERT_EQUAL_INT(-1, tsrb_peek_one(&_tsrb));
    TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, 0xff));
    res = tsrb_peek_one(&_tsrb);
    TEST_ASSERT_EQUAL_INT(0xff, res);
    /* 0xff is -1 in signed int8_t */
    TEST_ASSERT(-1 != res);
}

static void test_peek(void)
{
    TEST_ASSERT(BUFFER_SIZE < sizeof(_io_buffer));
    TEST_ASSERT_EQUAL_INT(0, tsrb_peek(&_tsrb, _io_buffer,
                                      sizeof(_io_buffer)));

    for (int i = 0; i < BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, TEST_INPUT + i));
    }
    TEST_ASSERT_EQUAL_INT(BUFFER_SIZE, tsrb_peek(&_tsrb, _io_buffer,
                                                sizeof(_io_buffer)));
    for (int i = 0; i < BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT((TEST_INPUT + i), _io_buffer[i]);
    }
    for (int i = BUFFER_SIZE; i < (int)sizeof(_io_buffer); i++) {
        TEST_ASSERT_EQUAL_INT(IO_BUFFER_CANARY, _io_buffer[i]);
    }
    TEST_ASSERT_EQUAL_INT(BUFFER_SIZE, tsrb_avail(&_tsrb));
}

static void test_drop(void)
{
    TEST_ASSERT(BUFFER_SIZE < sizeof(_io_buffer));
    TEST_ASSERT_EQUAL_INT(0, tsrb_drop(&_tsrb, sizeof(_io_buffer)));

    for (int i = 0; i < BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, TEST_INPUT + i));
    }
    TEST_ASSERT_EQUAL_INT(TEST_DROP_NUM, tsrb_drop(&_tsrb, TEST_DROP_NUM));
    TEST_ASSERT_EQUAL_INT(BUFFER_SIZE - TEST_DROP_NUM, tsrb_avail(&_tsrb));
    TEST_ASSERT_EQUAL_INT(BUFFER_SIZE - TEST_DROP_NUM,
                          tsrb_get(&_tsrb, _io_buffer, sizeof(_io_buffer)));
    for (int i = 0; i < (int)(BUFFER_SIZE - TEST_DROP_NUM); i++) {
        TEST_ASSERT_EQUAL_INT((TEST_INPUT + (char)TEST_DROP_NUM + (char)i),
                              _io_buffer[i]);
    }
    for (int i = BUFFER_SIZE - TEST_DROP_NUM;
         i < (int)sizeof(_io_buffer); i++) {
        TEST_ASSERT_EQUAL_INT(IO_BUFFER_CANARY, _io_buffer[i]);
    }
}

static void test_add_one(void)
{
    test_get_one();     /* Do tests done for tsrb_get_one again, as they do
                         * tsrb_add_one */

    for (int i = 0; i < BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, tsrb_add_one(&_tsrb, TEST_INPUT));
    }
    TEST_ASSERT_EQUAL_INT(-1, tsrb_add_one(&_tsrb, TEST_INPUT));
}

static void test_add(void)
{
    for (int i = 0; i < (int)sizeof(_io_buffer); i++) {
        _io_buffer[i] = TEST_INPUT + i;
    }
    TEST_ASSERT_EQUAL_INT(0, tsrb_add(&_tsrb, _io_buffer, 0));
    TEST_ASSERT_EQUAL_INT(BUFFER_SIZE, tsrb_add(&_tsrb, _io_buffer,
                                                sizeof(_io_buffer)));
    TEST_ASSERT_EQUAL_INT(1, tsrb_full(&_tsrb));
    for (int i = 0; i < BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT((TEST_INPUT + i), tsrb_get_one(&_tsrb));
    }
}

static Test *tests_tsrb_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_clear),
        new_TestFixture(test_empty),
        new_TestFixture(test_avail),
        new_TestFixture(test_full),
        new_TestFixture(test_free),
        new_TestFixture(test_get_one),
        new_TestFixture(test_peek_one),
        new_TestFixture(test_get),
        new_TestFixture(test_peek),
        new_TestFixture(test_drop),
        new_TestFixture(test_add_one),
        new_TestFixture(test_add),
    };

    EMB_UNIT_TESTCALLER(tsrb_tests, NULL, tear_down, fixtures);

    return (Test *)&tsrb_tests;
}

void tests_tsrb(void)
{
    TESTS_RUN(tests_tsrb_tests());
}
/** @} */
