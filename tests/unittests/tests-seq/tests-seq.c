/*
 * SPDX-FileCopyrightText: 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdint.h>

#include "embUnit/embUnit.h"

#include "unittests-constants.h"
#include "seq.h"
#include "tests-seq.h"

seq8_t s8_1, s8_2;
seq16_t s16_1, s16_2;
seq32_t s32_1, s32_2;
seq64_t s64_1, s64_2;

static void set_up(void)
{
    s8_1 = 0; s8_2 = 0;
    s16_1 = 0; s16_2 = 0;
    s32_1 = 0; s32_2 = 0;
    s64_1 = 0; s64_2 = 0;
}

/* --------------------------------------- seq8_t tests --------------------------------------- */
static void test_seq8_equal(void)
{
    s8_1 = 0; s8_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq8_compare(s8_1, s8_2));
    s8_1 = TEST_UINT8; s8_2 = TEST_UINT8;
    TEST_ASSERT_EQUAL_INT(0, seq8_compare(s8_1, s8_2));
}

static void test_seq8_custom_space_equal(void)
{
    s8_1 = 0; s8_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq8_compares(s8_1, s8_2, 127));
    s8_1 = 42; s8_2 = 42;
    TEST_ASSERT_EQUAL_INT(0, seq8_compares(s8_1, s8_2, 127));
}

static void test_seq8_less_than(void)
{
    s8_1 = 0; s8_2 = 0;
    s8_2 = seq8_inc(s8_2);
    TEST_ASSERT_EQUAL_INT(-1, seq8_compare(s8_1, s8_2));
    s8_1 = UINT8_MAX; s8_2 = UINT8_MAX;
    s8_2 = seq8_add(s8_2, SEQ_LIMIT(UINT8_MAX));
    TEST_ASSERT_EQUAL_INT(-1, seq8_compare(s8_1, s8_2));
}

static void test_seq8_custom_space_less_than(void)
{
    s8_1 = 0; s8_2 = 0;
    s8_2 = seq8_incs(s8_2, 127);
    TEST_ASSERT_EQUAL_INT(-1, seq8_compares(s8_1, s8_2, 127));
    s8_1 = 127; s8_2 = 127;
    s8_2 = seq8_adds(s8_2, SEQ_LIMIT(127), 127);
    TEST_ASSERT_EQUAL_INT(-1, seq8_compares(s8_1, s8_2, 127));
}

static void test_seq8_greater_than(void)
{
    s8_1 = 0; s8_2 = 0;
    s8_2 = seq8_add(s8_2, (SEQ_LIMIT(UINT8_MAX)));
    s8_2 = seq8_add(s8_2, 2);
    TEST_ASSERT_EQUAL_INT(1, seq8_compare(s8_1, s8_2));
}

static void test_seq8_custom_space_greater_than(void)
{
    s8_1 = 0; s8_2 = 0;
    s8_2 = seq8_adds(s8_2, SEQ_LIMIT(127), 127);
    s8_2 = seq8_adds(s8_2, 2, 127);
    TEST_ASSERT_EQUAL_INT(1, seq8_compares(s8_1, s8_2, 127));
}

static void test_seq8_add(void)
{
    s8_1 = 0; s8_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq8_add(s8_1, 0));
    s8_1 = seq8_add(s8_1, 0);
    TEST_ASSERT_EQUAL_INT(0, seq8_compare(s8_1, s8_2));
    TEST_ASSERT_EQUAL_INT(s8_2, seq8_add(s8_2, SEQ_LIMIT(UINT8_MAX) + 1));
    s8_1 = UINT8_MAX; s8_2 = UINT8_MAX;
    TEST_ASSERT_EQUAL_INT((seq8_t)(s8_1 + SEQ_LIMIT(UINT8_MAX)),
            seq8_add(s8_1, SEQ_LIMIT(UINT8_MAX)));
}

