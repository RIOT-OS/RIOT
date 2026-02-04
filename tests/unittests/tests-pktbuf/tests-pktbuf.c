/*
 * SPDX-FileCopyrightText: 2014 Martine Lenders <mail@martine-lenders.eu>
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdint.h>
#include <sys/uio.h>

#include "embUnit.h"

#include "net/gnrc/nettype.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"

#include "unittests-constants.h"
#include "tests-pktbuf.h"

#define ALIGNMENT_SIZE (2 * sizeof(uintptr_t))

typedef struct __attribute__((packed)) {
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    int8_t s8;
    int16_t s16;
    int32_t s32;
    int64_t s64;
}
test_pktbuf_struct_t;

static void set_up(void)
{
    gnrc_pktbuf_init();
}

static void test_pktbuf_init(void)
{
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    TEST_ASSERT_NOT_NULL(gnrc_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                                         GNRC_NETTYPE_TEST));
    TEST_ASSERT_NOT_NULL(gnrc_pktbuf_add(NULL, TEST_STRING8, sizeof(TEST_STRING8),
                                         GNRC_NETTYPE_TEST));
    TEST_ASSERT_NOT_NULL(gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                         GNRC_NETTYPE_TEST));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(!gnrc_pktbuf_is_empty());
    gnrc_pktbuf_init();
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NULL__data_NULL__size_0(void)
{
    gnrc_pktsnip_t *pkt;

    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, NULL, 0, GNRC_NETTYPE_TEST)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(0, pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(!gnrc_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NOT_NULL__data_NULL__size_0(void)
{
    gnrc_pktsnip_t *pkt;
    gnrc_pktsnip_t *next = gnrc_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                                           GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(next);

    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(next, NULL, 0, GNRC_NETTYPE_TEST)));
    TEST_ASSERT(pkt->next = next);
    TEST_ASSERT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(0, pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    TEST_ASSERT_NULL(next->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, next->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), next->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, next->type);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(!gnrc_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NULL__data_NOT_NULL__size_0(void)
{
    gnrc_pktsnip_t *pkt;

    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, TEST_STRING8, 0, GNRC_NETTYPE_TEST)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(0, pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(!gnrc_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NOT_NULL__data_NOT_NULL__size_0(void)
{
    gnrc_pktsnip_t *pkt;
    gnrc_pktsnip_t *next = gnrc_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                                           GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(next);

    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(next, TEST_STRING8, 0, GNRC_NETTYPE_TEST)));

    TEST_ASSERT(pkt->next = next);
    TEST_ASSERT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(0, pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    TEST_ASSERT_NULL(next->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, next->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), next->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, next->type);

    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(!gnrc_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NULL__data_NULL__size_not_0(void)
{
    gnrc_pktsnip_t *pkt;

    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(NULL, NULL, sizeof(TEST_STRING8),
                                GNRC_NETTYPE_TEST)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_NOT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);

    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(!gnrc_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NOT_NULL__data_NULL__size_not_0(void)
{
    gnrc_pktsnip_t *pkt, *next = gnrc_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                                                 GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(next);

    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(next, NULL, sizeof(TEST_STRING8),
                                                GNRC_NETTYPE_TEST)));

    TEST_ASSERT(pkt->next == next);
    TEST_ASSERT_NOT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);

    TEST_ASSERT_NULL(next->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, next->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), next->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);

    TEST_ASSERT(!gnrc_pktbuf_is_empty());
}

static void test_pktbuf_add__pkt_NOT_NULL__data_NOT_NULL__size_not_0(void)
{
    gnrc_pktsnip_t *pkt, *next = gnrc_pktbuf_add(NULL, TEST_STRING4, sizeof(TEST_STRING4),
                                                 GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(next);

    TEST_ASSERT_NOT_NULL((pkt = gnrc_pktbuf_add(next, TEST_STRING8, sizeof(TEST_STRING8),
                                                GNRC_NETTYPE_TEST)));

    TEST_ASSERT(pkt->next == next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);

    TEST_ASSERT_NULL(next->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, next->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), next->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);

    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(!gnrc_pktbuf_is_empty());
}

#ifndef MODULE_GNRC_PKTBUF_MALLOC   /* to long for standard malloc on native ;-) */
static void test_pktbuf_add__memfull(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, NULL, 1, GNRC_NETTYPE_TEST);
    while (pkt) {
        pkt = gnrc_pktbuf_add(NULL, NULL, 1, GNRC_NETTYPE_TEST);
    }

    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(!gnrc_pktbuf_is_empty());
}
#endif

