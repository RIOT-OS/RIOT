/*
 * Copyright (C) 2016 Freie Universität Berlin
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

#include <inttypes.h>

#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6/nib/pl.h"

#include "_nib-internal.h"

#include "unittests-constants.h"

#include "tests-gnrc_ipv6_nib.h"

#define LINK_LOCAL_PREFIX   { 0xfe, 0x08, 0, 0, 0, 0, 0, 0 }
#define GLOBAL_PREFIX       { 0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0 }
#define L2ADDR              { 0x90, 0xd5, 0x8e, 0x8c, 0x92, 0x43, 0x73, 0x5c }
#define GLOBAL_PREFIX_LEN   (30)
#define IFACE               (6)

static void set_up(void)
{
    evtimer_event_t *tmp;

    for (evtimer_event_t *ptr = _nib_evtimer.events;
         (ptr != NULL) && (tmp = (ptr->next), 1);
         ptr = tmp) {
        evtimer_del((evtimer_t *)(&_nib_evtimer), ptr);
    }
    _nib_init();
}

/*
 * Tries to create a prefix list entry with the unspecified address (::) as
 * prefix.
 * Expected result: gnrc_ipv6_nib_pl_set() returns -EINVAL
 */
static void test_nib_pl_set__EINVAL_unspec_addr(void)
{
    TEST_ASSERT_EQUAL_INT(-EINVAL, gnrc_ipv6_nib_pl_set(IFACE,
                                                        &ipv6_addr_unspecified,
                                                        GLOBAL_PREFIX_LEN,
                                                        UINT32_MAX, UINT32_MAX));
}

/*
 * Tries to create a prefix list entry with a link-local prefix (fe80::) as
 * prefix.
 * Expected result: gnrc_ipv6_nib_pl_set() returns -EINVAL
 */
static void test_nib_pl_set__EINVAL_link_local(void)
{
    TEST_ASSERT_EQUAL_INT(-EINVAL, gnrc_ipv6_nib_pl_set(IFACE,
                                                        &ipv6_addr_link_local_prefix,
                                                        GLOBAL_PREFIX_LEN,
                                                        UINT32_MAX,
                                                        UINT32_MAX));
}

/*
 * Tries to create a prefix list entry with the multicast address as
 * prefix.
 * Expected result: gnrc_ipv6_nib_pl_set() returns -EINVAL
 */
static void test_nib_pl_set__EINVAL_mc_addr(void)
{
    TEST_ASSERT_EQUAL_INT(-EINVAL, gnrc_ipv6_nib_pl_set(IFACE,
                                                        &ipv6_addr_all_nodes_link_local,
                                                        GLOBAL_PREFIX_LEN,
                                                        UINT32_MAX,
                                                        UINT32_MAX));
}

/*
 * Tries to create multiple prefix list entries with prefix length 0 and > 128.
 * Expected result: gnrc_ipv6_nib_pl_set() always returns -EINVAL
 */
static void test_nib_pl_set__EINVAL_pfx_len(void)
{
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX } } };
    TEST_ASSERT_EQUAL_INT(-EINVAL, gnrc_ipv6_nib_pl_set(IFACE, &pfx, 0,
                                                        UINT32_MAX,
                                                        UINT32_MAX));
}

#if CONFIG_GNRC_IPV6_NIB_NUMOF < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF
#define MAX_NUMOF   (CONFIG_GNRC_IPV6_NIB_NUMOF)
#else /* CONFIG_GNRC_IPV6_NIB_NUMOF < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF */
#define MAX_NUMOF   (CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF)
#endif

/*
 * Creates MAX_NUMOF prefix list entries with different interfaces and then
 * tries to create another one
 * Expected result: gnrc_ipv6_nib_pl_set() returns -ENOMEM
 */
static void test_nib_pl_set__ENOMEM_diff_iface(void)
{
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                            { .u64 = TEST_UINT64 } } };
    unsigned iface = IFACE;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(iface, &pfx,
                                                      GLOBAL_PREFIX_LEN,
                                                      UINT32_MAX, UINT32_MAX));
        iface++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_pl_set(iface, &pfx,
                                                        GLOBAL_PREFIX_LEN,
                                                        UINT32_MAX,
                                                        UINT32_MAX));
}

