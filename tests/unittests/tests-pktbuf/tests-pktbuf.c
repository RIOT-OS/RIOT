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

#include "embUnit.h"

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
    pkt_t *pkt, *pkt_prev = NULL;

    for (int i = 0; i < 9; i++) {
        pkt = pktbuf_alloc((PKTBUF_SIZE / 10) + 4);

        TEST_ASSERT_NOT_NULL(pkt);

        if (pkt_prev != NULL) {
            TEST_ASSERT(pkt_prev < pkt);
            TEST_ASSERT(pkt_prev->payload_data < pkt->payload_data);
        }

        pkt_prev = pkt;
    }
}

static void test_pktbuf_realloc_payload_0(void)
{
    pkt_t *pkt = pktbuf_alloc(512);

    TEST_ASSERT_NULL(pktbuf_realloc_payload(pkt, 0));
}

static void test_pktbuf_realloc_payload_memfull(void)
{
    pkt_t *pkt = pktbuf_alloc(512);

    TEST_ASSERT_NULL(pktbuf_realloc_payload(pkt, PKTBUF_SIZE + 1));
}

static void test_pktbuf_realloc_payload_memfull2(void)
{
    pkt_t *pkt = pktbuf_alloc(512);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(512));
    TEST_ASSERT_NULL(pktbuf_realloc_payload(pkt, PKTBUF_SIZE - 512));
}

static void test_pktbuf_realloc_payload_memfull3(void)
{
    pkt_t *pkt;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));

    pkt = pktbuf_alloc(512);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(73));

    TEST_ASSERT_NULL(pktbuf_realloc_payload(pkt, PKTBUF_SIZE - 512));
}

static void test_pktbuf_realloc_payload_smaller(void)
{
    pkt_t *pkt;

    pkt = pktbuf_alloc(512);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));

    TEST_ASSERT(pkt == pktbuf_realloc_payload(pkt, 128));
}

static void test_pktbuf_realloc_payload_memenough(void)
{
    pkt_t *pkt;

    pkt = pktbuf_alloc(128);

    TEST_ASSERT_NOT_NULL(pkt);

    TEST_ASSERT(pkt == pktbuf_realloc_payload(pkt, 200));
}

static void test_pktbuf_realloc_payload_memenough2(void)
{
    pkt_t *pkt, *pkt2;

    pkt = pktbuf_alloc(128);

    TEST_ASSERT_NOT_NULL(pkt);

    pkt2 = pktbuf_alloc(128);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));

    pktbuf_release(pkt2);

    TEST_ASSERT(pkt == pktbuf_realloc_payload(pkt, 200));
}

static void test_pktbuf_realloc_payload_nomemenough(void)
{
    pkt_t *pkt, *pkt2;

    pkt = pktbuf_alloc(128);

    TEST_ASSERT_NOT_NULL(pkt);

    pkt2 = pktbuf_alloc(128);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));

    pktbuf_release(pkt2);

    TEST_ASSERT(pkt != pktbuf_realloc_payload(pkt, 512));
}

