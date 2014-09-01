/*
 * Copyright (C) 2014 Martine Lenders <mail@martine-lenders.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file    tests-pktbuf.c
 */
#include <errno.h>
#include <stdint.h>

#include "embUnit/embUnit.h"

#include "pktbuf.h"

#include "tests-pktbuf.h"

typedef struct __attribute__((packed)) {
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    int8_t s8;
    int16_t s16;
    int32_t s32;
    int64_t s64;
} test_pktbuf_struct_t;

static void tear_down(void)
{
    pktbuf_reset();
}

static void test_pktbuf_alloc_0(void)
{
    TEST_ASSERT_NULL(pktbuf_alloc(0));
}

static void test_pktbuf_alloc_memfull(void)
{
    for (int i = 0; i < 9; i++) {
        TEST_ASSERT_NOT_NULL(pktbuf_alloc((PKTBUF_SIZE / 10) + 4));
        /* Why 4? Because: http://xkcd.com/221/, thats why ;-) */
    }

    TEST_ASSERT_NULL(pktbuf_alloc((PKTBUF_SIZE / 10) + 4));
}

static void test_pktbuf_alloc_success(void)
{
    void *data, *data_prev = NULL;

    for (int i = 0; i < 9; i++) {
        data = pktbuf_alloc((PKTBUF_SIZE / 10) + 4);

        TEST_ASSERT(data_prev < data);

        data_prev = data;
    }
}

static void test_pktbuf_realloc_0(void)
{
    void *data = pktbuf_alloc(512);

    TEST_ASSERT_NULL(pktbuf_realloc(data, 0));
}

static void test_pktbuf_realloc_memfull(void)
{
    void *data = pktbuf_alloc(512);

    TEST_ASSERT_NULL(pktbuf_realloc(data, PKTBUF_SIZE + 1));
}

static void test_pktbuf_realloc_memfull2(void)
{
    void *data = pktbuf_alloc(512);

    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(512));
    TEST_ASSERT_NULL(pktbuf_realloc(data, PKTBUF_SIZE - 512));
}

static void test_pktbuf_realloc_memfull3(void)
{
    void *data;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));

    data = pktbuf_alloc(512);

    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(73));

    TEST_ASSERT_NULL(pktbuf_realloc(data, PKTBUF_SIZE - 512));
}

static void test_pktbuf_realloc_smaller(void)
{
    void *data;

    data = pktbuf_alloc(512);

    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));

    TEST_ASSERT(data == pktbuf_realloc(data, 128));
}

static void test_pktbuf_realloc_memenough(void)
{
    void *data;

    data = pktbuf_alloc(128);

    TEST_ASSERT_NOT_NULL(data);

    TEST_ASSERT(data == pktbuf_realloc(data, 200));
}

static void test_pktbuf_realloc_memenough2(void)
{
    void *data, *data2;

    data = pktbuf_alloc(128);

    TEST_ASSERT_NOT_NULL(data);

    data2 = pktbuf_alloc(128);

    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));

    pktbuf_release(data2);

    TEST_ASSERT(data == pktbuf_realloc(data, 200));
}

static void test_pktbuf_realloc_nomemenough(void)
{
    void *data, *data2;

    data = pktbuf_alloc(128);

    TEST_ASSERT_NOT_NULL(data);

    data2 = pktbuf_alloc(128);

    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));

    pktbuf_release(data2);

    TEST_ASSERT(data != pktbuf_realloc(data, 512));
}

static void test_pktbuf_realloc_unknown_ptr(void)
{
    char *data = "abcd", *new_data = pktbuf_realloc(data, 5);

    TEST_ASSERT_NOT_NULL(new_data);
    TEST_ASSERT(data != new_data);
    TEST_ASSERT_EQUAL_STRING(data, new_data);
}

static void test_pktbuf_insert_size_0(void)
{
    TEST_ASSERT_NULL(pktbuf_insert("", 0));
}

static void test_pktbuf_insert_data_NULL(void)
{
    TEST_ASSERT_NULL(pktbuf_insert(NULL, 4));
}

static void test_pktbuf_insert_memfull(void)
{
    while (pktbuf_insert("abc", 4));

    TEST_ASSERT_NULL(pktbuf_insert("abc", 4));
}

static void test_pktbuf_insert_success(void)
{
    char *data, *data_prev = NULL;

    for (int i = 0; i < 10; i++) {
        data = (char *)pktbuf_insert("abc", 4);

        TEST_ASSERT(data_prev < data);
        TEST_ASSERT_EQUAL_STRING("abc", data);

        data_prev = data;
    }
}

#ifdef DEVELHELP
static void test_pktbuf_copy_efault(void)
{
    char *data = (char *)pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(-EFAULT, pktbuf_copy(data, NULL, 3));
    TEST_ASSERT_EQUAL_STRING("abcd", data);
}
#endif

