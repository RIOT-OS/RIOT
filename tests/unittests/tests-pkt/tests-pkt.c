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
#include "pkt.h"

#include "tests-pkt.h"

#define _INIT_ELEM(len, data, next) \
    { (len), (data), PKT_PROTO_UNKNOWN, (next) }
#define _INIT_ELEM_STATIC_DATA(data, next) _INIT_ELEM(sizeof(data), data, next)

#define TEST_PKTSIZE    (TEST_UINT16)

static void test_pkt_hlist_advance__NULL(void)
{
    pkt_hlist_t **list = NULL;

    pkt_hlist_advance(list);

    TEST_ASSERT_NULL(list);
}

static void test_pkt_hlist_advance__ptr_NULL(void)
{
    pkt_hlist_t *list = NULL;

    pkt_hlist_advance(&list);

    TEST_ASSERT_NULL(list);
}

static void test_pkt_hlist_advance__1_elem(void)
{
    pkt_hlist_t hdr = _INIT_ELEM(0, NULL, NULL);
    pkt_hlist_t *list = &hdr;

    pkt_hlist_advance(&list);

    TEST_ASSERT_NULL(list);
}

static void test_pkt_hlist_advance__2_elem(void)
{
    pkt_hlist_t hdr1 = _INIT_ELEM(0, NULL, NULL);
    pkt_hlist_t hdr2 = _INIT_ELEM(0, NULL, &hdr1);
    pkt_hlist_t *list = &hdr2;

    pkt_hlist_advance(&list);

    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT(&hdr1 == list);

    pkt_hlist_advance(&list);

    TEST_ASSERT_NULL(list);
}

static void test_pkt_total_header_len__headers_NULL(void)
{
    pkt_t pkt = _INIT_ELEM(0, NULL, NULL);

    TEST_ASSERT_EQUAL_INT(0, pkt_total_header_len(&pkt));
}

static void test_pkt_total_header_len__1_header__hdata_NULL__hlen_0(void)
{
    pkt_hlist_t hdr = _INIT_ELEM(0, NULL, NULL);
    pkt_t pkt = _INIT_ELEM(0, NULL, &hdr);

    TEST_ASSERT_EQUAL_INT(0, pkt_total_header_len(&pkt));
}

static void test_pkt_total_header_len__1_header__hdata_NULL__hlen_MAX(void)
{
    pkt_hlist_t hdr = _INIT_ELEM(PKTSIZE_MAX, NULL, NULL);
    pkt_t pkt = _INIT_ELEM(0, NULL, &hdr);

    TEST_ASSERT_EQUAL_INT(PKTSIZE_MAX, pkt_total_header_len(&pkt));
}

static void test_pkt_total_header_len__1_header__hdata_NULL__hlen_value(void)
{
    pkt_hlist_t hdr = _INIT_ELEM(TEST_UINT16, NULL, NULL);
    pkt_t pkt = _INIT_ELEM(0, NULL, &hdr);

    TEST_ASSERT_EQUAL_INT(TEST_PKTSIZE, pkt_total_header_len(&pkt));
}

static void test_pkt_total_header_len__1_header__hdata_value__hlen_value(void)
{
    pkt_hlist_t hdr = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    pkt_t pkt = _INIT_ELEM(0, NULL, &hdr);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt_total_header_len(&pkt));
}

static void test_pkt_total_header_len__2_headers__hdata_value__hlen_value(void)
{
    pkt_hlist_t hdr1 = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    pkt_hlist_t hdr2 = _INIT_ELEM_STATIC_DATA(TEST_STRING12, &hdr1);
    pkt_t pkt = _INIT_ELEM(0, NULL, &hdr2);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) + sizeof(TEST_STRING12),
                          pkt_total_header_len(&pkt));
}

static void test_pkt_total_len__headers_NULL__payload_len_0(void)
{
    pkt_t pkt = _INIT_ELEM(0, NULL, NULL);

    TEST_ASSERT_EQUAL_INT(0, pkt_total_len(&pkt));
}