/*
 * Creates CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF prefix list entries with different prefix of
 * the same length and then tries to create another one
 * Expected result: gnrc_ipv6_nib_pl_set() returns -ENOMEM
 */
static void test_nib_pl_set__ENOMEM_diff_pfx(void)
{
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                               { .u64 = TEST_UINT64 } } };

    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(IFACE, &pfx,
                                                      GLOBAL_PREFIX_LEN,
                                                      UINT32_MAX, UINT32_MAX));
        pfx.u16[0].u16++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_pl_set(IFACE, &pfx,
                                                        GLOBAL_PREFIX_LEN,
                                                        UINT32_MAX,
                                                        UINT32_MAX));
}

/*
 * Creates CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF prefix list entries with different prefix of
 * the same length and different interfaces and then tries to create another one
 * Expected result: gnrc_ipv6_nib_pl_set() returns -ENOMEM
 */
static void test_nib_pl_set__ENOMEM_diff_iface_pfx(void)
{
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                               { .u64 = TEST_UINT64 } } };
    unsigned iface = IFACE;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(iface, &pfx,
                                                      GLOBAL_PREFIX_LEN,
                                                      UINT32_MAX, UINT32_MAX));
        iface++;
        pfx.u16[0].u16++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_pl_set(iface, &pfx,
                                                        GLOBAL_PREFIX_LEN,
                                                        UINT32_MAX,
                                                        UINT32_MAX));
}

/*
 * Creates CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF prefix list entries with prefixes of
 * different length and then tries to create another one
 * Expected result: gnrc_ipv6_nib_pl_set() returns -ENOMEM
 */
static void test_nib_pl_set__ENOMEM_diff_pfx_len(void)
{
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                            { .u64 = TEST_UINT64 } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN;

    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(IFACE, &pfx, pfx_len,
                                                      UINT32_MAX, UINT32_MAX));
        pfx_len--;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_pl_set(IFACE, &pfx, pfx_len,
                                                        UINT32_MAX,
                                                        UINT32_MAX));
}

/*
 * Creates MAX_NUMOF prefix list entries with prefixes of different length and
 * different interfaces and then tries to create another one
 * Expected result: gnrc_ipv6_nib_pl_set() returns -ENOMEM
 */
static void test_nib_pl_set__ENOMEM_diff_iface_pfx_len(void)
{
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                            { .u64 = TEST_UINT64 } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN, iface = IFACE;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(iface, &pfx, pfx_len,
                                                      UINT32_MAX, UINT32_MAX));
        pfx_len--;
        iface++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_pl_set(iface, &pfx, pfx_len,
                                                        UINT32_MAX,
                                                        UINT32_MAX));
}

/*
 * Creates CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF prefix list entries with different prefixes
 * and then tries to create another one
 * Expected result: gnrc_ipv6_nib_pl_set() returns -ENOMEM
 */
static void test_nib_pl_set__ENOMEM_diff_pfx_pfx_len(void)
{
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                 { .u64 = TEST_UINT64 } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN;

    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_OFFL_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(IFACE, &pfx, pfx_len,
                                                      UINT32_MAX, UINT32_MAX));
        pfx_len--;
        pfx.u16[0].u16++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_pl_set(IFACE, &pfx, pfx_len,
                                                        UINT32_MAX,
                                                        UINT32_MAX));
}

/*
 * Creates MAX_NUMOF prefix list entries with different prefixes and different
 * interfaces then tries to create another one
 * Expected result: gnrc_ipv6_nib_pl_set() returns -ENOMEM
 */
static void test_nib_pl_set__ENOMEM_diff_iface_pfx_pfx_len(void)
{
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                 { .u64 = TEST_UINT64 } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN, iface = IFACE;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(iface, &pfx, pfx_len,
                                                      UINT32_MAX, UINT32_MAX));
        pfx_len--;
        pfx.u16[0].u16++;
        iface++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_pl_set(iface, &pfx, pfx_len,
                                                        UINT32_MAX,
                                                        UINT32_MAX));
}

