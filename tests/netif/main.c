/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests common netif functions
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "common.h"
#include "embUnit.h"
#include "embUnit/embUnit.h"
#include "net/ethernet.h"
#include "net/ipv6.h"
#include "net/netif.h"
#include "test_utils/expect.h"

static void test_netif_addr_to_str(void)
{
    static const uint8_t ethernet_l2addr[] = ETHERNET_SRC;
    static const uint8_t ieee802154_l2addr_long[] = IEEE802154_LONG_SRC;
    static const uint8_t ieee802154_l2addr_short[] = IEEE802154_SHORT_SRC;
    static const uint8_t netif0_l2addr[] = NETIF0_SRC;
    char out[sizeof(netif0_l2addr) * 3];

    TEST_ASSERT(out == netif_addr_to_str(NULL, 0, out));
    TEST_ASSERT_EQUAL_STRING("", &out[0]);
    TEST_ASSERT(out == netif_addr_to_str(ethernet_l2addr,
                                              sizeof(ethernet_l2addr), out));
    TEST_ASSERT_EQUAL_STRING("3E:E6:B5:22:FD:0A", &out[0]);
    TEST_ASSERT(out == netif_addr_to_str(ieee802154_l2addr_long,
                                              sizeof(ieee802154_l2addr_long),
                                              out));
    TEST_ASSERT_EQUAL_STRING("3E:E6:B5:0F:19:22:FD:0A", &out[0]);
    TEST_ASSERT(out == netif_addr_to_str(ieee802154_l2addr_short,
                                              sizeof(ieee802154_l2addr_short),
                                              out));
    TEST_ASSERT_EQUAL_STRING("FD:0A", &out[0]);
    TEST_ASSERT(out == netif_addr_to_str(netif0_l2addr,
                                              sizeof(netif0_l2addr),
                                              out));
    TEST_ASSERT_EQUAL_STRING("3E:E7:B5:0F:19:22:FD:0A", &out[0]);
}

static void test_netif_addr_from_str(void)
{
    static const uint8_t ethernet_l2addr[] = ETHERNET_SRC;
    static const uint8_t ieee802154_l2addr_long[] = IEEE802154_LONG_SRC;
    static const uint8_t ieee802154_l2addr_short[] = IEEE802154_SHORT_SRC;
    uint8_t out[GNRC_NETIF_L2ADDR_MAXLEN];

    TEST_ASSERT_EQUAL_INT(0, netif_addr_from_str("", out));
    TEST_ASSERT_EQUAL_INT(sizeof(ethernet_l2addr),
                          netif_addr_from_str("3E:E6:B5:22:FD:0A", out));
    TEST_ASSERT_EQUAL_INT(0, memcmp(ethernet_l2addr, out,
                                    sizeof(ethernet_l2addr)));
    TEST_ASSERT_EQUAL_INT(sizeof(ieee802154_l2addr_long),
                          netif_addr_from_str("3E:E6:B5:0F:19:22:FD:0A",
                                                   out));
    TEST_ASSERT_EQUAL_INT(0, memcmp(ieee802154_l2addr_long, out,
                                    sizeof(ieee802154_l2addr_long)));
    TEST_ASSERT_EQUAL_INT(sizeof(ieee802154_l2addr_short),
                          netif_addr_from_str("FD:0A", out));
    TEST_ASSERT_EQUAL_INT(0, memcmp(ieee802154_l2addr_short, out,
                                    sizeof(ieee802154_l2addr_short)));
}

static Test *embunit_tests_netif(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_netif_addr_to_str),
        new_TestFixture(test_netif_addr_from_str),
        /* only add tests not involving output here */
    };
    EMB_UNIT_TESTCALLER(tests, NULL, NULL, fixtures);

    return (Test *)&tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(embunit_tests_netif());
    TESTS_END();

    return 0;
}
