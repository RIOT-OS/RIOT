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
 * @file
 */
#include <errno.h>
#include <stdint.h>

#include "embUnit.h"

#include "net/ng_nettype.h"
#include "net/ng_pkt.h"
#include "net/ng_pktbuf.h"

#include "unittests-constants.h"
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
    ng_pktbuf_reset();
}

static void test_pktbuf_reset(void)
{
    TEST_ASSERT(ng_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL(ng_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                                       NG_NETTYPE_UNDEF));
    TEST_ASSERT_NOT_NULL(ng_pktbuf_add(NULL, TEST_STRING8, sizeof(TEST_STRING8),
                                       NG_NETTYPE_UNDEF));
    TEST_ASSERT_NOT_NULL(ng_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                       NG_NETTYPE_UNDEF));
    TEST_ASSERT(!ng_pktbuf_is_empty());
    ng_pktbuf_reset();
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NULL__data_NULL__size_0(void)
{
    ng_pktsnip_t *pkt;

    TEST_ASSERT_NOT_NULL((pkt = ng_pktbuf_add(NULL, NULL, 0, NG_NETTYPE_UNDEF)));

    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(0, pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);

    TEST_ASSERT(!ng_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NOT_NULL__data_NULL__size_0(void)
{
    ng_pktsnip_t *pkt, *next = ng_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                               NG_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(next);

    TEST_ASSERT_NOT_NULL((pkt = ng_pktbuf_add(next, NULL, 0, NG_NETTYPE_UNDEF)));

    TEST_ASSERT(pkt->next == next);
    TEST_ASSERT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(0, pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);

    TEST_ASSERT_NULL(next->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, next->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), next->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);

    TEST_ASSERT(!ng_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NULL__data_NOT_NULL__size_0(void)
{
    ng_pktsnip_t *pkt;

    TEST_ASSERT_NOT_NULL((pkt = ng_pktbuf_add(NULL, TEST_STRING8, 0, NG_NETTYPE_UNDEF)));

    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_NOT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(0, pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);

    TEST_ASSERT(!ng_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NOT_NULL__data_NOT_NULL__size_0(void)
{
    ng_pktsnip_t *pkt, *next = ng_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                               NG_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(next);

    TEST_ASSERT_NOT_NULL((pkt = ng_pktbuf_add(next, TEST_STRING8, 0, NG_NETTYPE_UNDEF)));

    TEST_ASSERT(pkt->next == next);
    TEST_ASSERT_NOT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(0, pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);

    TEST_ASSERT_NULL(next->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, next->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), next->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);

    TEST_ASSERT(!ng_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NULL__data_NULL__size_not_0(void)
{
    ng_pktsnip_t *pkt;

    TEST_ASSERT_NOT_NULL((pkt = ng_pktbuf_add(NULL, NULL, sizeof(TEST_STRING8),
                                NG_NETTYPE_UNDEF)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_NOT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);

    TEST_ASSERT(!ng_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NOT_NULL__data_NULL__size_not_0(void)
{
    ng_pktsnip_t *pkt, *next = ng_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                               NG_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(next);

    TEST_ASSERT_NOT_NULL((pkt = ng_pktbuf_add(next, NULL, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF)));

    TEST_ASSERT(pkt->next == next);
    TEST_ASSERT_NOT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);

    TEST_ASSERT_NULL(next->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, next->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), next->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);

    TEST_ASSERT(!ng_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NOT_NULL__data_NOT_NULL__size_not_0(void)
{
    ng_pktsnip_t *pkt, *next = ng_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                               NG_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(next);

    TEST_ASSERT_NOT_NULL((pkt = ng_pktbuf_add(next, TEST_STRING8, sizeof(TEST_STRING8),
                                NG_NETTYPE_UNDEF)));

    TEST_ASSERT(pkt->next == next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);

    TEST_ASSERT_NULL(next->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, next->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), next->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);

    TEST_ASSERT(!ng_pktbuf_is_empty());
}

static void test_pktbuf_add__in_place(void)
{
    ng_pktsnip_t *pkt = ng_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                      NG_NETTYPE_UNDEF);
    ng_pktsnip_t *header;

    TEST_ASSERT_NOT_NULL((header = ng_pktbuf_add(pkt, pkt->data, 4, NG_NETTYPE_UNDEF)));
    TEST_ASSERT(header == pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, header->data); /* there is no 0 byte */
    TEST_ASSERT_EQUAL_INT(4, header->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, header->type);
    TEST_ASSERT_EQUAL_INT(1, header->users);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16 + 4, pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16) - 4, pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
    ng_pktbuf_release(header);
    ng_pktbuf_release(pkt);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

#if NG_PKTBUF_SIZE > 0
static void test_pktbuf_add__memfull(void)
{
    TEST_ASSERT_NULL(ng_pktbuf_add(NULL, NULL, NG_PKTBUF_SIZE + 1, NG_NETTYPE_UNDEF));
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_add__memfull2(void)
{
    TEST_ASSERT_NULL(ng_pktbuf_add(NULL, NULL, NG_PKTBUF_SIZE - sizeof(ng_pktsnip_t),
                                   NG_NETTYPE_UNDEF));
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_add__memfull3(void)
{
    for (int i = 0; i < 9; i++) {
        TEST_ASSERT_NOT_NULL(ng_pktbuf_add(NULL, NULL, (NG_PKTBUF_SIZE / 10) + 4, NG_NETTYPE_UNDEF));
        /* Why 4? Because: http://xkcd.com/221/, thats why ;-) */
    }

    TEST_ASSERT_NULL(ng_pktbuf_add(NULL, NULL, (NG_PKTBUF_SIZE / 10) + 4, NG_NETTYPE_UNDEF));
}
#endif

static void test_pktbuf_add__success(void)
{
    ng_pktsnip_t *pkt, *pkt_prev = NULL;

    for (int i = 0; i < 9; i++) {
        pkt = ng_pktbuf_add(NULL, NULL, (NG_PKTBUF_SIZE / 10) + 4, NG_NETTYPE_UNDEF);

        TEST_ASSERT_NOT_NULL(pkt);
        TEST_ASSERT_NULL(pkt->next);
        TEST_ASSERT_NOT_NULL(pkt->data);
        TEST_ASSERT_EQUAL_INT((NG_PKTBUF_SIZE / 10) + 4, pkt->size);
        TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);
        TEST_ASSERT_EQUAL_INT(1, pkt->users);

        if (pkt_prev != NULL) {
            TEST_ASSERT(pkt_prev < pkt);
            TEST_ASSERT(pkt_prev->data < pkt->data);
        }

        pkt_prev = pkt;
    }
}

static void test_pktbuf_add__packed_struct(void)
{
    test_pktbuf_struct_t data = { 0x4d, 0xef43, 0xacdef574, 0x43644305695afde5,
                                  34, -4469, 149699748, -46590430597
                                };
    test_pktbuf_struct_t *data_cpy;
    ng_pktsnip_t *pkt = ng_pktbuf_add(NULL, &data, sizeof(test_pktbuf_struct_t), NG_NETTYPE_UNDEF);
    data_cpy = (test_pktbuf_struct_t *)pkt->data;

    TEST_ASSERT_EQUAL_INT(data.u8, data_cpy->u8);
    TEST_ASSERT_EQUAL_INT(data.u16, data_cpy->u16);
    TEST_ASSERT_EQUAL_INT(data.u32, data_cpy->u32);
    TEST_ASSERT_EQUAL_INT(data.u64, data_cpy->u64);
    TEST_ASSERT_EQUAL_INT(data.s8, data_cpy->s8);
    TEST_ASSERT_EQUAL_INT(data.s16, data_cpy->s16);
    TEST_ASSERT_EQUAL_INT(data.s32, data_cpy->s32);
    TEST_ASSERT_EQUAL_INT(data.s64, data_cpy->s64);
}

static void test_pktbuf_add__unaligned_in_aligned_hole(void)
{
    ng_pktsnip_t *pkt1 = ng_pktbuf_add(NULL, NULL, 8, NG_NETTYPE_UNDEF);
    ng_pktsnip_t *pkt2 = ng_pktbuf_add(NULL, NULL, 8, NG_NETTYPE_UNDEF);
    ng_pktsnip_t *pkt3 = ng_pktbuf_add(NULL, NULL, 8, NG_NETTYPE_UNDEF);
    ng_pktsnip_t *pkt4;
    void *tmp_data2 = pkt2->data;

    ng_pktbuf_release(pkt2);
    pkt4 = ng_pktbuf_add(NULL, TEST_STRING12, 9, NG_NETTYPE_UNDEF);

    TEST_ASSERT(tmp_data2 != pkt4->data);

    ng_pktbuf_release(pkt1);
    ng_pktbuf_release(pkt3);
    ng_pktbuf_release(pkt4);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__pkt_NULL(void)
{
    TEST_ASSERT_EQUAL_INT(ENOENT, ng_pktbuf_realloc_data(NULL, 0));
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__pkt_wrong(void)
{
    ng_pktsnip_t pkt = { 1, NULL, TEST_STRING8, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF };

    TEST_ASSERT_EQUAL_INT(ENOENT, ng_pktbuf_realloc_data(&pkt, 0));
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__pkt_data_wrong(void)
{
    ng_pktsnip_t *pkt = ng_pktbuf_add(NULL, NULL, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF);
    void *orig_data = pkt->data;
    pkt->data = TEST_STRING8;

    TEST_ASSERT_EQUAL_INT(ENOENT, ng_pktbuf_realloc_data(pkt, 0));
    pkt->data = orig_data;
    ng_pktbuf_release(pkt);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__pkt_users_gt_1(void)
{
    ng_pktsnip_t *pkt = ng_pktbuf_add(NULL, NULL, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF);
    ng_pktbuf_hold(pkt, 1);

    TEST_ASSERT_EQUAL_INT(EINVAL, ng_pktbuf_realloc_data(pkt, sizeof(TEST_STRING8) - 1));
    ng_pktbuf_release(pkt);
    ng_pktbuf_release(pkt);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__pkt_next_neq_NULL(void)
{
    ng_pktsnip_t *pkt = ng_pktbuf_add(NULL, NULL, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(ng_pktbuf_add(pkt, pkt->data, sizeof(TEST_STRING4), NG_NETTYPE_UNDEF));
    TEST_ASSERT_EQUAL_INT(EINVAL, ng_pktbuf_realloc_data(pkt, sizeof(TEST_STRING8) - 1));
    ng_pktbuf_release(pkt);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__size_0(void)
{
    ng_pktsnip_t *pkt = ng_pktbuf_add(NULL, NULL, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF);

    TEST_ASSERT_EQUAL_INT(ENOMEM, ng_pktbuf_realloc_data(pkt, 0));
    ng_pktbuf_release(pkt);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

#if NG_PKTBUF_SIZE > 0
static void test_pktbuf_realloc_data__memfull(void)
{
    ng_pktsnip_t *pkt = ng_pktbuf_add(NULL, NULL, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF);

    TEST_ASSERT_EQUAL_INT(ENOMEM, ng_pktbuf_realloc_data(pkt, NG_PKTBUF_SIZE + 1));
    ng_pktbuf_release(pkt);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__memfull2(void)
{
    ng_pktsnip_t *pkt = ng_pktbuf_add(NULL, NULL, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(ng_pktbuf_add(NULL, NULL, sizeof(TEST_STRING16), NG_NETTYPE_UNDEF));
    TEST_ASSERT_EQUAL_INT(ENOMEM, ng_pktbuf_realloc_data(pkt, NG_PKTBUF_SIZE - sizeof(TEST_STRING8)));
}

static void test_pktbuf_realloc_data__memfull3(void)
{
    ng_pktsnip_t *pkt;

    TEST_ASSERT_NOT_NULL(ng_pktbuf_add(NULL, NULL, sizeof(TEST_STRING16), NG_NETTYPE_UNDEF));

    pkt = ng_pktbuf_add(NULL, NULL, TEST_UINT8, NG_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(ng_pktbuf_add(NULL, NULL, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF));
    TEST_ASSERT_EQUAL_INT(ENOMEM, ng_pktbuf_realloc_data(pkt, NG_PKTBUF_SIZE - TEST_UINT8));
}

/* dynamic malloc is a little more unpredictable ;-) */
static void test_pktbuf_realloc_data__shrink(void)
{
    ng_pktsnip_t *pkt;
    void *exp_data;

    pkt = ng_pktbuf_add(NULL, NULL, sizeof(TEST_STRING16), NG_NETTYPE_UNDEF);
    exp_data = pkt->data;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(ng_pktbuf_add(NULL, NULL, 4, NG_NETTYPE_UNDEF));

    TEST_ASSERT_EQUAL_INT(0, ng_pktbuf_realloc_data(pkt, sizeof(TEST_STRING8)));
    TEST_ASSERT(exp_data == pkt->data);
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_realloc_data__memenough(void)
{
    ng_pktsnip_t *pkt;
    void *exp_data;

    pkt = ng_pktbuf_add(NULL, NULL, sizeof(TEST_STRING16), NG_NETTYPE_UNDEF);
    exp_data = pkt->data;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(0, ng_pktbuf_realloc_data(pkt, sizeof(TEST_STRING8)));
    TEST_ASSERT(exp_data == pkt->data);
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_realloc_data__nomemenough(void)
{
    ng_pktsnip_t *pkt1, *pkt2;
    void *exp_data;

    pkt1 = ng_pktbuf_add(NULL, TEST_STRING8, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF);
    exp_data = pkt1->data;

    TEST_ASSERT_NOT_NULL(pkt1);

    pkt2 = ng_pktbuf_add(NULL, NULL, 1, NG_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(pkt2);
    TEST_ASSERT_NOT_NULL(ng_pktbuf_add(NULL, NULL, 4, NG_NETTYPE_UNDEF));

    ng_pktbuf_release(pkt2);

    TEST_ASSERT_EQUAL_INT(0, ng_pktbuf_realloc_data(pkt1, 200));
    TEST_ASSERT(exp_data != pkt1->data);
    TEST_ASSERT_NULL(pkt1->next);
    TEST_ASSERT_EQUAL_INT(200, pkt1->size);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt1->data);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt1->type);
    TEST_ASSERT_EQUAL_INT(1, pkt1->users);
}
#endif

static void test_pktbuf_realloc_data__success(void)
{
    ng_pktsnip_t *pkt;

    pkt = ng_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16), NG_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(pkt);

    TEST_ASSERT_EQUAL_INT(0, ng_pktbuf_realloc_data(pkt, sizeof(TEST_STRING8)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_realloc_data__success2(void)
{
    ng_pktsnip_t *pkt;

    pkt = ng_pktbuf_add(NULL, TEST_STRING8, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(pkt);

    TEST_ASSERT_EQUAL_INT(0, ng_pktbuf_realloc_data(pkt, sizeof(TEST_STRING16)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16), pkt->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_realloc_data__further_down_the_line(void)
{
    ng_pktsnip_t *pkt1, *pkt2, *header;
    void *exp_data;

    pkt1 = ng_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16), NG_NETTYPE_UNDEF);
    exp_data = pkt1->data;

    TEST_ASSERT_NOT_NULL(pkt1);

    header = ng_pktbuf_add(pkt1, pkt1->data, 4, NG_NETTYPE_UNDEF);
    pkt2 = ng_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16), NG_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(header);
    TEST_ASSERT(header == pkt1->next);
    TEST_ASSERT_EQUAL_INT(4, header->size);
    TEST_ASSERT(((uint8_t *)pkt1->data) == (((uint8_t *)header->data) + 4));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16) - 4, pkt1->size);

    TEST_ASSERT_EQUAL_INT(0, ng_pktbuf_realloc_data(header, 40));
    TEST_ASSERT(exp_data != header->data);
    TEST_ASSERT_NULL(header->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, header->data);
    TEST_ASSERT_EQUAL_INT(40, header->size);
    TEST_ASSERT_EQUAL_INT(NG_NETTYPE_UNDEF, header->type);
    TEST_ASSERT_EQUAL_INT(1, header->users);
    ng_pktbuf_release(pkt1);
    ng_pktbuf_release(pkt2);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_hold__pkt_null(void)
{
    ng_pktbuf_hold(NULL, 1);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_hold__pkt_external(void)
{
    ng_pktsnip_t pkt = { 1, NULL, TEST_STRING8, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF };

    ng_pktbuf_hold(&pkt, 1);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_hold__success(void)
{
    ng_pktsnip_t *pkt = ng_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16), NG_NETTYPE_UNDEF);

    for (uint8_t i = 0; i < TEST_UINT8; i++) {
        uint8_t prev_users = pkt->users;
        ng_pktbuf_hold(pkt, 1);
        TEST_ASSERT_EQUAL_INT(prev_users + 1, pkt->users);
    }
}

static void test_pktbuf_hold__success2(void)
{
    ng_pktsnip_t *pkt = ng_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16), NG_NETTYPE_UNDEF);

    ng_pktbuf_hold(pkt, TEST_UINT8);

    TEST_ASSERT_EQUAL_INT(TEST_UINT8 + 1, pkt->users);
}

static void test_pktbuf_release__pkt_null(void)
{
    ng_pktbuf_release(NULL);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_release__pkt_external(void)
{
    ng_pktsnip_t pkt = { 1, NULL, TEST_STRING8, sizeof(TEST_STRING8), NG_NETTYPE_UNDEF };

    ng_pktbuf_release(&pkt);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_release__success(void)
{
    ng_pktsnip_t *pkt = ng_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16), NG_NETTYPE_UNDEF);

    for (uint8_t i = 0; i < TEST_UINT8; i++) {
        uint8_t prev_users = pkt->users;
        ng_pktbuf_hold(pkt, 1);
        TEST_ASSERT_EQUAL_INT(prev_users + 1, pkt->users);
    }

    TEST_ASSERT(!ng_pktbuf_is_empty());

    for (uint8_t i = 0; i < TEST_UINT8; i++) {
        uint8_t prev_users = pkt->users;
        ng_pktbuf_release(pkt);
        TEST_ASSERT_EQUAL_INT(prev_users - 1, pkt->users);
    }

    TEST_ASSERT(!ng_pktbuf_is_empty());
    ng_pktbuf_release(pkt);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_start_write__NULL(void)
{
    ng_pktbuf_start_write(NULL);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_start_write__pkt_users_1(void)
{
    ng_pktsnip_t *pkt_copy, *pkt = ng_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                   NG_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL((pkt_copy = ng_pktbuf_start_write(pkt)));
    TEST_ASSERT(pkt == pkt_copy);
    ng_pktbuf_release(pkt);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

static void test_pktbuf_start_write__pkt_users_2(void)
{
    ng_pktsnip_t *pkt_copy, *pkt = ng_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                   NG_NETTYPE_UNDEF);

    ng_pktbuf_hold(pkt, 1);
    TEST_ASSERT_NOT_NULL((pkt_copy = ng_pktbuf_start_write(pkt)));
    TEST_ASSERT(pkt != pkt_copy);
    TEST_ASSERT(pkt->next == pkt_copy->next);
    TEST_ASSERT_EQUAL_STRING(pkt->data, pkt_copy->data);
    TEST_ASSERT_EQUAL_INT(pkt->size, pkt_copy->size);
    TEST_ASSERT_EQUAL_INT(pkt->type, pkt_copy->type);
    TEST_ASSERT_EQUAL_INT(pkt->users, pkt_copy->users);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);

    ng_pktbuf_release(pkt_copy);
    ng_pktbuf_release(pkt);
    TEST_ASSERT(ng_pktbuf_is_empty());
}

Test *tests_pktbuf_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pktbuf_reset),
        new_TestFixture(test_pktbuf_add__pkt_NULL__data_NULL__size_0),
        new_TestFixture(test_pktbuf_add__pkt_NOT_NULL__data_NULL__size_0),
        new_TestFixture(test_pktbuf_add__pkt_NULL__data_NOT_NULL__size_0),
        new_TestFixture(test_pktbuf_add__pkt_NOT_NULL__data_NOT_NULL__size_0),
        new_TestFixture(test_pktbuf_add__pkt_NULL__data_NULL__size_not_0),
        new_TestFixture(test_pktbuf_add__pkt_NOT_NULL__data_NULL__size_not_0),
        new_TestFixture(test_pktbuf_add__pkt_NOT_NULL__data_NOT_NULL__size_not_0),
        new_TestFixture(test_pktbuf_add__in_place),
#if NG_PKTBUF_SIZE > 0
        new_TestFixture(test_pktbuf_add__memfull),
        new_TestFixture(test_pktbuf_add__memfull2),
        new_TestFixture(test_pktbuf_add__memfull3),
#endif
        new_TestFixture(test_pktbuf_add__success),
        new_TestFixture(test_pktbuf_add__packed_struct),
        new_TestFixture(test_pktbuf_add__unaligned_in_aligned_hole),
        new_TestFixture(test_pktbuf_realloc_data__pkt_NULL),
        new_TestFixture(test_pktbuf_realloc_data__pkt_wrong),
        new_TestFixture(test_pktbuf_realloc_data__pkt_data_wrong),
        new_TestFixture(test_pktbuf_realloc_data__pkt_users_gt_1),
        new_TestFixture(test_pktbuf_realloc_data__pkt_next_neq_NULL),
        new_TestFixture(test_pktbuf_realloc_data__size_0),
#if NG_PKTBUF_SIZE > 0
        new_TestFixture(test_pktbuf_realloc_data__memfull),
        new_TestFixture(test_pktbuf_realloc_data__memfull2),
        new_TestFixture(test_pktbuf_realloc_data__memfull3),
        new_TestFixture(test_pktbuf_realloc_data__nomemenough),
        new_TestFixture(test_pktbuf_realloc_data__shrink),
        new_TestFixture(test_pktbuf_realloc_data__memenough),
#endif
        new_TestFixture(test_pktbuf_realloc_data__success),
        new_TestFixture(test_pktbuf_realloc_data__success2),
        new_TestFixture(test_pktbuf_realloc_data__further_down_the_line),
        new_TestFixture(test_pktbuf_hold__pkt_null),
        new_TestFixture(test_pktbuf_hold__pkt_external),
        new_TestFixture(test_pktbuf_hold__success),
        new_TestFixture(test_pktbuf_hold__success2),
        new_TestFixture(test_pktbuf_release__pkt_null),
        new_TestFixture(test_pktbuf_release__pkt_external),
        new_TestFixture(test_pktbuf_release__success),
        new_TestFixture(test_pktbuf_start_write__NULL),
        new_TestFixture(test_pktbuf_start_write__pkt_users_1),
        new_TestFixture(test_pktbuf_start_write__pkt_users_2),
    };

    EMB_UNIT_TESTCALLER(ng_pktbuf_tests, NULL, tear_down, fixtures);

    return (Test *)&ng_pktbuf_tests;
}

void tests_pktbuf(void)
{
    TESTS_RUN(tests_pktbuf_tests());
}
/** @} */
