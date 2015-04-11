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

#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/nc.h"

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

static void set_up(void)
{
    ng_ipv6_nc_init();
}

static void test_ipv6_nc_add__EADDRINUSE(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    TEST_ASSERT_EQUAL_INT(0, ng_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr,
                                            TEST_STRING4, sizeof(TEST_STRING4),
                                            0));
    TEST_ASSERT_EQUAL_INT(-EADDRINUSE, ng_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr,
                          TEST_STRING4, sizeof(TEST_STRING4), 0));
}

static void test_ipv6_nc_add__EFAULT(void)
{
    TEST_ASSERT_EQUAL_INT(-EFAULT, ng_ipv6_nc_add(DEFAULT_TEST_NETIF, NULL,
                          TEST_STRING4, sizeof(TEST_STRING4), 0));
}

static void test_ipv6_nc_add__EINVAL_KERNEL_PID_UNDEF(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ng_ipv6_nc_add(KERNEL_PID_UNDEF, &addr,
                          TEST_STRING4, sizeof(TEST_STRING4), 0));
}

static void test_ipv6_nc_add__EINVAL_ipv6_addr_unspecified(void)
{
    ng_ipv6_addr_t addr = NG_IPV6_ADDR_UNSPECIFIED;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ng_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr,
                          TEST_STRING4, sizeof(TEST_STRING4), 0));
}

static void test_ipv6_nc_add__EINVAL_l2addr_too_long(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    TEST_ASSERT_EQUAL_INT(-EINVAL, ng_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr,
                          TEST_STRING4, NG_IPV6_NC_L2_ADDR_MAX + TEST_UINT8, 0));
}

static void test_ipv6_nc_add__ENOMEM(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    for (int i = 0; i < NG_IPV6_NC_SIZE; i++) {
        TEST_ASSERT_EQUAL_INT(0, ng_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr,
                                                TEST_STRING4, sizeof(TEST_STRING4), 0));
        addr.u16[7].u16++;
    }

    TEST_ASSERT_EQUAL_INT(-ENOMEM, ng_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr,
                          TEST_STRING4, sizeof(TEST_STRING4), 0));
}

static void test_ipv6_nc_add__success(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    TEST_ASSERT_EQUAL_INT(0, ng_ipv6_nc_add(DEFAULT_TEST_NETIF, &addr,
                                            TEST_STRING4, sizeof(TEST_STRING4), 0));

    TEST_ASSERT_NOT_NULL(ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_nc_remove__no_entry_pid(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */
    ng_ipv6_nc_remove(OTHER_TEST_NETIF, &addr);
    TEST_ASSERT_NOT_NULL(ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
    TEST_ASSERT_NULL(ng_ipv6_nc_get(OTHER_TEST_NETIF, &addr));
}

static void test_ipv6_nc_remove__no_entry_addr1(void)
{
    ng_ipv6_addr_t addr = OTHER_TEST_IPV6_ADDR;
    ng_ipv6_addr_t exp_addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */
    ng_ipv6_nc_remove(DEFAULT_TEST_NETIF, &addr);
    TEST_ASSERT_NOT_NULL(ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &exp_addr));
    TEST_ASSERT_NULL(ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_nc_remove__no_entry_addr2(void)
{
    ng_ipv6_addr_t addr = NG_IPV6_ADDR_UNSPECIFIED;
    ng_ipv6_addr_t exp_addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */
    ng_ipv6_nc_remove(DEFAULT_TEST_NETIF, &addr);
    TEST_ASSERT_NOT_NULL(ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &exp_addr));
    TEST_ASSERT_NULL(ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_nc_remove__success(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */
    TEST_ASSERT_NOT_NULL(ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
    ng_ipv6_nc_remove(DEFAULT_TEST_NETIF, &addr);
    TEST_ASSERT_NULL(ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_nc_get__empty(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    TEST_ASSERT_NULL(ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_nc_get__different_if(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NULL(ng_ipv6_nc_get(OTHER_TEST_NETIF, &addr));
}

static void test_ipv6_nc_get__different_addr(void)
{
    ng_ipv6_addr_t addr = OTHER_TEST_IPV6_ADDR;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NULL(ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_nc_get__success_if_local(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ng_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, entry->iface);
    TEST_ASSERT(ng_ipv6_addr_equal(&(entry->ipv6_addr), &addr));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, (char *)entry->l2_addr);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), entry->l2_addr_len);
    TEST_ASSERT_EQUAL_INT(0, entry->flags);
}

static void test_ipv6_nc_get__success_if_global(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ng_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = ng_ipv6_nc_get(KERNEL_PID_UNDEF, &addr)));
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, entry->iface);
    TEST_ASSERT(ng_ipv6_addr_equal(&(entry->ipv6_addr), &addr));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, (char *)entry->l2_addr);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), entry->l2_addr_len);
    TEST_ASSERT_EQUAL_INT(0, entry->flags);
}

static void test_ipv6_nc_get_reachable__incomplete_if_local(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ng_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr)));
    entry->flags = (NG_IPV6_NC_STATE_INCOMPLETE << NG_IPV6_NC_STATE_POS);
    TEST_ASSERT_NULL(ng_ipv6_nc_get_reachable(DEFAULT_TEST_NETIF, &addr));
}

static void test_ipv6_nc_get_reachable__incomplete_if_global(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ng_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = ng_ipv6_nc_get(KERNEL_PID_UNDEF, &addr)));
    entry->flags = (NG_IPV6_NC_STATE_INCOMPLETE << NG_IPV6_NC_STATE_POS);
    TEST_ASSERT_NULL(ng_ipv6_nc_get_reachable(KERNEL_PID_UNDEF, &addr));
}

