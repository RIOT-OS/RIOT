/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
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
#include <stdlib.h>

#include "embUnit.h"

#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/ipv6/netif.h"

#include "unittests-constants.h"
#include "tests-ipv6_nc.h"

/* default interface for testing */
#define DEFAULT_TEST_NETIF      (TEST_UINT16)
/* default IPv6 addr for testing */
#define DEFAULT_TEST_IPV6_ADDR  { { \
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f \
        } \
    }

/* another interface for testing */
#define OTHER_TEST_NETIF        (TEST_UINT16 + TEST_UINT8)
/* another IPv6 addr for testing */
#define OTHER_TEST_IPV6_ADDR    { { \
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f \
        } \
    }

/* a third IPv6 addr for testing */
#define THIRD_TEST_IPV6_ADDR    { { \
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
            0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f \
        } \
    }

static void set_up(void)
{
    gnrc_ipv6_nc_init();
    gnrc_ipv6_netif_add(DEFAULT_TEST_NETIF);
}

static void tear_down(void)
{
    /* remove all ncache entries, this is necesaary in case
     * the last test initializes timers. */
    gnrc_ipv6_nc_init();
    gnrc_ipv6_netif_init();
}

static void test_ipv6_nc_add__address_registered(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry1, *entry2;

    TEST_ASSERT_NOT_NULL((entry1 = gnrc_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr, TEST_STRING4,
                                                    sizeof(TEST_STRING4), 0)));
    TEST_ASSERT_NOT_NULL((entry2 = gnrc_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr, TEST_STRING4,
                                                    sizeof(TEST_STRING4), 0)));
    TEST_ASSERT(entry1 == entry2);
}

static void test_ipv6_nc_add__address_NULL(void)
{
    TEST_ASSERT_NULL(gnrc_ipv6_nc_add(DEFAULT_TEST_NETIF, NULL, TEST_STRING4,
                                      sizeof(TEST_STRING4), 0));
}

static void test_ipv6_nc_add__iface_KERNEL_PID_UNDEF(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    TEST_ASSERT_NOT_NULL(gnrc_ipv6_nc_add(KERNEL_PID_UNDEF, &addr, TEST_STRING4,
                                          sizeof(TEST_STRING4), 0));
}

static void test_ipv6_nc_add__addr_unspecified(void)
{
    ipv6_addr_t addr = IPV6_ADDR_UNSPECIFIED;

    TEST_ASSERT_NULL(gnrc_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr, TEST_STRING4,
                                      sizeof(TEST_STRING4), 0));
}

static void test_ipv6_nc_add__l2addr_too_long(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    TEST_ASSERT_NULL(gnrc_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr, TEST_STRING4,
                                      GNRC_IPV6_NC_L2_ADDR_MAX + TEST_UINT8, 0));
}

static void test_ipv6_nc_add__full(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    for (int i = 0; i < GNRC_IPV6_NC_SIZE; i++) {
        TEST_ASSERT_NOT_NULL(gnrc_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr, TEST_STRING4,
                                              sizeof(TEST_STRING4), 0));
        addr.u16[7].u16++;
    }

    TEST_ASSERT_NULL(gnrc_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr, TEST_STRING4,
                                      sizeof(TEST_STRING4), 0));
}