static void test_seq8_custom_space_add(void)
{
    s8_1 = 0; s8_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq8_adds(s8_1, 0, 127));
    s8_1 = seq8_adds(s8_1, 0, 127);
    TEST_ASSERT_EQUAL_INT(0, seq8_compares(s8_1, s8_2, 127));
    TEST_ASSERT_EQUAL_INT(s8_2, seq8_adds(s8_2, SEQ_LIMIT(127) + 1, 127));
    s8_1 = 127; s8_2 = 127;
    TEST_ASSERT_EQUAL_INT((s8_1 + SEQ_LIMIT(127)) % (127 + 1),
            seq8_adds(s8_1, SEQ_LIMIT(127), 127));
}

static void test_seq8_compare_undefined(void)
{
    s8_1 = 0; s8_2 = SEQ_LIMIT(UINT8_MAX) + 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq8_compare(s8_1, s8_2));

    s8_1 = (0 + 2); s8_2 = (SEQ_LIMIT(UINT8_MAX) + 1 + 2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq8_compare(s8_1, s8_2));
}

static void test_seq8_custom_space_compare_undefined(void)
{
    s8_1 = 0; s8_2 = SEQ_LIMIT(127) + 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq8_compares(s8_1, s8_2, 127));

    s8_1 = (0 + 2); s8_2 = (SEQ_LIMIT(127) + 1 + 2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq8_compares(s8_1, s8_2, 127));
}

/* --------------------------------------- seq16_t tests --------------------------------------- */
static void test_seq16_equal(void)
{
    s16_1 = 0; s16_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq16_compare(s16_1, s16_2));
    s16_1 = TEST_UINT16; s16_2 = TEST_UINT16;
    TEST_ASSERT_EQUAL_INT(0, seq16_compare(s16_1, s16_2));
}

static void test_seq16_custom_space_equal(void)
{
    s16_1 = 0; s16_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq16_compares(s16_1, s16_2, 1023));
    s16_1 = 42; s16_2 = 42;
    TEST_ASSERT_EQUAL_INT(0, seq16_compares(s16_1, s16_2, 1023));
}

static void test_seq16_less_than(void)
{
    s16_1 = 0; s16_2 = 0;
    s16_2 = seq16_inc(s16_2);
    TEST_ASSERT_EQUAL_INT(-1, seq16_compare(s16_1, s16_2));
    s16_1 = UINT16_MAX; s16_2 = UINT16_MAX;
    s16_2 = seq16_add(s16_2, SEQ_LIMIT(UINT16_MAX));
    TEST_ASSERT_EQUAL_INT(-1, seq16_compare(s16_1, s16_2));
}

static void test_seq16_custom_space_less_than(void)
{
    s16_1 = 0; s16_2 = 0;
    s16_2 = seq16_incs(s16_2, 1023);
    TEST_ASSERT_EQUAL_INT(-1, seq16_compares(s16_1, s16_2, 1023));
    s16_1 = 1023; s16_2 = 1023;
    s16_2 = seq16_adds(s16_2, SEQ_LIMIT(1023), 1023);
    TEST_ASSERT_EQUAL_INT(-1, seq16_compares(s16_1, s16_2, 1023));
}

static void test_seq16_greater_than(void)
{
    s16_1 = 0; s16_2 = 0;
    s16_2 = seq16_add(s16_2, (SEQ_LIMIT(UINT16_MAX)));
    s16_2 = seq16_add(s16_2, 2);
    TEST_ASSERT_EQUAL_INT(1, seq16_compare(s16_1, s16_2));
}

static void test_seq16_custom_space_greater_than(void)
{
    s16_1 = 0; s16_2 = 0;
    s16_2 = seq16_adds(s16_2, SEQ_LIMIT(1023), 1023);
    s16_2 = seq16_adds(s16_2, 2, 1023);
    TEST_ASSERT_EQUAL_INT(1, seq16_compares(s16_1, s16_2, 1023));
}