/*
 * Creates MAX_NUMOF prefix list entries with different prefixes and different
 * interfaces and then tries to add another equal to the last.
 * Expected result: should return not NULL (the last)
 */
static void test_nib_pl_set__success_duplicate(void)
{
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                 { .u64 = TEST_UINT64 } } };
    unsigned pfx_len = GLOBAL_PREFIX_LEN, iface = IFACE;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        pfx_len--;
        pfx.u16[0].u16++;
        iface++;
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(iface, &pfx, pfx_len,
                                                      UINT32_MAX, UINT32_MAX));
    }
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(iface, &pfx, pfx_len,
                                                  UINT32_MAX, UINT32_MAX));
}

/*
 * Creates a prefix list entry then creates it again with a different valid and
 * preferred lifetime.
 * Expected result: should be able to be created and the lifetimes should have
 * been changed
 */
static void test_nib_pl_set__success_change(void)
{
    gnrc_ipv6_nib_pl_t ple;
    void *iter_state = NULL;
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                            { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(IFACE, &pfx,
                                                  GLOBAL_PREFIX_LEN,
                                                  UINT32_MAX, UINT32_MAX));
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(IFACE, &pfx,
                                                  GLOBAL_PREFIX_LEN,
                                                  TEST_UINT32,
                                                  TEST_UINT32 - TEST_UINT8));
    TEST_ASSERT(gnrc_ipv6_nib_pl_iter(0, &iter_state, &ple));
    TEST_ASSERT(ple.valid_until != UINT32_MAX);
    TEST_ASSERT(ple.pref_until != UINT32_MAX);
    TEST_ASSERT(!gnrc_ipv6_nib_pl_iter(0, &iter_state, &ple));
}

/*
 * Creates a prefix list entry then creates it again with a different valid and
 * preferred lifetime.
 * Expected result: a new entry should exist and contain the given prefix,
 * interface, and lifetimes
 */
static void test_nib_pl_set__success(void)
{
    gnrc_ipv6_nib_pl_t ple;
    void *iter_state = NULL;
    static const ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                            { .u64 = TEST_UINT64 } } };

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(IFACE, &pfx,
                                                  GLOBAL_PREFIX_LEN,
                                                  UINT32_MAX, UINT32_MAX));
    TEST_ASSERT(gnrc_ipv6_nib_pl_iter(0, &iter_state, &ple));
    TEST_ASSERT(ipv6_addr_match_prefix(&ple.pfx, &pfx) >= GLOBAL_PREFIX_LEN);
    TEST_ASSERT_EQUAL_INT(GLOBAL_PREFIX_LEN, ple.pfx_len);
    TEST_ASSERT_EQUAL_INT(IFACE, ple.iface);
    TEST_ASSERT_EQUAL_INT(UINT32_MAX, ple.valid_until);
    TEST_ASSERT_EQUAL_INT(UINT32_MAX, ple.pref_until);
    TEST_ASSERT(!gnrc_ipv6_nib_pl_iter(0, &iter_state, &ple));
}

/*
 * Creates MAX_NUMOF prefix list entries with different prefix and interfaces
 * and then tries to delete one with yet another prefix and interface.
 * Expected result: There should be still CONFIG_GNRC_IPV6_NIB_NUMOF entries in the
 * neighbor cache
 */
static void test_nib_pl_del__unknown(void)
{
    void *iter_state = NULL;
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                 { .u64 = TEST_UINT64 } } };
    gnrc_ipv6_nib_pl_t ple;
    unsigned pfx_len = GLOBAL_PREFIX_LEN, iface = IFACE, count = 0;

    for (unsigned i = 0; i < MAX_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(iface, &pfx, pfx_len,
                                                      UINT32_MAX, UINT32_MAX));
        pfx_len--;
        pfx.u16[0].u16++;
        iface++;
    }
    gnrc_ipv6_nib_pl_del(iface, &pfx, pfx_len);
    while (gnrc_ipv6_nib_pl_iter(0, &iter_state, &ple)) {
        count++;
    }
    TEST_ASSERT_EQUAL_INT(MAX_NUMOF, count);
}

/*
 * Creates a prefix entry and removes it.
 * Expected result: prefix entry should be empty
 */