static void test_pkt_total_len__headers_NULL__payload_len_MAX(void)
{
    pkt_t pkt = _INIT_ELEM(PKTSIZE_MAX, NULL, NULL);

    TEST_ASSERT_EQUAL_INT(PKTSIZE_MAX, pkt_total_len(&pkt));
}

static void test_pkt_total_len__headers_NULL__payload_len_value(void)
{
    pkt_t pkt = _INIT_ELEM(TEST_PKTSIZE, NULL, NULL);

    TEST_ASSERT_EQUAL_INT(TEST_PKTSIZE, pkt_total_len(&pkt));
}

static void test_pkt_total_len__1_header__payload_len_0(void)
{
    pkt_hlist_t hdr = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    pkt_t pkt = _INIT_ELEM(0, NULL, &hdr);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt_total_len(&pkt));
}

static void test_pkt_total_len__1_header__payload_len_MAX(void)
{
    pkt_hlist_t hdr = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    pkt_t pkt = _INIT_ELEM(PKTSIZE_MAX, NULL, &hdr);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) - 1, pkt_total_len(&pkt));
    /* size should overflow */
}

static void test_pkt_total_len__1_header__payload_len_value(void)
{
    pkt_hlist_t hdr = _INIT_ELEM_STATIC_DATA(TEST_STRING8, NULL);
    pkt_t pkt = _INIT_ELEM(TEST_PKTSIZE, NULL, &hdr);

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) + TEST_PKTSIZE, pkt_total_len(&pkt));
}

static void test_pkt_add_header(void)
{
    pkt_hlist_t hdr1 = _INIT_ELEM(0, NULL, NULL);
    pkt_hlist_t hdr2 = _INIT_ELEM(0, NULL, NULL);
    pkt_t pkt = _INIT_ELEM(0, NULL, NULL);

    pkt_add_header(&pkt, &hdr1);

    TEST_ASSERT(&hdr1 == pkt.headers);
    TEST_ASSERT_NULL(pkt.headers->next);

    pkt_add_header(&pkt, &hdr2);

    TEST_ASSERT(&hdr2 == pkt.headers);
    TEST_ASSERT(&hdr1 == pkt.headers->next);
    TEST_ASSERT_NULL(pkt.headers->next->next);
}

static void test_pkt_add_header__NULL(void)
{
    pkt_hlist_t hdr1 = _INIT_ELEM(0, NULL, NULL);
    pkt_hlist_t hdr2 = _INIT_ELEM(0, NULL, NULL);
    pkt_t pkt = _INIT_ELEM(0, NULL, NULL);

    pkt_add_header(&pkt, &hdr1);
    pkt_add_header(&pkt, &hdr2);
    pkt_add_header(&pkt, NULL);

    TEST_ASSERT(&hdr2 == pkt.headers);
    TEST_ASSERT(&hdr1 == pkt.headers->next);
    TEST_ASSERT_NULL(pkt.headers->next->next);
}

static void test_pkt_remove_header__NULL(void)
{
    pkt_hlist_t hdr1 = _INIT_ELEM(0, NULL, NULL);
    pkt_hlist_t hdr2 = _INIT_ELEM(0, NULL, NULL);
    pkt_t pkt = _INIT_ELEM(0, NULL, NULL);

    pkt_add_header(&pkt, &hdr1);
    pkt_add_header(&pkt, &hdr2);
    pkt_remove_header(&pkt, NULL);

    TEST_ASSERT(&hdr2 == pkt.headers);
    TEST_ASSERT(&hdr1 == pkt.headers->next);
    TEST_ASSERT_NULL(pkt.headers->next->next);
}

static void test_pkt_remove_header__1st_header_first(void)
{
    pkt_hlist_t hdr1 = _INIT_ELEM(0, NULL, NULL);
    pkt_hlist_t hdr2 = _INIT_ELEM(0, NULL, NULL);
    pkt_t pkt = _INIT_ELEM(0, NULL, NULL);

    pkt_add_header(&pkt, &hdr1);
    pkt_add_header(&pkt, &hdr2);
    pkt_remove_header(&pkt, &hdr2);

    TEST_ASSERT(&hdr1 == pkt.headers);
    TEST_ASSERT_NULL(pkt.headers->next);

    pkt_remove_header(&pkt, &hdr1);

    TEST_ASSERT_NULL(pkt.headers);
}

