/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
  * @ingroup    tests
  * @{
  *
  * @file
  * @brief      Unit tests for l2util
  *
  * @author     Martine Lenders <m.lenders@fu-berlin.de>
  */

#include <stdint.h>

#include "embUnit.h"
#include "net/eui48.h"
#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev.h"

#include "net/l2util.h"

#define TEST_ADDR           { 0x21, 0x55, 0x31, 0x02, 0x41, 0xfd, 0xfb, 0xfd }
#define TEST_802154_S_IID   { 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x21, 0x55 }
#define TEST_CC110X_IID     { 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x21 }
#define TEST_EUI48_EUI64    { 0x21, 0x55, 0x31, 0xff, 0xfe, 0x02, 0x41, 0xfd }
#define TEST_EUI48_IID      { 0x23, 0x55, 0x31, 0xff, 0xfe, 0x02, 0x41, 0xfd }
#define TEST_EUI64_IID      { 0x23, 0x55, 0x31, 0x02, 0x41, 0xfd, 0xfb, 0xfd }
#define TEST_IPV6_GROUP     { 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                              0x3f, 0x6c, 0xa1, 0xbb, 0xe5, 0x03, 0x6b, 0xe2 }
/* see https://tools.ietf.org/html/rfc2464#section-7 */
#define TEST_ETHERNET_GROUP { 0x33, 0x33, 0xe5, 0x03, 0x6b, 0xe2 }

static void test_eui64_from_addr__success(void)
{
    static const uint8_t test_addr[L2UTIL_ADDR_MAX_LEN] = TEST_ADDR;
    static const eui64_t test_802154_s = { .uint8 = TEST_802154_S_IID };
    static const eui64_t test_cc110x = { .uint8 = TEST_CC110X_IID };
    static const eui64_t test_eui48 = { .uint8 = TEST_EUI48_EUI64 };
    static const eui64_t test_eui64 = { .uint8 = TEST_ADDR };
    eui64_t res;

    /* test Ethernet */
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_eui64_from_addr(NETDEV_TYPE_ETHERNET,
                                                 test_addr, sizeof(eui48_t),
                                                 &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_eui48, &res, sizeof(eui64_t)));
    /* test IEEE 802.15.4 */
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_eui64_from_addr(NETDEV_TYPE_IEEE802154,
                                                 test_addr, sizeof(eui64_t),
                                                 &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_eui64, &res, sizeof(eui64_t)));
    /* test BLE */
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_eui64_from_addr(NETDEV_TYPE_BLE,
                                                 test_addr, sizeof(eui48_t),
                                                 &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_eui48, &res, sizeof(eui64_t)));
    /* test cc110x */
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_eui64_from_addr(NETDEV_TYPE_CC110X,
                                                 test_addr, sizeof(uint8_t),
                                                 &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_cc110x, &res, sizeof(eui64_t)));
    /* test NRFMIN */
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_eui64_from_addr(NETDEV_TYPE_NRFMIN,
                                                 test_addr, sizeof(uint16_t),
                                                 &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_802154_s, &res, sizeof(eui64_t)));
    /* test ESP-Now */
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_eui64_from_addr(NETDEV_TYPE_ESP_NOW,
                                                 test_addr, sizeof(eui48_t),
                                                 &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_eui48, &res, sizeof(eui64_t)));
}

static void test_eui64_from_addr__EINVAL(void)
{
    static const uint8_t test_addr[L2UTIL_ADDR_MAX_LEN] = TEST_ADDR;
    eui64_t res = { .uint8 = { 0 } };

    /* test Ethernet */
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_eui64_from_addr(NETDEV_TYPE_ETHERNET,
                                                 test_addr, sizeof(eui64_t),
                                                 &res));
    /* test IEEE 802.15.4 */
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_eui64_from_addr(NETDEV_TYPE_IEEE802154,
                                                 test_addr, sizeof(uint16_t),
                                                 &res));
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_eui64_from_addr(NETDEV_TYPE_IEEE802154,
                                                 test_addr, sizeof(eui48_t),
                                                 &res));
    /* test BLE */
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_eui64_from_addr(NETDEV_TYPE_BLE,
                                                 test_addr, sizeof(uint16_t),
                                                 &res));
    /* test cc110x */
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_eui64_from_addr(NETDEV_TYPE_CC110X,
                                                 test_addr, sizeof(uint64_t),
                                                 &res));
    /* test NRFMIN */
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_eui64_from_addr(NETDEV_TYPE_NRFMIN,
                                                 test_addr, sizeof(uint64_t),
                                                 &res));
    /* test ESP-Now */
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_eui64_from_addr(NETDEV_TYPE_ESP_NOW,
                                                 test_addr, 0,
                                                 &res));
}

