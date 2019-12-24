/*
 * Copyright (C) 2017 Freie Universität Berlin
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

#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6/nib/nc.h"

#include "_nib-internal.h"

#include "unittests-constants.h"

#include "tests-gnrc_ipv6_nib.h"

#define LINK_LOCAL_PREFIX   { 0xfe, 0x08, 0, 0, 0, 0, 0, 0 }
#define GLOBAL_PREFIX       { 0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0 }
#define L2ADDR              { 0x90, 0xd5, 0x8e, 0x8c, 0x92, 0x43, 0x73, 0x5c }
#define IFACE               (6)

static void set_up(void)
{
    gnrc_ipv6_nib_init();
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF neighbor cache entries with different addresses
 * and then tries to create another one
 * Expected result: gnrc_ipv6_nib_nc_set() returns -ENOMEM
 */
static void test_nib_nc_set__ENOMEM_diff_addr(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    uint8_t l2addr[] = L2ADDR;

    for (unsigned i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_nc_set(&addr, IFACE, l2addr,
                                                      sizeof(l2addr)));
        addr.u64[1].u64++;
        l2addr[7]++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_nc_set(&addr, IFACE, l2addr,
                                                        sizeof(l2addr)));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF neighbor cache entries with different interfaces
 * and then tries to create another one
 * Expected result: gnrc_ipv6_nib_nc_set() returns -ENOMEM
 */
static void test_nib_nc_set__ENOMEM_diff_iface(void)
{
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                             { .u64 = TEST_UINT64 } } };
    static const uint8_t l2addr[] = L2ADDR;
    unsigned iface = IFACE;

    for (unsigned i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_nc_set(&addr, iface, l2addr,
                                                      sizeof(l2addr)));
        iface++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_nc_set(&addr, iface, l2addr,
                                                        sizeof(l2addr)));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF neighbor cache entries with different addresses
 * and interfaces and then tries to create another one
 * Expected result: gnrc_ipv6_nib_nc_set() returns -ENOMEM
 */
static void test_nib_nc_set__ENOMEM_diff_addr_iface(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    uint8_t l2addr[] = L2ADDR;
    unsigned iface = IFACE;

    for (unsigned i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_nc_set(&addr, iface, l2addr,
                                                      sizeof(l2addr)));
        addr.u64[1].u64++;
        iface++;
        l2addr[7]++;
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_nc_set(&addr, iface, l2addr,
                                                        sizeof(l2addr)));
}

/*
 * Creates a neighbor cache entry.
 * Expected result: a new entry should exist and contain the given address
 * and interface
 */
static void test_nib_nc_set__success(void)
{
    void *iter_state = NULL;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                             { .u64 = TEST_UINT64 } } };
    static const uint8_t l2addr[] = L2ADDR;
    gnrc_ipv6_nib_nc_t nce;

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_nc_set(&addr, IFACE, l2addr,
                                                  sizeof(l2addr)));
    TEST_ASSERT(gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&nce.ipv6, &addr, sizeof(addr)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&nce.l2addr, &l2addr, sizeof(l2addr)));
    TEST_ASSERT_EQUAL_INT(sizeof(l2addr), nce.l2addr_len);
    TEST_ASSERT_EQUAL_INT(IFACE, gnrc_ipv6_nib_nc_get_iface(&nce));
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED,
                          gnrc_ipv6_nib_nc_get_nud_state(&nce));
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_AR_STATE_MANUAL,
                          gnrc_ipv6_nib_nc_get_ar_state(&nce));
    TEST_ASSERT(!gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF neighbor cache entries with different IP
 * addresses and interface identifiers and then tries to add another that is
 * equal to the last.
 * Expected result: gnrc_ipv6_nib_nc_set() returns 0
 */
static void test_nib_nc_set__success_duplicate(void)
{
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    uint8_t l2addr[] = L2ADDR;
    unsigned iface = 0;

    for (unsigned i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        addr.u64[1].u64++;
        iface++;
        l2addr[7]++;
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_nc_set(&addr, iface, l2addr,
                                                      sizeof(l2addr)));
    }
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_nc_set(&addr, iface, l2addr,
                                                  sizeof(l2addr)));
}

/*
 * Creates GNRC_IPV6_NIB_NUMOF neighbor cache entries with different addresses
 * and interfaces and then tries to delete one with yet another address.
 * Expected result: There should be still GNRC_IPV6_NIB_NUMOF entries in the
 * neighbor cache
 */
static void test_nib_nc_del__unknown(void)
{
    void *iter_state = NULL;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                { .u64 = TEST_UINT64 } } };
    uint8_t l2addr[] = L2ADDR;
    gnrc_ipv6_nib_nc_t nce;
    unsigned iface = IFACE, count = 0;

    for (unsigned i = 0; i < GNRC_IPV6_NIB_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_nc_set(&addr, iface, l2addr,
                                                      sizeof(l2addr)));
        addr.u64[1].u64++;
        iface++;
        l2addr[7]++;
    }
    gnrc_ipv6_nib_nc_del(&addr, iface);
    while (gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce)) {
        count++;
    }
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NUMOF, count);
}

/*
 * Creates a neighbor cache entry and removes it.
 * Expected result: neighbor cache should be empty
 */