static void test_ipv6_nc_add__success(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry1, *entry2;

    TEST_ASSERT_NOT_NULL((entry1 = gnrc_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr, TEST_STRING4,
                                                    sizeof(TEST_STRING4), 0)));

    TEST_ASSERT_NOT_NULL((entry2 = gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT(entry1 == entry2);
}

static void test_ipv6_nc_add__address_update_despite_free_entry(void)
{
    ipv6_addr_t default_addr = DEFAULT_TEST_IPV6_ADDR;
    ipv6_addr_t other_addr = OTHER_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry1, *entry2;

    TEST_ASSERT_NOT_NULL(gnrc_ipv6_nc_add(OTHER_TEST_NETIF, &other_addr, TEST_STRING4,
                                          sizeof(TEST_STRING4), 0));
    TEST_ASSERT_NOT_NULL((entry1 = gnrc_ipv6_nc_add(DEFAULT_TEST_NETIF, &default_addr, TEST_STRING4,
                                                    sizeof(TEST_STRING4), 0)));

    /* create space by removing first entry and see if duplicate is still detected & updated */
    gnrc_ipv6_nc_remove(OTHER_TEST_NETIF, &other_addr);
    TEST_ASSERT_NOT_NULL((entry2 = gnrc_ipv6_nc_add(DEFAULT_TEST_NETIF, &default_addr, TEST_STRING4,
                                                    sizeof(TEST_STRING4), 0)));
    TEST_ASSERT(entry1 == entry2);
}

static void test_ipv6_nc_remove__no_entry_pid(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */
    gnrc_ipv6_nc_remove(OTHER_TEST_NETIF, &addr);
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
    TEST_ASSERT_NULL(gnrc_ipv6_nc_get(OTHER_TEST_NETIF, &addr));
}

static void test_ipv6_nc_remove__no_entry_addr1(void)
{
    ipv6_addr_t addr = OTHER_TEST_IPV6_ADDR;
    ipv6_addr_t exp_addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */
    gnrc_ipv6_nc_remove(DEFAULT_TEST_NETIF, &addr);
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &exp_addr));
    TEST_ASSERT_NULL(gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_nc_remove__no_entry_addr2(void)
{
    ipv6_addr_t addr = IPV6_ADDR_UNSPECIFIED;
    ipv6_addr_t exp_addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */
    gnrc_ipv6_nc_remove(DEFAULT_TEST_NETIF, &addr);
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &exp_addr));
    TEST_ASSERT_NULL(gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_nc_remove__success(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
    gnrc_ipv6_nc_remove(DEFAULT_TEST_NETIF, &addr);
    TEST_ASSERT_NULL(gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_nc_get__empty(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    TEST_ASSERT_NULL(gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_nc_get__different_if(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NULL(gnrc_ipv6_nc_get(OTHER_TEST_NETIF, &addr));
}

static void test_ipv6_nc_get__different_addr(void)
{
    ipv6_addr_t addr = OTHER_TEST_IPV6_ADDR;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NULL(gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_nc_get__success_if_local(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, entry->iface);
    TEST_ASSERT(ipv6_addr_equal(&(entry->ipv6_addr), &addr));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, (char *)entry->l2_addr);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), entry->l2_addr_len);
    TEST_ASSERT_EQUAL_INT(0, entry->flags);
}

static void test_ipv6_nc_get__success_if_global(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get(KERNEL_PID_UNDEF, &addr)));
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, entry->iface);
    TEST_ASSERT(ipv6_addr_equal(&(entry->ipv6_addr), &addr));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, (char *)entry->l2_addr);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), entry->l2_addr_len);
    TEST_ASSERT_EQUAL_INT(0, entry->flags);
}

static void test_ipv6_nc_get_next__empty(void)
{
    TEST_ASSERT_NULL(gnrc_ipv6_nc_get_next(NULL));
}

static void test_ipv6_nc_get_next__1_entry(void)
{
    gnrc_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get_next(NULL)));
    TEST_ASSERT_NULL(gnrc_ipv6_nc_get_next(entry));
}

static void test_ipv6_nc_get_next__2_entries(void)
{
    ipv6_addr_t addr = OTHER_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr, TEST_STRING8,
                                          sizeof(TEST_STRING8) - 1, 0));


    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get_next(NULL)));
    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get_next(entry)));
    TEST_ASSERT_NULL(gnrc_ipv6_nc_get_next(entry));
}

static void test_ipv6_nc_get_next__holey(void)
{
    ipv6_addr_t addr1 = OTHER_TEST_IPV6_ADDR;
    ipv6_addr_t addr2 = THIRD_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry = NULL, *exp_entry = NULL;

    /* adds DEFAULT_TEST_IPV6_ADDR and OTHER_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */
    test_ipv6_nc_get_next__2_entries();
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr2, TEST_STRING8,
                                          sizeof(TEST_STRING8) - 2, 0));
    TEST_ASSERT_NOT_NULL((exp_entry = gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr2)));
    gnrc_ipv6_nc_remove(DEFAULT_TEST_NETIF, &addr1);

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get_next(NULL)));
    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get_next(entry)));
    TEST_ASSERT(exp_entry == entry);
    TEST_ASSERT_NULL(gnrc_ipv6_nc_get_next(entry));
}