static void test_seq16_add(void)
{
    s16_1 = 0; s16_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq16_add(s16_1, 0));
    s16_1 = seq16_add(s16_1, 0);
    TEST_ASSERT_EQUAL_INT(0, seq16_compare(s16_1, s16_2));
    TEST_ASSERT_EQUAL_INT(s16_2, seq16_add(s16_2, SEQ_LIMIT(UINT16_MAX) + 1));
    s16_1 = UINT16_MAX; s16_2 = UINT16_MAX;
    TEST_ASSERT_EQUAL_INT((seq16_t)(s16_1 + SEQ_LIMIT(UINT16_MAX)),
            seq16_add(s16_1, SEQ_LIMIT(UINT16_MAX)));
}

static void test_seq16_custom_space_add(void)
{
    s16_1 = 0; s16_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq16_adds(s16_1, 0, 1023));
    s16_1 = seq16_adds(s16_1, 0, 1023);
    TEST_ASSERT_EQUAL_INT(0, seq16_compares(s16_1, s16_2, 1023));
    TEST_ASSERT_EQUAL_INT(s16_2, seq16_adds(s16_2, SEQ_LIMIT(1023) + 1, 1023));
    s16_1 = 1023; s16_2 = 1023;
    TEST_ASSERT_EQUAL_INT((s16_1 + SEQ_LIMIT(1023)) % (1023 + 1),
            seq16_adds(s16_1, SEQ_LIMIT(1023), 1023));
}

static void test_seq16_compare_undefined(void)
{
    s16_1 = 0; s16_2 = SEQ_LIMIT(UINT16_MAX) + 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq16_compare(s16_1, s16_2));

    s16_1 = (0 + 2); s16_2 = (SEQ_LIMIT(UINT16_MAX) + 1 + 2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq16_compare(s16_1, s16_2));
}

static void test_seq16_custom_space_compare_undefined(void)
{
    s16_1 = 0; s16_2 = SEQ_LIMIT(1023) + 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq16_compares(s16_1, s16_2, 1023));

    s16_1 = (0 + 2); s16_2 = (SEQ_LIMIT(1023) + 1 + 2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq16_compares(s16_1, s16_2, 1023));
}

/* --------------------------------------- seq32_t tests --------------------------------------- */
static void test_seq32_equal(void)
{
    s32_1 = 0; s32_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq32_compare(s32_1, s32_2));
    s32_1 = TEST_UINT32; s32_2 = TEST_UINT32;
    TEST_ASSERT_EQUAL_INT(0, seq32_compare(s32_1, s32_2));
}

static void test_seq32_custom_space_equal(void)
{
    s32_1 = 0; s32_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq32_compares(s32_1, s32_2, ((1ULL << 20) - 1)));
    s32_1 = 42; s32_2 = 42;
    TEST_ASSERT_EQUAL_INT(0, seq32_compares(s32_1, s32_2, ((1ULL << 20) - 1)));
}

static void test_seq32_less_than(void)
{
    s32_1 = 0; s32_2 = 0;
    s32_2 = seq32_inc(s32_2);
    TEST_ASSERT_EQUAL_INT(-1, seq32_compare(s32_1, s32_2));
    s32_1 = UINT32_MAX; s32_2 = UINT32_MAX;
    s32_2 = seq32_add(s32_2, SEQ_LIMIT(UINT32_MAX));
    TEST_ASSERT_EQUAL_INT(-1, seq32_compare(s32_1, s32_2));
}

static void test_seq32_custom_space_less_than(void)
{
    s32_1 = 0; s32_2 = 0;
    s32_2 = seq32_incs(s32_2, ((1ULL << 20) - 1));
    TEST_ASSERT_EQUAL_INT(-1, seq32_compares(s32_1, s32_2, ((1ULL << 20) - 1)));
    s32_1 = ((1ULL << 20) - 1); s32_2 = ((1ULL << 20) - 1);
    s32_2 = seq32_adds(s32_2, SEQ_LIMIT(((1ULL << 20) - 1)), ((1ULL << 20) - 1));
    TEST_ASSERT_EQUAL_INT(-1, seq32_compares(s32_1, s32_2, ((1ULL << 20) - 1)));
}

