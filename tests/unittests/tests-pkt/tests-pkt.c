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

static void test_pkt_total_header_len__headers_NULL(void)
{
    pkt_t pkt = { NULL, NULL, 0 };

    TEST_ASSERT_EQUAL_INT(0, pkt_total_header_len(&pkt));
}

static void test_pkt_total_header_len__1_header__hdata_NULL__hlen_0(void)
{
    pkt_hlist_t hdr = { NULL, NULL, PKT_PROTO_UNKNOWN, NULL, 0};
    pkt_t pkt = { &hdr, NULL, 0 };

    hdr.next = &hdr;
    hdr.prev = &hdr;

    TEST_ASSERT_EQUAL_INT(0, pkt_total_header_len(&pkt));
}

static void test_pkt_total_header_len__1_header__hdata_NULL__hlen_MAX(void)
{
    pkt_hlist_t hdr = { NULL, NULL, PKT_PROTO_UNKNOWN, NULL, UINT8_MAX };
    pkt_t pkt = { &hdr, NULL, 0 };

    hdr.next = &hdr;
    hdr.prev = &hdr;

    TEST_ASSERT_EQUAL_INT(UINT8_MAX, pkt_total_header_len(&pkt));
}

static void test_pkt_total_header_len__1_header__hdata_NULL__hlen_value(void)
{
    pkt_hlist_t hdr = { NULL, NULL, PKT_PROTO_UNKNOWN, NULL, TEST_UINT8 };
    pkt_t pkt = { &hdr, NULL, 0 };

    hdr.next = &hdr;
    hdr.prev = &hdr;

    TEST_ASSERT_EQUAL_INT(TEST_UINT8, pkt_total_header_len(&pkt));
}

static void test_pkt_total_header_len__1_header__hdata_value__hlen_value(void)
{
    pkt_hlist_t hdr = { NULL, NULL, PKT_PROTO_UNKNOWN, TEST_STRING8,
                        sizeof(TEST_STRING8) };
    pkt_t pkt = { &hdr, NULL, 0 };

    hdr.next = &hdr;
    hdr.prev = &hdr;

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt_total_header_len(&pkt));
}

static void test_pkt_total_header_len__2_headers__hdata_value__hlen_value(void)
{
    pkt_hlist_t hdr1 = { NULL, NULL, PKT_PROTO_UNKNOWN, TEST_STRING8,
                         sizeof(TEST_STRING8) };
    pkt_hlist_t hdr2 = { NULL, NULL, PKT_PROTO_UNKNOWN, TEST_STRING12,
                         sizeof(TEST_STRING12) };
    pkt_t pkt = { &hdr1, NULL, 0 };

    hdr1.next = &hdr2;
    hdr1.prev = &hdr2;
    hdr2.next = &hdr1;
    hdr2.prev = &hdr1;

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) + sizeof(TEST_STRING12),
                          pkt_total_header_len(&pkt));
}

static void test_pkt_total_len__headers_NULL__payload_len_0(void)
{
    pkt_t pkt = { NULL, NULL, 0 };

    TEST_ASSERT_EQUAL_INT(0, pkt_total_len(&pkt));
}

static void test_pkt_total_len__headers_NULL__payload_len_MAX(void)
{
    pkt_t pkt = { NULL, NULL, PKTSIZE_MAX };

    TEST_ASSERT_EQUAL_INT(PKTSIZE_MAX, pkt_total_len(&pkt));
}

static void test_pkt_total_len__headers_NULL__payload_len_value(void)
{
    pkt_t pkt = { NULL, NULL, TEST_UINT16 };

    TEST_ASSERT_EQUAL_INT(TEST_UINT16, pkt_total_len(&pkt));
}

static void test_pkt_total_len__1_header__payload_len_0(void)
{
    pkt_hlist_t hdr = { NULL, NULL, PKT_PROTO_UNKNOWN, TEST_STRING8,
                        sizeof(TEST_STRING8) };
    pkt_t pkt = { &hdr, NULL, 0 };

    hdr.next = &hdr;
    hdr.prev = &hdr;

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8), pkt_total_len(&pkt));
}

static void test_pkt_total_len__1_header__payload_len_MAX(void)
{
    pkt_hlist_t hdr = { NULL, NULL, PKT_PROTO_UNKNOWN, TEST_STRING8,
                        sizeof(TEST_STRING8) };
    pkt_t pkt = { &hdr, NULL, PKTSIZE_MAX };

    hdr.next = &hdr;
    hdr.prev = &hdr;

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) - 1, pkt_total_len(&pkt));
    /* size should overflow */
}

static void test_pkt_total_len__1_header__payload_len_value(void)
{
    pkt_hlist_t hdr = { NULL, NULL, PKT_PROTO_UNKNOWN, TEST_STRING8,
                        sizeof(TEST_STRING8) };
    pkt_t pkt = { &hdr, NULL, TEST_UINT16 };

    hdr.next = &hdr;
    hdr.prev = &hdr;

    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING8) + TEST_UINT16, pkt_total_len(&pkt));
}

static void test_pkt_hlist_advance__NULL(void)
{
    pkt_hlist_t **list = NULL;

    pkt_hlist_advance(list);

    TEST_ASSERT_NULL(list);
}