static void test_ipv6_nc_get_next_router__empty(void)
{
    TEST_ASSERT_NULL(gnrc_ipv6_nc_get_next_router(NULL));
}

static void test_ipv6_nc_get_next_router__first_entry(void)
{
    gnrc_ipv6_nc_t *entry = NULL;

    /* adds DEFAULT_TEST_IPV6_ADDR and OTHER_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */
    test_ipv6_nc_get_next__2_entries();
    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get_next(NULL)));
    entry->flags = (GNRC_IPV6_NC_STATE_REACHABLE << GNRC_IPV6_NC_STATE_POS);
    entry->flags |= GNRC_IPV6_NC_IS_ROUTER;

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get_next_router(NULL)));
    TEST_ASSERT_NULL(gnrc_ipv6_nc_get_next_router(entry));
}

static void test_ipv6_nc_get_next_router__second_entry(void)
{
    gnrc_ipv6_nc_t *entry1 = NULL, *entry2 = NULL;

    /* adds DEFAULT_TEST_IPV6_ADDR and OTHER_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */
    test_ipv6_nc_get_next__2_entries();
    TEST_ASSERT_NOT_NULL((entry1 = gnrc_ipv6_nc_get_next(NULL)));
    TEST_ASSERT_NOT_NULL((entry2 = gnrc_ipv6_nc_get_next(entry1)));
    entry2->flags = (GNRC_IPV6_NC_STATE_REACHABLE << GNRC_IPV6_NC_STATE_POS);
    entry2->flags |= GNRC_IPV6_NC_IS_ROUTER;

    TEST_ASSERT_NOT_NULL((entry1 = gnrc_ipv6_nc_get_next_router(NULL)));
    TEST_ASSERT(entry2 == entry1);
    TEST_ASSERT_NULL(gnrc_ipv6_nc_get_next_router(entry1));
}

static void test_ipv6_nc_is_reachable__incomplete(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get(KERNEL_PID_UNDEF, &addr)));
    entry->flags = (GNRC_IPV6_NC_STATE_INCOMPLETE << GNRC_IPV6_NC_STATE_POS);
    TEST_ASSERT(!gnrc_ipv6_nc_is_reachable(entry));
}

static void test_ipv6_nc_is_reachable__reachable(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr)));
    entry->flags = (GNRC_IPV6_NC_STATE_REACHABLE << GNRC_IPV6_NC_STATE_POS);
    TEST_ASSERT(gnrc_ipv6_nc_is_reachable(entry));
}

static void test_ipv6_nc_is_reachable__unmanaged(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr)));
    entry->flags = (GNRC_IPV6_NC_STATE_UNMANAGED << GNRC_IPV6_NC_STATE_POS);
    TEST_ASSERT(gnrc_ipv6_nc_is_reachable(entry));
}

static void test_ipv6_nc_still_reachable__incomplete(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry = NULL;

    /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF and sets flags to
     * (GNRC_IPV6_NC_STATE_INCOMPLETE << GNRC_IPV6_NC_STATE_POS) */
    test_ipv6_nc_is_reachable__incomplete();

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get(KERNEL_PID_UNDEF, &addr)));
    TEST_ASSERT(!gnrc_ipv6_nc_is_reachable(entry));
    TEST_ASSERT_NOT_NULL(gnrc_ipv6_nc_still_reachable(&addr));
    TEST_ASSERT(!gnrc_ipv6_nc_is_reachable(entry));
}

static void test_ipv6_nc_still_reachable__success(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry = NULL;

    /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF and sets flags to
     * (GNRC_IPV6_NC_STATE_REACHABLE << GNRC_IPV6_NC_STATE_POS) */
    test_ipv6_nc_is_reachable__reachable();

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get(KERNEL_PID_UNDEF, &addr)));
    TEST_ASSERT(gnrc_ipv6_nc_is_reachable(entry));

    entry->flags = (GNRC_IPV6_NC_STATE_STALE << GNRC_IPV6_NC_STATE_POS);

    TEST_ASSERT_NOT_NULL(gnrc_ipv6_nc_still_reachable(&addr));
    TEST_ASSERT(gnrc_ipv6_nc_is_reachable(entry));
}

