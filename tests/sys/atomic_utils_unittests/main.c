/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Unittests for the atomic utils module
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <string.h>

#include "embUnit.h"

#include "atomic_utils.h"
#include "volatile_utils.h"
#include "random.h"

#define ENABLE_DEBUG 0
#include "debug.h"

typedef uint8_t (*fetch_op_u8_t)(volatile uint8_t *dest, uint8_t val);
typedef uint16_t (*fetch_op_u16_t)(volatile uint16_t *dest, uint16_t val);
typedef uint32_t (*fetch_op_u32_t)(volatile uint32_t *dest, uint32_t val);
typedef uint64_t (*fetch_op_u64_t)(volatile uint64_t *dest, uint64_t val);

static void test_load_store(void)
{
    uint8_t u8 = 42;
    atomic_store_u8(&u8, 13);
    TEST_ASSERT_EQUAL_INT(atomic_load_u8(&u8), 13);

    uint16_t u16 = 42;
    atomic_store_u16(&u16, 1337);
    TEST_ASSERT_EQUAL_INT(atomic_load_u16(&u16), 1337);

    uint32_t u32 = 42;
    atomic_store_u32(&u32, 0x13371337);
    TEST_ASSERT_EQUAL_INT(atomic_load_u32(&u32), 0x13371337);

    uint64_t u64 = 42;
    atomic_store_u64(&u64, 0x1337133713371337);
    TEST_ASSERT_EQUAL_INT(atomic_load_u64(&u64), 0x1337133713371337);

    void *ptr = NULL;
    atomic_store_ptr(&ptr, &u64);
    TEST_ASSERT(atomic_load_ptr(&ptr) == (void *)&u64);
}

static void test_fetch_op_u8(fetch_op_u8_t atomic_op, fetch_op_u8_t op)
{
    uint8_t i = 0;
    do {
        uint8_t state1 = 0x55, state2 = 0x55;
        uint8_t res1 = atomic_op(&state1, i);
        uint8_t res2 = op(&state2, i);
        TEST_ASSERT_EQUAL_INT(state1, state2);
        TEST_ASSERT_EQUAL_INT(res1, res2);
        i++;
    } while (i);
}

static void test_fetch_ops_u8(void)
{
    test_fetch_op_u8(atomic_fetch_add_u8, volatile_fetch_add_u8);
    test_fetch_op_u8(atomic_fetch_sub_u8, volatile_fetch_sub_u8);
    test_fetch_op_u8(atomic_fetch_or_u8, volatile_fetch_or_u8);
    test_fetch_op_u8(atomic_fetch_xor_u8, volatile_fetch_xor_u8);
    test_fetch_op_u8(atomic_fetch_and_u8, volatile_fetch_and_u8);
    test_fetch_op_u8(semi_atomic_fetch_add_u8, volatile_fetch_add_u8);
    test_fetch_op_u8(semi_atomic_fetch_sub_u8, volatile_fetch_sub_u8);
    test_fetch_op_u8(semi_atomic_fetch_or_u8, volatile_fetch_or_u8);
    test_fetch_op_u8(semi_atomic_fetch_xor_u8, volatile_fetch_xor_u8);
    test_fetch_op_u8(semi_atomic_fetch_and_u8, volatile_fetch_and_u8);
}

static void test_fetch_op_u16(fetch_op_u16_t atomic_op, fetch_op_u16_t op)
{
    uint8_t i = 0;
    do {
        uint16_t state1 = 0x5555, state2 = 0x5555;
        uint16_t num = random_uint32();
        uint16_t res1 = atomic_op(&state1, num);
        uint16_t res2 = op(&state2, num);
        TEST_ASSERT_EQUAL_INT(state1, state2);
        TEST_ASSERT_EQUAL_INT(res1, res2);
        i++;
    } while (i);
}

static void test_fetch_ops_u16(void)
{
    test_fetch_op_u16(atomic_fetch_add_u16, volatile_fetch_add_u16);
    test_fetch_op_u16(atomic_fetch_sub_u16, volatile_fetch_sub_u16);
    test_fetch_op_u16(atomic_fetch_or_u16, volatile_fetch_or_u16);
    test_fetch_op_u16(atomic_fetch_xor_u16, volatile_fetch_xor_u16);
    test_fetch_op_u16(atomic_fetch_and_u16, volatile_fetch_and_u16);
    test_fetch_op_u16(semi_atomic_fetch_add_u16, volatile_fetch_add_u16);
    test_fetch_op_u16(semi_atomic_fetch_sub_u16, volatile_fetch_sub_u16);
    test_fetch_op_u16(semi_atomic_fetch_or_u16, volatile_fetch_or_u16);
    test_fetch_op_u16(semi_atomic_fetch_xor_u16, volatile_fetch_xor_u16);
    test_fetch_op_u16(semi_atomic_fetch_and_u16, volatile_fetch_and_u16);
}

static void test_fetch_op_u32(fetch_op_u32_t atomic_op, fetch_op_u32_t op)
{
    uint8_t i = 0;
    do {
        uint32_t state1 = 0x55555555, state2 = 0x55555555;
        uint32_t num = random_uint32();
        uint32_t res1 = atomic_op(&state1, num);
        uint32_t res2 = op(&state2, num);
        TEST_ASSERT_EQUAL_INT(state1, state2);
        TEST_ASSERT_EQUAL_INT(res1, res2);
        i++;
    } while (i);
}