static void test_pkt_remove_header__nth_header_first(void)
{
    pkt_hlist_t hdr1 = _INIT_ELEM(0, NULL, NULL);
    pkt_hlist_t hdr2 = _INIT_ELEM(0, NULL, NULL);
    pkt_t pkt = _INIT_ELEM(0, NULL, NULL);

    pkt_add_header(&pkt, &hdr1);
    pkt_add_header(&pkt, &hdr2);
    pkt_remove_header(&pkt, &hdr1);

    TEST_ASSERT(&hdr2 == pkt.headers);
    TEST_ASSERT_NULL(pkt.headers->next);

    pkt_remove_header(&pkt, &hdr2);

    TEST_ASSERT_NULL(pkt.headers);
}

static void test_pkt_remove_first_header__pkt_NULL(void)
{
    TEST_ASSERT_NULL(pkt_remove_first_header(NULL));
}

static void test_pkt_remove_first_header__hdr_empty(void)
{
    pkt_t pkt = _INIT_ELEM(0, NULL, NULL);

    TEST_ASSERT_NULL(pkt_remove_first_header(&pkt));
}

static void test_pkt_remove_first_header(void)
{
    pkt_hlist_t hdr1 = _INIT_ELEM(0, NULL, NULL);
    pkt_hlist_t hdr2 = _INIT_ELEM(0, NULL, NULL);
    pkt_t pkt = _INIT_ELEM(0, NULL, NULL);

    pkt_add_header(&pkt, &hdr1);
    pkt_add_header(&pkt, &hdr2);

    TEST_ASSERT(&hdr2 == pkt_remove_first_header(&pkt));
    TEST_ASSERT(&hdr1 == pkt.headers);
    TEST_ASSERT_NULL(pkt.headers->next);
}

Test *tests_pkt_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pkt_hlist_advance__NULL),
        new_TestFixture(test_pkt_hlist_advance__ptr_NULL),
        new_TestFixture(test_pkt_hlist_advance__1_elem),
        new_TestFixture(test_pkt_hlist_advance__2_elem),
        new_TestFixture(test_pkt_total_header_len__headers_NULL),
        new_TestFixture(test_pkt_total_header_len__1_header__hdata_NULL__hlen_0),
        new_TestFixture(test_pkt_total_header_len__1_header__hdata_NULL__hlen_MAX),
        new_TestFixture(test_pkt_total_header_len__1_header__hdata_NULL__hlen_value),
        new_TestFixture(test_pkt_total_header_len__1_header__hdata_value__hlen_value),
        new_TestFixture(test_pkt_total_header_len__2_headers__hdata_value__hlen_value),
        new_TestFixture(test_pkt_total_len__headers_NULL__payload_len_0),
        new_TestFixture(test_pkt_total_len__headers_NULL__payload_len_MAX),
        new_TestFixture(test_pkt_total_len__headers_NULL__payload_len_value),
        new_TestFixture(test_pkt_total_len__1_header__payload_len_0),
        new_TestFixture(test_pkt_total_len__1_header__payload_len_MAX),
        new_TestFixture(test_pkt_total_len__1_header__payload_len_value),
        new_TestFixture(test_pkt_add_header),
        new_TestFixture(test_pkt_add_header__NULL),
        new_TestFixture(test_pkt_remove_header__NULL),
        new_TestFixture(test_pkt_remove_header__1st_header_first),
        new_TestFixture(test_pkt_remove_header__nth_header_first),
        new_TestFixture(test_pkt_remove_first_header__pkt_NULL),
        new_TestFixture(test_pkt_remove_first_header__hdr_empty),
        new_TestFixture(test_pkt_remove_first_header),
    };

    EMB_UNIT_TESTCALLER(pkt_tests, NULL, NULL, fixtures);

    return (Test *)&pkt_tests;
}

void tests_pkt(void)
{
    TESTS_RUN(tests_pkt_tests());
}
/** @} */