static void test_eui64_from_addr__ENOTSUP(void)
{
    static const uint8_t test_addr[L2UTIL_ADDR_MAX_LEN] = TEST_ADDR;
    eui64_t res = { .uint8 = { 0 } };

    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          l2util_eui64_from_addr(NETDEV_TYPE_UNKNOWN,
                                                 test_addr, 0,
                                                 &res));
}

static void test_iid_from_addr__success(void)
{
    static const uint8_t test_addr[L2UTIL_ADDR_MAX_LEN] = TEST_ADDR;
    static const eui64_t test_802154_s = { .uint8 = TEST_802154_S_IID };
    static const eui64_t test_cc110x = { .uint8 = TEST_CC110X_IID };
    static const eui64_t test_eui48 = { .uint8 = TEST_EUI48_IID };
    static const eui64_t test_eui64 = { .uint8 = TEST_EUI64_IID };
    static const eui64_t test_ble = { .uint8 = TEST_EUI48_EUI64 };
    eui64_t res;

    /* test Ethernet */
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_ETHERNET,
                                                    test_addr, sizeof(eui48_t),
                                                    &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_eui48, &res, sizeof(eui64_t)));
    /* test IEEE 802.15.4 */
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_IEEE802154,
                                                    test_addr, sizeof(eui64_t),
                                                    &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_eui64, &res, sizeof(eui64_t)));
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_IEEE802154,
                                                    test_addr,
                                                    IEEE802154_SHORT_ADDRESS_LEN,
                                                    &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_802154_s, &res, sizeof(eui64_t)));
    /* test BLE */
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_BLE,
                                                    test_addr, sizeof(eui48_t),
                                                    &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_ble, &res, sizeof(eui64_t)));
    /* test cc110x */
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_CC110X,
                                                    test_addr, sizeof(uint8_t),
                                                    &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_cc110x, &res, sizeof(eui64_t)));
    /* test NRFMIN */
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_NRFMIN,
                                                    test_addr, sizeof(uint16_t),
                                                    &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_802154_s, &res, sizeof(eui64_t)));
    /* test ESP-Now */
    res.uint64.u64 = 0;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_ESP_NOW,
                                                    test_addr, sizeof(eui48_t),
                                                    &res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_eui48, &res, sizeof(eui64_t)));
}

static void test_iid_from_addr__EINVAL(void)
{
    static const uint8_t test_addr[L2UTIL_ADDR_MAX_LEN] = TEST_ADDR;
    eui64_t res = { .uint8 = { 0 } };

    /* test Ethernet */
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_ETHERNET,
                                                    test_addr, sizeof(eui64_t),
                                                    &res));
    /* test IEEE 802.15.4 */
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_IEEE802154,
                                                    test_addr, sizeof(eui48_t),
                                                    &res));
    /* test BLE */
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_BLE,
                                                    test_addr, sizeof(uint16_t),
                                                    &res));
    /* test cc110x */
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_CC110X,
                                                    test_addr, sizeof(uint64_t),
                                                    &res));
    /* test NRFMIN */
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_NRFMIN,
                                                    test_addr, sizeof(uint64_t),
                                                    &res));
    /* test ESP-Now */
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_ESP_NOW,
                                                    test_addr, 0,
                                                    &res));
}

static void test_iid_from_addr__ENOTSUP(void)
{
    static const uint8_t test_addr[L2UTIL_ADDR_MAX_LEN] = TEST_ADDR;
    eui64_t res = { .uint8 = { 0 } };

    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          l2util_ipv6_iid_from_addr(NETDEV_TYPE_UNKNOWN,
                                                    test_addr, 0,
                                                    &res));
}