static void test_seq32_greater_than(void)
{
    s32_1 = 0; s32_2 = 0;
    s32_2 = seq32_add(s32_2, (SEQ_LIMIT(UINT32_MAX)));
    s32_2 = seq32_add(s32_2, 2);
    TEST_ASSERT_EQUAL_INT(1, seq32_compare(s32_1, s32_2));
}

static void test_seq32_custom_space_greater_than(void)
{
    s32_1 = 0; s32_2 = 0;
    s32_2 = seq32_adds(s32_2, SEQ_LIMIT(((1ULL << 20) - 1)), ((1ULL << 20) - 1));
    s32_2 = seq32_adds(s32_2, 2, ((1ULL << 20) - 1));
    TEST_ASSERT_EQUAL_INT(1, seq32_compares(s32_1, s32_2, ((1ULL << 20) - 1)));
}

static void test_seq32_add(void)
{
    s32_1 = 0; s32_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq32_add(s32_1, 0));
    s32_1 = seq32_add(s32_1, 0);
    TEST_ASSERT_EQUAL_INT(0, seq32_compare(s32_1, s32_2));
    TEST_ASSERT_EQUAL_INT(s32_2, seq32_add(s32_2, SEQ_LIMIT(UINT32_MAX) + 1));
    s32_1 = UINT32_MAX; s32_2 = UINT32_MAX;
    TEST_ASSERT_EQUAL_INT((seq32_t)(s32_1 + SEQ_LIMIT(UINT32_MAX)),
                    seq32_add(s32_1, SEQ_LIMIT(UINT32_MAX)));
}

static void test_seq32_custom_space_add(void)
{
    s32_1 = 0; s32_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq32_adds(s32_1, 0, ((1ULL << 20) - 1)));
    s32_1 = seq32_adds(s32_1, 0, ((1ULL << 20) - 1));
    TEST_ASSERT_EQUAL_INT(0, seq32_compares(s32_1, s32_2, ((1ULL << 20) - 1)));
    TEST_ASSERT_EQUAL_INT(s32_2, seq32_adds(s32_2, SEQ_LIMIT(((1ULL << 20) - 1)) + 1,
                            ((1ULL << 20) - 1)));
    s32_1 = ((1ULL << 20) - 1); s32_2 = ((1ULL << 20) - 1);
    TEST_ASSERT_EQUAL_INT((s32_1 + SEQ_LIMIT(((1ULL << 20) - 1))) % (((1ULL << 20) - 1) + 1),
            seq32_adds(s32_1, SEQ_LIMIT(((1ULL << 20) - 1)), ((1ULL << 20) - 1)));
}

static void test_seq32_compare_undefined(void)
{
    s32_1 = 0; s32_2 = SEQ_LIMIT(UINT32_MAX) + 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq32_compare(s32_1, s32_2));

    s32_1 = (0 + 2); s32_2 = (SEQ_LIMIT(UINT32_MAX) + 1 + 2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq32_compare(s32_1, s32_2));
}

static void test_seq32_custom_space_compare_undefined(void)
{
    s32_1 = 0; s32_2 = SEQ_LIMIT(((1ULL << 20) - 1)) + 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq32_compares(s32_1, s32_2, ((1ULL << 20) - 1)));

    s32_1 = (0 + 2); s32_2 = (SEQ_LIMIT(((1ULL << 20) - 1)) + 1 + 2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq32_compares(s32_1, s32_2, ((1ULL << 20) - 1)));
}

/* --------------------------------------- seq64_t tests --------------------------------------- */
static void test_seq64_equal(void)
{
    s64_1 = 0; s64_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq64_compare(s64_1, s64_2));
    s64_1 = TEST_UINT64; s64_2 = TEST_UINT64;
    TEST_ASSERT_EQUAL_INT(0, seq64_compare(s64_1, s64_2));
}