static void test_pktbuf_add__success(void)
{
    gnrc_pktsnip_t *pkt, *pkt_prev = NULL;

    for (int i = 0; i < 9; i++) {
        pkt = gnrc_pktbuf_add(NULL, NULL, (CONFIG_GNRC_PKTBUF_SIZE / 10) + 4, GNRC_NETTYPE_TEST);

        TEST_ASSERT_NOT_NULL(pkt);
        TEST_ASSERT_NULL(pkt->next);
        TEST_ASSERT_NOT_NULL(pkt->data);
        TEST_ASSERT_EQUAL_INT((CONFIG_GNRC_PKTBUF_SIZE / 10) + 4, pkt->size);
        TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
        TEST_ASSERT_EQUAL_INT(1, pkt->users);

        if (pkt_prev != NULL) {
            TEST_ASSERT(pkt_prev < pkt);
            TEST_ASSERT(pkt_prev->data < pkt->data);
        }

        pkt_prev = pkt;
    }
    TEST_ASSERT(gnrc_pktbuf_is_sane());
}

static void test_pktbuf_add__packed_struct(void)
{
    test_pktbuf_struct_t data = { 0x4d, 0xef43, 0xacdef574, 0x43644305695afde5,
                                  34, -4469, 149699748, -46590430597
                                };
    test_pktbuf_struct_t *data_cpy;
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, &data, sizeof(test_pktbuf_struct_t),
                                          GNRC_NETTYPE_TEST);
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

#ifndef MODULE_GNRC_PKTBUF_MALLOC   /* alignment-handling left to malloc, so no certainty here */
static void test_pktbuf_add__unaligned_in_aligned_hole(void)
{
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, NULL, ALIGNMENT_SIZE, GNRC_NETTYPE_TEST);
    gnrc_pktsnip_t *pkt2 = gnrc_pktbuf_add(NULL, NULL, ALIGNMENT_SIZE, GNRC_NETTYPE_TEST);
    gnrc_pktsnip_t *pkt3 = gnrc_pktbuf_add(NULL, NULL, ALIGNMENT_SIZE, GNRC_NETTYPE_TEST);
    gnrc_pktsnip_t *pkt4;
    void *tmp_data2 = pkt2->data;

    gnrc_pktbuf_release(pkt2);
    pkt4 = gnrc_pktbuf_add(NULL, TEST_STRING64, ALIGNMENT_SIZE + 1, GNRC_NETTYPE_TEST);

    TEST_ASSERT(tmp_data2 != pkt4->data);

    gnrc_pktbuf_release(pkt1);
    gnrc_pktbuf_release(pkt3);
    gnrc_pktbuf_release(pkt4);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}
#endif

static void test_pktbuf_add__0_sized_release(void)
{
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, NULL, 0, GNRC_NETTYPE_TEST);
    gnrc_pktsnip_t *pkt2 = gnrc_pktbuf_add(NULL, NULL, 8, GNRC_NETTYPE_TEST);

    gnrc_pktbuf_release(pkt1);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(!gnrc_pktbuf_is_empty());
    gnrc_pktbuf_release(pkt2);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_mark__pkt_NULL__size_0(void)
{
    TEST_ASSERT_NULL(gnrc_pktbuf_mark(NULL, 0, GNRC_NETTYPE_TEST));
}

static void test_pktbuf_mark__pkt_NULL__size_not_0(void)
{
    TEST_ASSERT_NULL(gnrc_pktbuf_mark(NULL, sizeof(TEST_STRING4), GNRC_NETTYPE_TEST));
}