static void test_pktbuf_copy_data_len_too_long(void)
{
    char *data = (char *)pktbuf_insert("ab", 3);

    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(-ENOMEM, pktbuf_copy(data, "cdef", 5));
    TEST_ASSERT_EQUAL_STRING("ab", data);
}

static void test_pktbuf_copy_data_len_too_long2(void)
{
    char *data = (char *)pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(-ENOMEM, pktbuf_copy(data + 2, "efgh", 5));
    TEST_ASSERT_EQUAL_STRING("abcd", data);
}

static void test_pktbuf_copy_data_len_0(void)
{
    char *data = (char *)pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(0, pktbuf_copy(data, "ef", 0));
    TEST_ASSERT_EQUAL_STRING("abcd", data);
}

static void test_pktbuf_copy_success(void)
{
    char *data = (char *)pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(3, pktbuf_copy(data, "ef", 3));
    TEST_ASSERT_EQUAL_STRING("ef", data);
}

static void test_pktbuf_copy_success2(void)
{
    char *data = (char *)pktbuf_insert("abcdef", 7);

    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_INT(2, pktbuf_copy(data + 3, "gh", 2));
    TEST_ASSERT_EQUAL_STRING("abcghf", data);
}

static void test_pktbuf_hold_ptr_null(void)
{
    char *data;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    data = (char *)pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());

    pktbuf_hold(NULL);
    pktbuf_release(data);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_STRING("abcd", data);
}

static void test_pktbuf_hold_wrong_ptr(void)
{
    char *data, wrong;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    data = (char *)pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());

    pktbuf_hold(&wrong);
    pktbuf_release(data);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_STRING("abcd", data);
}

static void test_pktbuf_hold_success(void)
{
    char *data;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    data = (char *)pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    pktbuf_hold(data);
    pktbuf_release(data);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_copy(data, "ef", 3));
    TEST_ASSERT_EQUAL_STRING("ef", data);
}

static void test_pktbuf_hold_success2(void)
{
    char *data;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    data = (char *)pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    pktbuf_hold(data + 4);
    pktbuf_release(data + 4);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_copy(data, "ef", 3));
    TEST_ASSERT_EQUAL_STRING("ef", data);
}

static void test_pktbuf_release_ptr_null(void)
{
    char *data;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    data = (char *)pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    pktbuf_release(NULL);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_copy(data, "ef", 3));
    TEST_ASSERT_EQUAL_STRING("ef", data);
}

static void test_pktbuf_release_wrong_ptr(void)
{
    char *data, wrong;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    data = (char *)pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    pktbuf_release(&wrong);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_copy(data, "ef", 3));
    TEST_ASSERT_EQUAL_STRING("ef", data);
}

static void test_pktbuf_release_success(void)
{
    char *data;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    data = (char *)pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());

    pktbuf_hold(data);
    pktbuf_hold(data);
    pktbuf_release(data + 3);
    pktbuf_release(data + 4);
    pktbuf_release(data + 2);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
}

static void test_pktbuf_release_success2(void)
{
    char *data1, *data2, *data3;

    data1 = (char *)pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(data1);
    data2 = (char *)pktbuf_insert("ef", 3);
    TEST_ASSERT_NOT_NULL(data2);
    data3 = (char *)pktbuf_insert("ghijkl", 7);
    TEST_ASSERT_NOT_NULL(data3);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());

    pktbuf_release(data2);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_INT(2, pktbuf_copy(data1, "m", 2));
    TEST_ASSERT_EQUAL_STRING("m", data1);
    TEST_ASSERT_EQUAL_INT(4, pktbuf_copy(data3, "nop", 4));
    TEST_ASSERT_EQUAL_STRING("nop", data3);
}

static void test_pktbuf_release_success3(void)
{
    char *data1, *data2, *data3;

    data1 = (char *)pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(data1);
    data2 = (char *)pktbuf_insert("ef", 3);
    TEST_ASSERT_NOT_NULL(data2);
    data3 = (char *)pktbuf_insert("ghijkl", 7);
    TEST_ASSERT_NOT_NULL(data3);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());

    pktbuf_release(data1);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_INT(2, pktbuf_copy(data2, "m", 2));
    TEST_ASSERT_EQUAL_STRING("m", data2);
    TEST_ASSERT_EQUAL_INT(4, pktbuf_copy(data3, "nop", 4));
    TEST_ASSERT_EQUAL_STRING("nop", data3);
}

static void test_pktbuf_release_success4(void)
{
    char *data1, *data2, *data3;

    data1 = (char *)pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(data1);
    data2 = (char *)pktbuf_insert("ef", 3);
    TEST_ASSERT_NOT_NULL(data2);
    data3 = (char *)pktbuf_insert("ghijkl", 7);
    TEST_ASSERT_NOT_NULL(data3);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());

    pktbuf_release(data3);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_INT(2, pktbuf_copy(data1, "m", 2));
    TEST_ASSERT_EQUAL_STRING("m", data1);
    TEST_ASSERT_EQUAL_INT(1, pktbuf_copy(data2, "", 1));
    TEST_ASSERT_EQUAL_STRING("", data2);
}

