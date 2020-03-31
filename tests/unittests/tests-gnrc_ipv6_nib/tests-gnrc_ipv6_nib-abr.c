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
#include "net/gnrc/ipv6/nib/abr.h"

#include "_nib-internal.h"

#include "unittests-constants.h"

#include "tests-gnrc_ipv6_nib.h"

#define GLOBAL_PREFIX       { 0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0 }

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
 * Creates CONFIG_GNRC_IPV6_NIB_ABR_NUMOF authoritative border router list entries with
 * different addresses and then tries to create another one
 * Expected result: gnrc_ipv6_nib_abr_add() returns -ENOMEM
 */
static void test_nib_abr_add__ENOMEM(void)
{
    void *iter_state = NULL;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                { .u64 = TEST_UINT64 } } };
    gnrc_ipv6_nib_abr_t abr;

    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_ABR_NUMOF; i++) {
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_abr_add(&addr));
        addr.u16[0].u16++;
        TEST_ASSERT(gnrc_ipv6_nib_abr_iter(&iter_state, &abr));
    }
    TEST_ASSERT_EQUAL_INT(-ENOMEM, gnrc_ipv6_nib_abr_add(&addr));
    TEST_ASSERT(!gnrc_ipv6_nib_abr_iter(&iter_state, &abr));
}

/*
 * Creates CONFIG_GNRC_IPV6_NIB_ABR_NUMOF authoritative border router list entries with
 * different addresses and then tries to add another equal to the last.
 * Expected result: should return 0.
 */
static void test_nib_abr_add__success(void)
{
    void *iter_state = NULL;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                { .u64 = TEST_UINT64 } } };
    gnrc_ipv6_nib_abr_t abr;

    for (unsigned i = 0; i < CONFIG_GNRC_IPV6_NIB_ABR_NUMOF; i++) {
        addr.u16[0].u16++;
        TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_abr_add(&addr));
        TEST_ASSERT(gnrc_ipv6_nib_abr_iter(&iter_state, &abr));
        TEST_ASSERT(memcmp(&abr.addr, &addr, sizeof(addr)) == 0);
    }
    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_abr_add(&addr));
    TEST_ASSERT(!gnrc_ipv6_nib_abr_iter(&iter_state, &abr));
}

/*
 * Creates an authoritative border router list entry and removes it.
 * Expected result: system does not crash ;-)
 */
static void test_nib_abr_del__success(void)
{
    void *iter_state = NULL;
    ipv6_addr_t addr = { .u64 = { { .u8 = GLOBAL_PREFIX },
                                  { .u64 = TEST_UINT64 } } };
    gnrc_ipv6_nib_abr_t abr;

    TEST_ASSERT_EQUAL_INT(0, gnrc_ipv6_nib_abr_add(&addr));
    TEST_ASSERT(gnrc_ipv6_nib_abr_iter(&iter_state, &abr));
    TEST_ASSERT(!gnrc_ipv6_nib_abr_iter(&iter_state, &abr));
    gnrc_ipv6_nib_abr_del(&addr);
    TEST_ASSERT(!gnrc_ipv6_nib_abr_iter(&iter_state, &abr));
}

Test *tests_gnrc_ipv6_nib_abr_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nib_abr_add__ENOMEM),
        new_TestFixture(test_nib_abr_add__success),
        new_TestFixture(test_nib_abr_del__success),
        /* gnrc_ipv6_nib_abr_iter() is tested during all the tests above */
    };

    EMB_UNIT_TESTCALLER(tests, set_up, NULL,
                        fixtures);

    return (Test *)&tests;
}