static void test_ipv6_nc_get_reachable__reachable_if_local(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ng_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = ng_ipv6_nc_get(DEFAULT_TEST_NETIF, &addr)));
    entry->flags = (NG_IPV6_NC_STATE_REACHABLE << NG_IPV6_NC_STATE_POS);
    TEST_ASSERT_NOT_NULL((entry = ng_ipv6_nc_get_reachable(DEFAULT_TEST_NETIF, &addr)));
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, entry->iface);
    TEST_ASSERT(ng_ipv6_addr_equal(&(entry->ipv6_addr), &addr));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, (char *)entry->l2_addr);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), entry->l2_addr_len);
    TEST_ASSERT_EQUAL_INT((NG_IPV6_NC_STATE_REACHABLE << NG_IPV6_NC_STATE_POS),
                          entry->flags);
}

static void test_ipv6_nc_get_reachable__reachable_if_global(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ng_ipv6_nc_t *entry = NULL;

    test_ipv6_nc_add__success(); /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF */

    TEST_ASSERT_NOT_NULL((entry = ng_ipv6_nc_get(KERNEL_PID_UNDEF, &addr)));
    entry->flags = (NG_IPV6_NC_STATE_REACHABLE << NG_IPV6_NC_STATE_POS);
    TEST_ASSERT_NOT_NULL((entry = ng_ipv6_nc_get_reachable(KERNEL_PID_UNDEF, &addr)));
    TEST_ASSERT_EQUAL_INT(DEFAULT_TEST_NETIF, entry->iface);
    TEST_ASSERT(ng_ipv6_addr_equal(&(entry->ipv6_addr), &addr));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING4, (char *)entry->l2_addr);
    TEST_ASSERT_EQUAL_INT(sizeof(TEST_STRING4), entry->l2_addr_len);
    TEST_ASSERT_EQUAL_INT((NG_IPV6_NC_STATE_REACHABLE << NG_IPV6_NC_STATE_POS),
                          entry->flags);
}

static void test_ipv6_nc_still_reachable__incomplete(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;

    /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF and sets flags to
     * (NG_IPV6_NC_STATE_INCOMPLETE << NG_IPV6_NC_STATE_POS) */
    test_ipv6_nc_get_reachable__incomplete_if_global();

    TEST_ASSERT_NULL(ng_ipv6_nc_get_reachable(KERNEL_PID_UNDEF, &addr));
    TEST_ASSERT_NOT_NULL(ng_ipv6_nc_still_reachable(&addr));
    TEST_ASSERT_NULL(ng_ipv6_nc_get_reachable(KERNEL_PID_UNDEF, &addr));
}

static void test_ipv6_nc_still_reachable__success(void)
{
    ng_ipv6_addr_t addr = DEFAULT_TEST_IPV6_ADDR;
    ng_ipv6_nc_t *entry = NULL;

    /* adds DEFAULT_TEST_IPV6_ADDR to DEFAULT_TEST_NETIF and sets flags to
     * (NG_IPV6_NC_STATE_REACHABLE << NG_IPV6_NC_STATE_POS) */
    test_ipv6_nc_get_reachable__reachable_if_global();

    TEST_ASSERT_NOT_NULL((entry = ng_ipv6_nc_get_reachable(KERNEL_PID_UNDEF, &addr)));

    entry->flags = (NG_IPV6_NC_STATE_STALE << NG_IPV6_NC_STATE_POS);

    TEST_ASSERT_NOT_NULL(ng_ipv6_nc_still_reachable(&addr));
    TEST_ASSERT_NOT_NULL((entry = ng_ipv6_nc_get_reachable(KERNEL_PID_UNDEF, &addr)));
    TEST_ASSERT_EQUAL_INT((NG_IPV6_NC_STATE_REACHABLE << NG_IPV6_NC_STATE_POS),
                          entry->flags);
}

Test *tests_ipv6_nc_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ipv6_nc_add__EADDRINUSE),
        new_TestFixture(test_ipv6_nc_add__EFAULT),
        new_TestFixture(test_ipv6_nc_add__EINVAL_KERNEL_PID_UNDEF),
        new_TestFixture(test_ipv6_nc_add__EINVAL_ipv6_addr_unspecified),
        new_TestFixture(test_ipv6_nc_add__EINVAL_l2addr_too_long),
        new_TestFixture(test_ipv6_nc_add__ENOMEM),
        new_TestFixture(test_ipv6_nc_add__success),
        new_TestFixture(test_ipv6_nc_remove__no_entry_pid),
        new_TestFixture(test_ipv6_nc_remove__no_entry_addr1),
        new_TestFixture(test_ipv6_nc_remove__no_entry_addr2),
        new_TestFixture(test_ipv6_nc_remove__success),
        new_TestFixture(test_ipv6_nc_get__empty),
        new_TestFixture(test_ipv6_nc_get__different_if),
        new_TestFixture(test_ipv6_nc_get__different_addr),
        new_TestFixture(test_ipv6_nc_get__success_if_local),
        new_TestFixture(test_ipv6_nc_get__success_if_global),
        new_TestFixture(test_ipv6_nc_get_reachable__incomplete_if_local),
        new_TestFixture(test_ipv6_nc_get_reachable__incomplete_if_global),
        new_TestFixture(test_ipv6_nc_get_reachable__reachable_if_local),
        new_TestFixture(test_ipv6_nc_get_reachable__reachable_if_global),
        new_TestFixture(test_ipv6_nc_still_reachable__incomplete),
        new_TestFixture(test_ipv6_nc_still_reachable__success),
    };

    EMB_UNIT_TESTCALLER(ipv6_nc_tests, set_up, NULL, fixtures);

    return (Test *)&ipv6_nc_tests;
}

void tests_ipv6_nc(void)
{
    TESTS_RUN(tests_ipv6_nc_tests());
}
/** @} */