static void test_pktbuf_mark__pkt_NOT_NULL__size_0(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                          GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NULL(gnrc_pktbuf_mark(pkt, 0, GNRC_NETTYPE_TEST));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_NOT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16), pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_mark__pkt_NOT_NULL__size_greater_than_pkt_size(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                          GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NULL(gnrc_pktbuf_mark(pkt, sizeof(TEST_STRING16) + 8,
                                      GNRC_NETTYPE_TEST));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_NOT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16), pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_mark__pkt_NOT_NULL__pkt_data_NULL(void)
{
    gnrc_pktsnip_t pkt = { .size  = sizeof(TEST_STRING16),
                           .type  = GNRC_NETTYPE_TEST,
                           .users = 1,
                         };

    TEST_ASSERT_NULL(gnrc_pktbuf_mark(&pkt, sizeof(TEST_STRING16) - 1,
                                      GNRC_NETTYPE_TEST));
    TEST_ASSERT_NULL(pkt.next);
    TEST_ASSERT_NULL(pkt.data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16), pkt.size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt.type);
    TEST_ASSERT_EQUAL_INT(1, pkt.users);
}

static void test_pktbuf_mark__success_large(void)
{
    uint8_t *data = (uint8_t *)(TEST_STRING16);
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, data, sizeof(TEST_STRING16),
                                           GNRC_NETTYPE_TEST);
    gnrc_pktsnip_t *pkt2;
    uint8_t exp_data1[sizeof(TEST_STRING16) - sizeof(TEST_STRING8)];
    uint8_t exp_data2[sizeof(TEST_STRING8)];

    memcpy(exp_data1, data + sizeof(exp_data2), sizeof(exp_data1));
    memcpy(exp_data2, data, sizeof(exp_data2));

    TEST_ASSERT_NOT_NULL(pkt1);
    TEST_ASSERT_NOT_NULL((pkt2 = gnrc_pktbuf_mark(pkt1, sizeof(TEST_STRING8), GNRC_NETTYPE_UNDEF)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(pkt1->next == pkt2);
    TEST_ASSERT_NOT_NULL(pkt1->data);
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_data1, pkt1->data, pkt1->size));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16) - sizeof(TEST_STRING8),
                          pkt1->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt1->type);
    TEST_ASSERT_EQUAL_INT(1, pkt1->users);
    TEST_ASSERT_NULL(pkt2->next);
    TEST_ASSERT_NOT_NULL(pkt2->data);
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_data2, pkt2->data, pkt2->size));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt2->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, pkt2->type);
    TEST_ASSERT_EQUAL_INT(1, pkt2->users);

    /* check if slightly larger packet would override data */
    gnrc_pktbuf_remove_snip(pkt1, pkt2);
    pkt2 = gnrc_pktbuf_add(NULL, TEST_STRING12, 12, GNRC_NETTYPE_TEST);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT_NOT_NULL(pkt1->data);
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_data1, pkt1->data, pkt1->size));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16) - sizeof(TEST_STRING8),
                          pkt1->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt1->type);
    TEST_ASSERT_EQUAL_INT(1, pkt1->users);

    /* check if everything can be cleaned up */
    gnrc_pktbuf_release(pkt1);
    gnrc_pktbuf_release(pkt2);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_mark__success_aligned(void)
{
    uint8_t *data = (uint8_t *)(TEST_STRING16);
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, data, sizeof(TEST_STRING16),
                                           GNRC_NETTYPE_TEST);
    gnrc_pktsnip_t *pkt2;
    uint8_t exp_data1[sizeof(TEST_STRING16) - 8];
    uint8_t exp_data2[8];

    memcpy(exp_data1, data + sizeof(exp_data2), sizeof(exp_data1));
    memcpy(exp_data2, data, sizeof(exp_data2));

    TEST_ASSERT_NOT_NULL(pkt1);
    TEST_ASSERT_NOT_NULL((pkt2 = gnrc_pktbuf_mark(pkt1, 8, GNRC_NETTYPE_UNDEF)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(pkt1->next == pkt2);
    TEST_ASSERT_NOT_NULL(pkt1->data);
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_data1, pkt1->data, pkt1->size));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16) - 8,
                          pkt1->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt1->type);
    TEST_ASSERT_EQUAL_INT(1, pkt1->users);
    TEST_ASSERT_NULL(pkt2->next);
    TEST_ASSERT_NOT_NULL(pkt2->data);
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_data2, pkt2->data, pkt2->size));
    TEST_ASSERT_EQUAL_INT(8, pkt2->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, pkt2->type);
    TEST_ASSERT_EQUAL_INT(1, pkt2->users);

    /* check if slightly larger packet would override data */
    gnrc_pktbuf_remove_snip(pkt1, pkt2);
    pkt2 = gnrc_pktbuf_add(NULL, TEST_STRING12, 12, GNRC_NETTYPE_TEST);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT_NOT_NULL(pkt1->data);
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_data1, pkt1->data, pkt1->size));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16) - 8,
                          pkt1->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt1->type);
    TEST_ASSERT_EQUAL_INT(1, pkt1->users);

    /* check if everything can be cleaned up */
    gnrc_pktbuf_release(pkt1);
    gnrc_pktbuf_release(pkt2);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_mark__success_small(void)
{
    uint8_t *data = (uint8_t *)(TEST_STRING16);
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, data, sizeof(TEST_STRING16),
                                           GNRC_NETTYPE_TEST);
    gnrc_pktsnip_t *pkt2;
    uint8_t exp_data1[sizeof(TEST_STRING16) - 1];
    uint8_t exp_data2[1];

    memcpy(exp_data1, data + sizeof(exp_data2), sizeof(exp_data1));
    memcpy(exp_data2, data, sizeof(exp_data2));

    TEST_ASSERT_NOT_NULL(pkt1);
    TEST_ASSERT_NOT_NULL((pkt2 = gnrc_pktbuf_mark(pkt1, 1, GNRC_NETTYPE_UNDEF)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(pkt1->next == pkt2);
    TEST_ASSERT_NOT_NULL(pkt1->data);
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_data1, pkt1->data, pkt1->size));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16) - 1, pkt1->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt1->type);
    TEST_ASSERT_EQUAL_INT(1, pkt1->users);
    TEST_ASSERT_NULL(pkt2->next);
    TEST_ASSERT_NOT_NULL(pkt2->data);
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_data2, pkt2->data, pkt2->size));
    TEST_ASSERT_EQUAL_INT(1, pkt2->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, pkt2->type);
    TEST_ASSERT_EQUAL_INT(1, pkt2->users);

    /* check if slightly larger packet would override data */
    gnrc_pktbuf_remove_snip(pkt1, pkt2);
    pkt2 = gnrc_pktbuf_add(NULL, TEST_STRING12, 3, GNRC_NETTYPE_TEST);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT_NOT_NULL(pkt1->data);
    TEST_ASSERT_EQUAL_INT(0, memcmp(exp_data1, pkt1->data, pkt1->size));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16) - 1, pkt1->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt1->type);
    TEST_ASSERT_EQUAL_INT(1, pkt1->users);

    /* check if everything can be cleaned up */
    gnrc_pktbuf_release(pkt1);
    gnrc_pktbuf_release(pkt2);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_mark__success_equally_sized(void)
{
    gnrc_pktsnip_t *pkt1 = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                           GNRC_NETTYPE_TEST);
    gnrc_pktsnip_t *pkt2;

    TEST_ASSERT_NOT_NULL(pkt1);
    TEST_ASSERT_NOT_NULL((pkt2 = gnrc_pktbuf_mark(pkt1, sizeof(TEST_STRING16),
                                                  GNRC_NETTYPE_UNDEF)));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(pkt1->next == pkt2);
    TEST_ASSERT_NULL(pkt1->data);
    TEST_ASSERT_EQUAL_INT(0, pkt1->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt1->type);
    TEST_ASSERT_EQUAL_INT(1, pkt1->users);
    TEST_ASSERT_NULL(pkt2->next);
    TEST_ASSERT_EQUAL_INT(0, memcmp(TEST_STRING16, pkt2->data, pkt2->size));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16), pkt2->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, pkt2->type);
    TEST_ASSERT_EQUAL_INT(1, pkt2->users);
    TEST_ASSERT(gnrc_pktbuf_is_sane());

    /* check if everything can be cleaned up */
    gnrc_pktbuf_release(pkt1);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__size_0(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, NULL, sizeof(TEST_STRING8), GNRC_NETTYPE_TEST);

    TEST_ASSERT_EQUAL_INT(0, gnrc_pktbuf_realloc_data(pkt, 0));
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    TEST_ASSERT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(0, pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

#ifndef MODULE_GNRC_PKTBUF_MALLOC   /* CONFIG_GNRC_PKTBUF_SIZE does not*/
                                    /* apply for gnrc_pktbuf_malloc */
static void test_pktbuf_realloc_data__memfull(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, NULL, sizeof(TEST_STRING8), GNRC_NETTYPE_TEST);

    TEST_ASSERT_EQUAL_INT(ENOMEM, gnrc_pktbuf_realloc_data(pkt, CONFIG_GNRC_PKTBUF_SIZE + 1));
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}
#endif