static void test_pkt_hlist_advance__ptr_NULL(void)
{
    pkt_hlist_t *ptr_list = NULL;
    pkt_hlist_t **list = &ptr_list;

    pkt_hlist_advance(list);

    TEST_ASSERT_NULL(ptr_list);
    TEST_ASSERT(list == &ptr_list);
}

static void test_pkt_hlist_advance__1_elem(void)
{
    pkt_hlist_t hdr = { NULL, NULL, PKT_PROTO_UNKNOWN, NULL, 0 };
    pkt_hlist_t *ptr_list = &hdr;
    pkt_hlist_t **list = &ptr_list;

    hdr.next = &hdr;
    hdr.prev = &hdr;

    pkt_hlist_advance(list);

    TEST_ASSERT_NOT_NULL(ptr_list);
    TEST_ASSERT(list == &ptr_list);
    TEST_ASSERT(ptr_list == &hdr);
}

static void test_pkt_hlist_advance__2_elem(void)
{
    pkt_hlist_t hdr1 = { NULL, NULL, PKT_PROTO_UNKNOWN, NULL, 0 };
    pkt_hlist_t hdr2 = { NULL, NULL, PKT_PROTO_UNKNOWN, NULL, 0 };
    pkt_hlist_t *ptr_list = &hdr1;
    pkt_hlist_t **list = &ptr_list;

    hdr1.next = &hdr2;
    hdr1.prev = &hdr2;
    hdr2.next = &hdr1;
    hdr2.prev = &hdr1;

    pkt_hlist_advance(list);

    TEST_ASSERT_NOT_NULL(ptr_list);
    TEST_ASSERT(list == &ptr_list);
    TEST_ASSERT(ptr_list == &hdr2);

    pkt_hlist_advance(list);

    TEST_ASSERT(ptr_list == &hdr1);
}

static void test_pkt_add_header(void)
{
    pkt_hlist_t hdr1 = { NULL, NULL, PKT_PROTO_UNKNOWN, NULL, 0 };
    pkt_hlist_t hdr2 = { NULL, NULL, PKT_PROTO_UNKNOWN, NULL, 0 };
    pkt_t pkt = { NULL, NULL, 0 };

    pkt_add_header(&pkt, &hdr1);

    TEST_ASSERT(pkt.headers == &hdr1);
    TEST_ASSERT(pkt.headers->next == &hdr1);
    TEST_ASSERT(pkt.headers->prev == &hdr1);

    pkt_add_header(&pkt, &hdr2);

    TEST_ASSERT(pkt.headers == &hdr2);
    TEST_ASSERT(pkt.headers->next == &hdr1);
    TEST_ASSERT(pkt.headers->prev == &hdr1);
    TEST_ASSERT(pkt.headers->next->next == &hdr2);
    TEST_ASSERT(pkt.headers->prev->prev == &hdr2);
}

static void test_pkt_remove_header(void)
{
    pkt_hlist_t hdr1 = { NULL, NULL, PKT_PROTO_UNKNOWN, NULL, 0 };
    pkt_hlist_t hdr2 = { NULL, NULL, PKT_PROTO_UNKNOWN, NULL, 0 };
    pkt_t pkt = { NULL, NULL, 0 };

    pkt_add_header(&pkt, &hdr1);
    pkt_add_header(&pkt, &hdr2);
    pkt_remove_header(&pkt, &hdr1);

    TEST_ASSERT(pkt.headers == &hdr2);
    TEST_ASSERT(pkt.headers->next == &hdr2);
    TEST_ASSERT(pkt.headers->prev == &hdr2);

    pkt_remove_header(&pkt, &hdr1);

    TEST_ASSERT(pkt.headers == &hdr2);
    TEST_ASSERT(pkt.headers->next == &hdr2);
    TEST_ASSERT(pkt.headers->prev == &hdr2);

    pkt_remove_header(&pkt, &hdr2);

    TEST_ASSERT_NULL(pkt.headers);
}

static void test_pkt_remove_first_header(void)
{
    pkt_hlist_t hdr1 = { NULL, NULL, PKT_PROTO_UNKNOWN, NULL, 0 };
    pkt_hlist_t hdr2 = { NULL, NULL, PKT_PROTO_UNKNOWN, NULL, 0 };
    pkt_t pkt = { NULL, NULL, 0 };

    pkt_add_header(&pkt, &hdr1);
    pkt_add_header(&pkt, &hdr2);
    pkt_remove_first_header(&pkt);

    TEST_ASSERT(pkt.headers == &hdr1);
    TEST_ASSERT(pkt.headers->next == &hdr1);
    TEST_ASSERT(pkt.headers->prev == &hdr1);
}

Test *tests_pkt_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
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
        new_TestFixture(test_pkt_hlist_advance__NULL),
        new_TestFixture(test_pkt_hlist_advance__ptr_NULL),
        new_TestFixture(test_pkt_hlist_advance__1_elem),
        new_TestFixture(test_pkt_hlist_advance__2_elem),
        new_TestFixture(test_pkt_add_header),
        new_TestFixture(test_pkt_remove_header),
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