static void test_nib_nc_del__success(void)
{
    void *iter_state = NULL;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                             { .u64 = TEST_UINT64 } } };
    static const uint8_t l2addr[] = L2ADDR;
    gnrc_ipv6_nib_nc_t nce;

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_nc_set(&addr, IFACE, l2addr,
                                                  sizeof(l2addr)));
    gnrc_ipv6_nib_nc_del(&addr, IFACE);
    TEST_ASSERT(!gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
}

/*
 * Creates a non-manual neighbor cache entry (as the NIB would create it on an
 * incoming NDP packet), sets it to UNREACHABLE and then calls
 * gnrc_ipv6_nib_mark_reachable().
 * Expected result: The created entry should be the only entry and its state
 * should be still unreachable. The event timer is still empty.
 */
static void test_nib_nc_mark_reachable__not_in_neighbor_cache(void)
{
    void *iter_state = NULL;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                { .u64 = TEST_UINT64 } } };
    gnrc_ipv6_nib_nc_t nce;

    TEST_ASSERT_NOT_NULL(_nib_nc_add(&addr, IFACE,
                                     GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE));

    /* check pre-state */
    TEST_ASSERT(gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE,
                          gnrc_ipv6_nib_nc_get_nud_state(&nce));
    TEST_ASSERT(!gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
    TEST_ASSERT_NULL(_nib_evtimer.events);

    addr.u64[1].u64++;
    gnrc_ipv6_nib_nc_mark_reachable(&addr);
    /* check if entry is still unmanaged */
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE,
                          gnrc_ipv6_nib_nc_get_nud_state(&nce));
    /* check if there are still no events */
    TEST_ASSERT_NULL(_nib_evtimer.events);
    /* check if still the only entry */
    iter_state = NULL;
    TEST_ASSERT(gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
    TEST_ASSERT(!gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
}

/*
 * Creates a manual neighbor cache entry , sets it to UNREACHABLE and then calls
 * gnrc_ipv6_nib_mark_reachable().
 * Expected result: The created entry should be the only entry and its state
 * should be still unreachable. The event timer is still empty.
 */
static void test_nib_nc_mark_reachable__unmanaged(void)
{
    void *iter_state = NULL;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                             { .u64 = TEST_UINT64 } } };
    gnrc_ipv6_nib_nc_t nce;

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_nc_set(&addr, IFACE, NULL, 0));
    /* check pre-state */
    TEST_ASSERT(gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED,
                          gnrc_ipv6_nib_nc_get_nud_state(&nce));
    TEST_ASSERT(!gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
    TEST_ASSERT_NULL(_nib_evtimer.events);
    gnrc_ipv6_nib_nc_mark_reachable(&addr);
    /* check if entry is still unmanaged */
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED,
                          gnrc_ipv6_nib_nc_get_nud_state(&nce));
    /* check if there are still no events */
    TEST_ASSERT_NULL(_nib_evtimer.events);
    /* check if still the only entry */
    iter_state = NULL;
    TEST_ASSERT(gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
    TEST_ASSERT(!gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
}

/*
 * Creates a non-manual neighbor cache entry (as the NIB would create it on an
 * incoming NDP packet), sets it to a state unequal to REACHABLE and then calls
 * gnrc_ipv6_nib_mark_reachable()
 * Expected result: the neighbor cache entry should be in state REACHABLE and a
 * reachability timeout event for that entry is in the event timer.
 */
static void test_nib_nc_mark_reachable__success(void)
{
    void *iter_state = NULL;
    static const ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                             { .u64 = TEST_UINT64 } } };
    gnrc_ipv6_nib_nc_t nce;

    TEST_ASSERT_NOT_NULL(_nib_nc_add(&addr, IFACE,
                                     GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE));

    /* check pre-state */
    TEST_ASSERT(gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNREACHABLE,
                          gnrc_ipv6_nib_nc_get_nud_state(&nce));
    TEST_ASSERT(!gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
    TEST_ASSERT_NULL(_nib_evtimer.events);

    gnrc_ipv6_nib_nc_mark_reachable(&addr);

    iter_state = NULL;
    TEST_ASSERT(gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
#if GNRC_IPV6_NIB_CONF_ARSM
    /* check if entry is reachable */
    TEST_ASSERT_EQUAL_INT(GNRC_IPV6_NIB_NC_INFO_NUD_STATE_REACHABLE,
                          gnrc_ipv6_nib_nc_get_nud_state(&nce));
#endif
    /* check if still the only entry */
    TEST_ASSERT(!gnrc_ipv6_nib_nc_iter(0, &iter_state, &nce));
}

Test *tests_gnrc_ipv6_nib_nc_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nib_nc_set__ENOMEM_diff_addr),
        new_TestFixture(test_nib_nc_set__ENOMEM_diff_iface),
        new_TestFixture(test_nib_nc_set__ENOMEM_diff_addr_iface),
        new_TestFixture(test_nib_nc_set__success),
        new_TestFixture(test_nib_nc_set__success_duplicate),
        new_TestFixture(test_nib_nc_del__unknown),
        new_TestFixture(test_nib_nc_del__success),
        new_TestFixture(test_nib_nc_mark_reachable__not_in_neighbor_cache),
        new_TestFixture(test_nib_nc_mark_reachable__unmanaged),
        new_TestFixture(test_nib_nc_mark_reachable__success),
        /* gnrc_ipv6_nib_nc_iter() is tested during all the tests above */
    };

    EMB_UNIT_TESTCALLER(tests, set_up, NULL,
                        fixtures);

    return (Test *)&tests;
}
