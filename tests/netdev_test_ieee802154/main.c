/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include <string.h>

#include "embUnit.h"
#include "embUnit/embUnit.h"
#include "net/gnrc/nettype.h"

#include "net/netdev_test/ieee802154.h"

#define TEST_EUI64  { 0xB8, 0x8C, 0xCC, 0xBA, 0xEF, 0x9A, 0x67, 0x42 }

static const eui64_t _test_eui = { .uint8 = TEST_EUI64 };
static netdev_test_ieee802154_t _netdev_test;

/**
 * @brief   Checks if netdev_test_ieee802154_t::eui64 is initialized when
 *          `eui64` parameter is NULL
 */
static void test_setup_NULL(void)
{
    unsigned zeros = 0;
    memset(&_netdev_test.eui64, 0, sizeof(_netdev_test.eui64));
    netdev_test_ieee802154_setup(&_netdev_test, NULL, NULL);
    /* ! all(b == 0 for b in _netdev_test.eui64) */
    for (unsigned i = 0; i < sizeof(_netdev_test.eui64); i++) {
        if (_netdev_test.eui64.uint8[i] == 0) {
            zeros++;
        }
    }
    TEST_ASSERT(zeros < sizeof(_netdev_test.eui64));
}

/**
 * @brief   Checks if netdev_test_ieee802154_t::eui64 is initialized with
 *          the constant for the `eui64` parameter.
 */
static void test_setup_const(void)
{
    memset(&_netdev_test.eui64, 0, sizeof(_netdev_test.eui64));
    netdev_test_ieee802154_setup(&_netdev_test, NULL, &_test_eui);
    for (unsigned i = 0; i < sizeof(_netdev_test.eui64); i++) {
        TEST_ASSERT_EQUAL_INT(_test_eui.uint8[i], _netdev_test.eui64.uint8[i]);
    }
}

/**
 * @brief   Tests if `NETOPT_DEVICE_TYPE` returns as documented
 */
static void test_device_type(void)
{
    netdev_t *dev = (netdev_t *)&_netdev_test;
    int res = 0;
    uint16_t device_type = 0;

    res = dev->driver->get(dev, NETOPT_DEVICE_TYPE, &device_type,
                           sizeof(device_type));
    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t), res);
    TEST_ASSERT_EQUAL_INT(NETDEV_TYPE_IEEE802154, device_type);
}

/**
 * @brief   Tests if `NETOPT_MAX_PACKET_SIZE` returns as documented
 */
static void test_max_packet_size(void)
{
    netdev_t *dev = (netdev_t *)&_netdev_test;
    int res = 0;
    uint16_t max_packet_size = 0;

    res = dev->driver->get(dev, NETOPT_MAX_PACKET_SIZE, &max_packet_size,
                           sizeof(max_packet_size));
    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t), res);
    TEST_ASSERT_EQUAL_INT(102U, max_packet_size);
}

/**
 * @brief   Tests if `NETOPT_SRC_LEN` returns as documented
 */
static void test_src_len(void)
{
    netdev_t *dev = (netdev_t *)&_netdev_test;
    int res = 0;
    uint16_t src_len = 0;

    res = dev->driver->get(dev, NETOPT_SRC_LEN, &src_len, sizeof(src_len));
    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t), res);
    TEST_ASSERT_EQUAL_INT(IEEE802154_LONG_ADDRESS_LEN, src_len);
}

/**
 * @brief   Tests if `NETOPT_ADDRESS_LONG` returns as documented
 */
static void test_address_long(void)
{
    netdev_t *dev = (netdev_t *)&_netdev_test;
    int res = 0;
    eui64_t address_long = { .uint8 = { 0 } };

    res = dev->driver->get(dev, NETOPT_ADDRESS_LONG, &address_long,
                           sizeof(address_long));
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t), res);
    for (unsigned i = 0; i < sizeof(_netdev_test.eui64); i++) {
        TEST_ASSERT_EQUAL_INT(_test_eui.uint8[i], address_long.uint8[i]);
    }
}

/**
 * @brief   Tests if `NETOPT_PROTO` returns as documented
 */
static void test_proto(void)
{
    netdev_t *dev = (netdev_t *)&_netdev_test;
    int res = 0;
    gnrc_nettype_t proto = 0;

    res = dev->driver->get(dev, NETOPT_PROTO, &proto, sizeof(proto));
#if IS_USED(MODULE_GNRC_NETTYPE_SIXLOWPAN)
    TEST_ASSERT_EQUAL_INT(sizeof(gnrc_nettype_t), res);
    TEST_ASSERT_EQUAL_INT(GNRC_NETTYPE_SIXLOWPAN, proto);
#else
    TEST_ASSERT_EQUAL_INT(-ENOTSUP, res);
#endif
}

static Test *tests_setup(void) {
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_setup_NULL),
        new_TestFixture(test_setup_const),
    };

    EMB_UNIT_TESTCALLER(tests, NULL, NULL, fixtures);

    return (Test *)&tests;
}

static Test *tests_callbacks(void) {
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_device_type),
        new_TestFixture(test_max_packet_size),
        new_TestFixture(test_src_len),
        new_TestFixture(test_address_long),
        new_TestFixture(test_proto),
    };

    EMB_UNIT_TESTCALLER(tests, test_setup_const, NULL, fixtures);

    return (Test *)&tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_setup());
    TESTS_RUN(tests_callbacks());
    TESTS_END();
    return 0;
}

/** @} */
