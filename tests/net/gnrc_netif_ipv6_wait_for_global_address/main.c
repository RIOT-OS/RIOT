/*
 * SPDX-FileCopyrightText: 2021 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests notification for global address
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "embUnit.h"
#include "embUnit/embUnit.h"
#include "net/ipv6.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/raw.h"
#include "net/netdev_test.h"
#include "ztimer.h"

#define TEST_NETIF_NUMOF    2
#define TEST_NETIF_PRIO     3

static netdev_test_t netdev_test[TEST_NETIF_NUMOF];
static gnrc_netif_t netif_test[TEST_NETIF_NUMOF];
static char netif_stack[TEST_NETIF_NUMOF][THREAD_STACKSIZE_DEFAULT];
static char adder_stack[THREAD_STACKSIZE_DEFAULT];

static gnrc_netif_t *_test_netif;
static const ipv6_addr_t _test_addr =  {{ 0x20, 0x01, 0x0d, 0xd8,
                                          0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00, 0x01 }};
static void tear_down(void)
{
    if (_test_netif == NULL) {
        return;
    }

    gnrc_netif_ipv6_addr_remove(_test_netif, &_test_addr);
    _test_netif = NULL;
}

static void *_adder_thread(void *netif)
{
    ztimer_sleep(ZTIMER_MSEC, 10);
    gnrc_netif_ipv6_addr_add(netif, &_test_addr, 64,
                             GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID);
    return NULL;
}

static void _add_delayed_addr(gnrc_netif_t *netif)
{
    _test_netif = netif;
    memset(adder_stack, 0, sizeof(adder_stack));
    thread_create(adder_stack, sizeof(adder_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  0,
                  _adder_thread, netif, "add_addr");
}

static void _assert_wait_blocks(gnrc_netif_t *add_netif,
                                gnrc_netif_t *wait_netif,
                                bool success)
{
    uint32_t now = ztimer_now(ZTIMER_MSEC);
    uint32_t timeout = 20;

    _add_delayed_addr(add_netif);
    TEST_ASSERT(gnrc_netif_ipv6_wait_for_global_address(wait_netif,
                                                        timeout) == success);
    if (success) {
        TEST_ASSERT((ztimer_now(ZTIMER_MSEC) - now) < timeout);
    } else {
        TEST_ASSERT((ztimer_now(ZTIMER_MSEC) - now) >= timeout);
    }
}

static void test_wait_timeout(void)
{
    TEST_ASSERT_EQUAL_INT(TEST_NETIF_NUMOF, gnrc_netif_numof());
    TEST_ASSERT(!gnrc_netif_ipv6_wait_for_global_address(NULL, 10));
    TEST_ASSERT(!gnrc_netif_ipv6_wait_for_global_address(&netif_test[0], 10));
}

static void test_wait_timeout_other_iface(void)
{
    TEST_ASSERT_EQUAL_INT(TEST_NETIF_NUMOF, gnrc_netif_numof());

    /* no event when adding addr to other interface */
    _assert_wait_blocks(&netif_test[1], &netif_test[0], false);
}

static void test_wait_success(void)
{
    /* event when adding addr to specified interface */
    _assert_wait_blocks(&netif_test[0], &netif_test[0], true);
}

static void test_wait_success_any_iface(void)
{
    /* event when adding addr to any interface */
    _assert_wait_blocks(&netif_test[0], NULL, true);
}

static Test *embunit_tests_gnrc_netif(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_wait_timeout),
        new_TestFixture(test_wait_timeout_other_iface),
        new_TestFixture(test_wait_success),
        new_TestFixture(test_wait_success_any_iface),
    };
    EMB_UNIT_TESTCALLER(tests, NULL, tear_down, fixtures);

    return (Test *)&tests;
}

static int netdev_get_device_type(netdev_t *dev, void *value, size_t max_len)
{
    (void)dev;
    (void)max_len;
    const uint16_t type = NETDEV_TYPE_SLIP;
    memcpy(value, &type, sizeof(type));
    return sizeof(uint16_t);
}

static void _setup_mock_netif(netdev_test_t *dev, gnrc_netif_t *netif,
                              void *stack, size_t stack_size, unsigned prio)
{
    netdev_test_setup(dev, NULL);
    netdev_test_set_get_cb(dev, NETOPT_DEVICE_TYPE, netdev_get_device_type);
    gnrc_netif_raw_create(netif, stack, stack_size, prio,
                          "netdev_test", &dev->netdev.netdev);
}

int main(void)
{
    for (unsigned i = 0; i < TEST_NETIF_NUMOF; ++i) {
        _setup_mock_netif(&netdev_test[i], &netif_test[i],
                          netif_stack[i], sizeof(netif_stack[i]),
                          TEST_NETIF_PRIO);
    }

    TESTS_START();
    TESTS_RUN(embunit_tests_gnrc_netif());
    TESTS_END();

    return 0;
}