static void test_seq64_custom_space_equal(void)
{
    s64_1 = 0; s64_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq64_compares(s64_1, s64_2, ((1ULL << 50) - 1)));
    s64_1 = 42; s64_2 = 42;
    TEST_ASSERT_EQUAL_INT(0, seq64_compares(s64_1, s64_2, ((1ULL << 50) - 1)));
}

static void test_seq64_less_than(void)
{
    s64_1 = 0; s64_2 = 0;
    s64_2 = seq64_inc(s64_2);
    TEST_ASSERT_EQUAL_INT(-1, seq64_compare(s64_1, s64_2));
    s64_1 = UINT64_MAX; s64_2 = UINT64_MAX;
    s64_2 = seq64_add(s64_2, SEQ_LIMIT(UINT64_MAX));
    TEST_ASSERT_EQUAL_INT(-1, seq64_compare(s64_1, s64_2));
}

static void test_seq64_custom_space_less_than(void)
{
    s64_1 = 0; s64_2 = 0;
    s64_2 = seq64_incs(s64_2, ((1ULL << 50) - 1));
    TEST_ASSERT_EQUAL_INT(-1, seq64_compares(s64_1, s64_2, ((1ULL << 50) - 1)));
    s64_1 = ((1ULL << 50) - 1); s64_2 = ((1ULL << 50) - 1);
    s64_2 = seq64_adds(s64_2, SEQ_LIMIT(((1ULL << 50) - 1)), ((1ULL << 50) - 1));
    TEST_ASSERT_EQUAL_INT(-1, seq64_compares(s64_1, s64_2, ((1ULL << 50) - 1)));
}

static void test_seq64_greater_than(void)
{
    s64_1 = 0; s64_2 = 0;
    s64_2 = seq64_add(s64_2, (SEQ_LIMIT(UINT64_MAX)));
    s64_2 = seq64_add(s64_2, 2);
    TEST_ASSERT_EQUAL_INT(1, seq64_compare(s64_1, s64_2));
}

static void test_seq64_custom_space_greater_than(void)
{
    s64_1 = 0; s64_2 = 0;
    s64_2 = seq64_adds(s64_2, SEQ_LIMIT(((1ULL << 50) - 1)), ((1ULL << 50) - 1));
    s64_2 = seq64_adds(s64_2, 2, ((1ULL << 50) - 1));
    TEST_ASSERT_EQUAL_INT(1, seq64_compares(s64_1, s64_2, ((1ULL << 50) - 1)));
}

static void test_seq64_add(void)
{
    s64_1 = 0; s64_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq64_add(s64_1, 0));
    s64_1 = seq64_add(s64_1, 0);
    TEST_ASSERT_EQUAL_INT(0, seq64_compare(s64_1, s64_2));
    TEST_ASSERT_EQUAL_INT(s64_2, seq64_add(s64_2, SEQ_LIMIT(UINT64_MAX) + 1));
    s64_1 = UINT64_MAX; s64_2 = UINT64_MAX;
    TEST_ASSERT_EQUAL_INT((seq64_t)(s64_1 + SEQ_LIMIT(UINT64_MAX)),
                    seq64_add(s64_1, SEQ_LIMIT(UINT64_MAX)));
}

static void test_seq64_custom_space_add(void)
{
    s64_1 = 0; s64_2 = 0;
    TEST_ASSERT_EQUAL_INT(0, seq64_adds(s64_1, 0, ((1ULL << 50) - 1)));
    s64_1 = seq64_adds(s64_1, 0, ((1ULL << 50) - 1));
    TEST_ASSERT_EQUAL_INT(0, seq64_compares(s64_1, s64_2, ((1ULL << 50) - 1)));
    TEST_ASSERT_EQUAL_INT(s64_2, seq64_adds(s64_2, SEQ_LIMIT(((1ULL << 50) - 1)) + 1,
                            ((1ULL << 50) - 1)));
    s64_1 = ((1ULL << 50) - 1); s64_2 = ((1ULL << 50) - 1);
    TEST_ASSERT_EQUAL_INT((s64_1 + SEQ_LIMIT(((1ULL << 50) - 1))) % (((1ULL << 50) - 1) + 1),
            seq64_adds(s64_1, SEQ_LIMIT(((1ULL << 50) - 1)), ((1ULL << 50) - 1)));
}