static void test_ipv6_nc_get_l2_addr__NULL_entry(void)
{
    gnrc_ipv6_nc_t *entry = NULL;
    uint8_t l2_addr[GNRC_IPV6_NC_L2_ADDR_MAX];
    uint8_t l2_addr_len = 0;

    TEST_ASSERT_EQUAL_INT(KERNEL_PID_UNDEF,
                          gnrc_ipv6_nc_get_l2_addr(l2_addr, &l2_addr_len, entry));
}

static void test_ipv6_nc_get_l2_addr__unreachable(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry = NULL;
    uint8_t l2_addr[GNRC_IPV6_NC_L2_ADDR_MAX];
    uint8_t l2_addr_len = 0;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get(KERNEL_PID_UNDEF, &addr)));
    entry->flags = (GNRC_IPV6_NC_STATE_INCOMPLETE << GNRC_IPV6_NC_STATE_POS);
    TEST_ASSERT_EQUAL_INT(KERNEL_PID_UNDEF,
                          gnrc_ipv6_nc_get_l2_addr(l2_addr, &l2_addr_len, entry));
}

static void test_ipv6_nc_get_l2_addr__reachable(void)
{
    ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    gnrc_ipv6_nc_t *entry = NULL;
    uint8_t l2_addr[GNRC_IPV6_NC_L2_ADDR_MAX];
    uint8_t l2_addr_len = 0;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = gnrc_ipv6_nc_get(KERNEL_PID_UNDEF, &addr)));
    entry->flags = (GNRC_IPV6_NC_STATE_REACHABLE << GNRC_IPV6_NC_STATE_POS);
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF,
                          gnrc_ipv6_nc_get_l2_addr(l2_addr, &l2_addr_len, entry));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, (char *)l2_addr);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), l2_addr_len);
}

Test *tests_ipv6_nc_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ipv6_nc_add__address_registered),
        new_TestFixture(test_ipv6_nc_add__address_NULL),
        new_TestFixture(test_ipv6_nc_add__iface_KERNEL_PID_UNDEF),
        new_TestFixture(test_ipv6_nc_add__addr_unspecified),
        new_TestFixture(test_ipv6_nc_add__l2addr_too_long),
        new_TestFixture(test_ipv6_nc_add__full),
        new_TestFixture(test_ipv6_nc_add__success),
        new_TestFixture(test_ipv6_nc_add__address_update_despite_free_entry),
        new_TestFixture(test_ipv6_nc_remove__no_entry_pid),
        new_TestFixture(test_ipv6_nc_remove__no_entry_addr1),
        new_TestFixture(test_ipv6_nc_remove__no_entry_addr2),
        new_TestFixture(test_ipv6_nc_remove__success),
        new_TestFixture(test_ipv6_nc_get__empty),
        new_TestFixture(test_ipv6_nc_get__different_if),
        new_TestFixture(test_ipv6_nc_get__different_addr),
        new_TestFixture(test_ipv6_nc_get__success_if_local),
        new_TestFixture(test_ipv6_nc_get__success_if_global),
        new_TestFixture(test_ipv6_nc_get_next__empty),
        new_TestFixture(test_ipv6_nc_get_next__1_entry),
        new_TestFixture(test_ipv6_nc_get_next__2_entries),
        new_TestFixture(test_ipv6_nc_get_next__holey),
        new_TestFixture(test_ipv6_nc_get_next_router__empty),
        new_TestFixture(test_ipv6_nc_get_next_router__first_entry),
        new_TestFixture(test_ipv6_nc_get_next_router__second_entry),
        new_TestFixture(test_ipv6_nc_is_reachable__incomplete),
        new_TestFixture(test_ipv6_nc_is_reachable__reachable),
        new_TestFixture(test_ipv6_nc_is_reachable__unmanaged),
        new_TestFixture(test_ipv6_nc_still_reachable__incomplete),
        new_TestFixture(test_ipv6_nc_still_reachable__success),
        new_TestFixture(test_ipv6_nc_get_l2_addr__NULL_entry),
        new_TestFixture(test_ipv6_nc_get_l2_addr__unreachable),
        new_TestFixture(test_ipv6_nc_get_l2_addr__reachable),
    };

    EMB_UNIT_TESTCALLER(ipv6_nc_tests, set_up, tear_down, fixtures);

    return (Test *)&ipv6_nc_tests;
}

void tests_ipv6_nc(void)
{
    TESTS_RUN(tests_ipv6_nc_tests());
}
/** @} */
