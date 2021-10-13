/*
 * Copyright (C) 2014, 2015 Martine Lenders <mail@martine-lenders.eu>
 * *             2015       Freie Universität Berlin
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
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "embUnit/embUnit.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/nettype.h"

#include "iolist.h"

#include "unittests-constants.h"
#include "tests-pkt.h"

#define _INIT_ELEM(len, _data, _next) \
    { .users = 1, .next = (_next), .data = (void *)(_data), \
      .size = (len), .type = GNRC_NETTYPE_UNDEF \
    }
#define _INIT_ELEM_STATIC_DATA(data, next) _INIT_ELEM(sizeof(data), (void *)(data), (next))

#define _INIT_ELEM_STATIC_TYPE(_type, _next) \
    { .users = 1, .next = (_next), .data = NULL, .size = 0, .type = (_type) }

static void test_pkt_prev_snip__NULL_NULL(void)
{
    TEST_ASSERT_NULL(gnrc_pkt_prev_snip(NULL, NULL));
}

static void test_pkt_prev_snip__pkt_NULL(void)
{
    gnrc_pktsnip_t pkt = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t *res;

    res = gnrc_pkt_prev_snip(&pkt, NULL);
    TEST_ASSERT((&pkt) == res);
}

static void test_pkt_prev_snip__NULL_snip(void)
{
    gnrc_pktsnip_t snip = _INIT_ELEM(SIZE_MAX, NULL, NULL);

    TEST_ASSERT_NULL(gnrc_pkt_prev_snip(NULL, &snip));
}

static void test_pkt_prev_snip__pkt_snip(void)
{
    gnrc_pktsnip_t snip1 = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t snip2 = _INIT_ELEM(SIZE_MAX, NULL, &snip1);
    gnrc_pktsnip_t pkt = _INIT_ELEM(SIZE_MAX, NULL, &snip2);
    gnrc_pktsnip_t *res;

    res = gnrc_pkt_prev_snip(&pkt, &snip1);
    TEST_ASSERT((&snip2) == res);
}

static void test_pkt_len__NULL(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_pkt_len(NULL));
}

static void test_pkt_len__1_elem__size_MAX(void)
{
    gnrc_pktsnip_t snip = _INIT_ELEM(SIZE_MAX, NULL, NULL);

    TEST_ASSERT_EQUAL_INT(SIZE_MAX, gnrc_pkt_len(&snip));
}

static void test_pkt_len__1_elem__size_0(void)
{
    gnrc_pktsnip_t snip = _INIT_ELEM(0, NULL, NULL);

    TEST_ASSERT_EQUAL_INT(0, gnrc_pkt_len(&snip));
}

static void test_pkt_len__1_elem__size_data(void)
{
    gnrc_pktsnip_t snip = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), gnrc_pkt_len(&snip));
}

static void test_pkt_len__2_elem(void)
{
    gnrc_pktsnip_t snip1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    gnrc_pktsnip_t snip2 = _INIT_ELEM_STATIC_DATA(TEST_STRING12, &snip1);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) + sizeof(TEST_STRING12),
                          gnrc_pkt_len(&snip2));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), gnrc_pkt_len(&snip1));
}

static void test_pkt_len__2_elem__overflow(void)
{
    gnrc_pktsnip_t snip1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    gnrc_pktsnip_t snip2 = _INIT_ELEM(SIZE_MAX, NULL, &snip1);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) - 1, gnrc_pkt_len(&snip2));
    /* size should overflow */
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), gnrc_pkt_len(&snip1));
}

static void test_pkt_len__3_elem(void)
{
    gnrc_pktsnip_t snip1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    gnrc_pktsnip_t snip2 = _INIT_ELEM_STATIC_DATA(TEST_STRING12, &snip1);
    gnrc_pktsnip_t snip3 = _INIT_ELEM(sizeof("a"), "a", &snip2);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) + sizeof(TEST_STRING12) + sizeof("a"),
                          gnrc_pkt_len(&snip3));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) + sizeof(TEST_STRING12), gnrc_pkt_len(&snip2));
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), gnrc_pkt_len(&snip1));
}

static void test_pkt_append(void)
{
    gnrc_pktsnip_t snip1 = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t snip2 = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t pkt = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t *res = &pkt;

    res = gnrc_pkt_append(res, &snip1);
    TEST_ASSERT((&pkt) == res);
    TEST_ASSERT((&snip1) == res->next);
    TEST_ASSERT_NULL(res->next->next);
    res = gnrc_pkt_append(res, &snip2);
    TEST_ASSERT((&pkt) == res);
    TEST_ASSERT((&snip1) == res->next);
    TEST_ASSERT((&snip2) == res->next->next);
}