static void test_pktbuf_realloc_payload_unknown_ptr(void)
{
    pkt_t pkt = { NULL, "abcd", 4, PKT_PROTO_UNKNOWN };
    pkt_t *new_pkt = pktbuf_realloc_payload(&pkt, 5);

    TEST_ASSERT_NOT_NULL(new_pkt);
    TEST_ASSERT_NOT_NULL(new_pkt->payload_data);
    TEST_ASSERT(&pkt != new_pkt);
    TEST_ASSERT(pkt.payload_data != new_pkt->payload_data);
    TEST_ASSERT_EQUAL_STRING(pkt.payload_data, new_pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(4, pkt.payload_len);
    TEST_ASSERT_EQUAL_INT(5, new_pkt->payload_len);
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
    pkt_t *pkt, *pkt_prev = NULL;

    for (int i = 0; i < 10; i++) {
        pkt = pktbuf_insert("abc", 4);

        TEST_ASSERT_NOT_NULL(pkt);

        if (pkt_prev != NULL) {
            TEST_ASSERT(pkt_prev < pkt);
            TEST_ASSERT(pkt_prev->payload_data < pkt->payload_data);
        }

        TEST_ASSERT_EQUAL_STRING("abc", pkt->payload_data);
        TEST_ASSERT_EQUAL_INT(4, pkt->payload_len);

        pkt_prev = pkt;
    }
}

#ifdef DEVELHELP
static void test_pktbuf_copy_einval(void)
{
    TEST_ASSERT_EQUAL_INT(-EINVAL, pktbuf_copy(NULL, "ab", 3));
}

static void test_pktbuf_copy_efault(void)
{
    pkt_t *pkt = pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(-EFAULT, pktbuf_copy(pkt->payload_data, NULL, 3));
    TEST_ASSERT_EQUAL_STRING("abcd", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(5, pkt->payload_len);
}

static void test_pktbuf_copy_einval2(void)
{
    pkt_t *pkt = pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(-EINVAL, pktbuf_copy(pkt->headers, "ab", 3));
    TEST_ASSERT_EQUAL_STRING("abcd", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(5, pkt->payload_len);
}
#endif

static void test_pktbuf_copy_data_len_too_long(void)
{
    pkt_t *pkt = pktbuf_insert("ab", 3);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(-ENOMEM, pktbuf_copy(pkt->payload_data, "cdef", 5));
    TEST_ASSERT_EQUAL_STRING("ab", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(3, pkt->payload_len);
}

static void test_pktbuf_copy_data_len_too_long2(void)
{
    pkt_t *pkt = pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(-ENOMEM, pktbuf_copy(((uint8_t *)(pkt->payload_data)) + 2, "efgh", 5));
    TEST_ASSERT_EQUAL_STRING("abcd", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(5, pkt->payload_len);
}

static void test_pktbuf_copy_data_len_0(void)
{
    pkt_t *pkt = pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(0, pktbuf_copy(pkt->payload_data, "ef", 0));
    TEST_ASSERT_EQUAL_STRING("abcd", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(5, pkt->payload_len);
}

static void test_pktbuf_copy_success(void)
{
    pkt_t *pkt = pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(3, pktbuf_copy(pkt->payload_data, "ef", 3));
    TEST_ASSERT_EQUAL_STRING("ef", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(5, pkt->payload_len);
}

static void test_pktbuf_copy_success2(void)
{
    pkt_t *pkt = pktbuf_insert("abcdef", 7);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(2, pktbuf_copy(((uint8_t *)(pkt->payload_data)) + 3, "gh", 2));
    TEST_ASSERT_EQUAL_STRING("abcghf", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(7, pkt->payload_len);
}

#ifdef DEVELHELP
static void test_pktbuf_add_header_efault(void)
{
    pkt_t *pkt = pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(-EFAULT, pktbuf_add_header(pkt, NULL, 3));
    TEST_ASSERT_EQUAL_STRING("abcd", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(5, pkt->payload_len);
    TEST_ASSERT_NULL(pkt->headers);
}

static void test_pktbuf_add_header_einval(void)
{
    TEST_ASSERT_EQUAL_INT(-EINVAL, pktbuf_add_header(NULL, "abcd", 3));
}
#endif

static void test_pktbuf_add_header_einval2(void)
{
    pkt_t pkt = { NULL, "abcd", 5, PKT_PROTO_UNKNOWN };

    TEST_ASSERT_EQUAL_INT(-EINVAL, pktbuf_add_header(&pkt, "ef", 3));
    TEST_ASSERT_EQUAL_STRING("abcd", pkt.payload_data);
    TEST_ASSERT_EQUAL_INT(5, pkt.payload_len);
    TEST_ASSERT_NULL(pkt.headers);
}

static void test_pktbuf_add_header_header_len_too_long(void)
{
    pkt_t *pkt = pktbuf_insert("ab", 3);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(-ENOMEM, pktbuf_add_header(pkt, "cdef", PKTBUF_SIZE + 1));
    TEST_ASSERT_EQUAL_STRING("ab", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(3, pkt->payload_len);
}

static void test_pktbuf_add_header_header_len_0(void)
{
    pkt_t *pkt = pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(-ENOMEM, pktbuf_add_header(pkt, "ef", 0));
    TEST_ASSERT_EQUAL_STRING("abcd", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(5, pkt->payload_len);
    TEST_ASSERT_NULL(pkt->headers);
}

static void test_pktbuf_add_header_success(void)
{
    pkt_t *pkt = pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(0, pktbuf_add_header(pkt, "ef", 2));
    TEST_ASSERT_EQUAL_STRING("abcd", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(5, pkt->payload_len);
    TEST_ASSERT_NOT_NULL(pkt->headers);
    TEST_ASSERT_EQUAL_STRING("ef", pkt->headers->header_data);
    TEST_ASSERT_EQUAL_INT(2, pkt->headers->header_len);
    TEST_ASSERT_NULL(pkt->headers->next);
}

static void test_pktbuf_add_header_success2(void)
{
    pkt_t *pkt = pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(0, pktbuf_add_header(pkt, "ef", 2));
    TEST_ASSERT_NOT_NULL(pkt->headers);
    TEST_ASSERT_NULL(pkt->headers->next);
    TEST_ASSERT_EQUAL_INT(0, pktbuf_add_header(pkt, "ghi", 3));
    TEST_ASSERT_EQUAL_STRING("abcd", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(5, pkt->payload_len);
    TEST_ASSERT_NOT_NULL(pkt->headers);
    TEST_ASSERT_EQUAL_STRING("ghi", pkt->headers->header_data);
    TEST_ASSERT_EQUAL_INT(3, pkt->headers->header_len);
    TEST_ASSERT_NOT_NULL(pkt->headers->next);
    TEST_ASSERT_EQUAL_STRING("ef", pkt->headers->next->header_data);
    TEST_ASSERT_EQUAL_INT(2, pkt->headers->next->header_len);
    TEST_ASSERT_NULL(pkt->headers->next->next);
}

static void test_pktbuf_remove_header_success(void)
{
    pkt_t *pkt = pktbuf_insert("abcd", 5);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(0, pktbuf_add_header(pkt, "ef", 3));
    TEST_ASSERT_EQUAL_INT(0, pktbuf_add_header(pkt, "ghi", 4));
    TEST_ASSERT_EQUAL_INT(0, pktbuf_add_header(pkt, "j", 2));
    pktbuf_remove_header(pkt, pkt->headers->next);

    TEST_ASSERT_EQUAL_STRING("abcd", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(5, pkt->payload_len);
    TEST_ASSERT_NOT_NULL(pkt->headers);
    TEST_ASSERT_EQUAL_STRING("j", pkt->headers->header_data);
    TEST_ASSERT_EQUAL_INT(2, pkt->headers->header_len);
    TEST_ASSERT_NOT_NULL(pkt->headers->next);
    TEST_ASSERT_EQUAL_STRING("ef", pkt->headers->next->header_data);
    TEST_ASSERT_EQUAL_INT(3, pkt->headers->next->header_len);
    TEST_ASSERT_NULL(pkt->headers->next->next);

    pktbuf_remove_header(pkt, pkt->headers);

    TEST_ASSERT_EQUAL_STRING("abcd", pkt->payload_data);
    TEST_ASSERT_EQUAL_INT(5, pkt->payload_len);
    TEST_ASSERT_NOT_NULL(pkt->headers);
    TEST_ASSERT_EQUAL_STRING("ef", pkt->headers->header_data);
    TEST_ASSERT_EQUAL_INT(3, pkt->headers->header_len);
    TEST_ASSERT_NULL(pkt->headers->next);

    pktbuf_remove_header(pkt, pkt->headers);

    TEST_ASSERT_NULL(pkt->headers);
}

static void test_pktbuf_hold_ptr_null(void)
{
    pkt_t *pkt;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    pkt = pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());

    pktbuf_hold(NULL);
    pktbuf_release(pkt);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_STRING("abcd", pkt->payload_data);
}

static void test_pktbuf_hold_wrong_ptr(void)
{
    pkt_t *pkt, wrong;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    pkt = pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());

    pktbuf_hold(&wrong);
    pktbuf_release(pkt);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_STRING("abcd", pkt->payload_data);
}

static void test_pktbuf_hold_success(void)
{
    pkt_t *pkt;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    pkt = pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    pktbuf_hold(pkt);
    pktbuf_release(pkt);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_copy(pkt->payload_data, "ef", 3));
    TEST_ASSERT_EQUAL_STRING("ef", pkt->payload_data);
}

static void test_pktbuf_hold_success2(void)
{
    pkt_t *pkt;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    pkt = pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    pktbuf_hold(pkt + 4);
    pktbuf_release(pkt + 4);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_copy(pkt->payload_data, "ef", 3));
    TEST_ASSERT_EQUAL_STRING("ef", pkt->payload_data);
}

static void test_pktbuf_release_ptr_null(void)
{
    pkt_t *pkt;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    pkt = pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    pktbuf_release(NULL);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_copy(pkt->payload_data, "ef", 3));
    TEST_ASSERT_EQUAL_STRING("ef", pkt->payload_data);
}

static void test_pktbuf_release_wrong_ptr(void)
{
    pkt_t *pkt, wrong;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    pkt = pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    pktbuf_release(&wrong);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_copy(pkt->payload_data, "ef", 3));
    TEST_ASSERT_EQUAL_STRING("ef", pkt->payload_data);
}

static void test_pktbuf_release_success(void)
{
    pkt_t *pkt;

    TEST_ASSERT_NOT_NULL(pktbuf_alloc(25));
    pkt = pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(pktbuf_alloc(16));

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());

    pktbuf_hold(pkt);
    pktbuf_hold(pkt);
    pktbuf_release(pkt);
    pktbuf_release(pkt);
    pktbuf_release(pkt);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
}

static void test_pktbuf_release_success2(void)
{
    pkt_t *pkt1, *pkt2, *pkt3;

    pkt1 = pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(pkt1);
    pkt2 = pktbuf_insert("ef", 3);
    TEST_ASSERT_NOT_NULL(pkt2);
    pkt3 = pktbuf_insert("ghijkl", 7);
    TEST_ASSERT_NOT_NULL(pkt3);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());

    pktbuf_release(pkt2);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_INT(2, pktbuf_copy(pkt1->payload_data, "m", 2));
    TEST_ASSERT_EQUAL_STRING("m", pkt1->payload_data);
    TEST_ASSERT_EQUAL_INT(4, pktbuf_copy(pkt3->payload_data, "nop", 4));
    TEST_ASSERT_EQUAL_STRING("nop", pkt3->payload_data);
}

static void test_pktbuf_release_success3(void)
{
    pkt_t *pkt1, *pkt2, *pkt3;

    pkt1 = pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(pkt1);
    pkt2 = pktbuf_insert("ef", 3);
    TEST_ASSERT_NOT_NULL(pkt2);
    pkt3 = pktbuf_insert("ghijkl", 7);
    TEST_ASSERT_NOT_NULL(pkt3);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());

    pktbuf_release(pkt1);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_INT(2, pktbuf_copy(pkt2->payload_data, "m", 2));
    TEST_ASSERT_EQUAL_STRING("m", pkt2->payload_data);
    TEST_ASSERT_EQUAL_INT(4, pktbuf_copy(pkt3->payload_data, "nop", 4));
    TEST_ASSERT_EQUAL_STRING("nop", pkt3->payload_data);
}

static void test_pktbuf_release_success4(void)
{
    pkt_t *pkt1, *pkt2, *pkt3;

    pkt1 = pktbuf_insert("abcd", 5);
    TEST_ASSERT_NOT_NULL(pkt1);
    pkt2 = pktbuf_insert("ef", 3);
    TEST_ASSERT_NOT_NULL(pkt2);
    pkt3 = pktbuf_insert("ghijkl", 7);
    TEST_ASSERT_NOT_NULL(pkt3);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());

    pktbuf_release(pkt3);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_INT(2, pktbuf_copy(pkt1->payload_data, "m", 2));
    TEST_ASSERT_EQUAL_STRING("m", pkt1->payload_data);
    TEST_ASSERT_EQUAL_INT(1, pktbuf_copy(pkt2->payload_data, "", 1));
    TEST_ASSERT_EQUAL_STRING("", pkt2->payload_data);
}

static void test_pktbuf_insert_packed_struct(void)
{
    test_pktbuf_struct_t data = { 0x4d, 0xef43, 0xacdef574, 0x43644305695afde5,
                                  34, -4469, 149699748, -46590430597
                                };
    test_pktbuf_struct_t *data_cpy;
    pkt_t *pkt;

    pkt = pktbuf_insert(&data, sizeof(test_pktbuf_struct_t));
    data_cpy = (test_pktbuf_struct_t *)pkt->payload_data;

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
    pkt_t *pkt1, *pkt2, *pkt3, *pkt4;

    pkt1 = pktbuf_insert("1234567890a", 12);
    TEST_ASSERT_NOT_NULL(pkt1);
    pkt2 = pktbuf_alloc(44);
    TEST_ASSERT_NOT_NULL(pkt2);
    pkt4 = pktbuf_alloc(4);
    TEST_ASSERT_NOT_NULL(pkt4);
    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_INT(12 + 44 + 4, pktbuf_bytes_allocated());

    pktbuf_release(pkt1);

    TEST_ASSERT_EQUAL_INT(2, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_INT(44 + 4, pktbuf_bytes_allocated());

    pkt3 = pktbuf_insert("bcdefghijklm", 13);
    TEST_ASSERT_NOT_NULL(pkt3);
    TEST_ASSERT(pkt1 != pkt3);

    TEST_ASSERT_EQUAL_INT(3, pktbuf_packets_allocated());
    TEST_ASSERT_EQUAL_INT(44 + 4 + 13, pktbuf_bytes_allocated());
}

Test *tests_pktbuf_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pktbuf_alloc_0),
        new_TestFixture(test_pktbuf_alloc_memfull),
        new_TestFixture(test_pktbuf_alloc_success),
        new_TestFixture(test_pktbuf_realloc_payload_0),
        new_TestFixture(test_pktbuf_realloc_payload_memfull),
        new_TestFixture(test_pktbuf_realloc_payload_memfull2),
        new_TestFixture(test_pktbuf_realloc_payload_memfull3),
        new_TestFixture(test_pktbuf_realloc_payload_smaller),
        new_TestFixture(test_pktbuf_realloc_payload_memenough),
        new_TestFixture(test_pktbuf_realloc_payload_memenough2),
        new_TestFixture(test_pktbuf_realloc_payload_nomemenough),
        new_TestFixture(test_pktbuf_realloc_payload_unknown_ptr),
        new_TestFixture(test_pktbuf_insert_size_0),
        new_TestFixture(test_pktbuf_insert_data_NULL),
        new_TestFixture(test_pktbuf_insert_memfull),
        new_TestFixture(test_pktbuf_insert_success),
#ifdef DEVELHELP
        new_TestFixture(test_pktbuf_copy_efault),
        new_TestFixture(test_pktbuf_copy_einval),
        new_TestFixture(test_pktbuf_copy_einval2),
#endif
        new_TestFixture(test_pktbuf_copy_data_len_too_long),
        new_TestFixture(test_pktbuf_copy_data_len_too_long2),
        new_TestFixture(test_pktbuf_copy_data_len_0),
        new_TestFixture(test_pktbuf_copy_success),
        new_TestFixture(test_pktbuf_copy_success2),
#ifdef DEVELHELP
        new_TestFixture(test_pktbuf_add_header_efault),
        new_TestFixture(test_pktbuf_add_header_einval),
#endif
        new_TestFixture(test_pktbuf_add_header_einval2),
        new_TestFixture(test_pktbuf_add_header_header_len_too_long),
        new_TestFixture(test_pktbuf_add_header_header_len_0),
        new_TestFixture(test_pktbuf_add_header_success),
        new_TestFixture(test_pktbuf_add_header_success2),
        new_TestFixture(test_pktbuf_remove_header_success),
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
