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
#include <stdint.h>

#include "embUnit/embUnit.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/nettype.h"

#include "unittests-constants.h"
#include "tests-pkt.h"

#define _INIT_ELEM(len, data, next) \
    { 1, (next), (data), (len), GNRC_NETTYPE_UNDEF }
#define _INIT_ELEM_STATIC_DATA(data, next) _INIT_ELEM(sizeof(data), data, next)

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

Test *tests_pkt_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pkt_len__NULL),
        new_TestFixture(test_pkt_len__1_elem__size_MAX),
        new_TestFixture(test_pkt_len__1_elem__size_0),
        new_TestFixture(test_pkt_len__1_elem__size_data),
        new_TestFixture(test_pkt_len__2_elem),
        new_TestFixture(test_pkt_len__2_elem__overflow),
        new_TestFixture(test_pkt_len__3_elem),
        new_TestFixture(test_pkt_count__1_elem),
        new_TestFixture(test_pkt_count__5_elem),
        new_TestFixture(test_pkt_count__null),
    };

    EMB_UNIT_TESTCALLER(pkt_tests, NULL, NULL, fixtures);

    return (Test *)&pkt_tests;
}

void tests_pkt(void)
{
    TESTS_RUN(tests_pkt_tests());
}
/** @} */