static void test_nib_pl_del__success(void)
{
    void *iter_state = NULL;
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                 { .u64 = TEST_UINT64 } } };
    gnrc_ipv6_nib_pl_t ple;

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(IFACE, &pfx, GLOBAL_PREFIX_LEN,
                                                  UINT32_MAX, UINT32_MAX));
    gnrc_ipv6_nib_pl_del(IFACE, &pfx, GLOBAL_PREFIX_LEN);
    TEST_ASSERT(!gnrc_ipv6_nib_pl_iter(0, &iter_state, &ple));
}

/**
 * Creates three prefix list entries and removes the second one.
 * The prefix list is then iterated.
 * Expected result: there should be two prefix list entries returned, the first
 * and the third one
 */
static void test_nib_pl_iter__empty_in_the_middle(void)
{
    gnrc_ipv6_nib_pl_t ple;
    void *iter_state = NULL;
    ipv6_addr_t pfx = { .u64 = { { .u8 = GLOBAL_PREFIX },
                               { .u64 = TEST_UINT64 } } };
    int count = 0;

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(IFACE, &pfx,
                                                  GLOBAL_PREFIX_LEN,
                                                  UINT32_MAX, UINT32_MAX));
    pfx.u16[0].u16++;
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(IFACE, &pfx,
                                                  GLOBAL_PREFIX_LEN,
                                                  UINT32_MAX, UINT32_MAX));
    pfx.u16[0].u16++;
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_pl_set(IFACE, &pfx,
                                                  GLOBAL_PREFIX_LEN,
                                                  UINT32_MAX, UINT32_MAX));
    pfx.u16[0].u16--;
    gnrc_ipv6_nib_pl_del(IFACE, &pfx, GLOBAL_PREFIX_LEN);
    pfx.u16[0].u16--;
    while (gnrc_ipv6_nib_pl_iter(0, &iter_state, &ple)) {
        TEST_ASSERT(ipv6_addr_match_prefix(&ple.pfx, &pfx) >= GLOBAL_PREFIX_LEN);
        TEST_ASSERT_EQUAL_INT(GLOBAL_PREFIX_LEN, ple.pfx_len);
        TEST_ASSERT_EQUAL_INT(IFACE, ple.iface);
        TEST_ASSERT_EQUAL_INT(UINT32_MAX, ple.valid_until);
        TEST_ASSERT_EQUAL_INT(UINT32_MAX, ple.pref_until);
        count++;
        pfx.u16[0].u16 += 2; /* we skip the second address */
    }
    TEST_ASSERT_EQUAL_INT(2, count);
}

Test *tests_gnrc_ipv6_nib_pl_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nib_pl_set__EINVAL_unspec_addr),
        new_TestFixture(test_nib_pl_set__EINVAL_link_local),
        new_TestFixture(test_nib_pl_set__EINVAL_mc_addr),
        new_TestFixture(test_nib_pl_set__EINVAL_pfx_len),
        new_TestFixture(test_nib_pl_set__ENOMEM_diff_pfx),
        new_TestFixture(test_nib_pl_set__ENOMEM_diff_iface),
        new_TestFixture(test_nib_pl_set__ENOMEM_diff_iface_pfx),
        new_TestFixture(test_nib_pl_set__ENOMEM_diff_pfx_len),
        new_TestFixture(test_nib_pl_set__ENOMEM_diff_iface_pfx_len),
        new_TestFixture(test_nib_pl_set__ENOMEM_diff_pfx_pfx_len),
        new_TestFixture(test_nib_pl_set__ENOMEM_diff_iface_pfx_pfx_len),
        new_TestFixture(test_nib_pl_set__success_duplicate),
        new_TestFixture(test_nib_pl_set__success_change),
        new_TestFixture(test_nib_pl_set__success),
        new_TestFixture(test_nib_pl_del__unknown),
        new_TestFixture(test_nib_pl_del__success),
        /* most of gnrc_ipv6_nib_pl_iter() is tested during all the tests above */
        new_TestFixture(test_nib_pl_iter__empty_in_the_middle),
    };

    EMB_UNIT_TESTCALLER(tests, set_up, NULL,
                        fixtures);

    return (Test *)&tests;
}