static void test_iid_to_addr__success(void)
{
    static const uint8_t test_addr[L2UTIL_ADDR_MAX_LEN] = TEST_ADDR;
    static const eui64_t test_802154_s = { .uint8 = TEST_802154_S_IID };
    static const eui64_t test_cc110x = { .uint8 = TEST_CC110X_IID };
    static const eui64_t test_eui48 = { .uint8 = TEST_EUI48_IID };
    static const eui64_t test_eui64 = { .uint8 = TEST_EUI64_IID };
    static const eui64_t test_ble = { .uint8 = TEST_EUI48_EUI64 };
    uint8_t res[L2UTIL_ADDR_MAX_LEN];

    /* test Ethernet */
    memset(res, 0, sizeof(res));
    TEST_ASSERT_EQUAL_INT(sizeof(eui48_t),
                          l2util_ipv6_iid_to_addr(NETDEV_TYPE_ETHERNET,
                                                  &test_eui48, res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(test_addr, res, sizeof(eui48_t)));
    /* test IEEE 802.15.4 */
    memset(res, 0, sizeof(res));
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_ipv6_iid_to_addr(NETDEV_TYPE_IEEE802154,
                                                  &test_eui64, res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(test_addr, res, sizeof(eui64_t)));
    /* test BLE */
    memset(res, 0, sizeof(res));
    TEST_ASSERT_EQUAL_INT(sizeof(eui48_t),
                          l2util_ipv6_iid_to_addr(NETDEV_TYPE_BLE,
                                                  &test_ble, res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(test_addr, res, sizeof(eui48_t)));
    /* test cc110x */
    memset(res, 0, sizeof(res));
    TEST_ASSERT_EQUAL_INT(sizeof(uint8_t),
                          l2util_ipv6_iid_to_addr(NETDEV_TYPE_CC110X,
                                                  &test_cc110x, res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(test_addr, res, sizeof(uint8_t)));
    /* test NRFMIN */
    memset(res, 0, sizeof(res));
    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t),
                          l2util_ipv6_iid_to_addr(NETDEV_TYPE_NRFMIN,
                                                  &test_802154_s, res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(test_addr, res, sizeof(uint16_t)));
    /* test ESP-Now */
    memset(res, 0, sizeof(res));
    TEST_ASSERT_EQUAL_INT(sizeof(eui48_t),
                          l2util_ipv6_iid_to_addr(NETDEV_TYPE_ESP_NOW,
                                                  &test_eui48, res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(test_addr, res, sizeof(eui48_t)));
}

static void test_iid_to_addr__ENOTSUP(void)
{
    static const eui64_t test_eui48 = { .uint8 = TEST_EUI48_IID };
    uint8_t res[L2UTIL_ADDR_MAX_LEN];

    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          l2util_ipv6_iid_to_addr(NETDEV_TYPE_UNKNOWN,
                                                  &test_eui48, res));
}

static void test_addr_len_from_l2ao__success(void)
{
    ndp_opt_t opt = { .type = NDP_OPT_SL2A };

    /* test Ethernet */
    opt.len = 1;
    TEST_ASSERT_EQUAL_INT(sizeof(eui48_t),
                          l2util_ndp_addr_len_from_l2ao(NETDEV_TYPE_ETHERNET,
                                                        &opt));
    /* test IEEE 802.15.4 */
    opt.len = 1;
    TEST_ASSERT_EQUAL_INT(IEEE802154_SHORT_ADDRESS_LEN,
                          l2util_ndp_addr_len_from_l2ao(NETDEV_TYPE_IEEE802154,
                                                        &opt));
    opt.len = 2;
    TEST_ASSERT_EQUAL_INT(sizeof(eui64_t),
                          l2util_ndp_addr_len_from_l2ao(NETDEV_TYPE_IEEE802154,
                                                        &opt));
    /* test BLE */
    opt.len = 1;
    TEST_ASSERT_EQUAL_INT(sizeof(eui48_t),
                          l2util_ndp_addr_len_from_l2ao(NETDEV_TYPE_BLE,
                                                        &opt));
    /* test cc110x */
    opt.len = 1;
    TEST_ASSERT_EQUAL_INT(sizeof(uint8_t),
                          l2util_ndp_addr_len_from_l2ao(NETDEV_TYPE_CC110X,
                                                        &opt));
    /* test NRFMIN */
    opt.len = 1;
    TEST_ASSERT_EQUAL_INT(sizeof(uint16_t),
                          l2util_ndp_addr_len_from_l2ao(NETDEV_TYPE_NRFMIN,
                                                        &opt));
    /* test ESP-Now */
    opt.len = 1;
    TEST_ASSERT_EQUAL_INT(sizeof(eui48_t),
                          l2util_ndp_addr_len_from_l2ao(NETDEV_TYPE_ESP_NOW,
                                                        &opt));
}

static void test_addr_len_from_l2ao__EINVAL(void)
{
    ndp_opt_t opt = { .type = NDP_OPT_SL2A };

    /* test Ethernet */
    opt.len = 0;
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_ndp_addr_len_from_l2ao(NETDEV_TYPE_ETHERNET,
                                                        &opt));
    /* test IEEE 802.15.4 */
    opt.len = 0;
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_ndp_addr_len_from_l2ao(NETDEV_TYPE_IEEE802154,
                                                        &opt));
    /* test BLE */
    opt.len = 0;
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_ndp_addr_len_from_l2ao(NETDEV_TYPE_BLE,
                                                        &opt));
    /* test ESP-Now */
    opt.len = 0;
    TEST_ASSERT_EQUAL_INT(-EINVAL,
                          l2util_ndp_addr_len_from_l2ao(NETDEV_TYPE_ESP_NOW,
                                                        &opt));
}