static void test_pkt_prepend(void)
{
    gnrc_pktsnip_t snip1 = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t snip2 = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t pkt = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t *res = &pkt;

    res = gnrc_pkt_prepend(res, &snip1);
    TEST_ASSERT((&snip1) == res);
    TEST_ASSERT((&pkt) == res->next);
    TEST_ASSERT_NULL(res->next->next);
    res = gnrc_pkt_prepend(res, &snip2);
    TEST_ASSERT((&snip2) == res);
    TEST_ASSERT((&snip1) == res->next);
    TEST_ASSERT((&pkt) == res->next->next);
}

static void test_pkt_delete__NULL(void)
{
    gnrc_pktsnip_t snip1 = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t snip2 = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t pkt = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t *res = &pkt;

    res = gnrc_pkt_prepend(res, &snip1);
    res = gnrc_pkt_prepend(res, &snip2);
    TEST_ASSERT((&snip2) == res);
    TEST_ASSERT((&snip1) == res->next);
    TEST_ASSERT((&pkt) == res->next->next);
    res = gnrc_pkt_delete(res, NULL);
    /* pkt did not change */
    TEST_ASSERT((&snip2) == res);
    TEST_ASSERT((&snip1) == res->next);
    TEST_ASSERT((&pkt) == res->next->next);
}

static void test_pkt_delete(void)
{
    gnrc_pktsnip_t snip1 = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t snip2 = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t pkt = _INIT_ELEM(SIZE_MAX, NULL, NULL);
    gnrc_pktsnip_t *res = &pkt;

    res = gnrc_pkt_prepend(res, &snip1);
    res = gnrc_pkt_prepend(res, &snip2);
    TEST_ASSERT((&snip2) == res);
    TEST_ASSERT((&snip1) == res->next);
    TEST_ASSERT((&pkt) == res->next->next);
    res = gnrc_pkt_delete(res, &snip1);
    TEST_ASSERT((&snip2) == res);
    TEST_ASSERT((&pkt) == res->next);
    res = gnrc_pkt_delete(res, &snip2);
    TEST_ASSERT((&pkt) == res);
    res = gnrc_pkt_delete(res, &pkt);
    TEST_ASSERT_NULL(res);
}

static void test_pkt_len_upto__NULL(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_pkt_len_upto(NULL, GNRC_NETTYPE_TEST));
}

static void test_pkt_len_upto__not_in_list(void)
{
    gnrc_pktsnip_t snip1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    gnrc_pktsnip_t snip2 = _INIT_ELEM_STATIC_DATA(TEST_STRING12, &snip1);
    gnrc_pktsnip_t snip3 = _INIT_ELEM(sizeof("a"), "a", &snip2);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) + sizeof(TEST_STRING12) + sizeof("a"),
                          gnrc_pkt_len_upto(&snip3, GNRC_NETTYPE_TEST));
}

static void test_pkt_len_upto__in_list(void)
{
    gnrc_pktsnip_t snip1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    gnrc_pktsnip_t snip2 = _INIT_ELEM_STATIC_TYPE(GNRC_NETTYPE_TEST, &snip1);
    gnrc_pktsnip_t snip3 = _INIT_ELEM(sizeof("a"), "a", &snip2);

    TEST_ASSERT_EQUAL_INT(sizeof("a"),
                          gnrc_pkt_len_upto(&snip3, GNRC_NETTYPE_TEST));
}

static void test_pkt_count__1_elem(void)
{
    gnrc_pktsnip_t snip1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);

    TEST_ASSERT_EQUAL_INT(1, gnrc_pkt_count(&snip1));
}

static void test_pkt_count__5_elem(void)
{
    gnrc_pktsnip_t snip1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    gnrc_pktsnip_t snip2 = _INIT_ELEM_STATIC_DATA(TEST_STRING12, &snip1);
    gnrc_pktsnip_t snip3 = _INIT_ELEM(sizeof("a"), "a", &snip2);
    gnrc_pktsnip_t snip4 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, &snip3);
    gnrc_pktsnip_t snip5 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, &snip4);

    TEST_ASSERT_EQUAL_INT(5, gnrc_pkt_count(&snip5));
}

static void test_pkt_count__null(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_pkt_count(NULL));
}