static void test_fetch_ops_u32(void)
{
    test_fetch_op_u32(atomic_fetch_add_u32, volatile_fetch_add_u32);
    test_fetch_op_u32(atomic_fetch_sub_u32, volatile_fetch_sub_u32);
    test_fetch_op_u32(atomic_fetch_or_u32, volatile_fetch_or_u32);
    test_fetch_op_u32(atomic_fetch_xor_u32, volatile_fetch_xor_u32);
    test_fetch_op_u32(atomic_fetch_and_u32, volatile_fetch_and_u32);
    test_fetch_op_u32(semi_atomic_fetch_add_u32, volatile_fetch_add_u32);
    test_fetch_op_u32(semi_atomic_fetch_sub_u32, volatile_fetch_sub_u32);
    test_fetch_op_u32(semi_atomic_fetch_or_u32, volatile_fetch_or_u32);
    test_fetch_op_u32(semi_atomic_fetch_xor_u32, volatile_fetch_xor_u32);
    test_fetch_op_u32(semi_atomic_fetch_and_u32, volatile_fetch_and_u32);
}

static void test_fetch_op_u64(fetch_op_u64_t atomic_op, fetch_op_u64_t op)
{
    uint8_t i = 0;
    do {
        uint64_t state1, state2;
        state1 = state2 = 0x5555555555555555;
        uint64_t num;
        random_bytes((void *)&num, sizeof(num));
        uint64_t res1 = atomic_op(&state1, num);
        uint64_t res2 = op(&state2, num);
        TEST_ASSERT_EQUAL_INT(state1, state2);
        TEST_ASSERT_EQUAL_INT(res1, res2);
        i++;
    } while (i);
}

static void test_fetch_ops_u64(void)
{
    test_fetch_op_u64(atomic_fetch_add_u64, volatile_fetch_add_u64);
    test_fetch_op_u64(atomic_fetch_sub_u64, volatile_fetch_sub_u64);
    test_fetch_op_u64(atomic_fetch_or_u64, volatile_fetch_or_u64);
    test_fetch_op_u64(atomic_fetch_xor_u64, volatile_fetch_xor_u64);
    test_fetch_op_u64(atomic_fetch_and_u64, volatile_fetch_and_u64);
    test_fetch_op_u64(semi_atomic_fetch_add_u64, volatile_fetch_add_u64);
    test_fetch_op_u64(semi_atomic_fetch_sub_u64, volatile_fetch_sub_u64);
    test_fetch_op_u64(semi_atomic_fetch_or_u64, volatile_fetch_or_u64);
    test_fetch_op_u64(semi_atomic_fetch_xor_u64, volatile_fetch_xor_u64);
    test_fetch_op_u64(semi_atomic_fetch_and_u64, volatile_fetch_and_u64);
}

static void test_atomic_set_bit(void)
{
    {
        uint8_t val1 = 0, val2 = 0;
        for (uint8_t i = 0; i < 8; i++) {
            atomic_set_bit_u8(atomic_bit_u8(&val1, i));
            val2 |= 1ULL << i;
            TEST_ASSERT_EQUAL_INT(val2, val1);
        }
    }

    {
        uint16_t val1 = 0, val2 = 0;
        for (uint8_t i = 0; i < 16; i++) {
            atomic_set_bit_u16(atomic_bit_u16(&val1, i));
            val2 |= 1ULL << i;
            TEST_ASSERT_EQUAL_INT(val2, val1);
        }
    }

    {
        uint32_t val1 = 0, val2 = 0;
        for (uint8_t i = 0; i < 32; i++) {
            atomic_set_bit_u32(atomic_bit_u32(&val1, i));
            val2 |= 1ULL << i;
            TEST_ASSERT_EQUAL_INT(val2, val1);
        }
    }

    {
        uint64_t val1 = 0, val2 = 0;
        for (uint8_t i = 0; i < 32; i++) {
            atomic_set_bit_u64(atomic_bit_u64(&val1, i));
            val2 |= 1ULL << i;
            TEST_ASSERT(val2 == val1);
        }
    }
}

static void test_atomic_clear_bit(void)
{
    {
        uint8_t val1 = 0xff, val2 = 0xff;
        for (uint8_t i = 0; i < 8; i++) {
            atomic_clear_bit_u8(atomic_bit_u8(&val1, i));
            val2 &= ~(1ULL << i);
            TEST_ASSERT_EQUAL_INT(val2, val1);
        }
    }

    {
        uint16_t val1 = 0xffff, val2 = 0xffff;
        for (uint8_t i = 0; i < 16; i++) {
            atomic_clear_bit_u16(atomic_bit_u16(&val1, i));
            val2 &= ~(1ULL << i);
            TEST_ASSERT_EQUAL_INT(val2, val1);
        }
    }

    {
        uint32_t val1 = 0xffffffff, val2 = 0xffffffff;
        for (uint8_t i = 0; i < 32; i++) {
            atomic_clear_bit_u32(atomic_bit_u32(&val1, i));
            val2 &= ~(1ULL << i);
            TEST_ASSERT_EQUAL_INT(val2, val1);
        }
    }

    {
        uint64_t val1 = 0xffffffffffffffff, val2 = 0xffffffffffffffff;
        for (uint8_t i = 0; i < 32; i++) {
            atomic_clear_bit_u64(atomic_bit_u64(&val1, i));
            val2 &= ~(1ULL << i);
            TEST_ASSERT(val2 == val1);
        }
    }
}

Test *tests_atomic_utils_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_load_store),
        new_TestFixture(test_fetch_ops_u8),
        new_TestFixture(test_fetch_ops_u16),
        new_TestFixture(test_fetch_ops_u32),
        new_TestFixture(test_fetch_ops_u64),
        new_TestFixture(test_atomic_set_bit),
        new_TestFixture(test_atomic_clear_bit),
    };

    EMB_UNIT_TESTCALLER(atomic_utils_tests, NULL, NULL, fixtures);

    return (Test *)&atomic_utils_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_atomic_utils_tests());
    TESTS_END();

    return 0;
}
