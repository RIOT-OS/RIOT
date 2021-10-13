/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "embUnit.h"

#include "net/gnrc/netif/conf.h"
#include "net/gnrc/netif/pktq.h"

#include "tests-gnrc_netif_pktq.h"

gnrc_netif_t _netif;

static void set_up(void)
{
    while (gnrc_netif_pktq_get(&_netif)) { }
}

static void test_pktq_get__empty(void)
{
    TEST_ASSERT_NULL(gnrc_netif_pktq_get(&_netif));
}

static void test_pktq_usage__empty(void)
{
    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_usage());
}

static void test_pktq_put__full(void)
{
    gnrc_pktsnip_t pkt;

    for (unsigned i = 0; i < CONFIG_GNRC_NETIF_PKTQ_POOL_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_put(&_netif, &pkt));
    }
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_pktq_put(&_netif, &pkt));
    TEST_ASSERT_EQUAL_INT(CONFIG_GNRC_NETIF_PKTQ_POOL_SIZE,
                          gnrc_netif_pktq_usage());
}

static void test_pktq_put_get1(void)
{
    gnrc_pktsnip_t pkt_in, *pkt_out;

    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_put(&_netif, &pkt_in));
    TEST_ASSERT_EQUAL_INT(1, gnrc_netif_pktq_usage());
    TEST_ASSERT_NOT_NULL((pkt_out = gnrc_netif_pktq_get(&_netif)));
    TEST_ASSERT(&pkt_in == pkt_out);
    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_usage());
}

static void test_pktq_put_get3(void)
{
    gnrc_pktsnip_t pkt_in[3];

    for (unsigned i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_put(&_netif, &pkt_in[i]));
    }
    TEST_ASSERT_EQUAL_INT(3, gnrc_netif_pktq_usage());
    for (unsigned i = 0; i < 3; i++) {
        gnrc_pktsnip_t *pkt_out;

        TEST_ASSERT_NOT_NULL((pkt_out = gnrc_netif_pktq_get(&_netif)));
        TEST_ASSERT(&pkt_in[i] == pkt_out);
    }
    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_usage());
}

static void test_pktq_push_back__full(void)
{
    gnrc_pktsnip_t pkt;

    for (unsigned i = 0; i < CONFIG_GNRC_NETIF_PKTQ_POOL_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_put(&_netif, &pkt));
    }
    TEST_ASSERT_EQUAL_INT(-1, gnrc_netif_pktq_push_back(&_netif, &pkt));
    TEST_ASSERT_EQUAL_INT(CONFIG_GNRC_NETIF_PKTQ_POOL_SIZE,
                          gnrc_netif_pktq_usage());
}

static void test_pktq_push_back_get1(void)
{
    gnrc_pktsnip_t pkt_in, *pkt_out;

    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_push_back(&_netif, &pkt_in));
    TEST_ASSERT_EQUAL_INT(1, gnrc_netif_pktq_usage());
    TEST_ASSERT_NOT_NULL((pkt_out = gnrc_netif_pktq_get(&_netif)));
    TEST_ASSERT(&pkt_in == pkt_out);
    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_usage());
}

static void test_pktq_push_back_get3(void)
{
    gnrc_pktsnip_t pkt_in[3];

    for (unsigned i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_push_back(&_netif, &pkt_in[i]));
    }
    TEST_ASSERT_EQUAL_INT(3, gnrc_netif_pktq_usage());
    for (unsigned i = 0; i < 3; i++) {
        gnrc_pktsnip_t *pkt_out;

        TEST_ASSERT_NOT_NULL((pkt_out = gnrc_netif_pktq_get(&_netif)));
        TEST_ASSERT(&pkt_in[3 - i - 1] == pkt_out);
    }
    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_usage());
}

static void test_pktq_empty(void)
{
    gnrc_pktsnip_t pkt_in;

    TEST_ASSERT(gnrc_netif_pktq_empty(&_netif));
    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_put(&_netif, &pkt_in));
    TEST_ASSERT(!gnrc_netif_pktq_empty(&_netif));
    TEST_ASSERT_NOT_NULL(gnrc_netif_pktq_get(&_netif));
    TEST_ASSERT(gnrc_netif_pktq_empty(&_netif));
    TEST_ASSERT_EQUAL_INT(0, gnrc_netif_pktq_push_back(&_netif, &pkt_in));
    TEST_ASSERT(!gnrc_netif_pktq_empty(&_netif));
    TEST_ASSERT_NOT_NULL(gnrc_netif_pktq_get(&_netif));
    TEST_ASSERT(gnrc_netif_pktq_empty(&_netif));
}

static Test *test_gnrc_netif_pktq(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_pktq_get__empty),
        new_TestFixture(test_pktq_usage__empty),
        new_TestFixture(test_pktq_put__full),
        new_TestFixture(test_pktq_put_get1),
        new_TestFixture(test_pktq_put_get3),
        new_TestFixture(test_pktq_push_back__full),
        new_TestFixture(test_pktq_push_back_get1),
        new_TestFixture(test_pktq_push_back_get3),
        new_TestFixture(test_pktq_empty),
    };

    EMB_UNIT_TESTCALLER(pktq_tests, set_up, NULL, fixtures);

    return (Test *)&pktq_tests;
}

void tests_gnrc_netif_pktq(void)
{
    TESTS_RUN(test_gnrc_netif_pktq());
}

/** @} */