static void test_pktsnip_search_type(void)
{
    /* init packet snips */
    gnrc_pktsnip_t snip1 = _INIT_ELEM_STATIC_TYPE(GNRC_NETTYPE_UNDEF, NULL);
    gnrc_pktsnip_t snip2 = _INIT_ELEM_STATIC_TYPE(GNRC_NETTYPE_TEST, &snip1);
    gnrc_pktsnip_t snip3 = _INIT_ELEM_STATIC_TYPE(GNRC_NETTYPE_IPV6, &snip2);
    /* successful searches */
    gnrc_pktsnip_t *res;
    TEST_ASSERT_NOT_NULL((res = gnrc_pktsnip_search_type(&snip3, GNRC_NETTYPE_UNDEF)));
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_UNDEF, res->type);
    TEST_ASSERT_NOT_NULL((res = gnrc_pktsnip_search_type(&snip3, GNRC_NETTYPE_TEST)));
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_TEST, res->type);
    TEST_ASSERT_NOT_NULL((res = gnrc_pktsnip_search_type(&snip3, GNRC_NETTYPE_IPV6)));
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_IPV6, res->type);
    /* failing searches  */
    TEST_ASSERT_NULL(gnrc_pktsnip_search_type(&snip1, GNRC_NETTYPE_TEST));
    TEST_ASSERT_NULL(gnrc_pktsnip_search_type(&snip2, GNRC_NETTYPE_IPV6));
    TEST_ASSERT_NULL(gnrc_pktsnip_search_type(&snip3, GNRC_NETTYPE_NUMOF));
}

static void test_pkt_equals_iolist(void)
{
    iolist_t iol;
    gnrc_pktsnip_t pkt;

    memset(&iol, '\0', sizeof(iol));
    memset(&pkt, '\0', sizeof(pkt));

    /* compare empty structs */
    TEST_ASSERT_EQUAL_INT(0, memcmp(&iol, &pkt, sizeof(iol)));

    /* check next pointer position */
    iol.iol_next = (void *)0xAAAAAAAA;
    pkt.next = (void *)0xAAAAAAAA;

    TEST_ASSERT_EQUAL_INT(0, memcmp(&iol, &pkt, sizeof(iol)));

    /* check data pointer position */
    iol.iol_base = &iol;
    pkt.data = &iol;

    TEST_ASSERT_EQUAL_INT(0, memcmp(&iol, &pkt, sizeof(iol)));

    /* check size position */
    iol.iol_len = (size_t)0x12345678;
    pkt.size = (size_t)0x12345678;

    TEST_ASSERT_EQUAL_INT(0, memcmp(&iol, &pkt, sizeof(iol)));

    TEST_ASSERT_EQUAL_INT(offsetof(iolist_t, iol_next), offsetof(gnrc_pktsnip_t, next));
    TEST_ASSERT_EQUAL_INT(offsetof(iolist_t, iol_base), offsetof(gnrc_pktsnip_t, data));
    TEST_ASSERT_EQUAL_INT(offsetof(iolist_t, iol_len), offsetof(gnrc_pktsnip_t, size));
}

Test *tests_pkt_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pkt_prev_snip__NULL_NULL),
        new_TestFixture(test_pkt_prev_snip__pkt_NULL),
        new_TestFixture(test_pkt_prev_snip__NULL_snip),
        new_TestFixture(test_pkt_prev_snip__pkt_snip),
        new_TestFixture(test_pkt_len__NULL),
        new_TestFixture(test_pkt_len__1_elem__size_MAX),
        new_TestFixture(test_pkt_len__1_elem__size_0),
        new_TestFixture(test_pkt_len__1_elem__size_data),
        new_TestFixture(test_pkt_len__2_elem),
        new_TestFixture(test_pkt_len__2_elem__overflow),
        new_TestFixture(test_pkt_len__3_elem),
        new_TestFixture(test_pkt_append),
        new_TestFixture(test_pkt_prepend),
        new_TestFixture(test_pkt_delete__NULL),
        new_TestFixture(test_pkt_delete),
        new_TestFixture(test_pkt_len_upto__NULL),
        new_TestFixture(test_pkt_len_upto__not_in_list),
        new_TestFixture(test_pkt_len_upto__in_list),
        new_TestFixture(test_pkt_count__1_elem),
        new_TestFixture(test_pkt_count__5_elem),
        new_TestFixture(test_pkt_count__null),
        new_TestFixture(test_pktsnip_search_type),
        new_TestFixture(test_pkt_equals_iolist),
    };

    EMB_UNIT_TESTCALLER(pkt_tests, NULL, NULL, fixtures);

    return (Test *)&pkt_tests;
}

void tests_pkt(void)
{
    TESTS_RUN(tests_pkt_tests());
}
/** @} */
