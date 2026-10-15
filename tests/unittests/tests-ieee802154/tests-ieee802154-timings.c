/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdint.h>

#include "embUnit.h"

#include "net/ieee802154/radio.h"
#include "net/ieee802154_timings.h"

#include "tests-ieee802154.h"

/* all expected values in microseconds */
static void _check_timings(const ieee802154_phy_conf_t *conf,
                           uint32_t symbol, uint32_t turnaround, uint32_t cca,
                           uint32_t unit_backoff, uint32_t ack_wait)
{
    TEST_ASSERT_EQUAL_INT(symbol, ieee802154_get_symbol_duration(conf));
    TEST_ASSERT_EQUAL_INT(turnaround, ieee802154_get_turnaround_time(conf));
    TEST_ASSERT_EQUAL_INT(cca, ieee802154_get_cca_time(conf));
    TEST_ASSERT_EQUAL_INT(unit_backoff, ieee802154_calculate_unit_backoff_period(conf));
    TEST_ASSERT_EQUAL_INT(ack_wait, ieee802154_calculate_ack_wait_duration(conf));
}

static void test_ieee802154_timings_oqpsk_2450(void)
{
    ieee802154_phy_conf_t conf = { .phy_mode = IEEE802154_PHY_OQPSK, .channel = 11 };

    _check_timings(&conf, 16, 192, 128, 320, 864);
}

static void test_ieee802154_timings_oqpsk_915(void)
{
    ieee802154_phy_conf_t conf = { .phy_mode = IEEE802154_PHY_OQPSK, .channel = 1 };

    _check_timings(&conf, 16, 192, 128, 320, 864);
}

static void test_ieee802154_timings_oqpsk_868(void)
{
    ieee802154_phy_conf_t conf = { .phy_mode = IEEE802154_PHY_OQPSK, .channel = 0 };

    _check_timings(&conf, 40, 480, 320, 800, 2160);
}

static void test_ieee802154_timings_bpsk_915(void)
{
    ieee802154_phy_conf_t conf = { .phy_mode = IEEE802154_PHY_BPSK, .channel = 1 };

    _check_timings(&conf, 25, 300, 200, 500, 3000);
}

static void test_ieee802154_timings_bpsk_868(void)
{
    ieee802154_phy_conf_t conf = { .phy_mode = IEEE802154_PHY_BPSK, .channel = 0 };

    _check_timings(&conf, 50, 600, 400, 1000, 6000);
}

Test *tests_ieee802154_timings_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ieee802154_timings_oqpsk_2450),
        new_TestFixture(test_ieee802154_timings_oqpsk_915),
        new_TestFixture(test_ieee802154_timings_oqpsk_868),
        new_TestFixture(test_ieee802154_timings_bpsk_915),
        new_TestFixture(test_ieee802154_timings_bpsk_868),
    };

    EMB_UNIT_TESTCALLER(ieee802154_timings_tests, NULL, NULL, fixtures);

    return (Test *)&ieee802154_timings_tests;
}