static void test_seq64_compare_undefined(void)
{
    s64_1 = 0; s64_2 = SEQ_LIMIT(UINT64_MAX) + 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq64_compare(s64_1, s64_2));

    s64_1 = (0 + 2); s64_2 = (SEQ_LIMIT(UINT64_MAX) + 1 + 2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq64_compare(s64_1, s64_2));
}

static void test_seq64_custom_space_compare_undefined(void)
{
    s64_1 = 0; s64_2 = SEQ_LIMIT(((1ULL << 50) - 1)) + 1;
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq64_compares(s64_1, s64_2, ((1ULL << 50) - 1)));

    s64_1 = (0 + 2); s64_2 = (SEQ_LIMIT(((1ULL << 50) - 1)) + 1 + 2);
    TEST_ASSERT_EQUAL_INT(-EINVAL, seq64_compares(s64_1, s64_2, ((1ULL << 50) - 1)));
}

Test *tests_seq_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_seq8_equal),
        new_TestFixture(test_seq8_custom_space_equal),
        new_TestFixture(test_seq8_less_than),
        new_TestFixture(test_seq8_custom_space_less_than),
        new_TestFixture(test_seq8_greater_than),
        new_TestFixture(test_seq8_custom_space_greater_than),
        new_TestFixture(test_seq8_add),
        new_TestFixture(test_seq8_custom_space_add),
        new_TestFixture(test_seq8_compare_undefined),
        new_TestFixture(test_seq8_custom_space_compare_undefined),
        new_TestFixture(test_seq16_equal),
        new_TestFixture(test_seq16_custom_space_equal),
        new_TestFixture(test_seq16_less_than),
        new_TestFixture(test_seq16_custom_space_less_than),
        new_TestFixture(test_seq16_greater_than),
        new_TestFixture(test_seq16_custom_space_greater_than),
        new_TestFixture(test_seq16_add),
        new_TestFixture(test_seq16_custom_space_add),
        new_TestFixture(test_seq16_compare_undefined),
        new_TestFixture(test_seq16_custom_space_compare_undefined),
        new_TestFixture(test_seq32_equal),
        new_TestFixture(test_seq32_custom_space_equal),
        new_TestFixture(test_seq32_less_than),
        new_TestFixture(test_seq32_custom_space_less_than),
        new_TestFixture(test_seq32_greater_than),
        new_TestFixture(test_seq32_custom_space_greater_than),
        new_TestFixture(test_seq32_add),
        new_TestFixture(test_seq32_custom_space_add),
        new_TestFixture(test_seq32_compare_undefined),
        new_TestFixture(test_seq32_custom_space_compare_undefined),
        new_TestFixture(test_seq64_equal),
        new_TestFixture(test_seq64_custom_space_equal),
        new_TestFixture(test_seq64_less_than),
        new_TestFixture(test_seq64_custom_space_less_than),
        new_TestFixture(test_seq64_greater_than),
        new_TestFixture(test_seq64_custom_space_greater_than),
        new_TestFixture(test_seq64_add),
        new_TestFixture(test_seq64_custom_space_add),
        new_TestFixture(test_seq64_compare_undefined),
        new_TestFixture(test_seq64_custom_space_compare_undefined),
    };

    EMB_UNIT_TESTCALLER(seq_tests, set_up, NULL, fixtures);

    return (Test *)&seq_tests;
}

void tests_seq(void)
{
    TESTS_RUN(tests_seq_tests());
}
/** @} */