static void test_pktbuf_insert_packed_struct(void)
{
    test_pktbuf_struct_t data = { 0x4d, 0xef43, 0xacdef574, 0x43644305695afde5,
                                  34, -4469, 149699748, -46590430597
                                };
    test_pktbuf_struct_t *data_cpy;

    data_cpy = (test_pktbuf_struct_t *)pktbuf_insert(&data, sizeof(test_pktbuf_struct_t));

    TEST_ASSERT_EQUAL_INT(data.u8, data_cpy->u8);
    TEST_ASSERT_EQUAL_INT(data.u16, data_cpy->u16);
    TEST_ASSERT_EQUAL_INT(data.u32, data_cpy->u32);
    TEST_ASSERT_EQUAL_INT(data.u64, data_cpy->u64);
    TEST_ASSERT_EQUAL_INT(data.s8, data_cpy->s8);
    TEST_ASSERT_EQUAL_INT(data.s16, data_cpy->s16);
    TEST_ASSERT_EQUAL_INT(data.s32, data_cpy->s32);
    TEST_ASSERT_EQUAL_INT(data.s64, data_cpy->s64);
}

static void test_pktbuf_alloc_off_by_one1(void)
{
    char *data1, *data2, *data3, *data4;

    data1 = (char *)pktbuf_insert("1234567890a", 12);
    TEST_ASSERT_NOT_NULL(data1);
    data2 = (char *)pktbuf_alloc(44);
    TEST_ASSERT_NOT_NULL(data2);
    data4 = (char *)pktbuf_alloc(4);
    TEST_ASSERT_NOT_NULL(data4);
    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_INT(12 + 44 + 4, pktbuf_bytes_allocated());

    pktbuf_release(data1);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_INT(44 + 4, pktbuf_bytes_allocated());

    data3 = (char *)pktbuf_insert("bcdefghijklm", 13);
    TEST_ASSERT_NOT_NULL(data3);
    TEST_ASSERT(data1 != data3);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_INT(44 + 4 + 13, pktbuf_bytes_allocated());
}

Test *tests_pktbuf_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pktbuf_alloc_0),
        new_TestFixture(test_pktbuf_alloc_memfull),
        new_TestFixture(test_pktbuf_alloc_success),
        new_TestFixture(test_pktbuf_realloc_0),
        new_TestFixture(test_pktbuf_realloc_memfull),
        new_TestFixture(test_pktbuf_realloc_memfull2),
        new_TestFixture(test_pktbuf_realloc_memfull3),
        new_TestFixture(test_pktbuf_realloc_smaller),
        new_TestFixture(test_pktbuf_realloc_memenough),
        new_TestFixture(test_pktbuf_realloc_memenough2),
        new_TestFixture(test_pktbuf_realloc_nomemenough),
        new_TestFixture(test_pktbuf_realloc_unknown_ptr),
        new_TestFixture(test_pktbuf_insert_size_0),
        new_TestFixture(test_pktbuf_insert_data_NULL),
        new_TestFixture(test_pktbuf_insert_memfull),
        new_TestFixture(test_pktbuf_insert_success),
#ifdef DEVELHELP
        new_TestFixture(test_pktbuf_copy_efault),
#endif
        new_TestFixture(test_pktbuf_copy_data_len_too_long),
        new_TestFixture(test_pktbuf_copy_data_len_too_long2),
        new_TestFixture(test_pktbuf_copy_data_len_0),
        new_TestFixture(test_pktbuf_copy_success),
        new_TestFixture(test_pktbuf_copy_success2),
        new_TestFixture(test_pktbuf_hold_ptr_null),
        new_TestFixture(test_pktbuf_hold_wrong_ptr),
        new_TestFixture(test_pktbuf_hold_success),
        new_TestFixture(test_pktbuf_hold_success2),
        new_TestFixture(test_pktbuf_release_ptr_null),
        new_TestFixture(test_pktbuf_release_wrong_ptr),
        new_TestFixture(test_pktbuf_release_success),
        new_TestFixture(test_pktbuf_release_success2),
        new_TestFixture(test_pktbuf_release_success3),
        new_TestFixture(test_pktbuf_release_success4),
        new_TestFixture(test_pktbuf_insert_packed_struct),
        new_TestFixture(test_pktbuf_alloc_off_by_one1),
    };

    EMB_UNIT_TESTCALLER(pktbuf_tests, NULL, tear_down, fixtures);

    return (Test *)&pktbuf_tests;
}

void tests_pktbuf(void)
{
    TESTS_RUN(tests_pktbuf_tests());
}
/** @} */