static void test_addr_len_from_l2ao__ENOTSUP(void)
{
    ndp_opt_t opt = { .type = NDP_OPT_SL2A };

    opt.len = 1;
    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          l2util_ndp_addr_len_from_l2ao(NETDEV_TYPE_UNKNOWN,
                                                        &opt));
}

static void test_ipv6_group_to_l2group__success(void)
{
    static const ipv6_addr_t test_group = {
        .u8 = TEST_IPV6_GROUP,
    };
    static const eui48_t test_ethernet = {
        .uint8 = TEST_ETHERNET_GROUP,
    };
    uint8_t res[L2UTIL_ADDR_MAX_LEN];

    /* test Ethernet */
    memset(res, 0, sizeof(res));
    TEST_ASSERT_EQUAL_INT(sizeof(test_ethernet),
                          l2util_ipv6_group_to_l2_group(NETDEV_TYPE_ETHERNET,
                                                        &test_group, res));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&test_ethernet, res,
                                    sizeof(test_ethernet)));
}

static void test_ipv6_group_to_l2group__ENOTSUP(void)
{
    static const ipv6_addr_t test_group = {
        .u8 = TEST_IPV6_GROUP,
    };
    uint8_t res[L2UTIL_ADDR_MAX_LEN];

    TEST_ASSERT_EQUAL_INT(-ENOTSUP,
                          l2util_ipv6_group_to_l2_group(NETDEV_TYPE_UNKNOWN,
                                                        &test_group, res));
}

TestRef test_l2util(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_eui64_from_addr__success),
        new_TestFixture(test_eui64_from_addr__EINVAL),
        new_TestFixture(test_eui64_from_addr__ENOTSUP),
        new_TestFixture(test_iid_from_addr__success),
        new_TestFixture(test_iid_from_addr__EINVAL),
        new_TestFixture(test_iid_from_addr__ENOTSUP),
        new_TestFixture(test_iid_to_addr__success),
        new_TestFixture(test_iid_to_addr__ENOTSUP),
        new_TestFixture(test_addr_len_from_l2ao__success),
        new_TestFixture(test_addr_len_from_l2ao__EINVAL),
        new_TestFixture(test_addr_len_from_l2ao__ENOTSUP),
        new_TestFixture(test_ipv6_group_to_l2group__success),
        new_TestFixture(test_ipv6_group_to_l2group__ENOTSUP),
    };

    EMB_UNIT_TESTCALLER(tests_l2util, NULL, NULL, fixtures);
    return (TestRef) & tests_l2util;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(test_l2util());
    TESTS_END();
}