static void test_pktbuf_realloc_data__shrink(void)
{
    gnrc_pktsnip_t *pkt;
    void *exp_data;

    pkt = gnrc_pktbuf_add(NULL, NULL, sizeof(TEST_STRING16), GNRC_NETTYPE_TEST);
    exp_data = pkt->data;

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NOT_NULL(gnrc_pktbuf_add(NULL, NULL, 4, GNRC_NETTYPE_TEST));

    TEST_ASSERT_EQUAL_INT(0, gnrc_pktbuf_realloc_data(pkt, 8));
    TEST_ASSERT(exp_data == pkt->data);
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(8, pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_realloc_data__memenough(void)
{
    gnrc_pktsnip_t *pkt;

    pkt = gnrc_pktbuf_add(NULL, NULL, sizeof(TEST_STRING16), GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_EQUAL_INT(0, gnrc_pktbuf_realloc_data(pkt, sizeof(TEST_STRING8)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_realloc_data__nomemenough(void)
{
    gnrc_pktsnip_t *pkt1, *pkt2;

    pkt1 = gnrc_pktbuf_add(NULL, TEST_STRING8, sizeof(TEST_STRING8), GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(pkt1);

    pkt2 = gnrc_pktbuf_add(NULL, NULL, 1, GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(pkt2);
    TEST_ASSERT_NOT_NULL(gnrc_pktbuf_add(NULL, NULL, 4, GNRC_NETTYPE_TEST));

    gnrc_pktbuf_release(pkt2);

    TEST_ASSERT_EQUAL_INT(0, gnrc_pktbuf_realloc_data(pkt1, 200));
    TEST_ASSERT_NULL(pkt1->next);
    TEST_ASSERT_EQUAL_INT(200, pkt1->size);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt1->data);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt1->type);
    TEST_ASSERT_EQUAL_INT(1, pkt1->users);
}

static void test_pktbuf_realloc_data__alignment(void)
{
    gnrc_pktsnip_t *pkt1, *pkt2, *pkt3;

    /* see: https://github.com/RIOT-OS/RIOT/pull/4602 */
    pkt1 = gnrc_pktbuf_add(NULL, TEST_STRING8, sizeof(TEST_STRING8), GNRC_NETTYPE_TEST);
    pkt2 = gnrc_pktbuf_add(NULL, NULL, 23, GNRC_NETTYPE_TEST);
    pkt3 = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16), GNRC_NETTYPE_UNDEF);

    TEST_ASSERT_NOT_NULL(pkt1);
    TEST_ASSERT_NOT_NULL(pkt2);
    TEST_ASSERT_NOT_NULL(pkt3);

    TEST_ASSERT_EQUAL_INT(0, gnrc_pktbuf_realloc_data(pkt2, 21));

    gnrc_pktbuf_release(pkt1);
    gnrc_pktbuf_release(pkt2);
    gnrc_pktbuf_release(pkt3);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_realloc_data__success(void)
{
    char exp_data[] = TEST_STRING16;
    gnrc_pktsnip_t *pkt;

    pkt = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16), GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(pkt);

    TEST_ASSERT_EQUAL_INT(0, gnrc_pktbuf_realloc_data(pkt, sizeof(TEST_STRING8)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt->size);
    for (unsigned int i = 0; i < pkt->size; i++) {
        uint8_t *data = pkt->data;
        TEST_ASSERT_EQUAL_INT(exp_data[i], data[i]);
    }
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_realloc_data__success2(void)
{
    gnrc_pktsnip_t *pkt;

    pkt = gnrc_pktbuf_add(NULL, TEST_STRING8, sizeof(TEST_STRING8), GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(pkt);

    TEST_ASSERT_EQUAL_INT(0, gnrc_pktbuf_realloc_data(pkt, sizeof(TEST_STRING16)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8, pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16), pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
}

static void test_pktbuf_realloc_data__success3(void)
{
    gnrc_pktsnip_t *pkt;

    pkt = gnrc_pktbuf_add(NULL, NULL, 0, GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NULL(pkt->data);

    TEST_ASSERT_EQUAL_INT(0, gnrc_pktbuf_realloc_data(pkt, sizeof(TEST_STRING16)));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_NOT_NULL(pkt->data);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING16), pkt->size);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, pkt->type);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);
    TEST_ASSERT(gnrc_pktbuf_is_sane());
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

#ifndef MODULE_GNRC_PKTBUF_MALLOC
static void test_pktbuf_merge_data__memfull(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, NULL, (CONFIG_GNRC_PKTBUF_SIZE / 4),
                                          GNRC_NETTYPE_TEST);

    pkt = gnrc_pktbuf_add(pkt, NULL, (CONFIG_GNRC_PKTBUF_SIZE / 4) + 1,
                          GNRC_NETTYPE_TEST);
    TEST_ASSERT_EQUAL_INT(ENOMEM, gnrc_pktbuf_merge(pkt));
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}
#endif /* MODULE_GNRC_PKTBUF_MALLOC */

static void test_pktbuf_merge_data__success1(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, NULL, 0, GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL(pkt);
    TEST_ASSERT_NULL(pkt->data);

    TEST_ASSERT_EQUAL_INT(0, gnrc_pktbuf_merge(pkt));
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_merge_data__success2(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, TEST_STRING4,
                                          sizeof(TEST_STRING4),
                                          GNRC_NETTYPE_TEST);

    pkt = gnrc_pktbuf_add(pkt, TEST_STRING8, sizeof(TEST_STRING8), GNRC_NETTYPE_TEST);
    pkt = gnrc_pktbuf_add(pkt, TEST_STRING16, sizeof(TEST_STRING16), GNRC_NETTYPE_TEST);

    TEST_ASSERT_EQUAL_INT(0, gnrc_pktbuf_merge(pkt));
    TEST_ASSERT_NULL(pkt->next);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING16, pkt->data);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING8,
                             (char *) pkt->data + sizeof(TEST_STRING16));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4,
                             (char *) pkt->data + sizeof(TEST_STRING16) +
                             sizeof(TEST_STRING8));
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
    TEST_ASSERT(gnrc_pktbuf_is_sane());
}

static void test_pktbuf_hold__pkt_null(void)
{
    gnrc_pktbuf_hold(NULL, 1);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_hold__pkt_external(void)
{
    gnrc_pktsnip_t pkt = { NULL, (void *)TEST_STRING8, sizeof(TEST_STRING8), 1, GNRC_NETTYPE_TEST };

    gnrc_pktbuf_hold(&pkt, 1);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_hold__success(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                          GNRC_NETTYPE_TEST);

    for (uint8_t i = 0; i < TEST_UINT8; i++) {
        uint8_t prev_users = pkt->users;
        gnrc_pktbuf_hold(pkt, 1);
        TEST_ASSERT_EQUAL_INT(prev_users + 1, pkt->users);
    }
}

static void test_pktbuf_hold__success2(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                          GNRC_NETTYPE_TEST);

    gnrc_pktbuf_hold(pkt, TEST_UINT8);

    TEST_ASSERT_EQUAL_INT(TEST_UINT8 + 1, pkt->users);
}

static void test_pktbuf_release__short_pktsnips(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, TEST_STRING8, sizeof(TEST_STRING8),
                                          GNRC_NETTYPE_UNDEF);
    gnrc_pktsnip_t *hdr = gnrc_pktbuf_mark(pkt, sizeof(TEST_STRING8) - 1, GNRC_NETTYPE_TEST);
    TEST_ASSERT(pkt);
    TEST_ASSERT(hdr);
    TEST_ASSERT(pkt->next == hdr);
    TEST_ASSERT(hdr->next == NULL);
    TEST_ASSERT_EQUAL_INT(hdr->size, sizeof(TEST_STRING8) - 1);
    TEST_ASSERT_EQUAL_INT(pkt->size, 1);
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_release__success(void)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                          GNRC_NETTYPE_TEST);

    for (uint8_t i = 0; i < TEST_UINT8; i++) {
        uint8_t prev_users = pkt->users;
        gnrc_pktbuf_hold(pkt, 1);
        TEST_ASSERT_EQUAL_INT(prev_users + 1, pkt->users);
    }

    TEST_ASSERT(!gnrc_pktbuf_is_empty());

    for (uint8_t i = 0; i < TEST_UINT8; i++) {
        uint8_t prev_users = pkt->users;
        gnrc_pktbuf_release(pkt);
        TEST_ASSERT_EQUAL_INT(prev_users - 1, pkt->users);
    }

    TEST_ASSERT(!gnrc_pktbuf_is_empty());
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_start_write__NULL(void)
{
    gnrc_pktbuf_start_write(NULL);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_start_write__pkt_users_1(void)
{
    gnrc_pktsnip_t *pkt_copy, *pkt = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                                     GNRC_NETTYPE_TEST);

    TEST_ASSERT_NOT_NULL((pkt_copy = gnrc_pktbuf_start_write(pkt)));
    TEST_ASSERT(pkt == pkt_copy);
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

static void test_pktbuf_start_write__pkt_users_2(void)
{
    gnrc_pktsnip_t *pkt_copy, *pkt = gnrc_pktbuf_add(NULL, TEST_STRING16, sizeof(TEST_STRING16),
                                                     GNRC_NETTYPE_TEST);

    gnrc_pktbuf_hold(pkt, 1);
    TEST_ASSERT_NOT_NULL((pkt_copy = gnrc_pktbuf_start_write(pkt)));
    TEST_ASSERT(pkt != pkt_copy);
    TEST_ASSERT(pkt->next == pkt_copy->next);
    TEST_ASSERT_EQUAL_STRING(pkt->data, pkt_copy->data);
    TEST_ASSERT_EQUAL_INT(pkt->size, pkt_copy->size);
    TEST_ASSERT_EQUAL_INT(pkt->type, pkt_copy->type);
    TEST_ASSERT_EQUAL_INT(pkt->users, pkt_copy->users);
    TEST_ASSERT_EQUAL_INT(1, pkt->users);

    gnrc_pktbuf_release(pkt_copy);
    gnrc_pktbuf_release(pkt);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

#ifndef MODULE_GNRC_PKTBUF_MALLOC
static void test_pktbuf_reverse_snips__too_full(void)
{
    gnrc_pktsnip_t *pkt, *pkt_next, *pkt_huge;
    const size_t pkt_huge_size = CONFIG_GNRC_PKTBUF_SIZE - (3 * ALIGNMENT_SIZE) -
                                 (3 * sizeof(gnrc_pktsnip_t)) - 4;

    pkt_next = gnrc_pktbuf_add(NULL, TEST_STRING16, ALIGNMENT_SIZE, GNRC_NETTYPE_TEST);
    TEST_ASSERT_NOT_NULL(pkt_next);
    /* hold to enforce duplication */
    gnrc_pktbuf_hold(pkt_next, 1);
    pkt = gnrc_pktbuf_add(pkt_next, TEST_STRING16, 8, GNRC_NETTYPE_TEST);
    TEST_ASSERT_NOT_NULL(pkt);
    /* filling up rest of packet buffer */
    pkt_huge = gnrc_pktbuf_add(NULL, NULL, pkt_huge_size, GNRC_NETTYPE_UNDEF);
    TEST_ASSERT_NOT_NULL(pkt_huge);
    TEST_ASSERT_NULL(gnrc_pktbuf_reverse_snips(pkt));
    gnrc_pktbuf_release(pkt_huge);
    /* release because of hold above */
    gnrc_pktbuf_release(pkt_next);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}
#endif /* MODULE_GNRC_PKTBUF_MALLOC */

static void test_pktbuf_reverse_snips__success(void)
{
    gnrc_pktsnip_t *pkt, *pkt_next, *pkt_reversed;

    pkt_next = gnrc_pktbuf_add(NULL, TEST_STRING8, 8, GNRC_NETTYPE_TEST);
    TEST_ASSERT_NOT_NULL(pkt_next);
    pkt = gnrc_pktbuf_add(pkt_next, TEST_STRING8, 8, GNRC_NETTYPE_TEST);
    TEST_ASSERT_NOT_NULL(pkt);
    pkt_reversed = gnrc_pktbuf_reverse_snips(pkt);
    TEST_ASSERT(pkt_reversed == pkt_next);
    TEST_ASSERT(pkt_reversed->next == pkt);
    gnrc_pktbuf_release(pkt_reversed);
    TEST_ASSERT(gnrc_pktbuf_is_empty());
}

Test *tests_pktbuf_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pktbuf_init),
        new_TestFixture(test_pktbuf_add__pkt_NULL__data_NULL__size_0),
        new_TestFixture(test_pktbuf_add__pkt_NOT_NULL__data_NULL__size_0),
        new_TestFixture(test_pktbuf_add__pkt_NULL__data_NOT_NULL__size_0),
        new_TestFixture(test_pktbuf_add__pkt_NOT_NULL__data_NOT_NULL__size_0),
        new_TestFixture(test_pktbuf_add__pkt_NULL__data_NULL__size_not_0),
        new_TestFixture(test_pktbuf_add__pkt_NOT_NULL__data_NULL__size_not_0),
        new_TestFixture(test_pktbuf_add__pkt_NOT_NULL__data_NOT_NULL__size_not_0),
#ifndef MODULE_GNRC_PKTBUF_MALLOC
        new_TestFixture(test_pktbuf_add__memfull),
#endif
        new_TestFixture(test_pktbuf_add__success),
        new_TestFixture(test_pktbuf_add__packed_struct),
#ifndef MODULE_GNRC_PKTBUF_MALLOC
        new_TestFixture(test_pktbuf_add__unaligned_in_aligned_hole),
#endif
        new_TestFixture(test_pktbuf_add__0_sized_release),
        new_TestFixture(test_pktbuf_mark__pkt_NULL__size_0),
        new_TestFixture(test_pktbuf_mark__pkt_NULL__size_not_0),
        new_TestFixture(test_pktbuf_mark__pkt_NOT_NULL__size_0),
        new_TestFixture(test_pktbuf_mark__pkt_NOT_NULL__size_greater_than_pkt_size),
        new_TestFixture(test_pktbuf_mark__pkt_NOT_NULL__pkt_data_NULL),
        new_TestFixture(test_pktbuf_mark__success_large),
        new_TestFixture(test_pktbuf_mark__success_aligned),
        new_TestFixture(test_pktbuf_mark__success_small),
        new_TestFixture(test_pktbuf_mark__success_equally_sized),
        new_TestFixture(test_pktbuf_realloc_data__size_0),
#ifndef MODULE_GNRC_PKTBUF_MALLOC
        new_TestFixture(test_pktbuf_realloc_data__memfull),
#endif
        new_TestFixture(test_pktbuf_realloc_data__nomemenough),
        new_TestFixture(test_pktbuf_realloc_data__shrink),
        new_TestFixture(test_pktbuf_realloc_data__memenough),
        new_TestFixture(test_pktbuf_realloc_data__alignment),
        new_TestFixture(test_pktbuf_realloc_data__success),
        new_TestFixture(test_pktbuf_realloc_data__success2),
        new_TestFixture(test_pktbuf_realloc_data__success3),
#ifndef MODULE_GNRC_PKTBUF_MALLOC
        new_TestFixture(test_pktbuf_merge_data__memfull),
#endif /* MODULE_GNRC_PKTBUF_MALLOC */
        new_TestFixture(test_pktbuf_merge_data__success1),
        new_TestFixture(test_pktbuf_merge_data__success2),
        new_TestFixture(test_pktbuf_hold__pkt_null),
        new_TestFixture(test_pktbuf_hold__pkt_external),
        new_TestFixture(test_pktbuf_hold__success),
        new_TestFixture(test_pktbuf_hold__success2),
        new_TestFixture(test_pktbuf_release__short_pktsnips),
        new_TestFixture(test_pktbuf_release__success),
        new_TestFixture(test_pktbuf_start_write__NULL),
        new_TestFixture(test_pktbuf_start_write__pkt_users_1),
        new_TestFixture(test_pktbuf_start_write__pkt_users_2),
#ifndef MODULE_GNRC_PKTBUF_MALLOC
        new_TestFixture(test_pktbuf_reverse_snips__too_full),
#endif /* MODULE_GNRC_PKTBUF_MALLOC */
        new_TestFixture(test_pktbuf_reverse_snips__success),
    };

    EMB_UNIT_TESTCALLER(gnrc_pktbuf_tests, set_up, NULL, fixtures);

    return (Test *)&gnrc_pktbuf_tests;
}

void tests_pktbuf(void)
{
    TESTS_RUN(tests_pktbuf_tests());
}
/** @} */
