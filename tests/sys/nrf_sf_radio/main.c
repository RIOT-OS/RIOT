/*
 * SPDX-FileCopyrightText: 2026 Xin He
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Single-board test for the nrf_sf_radio module
 *
 * @author      Xin He <xin.he@mailbox.tu-dresden.de>
 *
 * @}
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_sf_radio/link_radio.h"
#include "nrf_sf_radio/radio_driver.h"

#define TEST_WAIT_US          (100U)
#define TEST_TX_DELAY_US      (500U)
#define TEST_TX_DEADLINE_US   (2000U)
#define TEST_RX_DELAY_US      (200U)
#define TEST_RX_WINDOW_US     (1000U)
#define TEST_RX_DEADLINE_US   (1500U)
#define TEST_RX_BUFFER_SIZE   (253U)

typedef struct {
    uint8_t channel;
    uint32_t frequency;
} channel_test_t;

typedef struct {
    int16_t requested;
    uint32_t expected;
} power_test_t;

static unsigned failures;

static void _check(bool condition, const char *name)
{
    if (condition) {
        printf("[OK] %s\n", name);
    }
    else {
        printf("[FAILED] %s\n", name);
        failures++;
    }
}

static bool _time_not_before(uint32_t lhs, uint32_t rhs)
{
    return (int32_t)(lhs - rhs) >= 0;
}

static bool _whitening_iv_matches(uint8_t channel)
{
    uint32_t expected = (1U << 6U) | channel;

    return (NRF_RADIO->DATAWHITEIV &
            RADIO_DATAWHITEIV_DATAWHITEIV_Msk) == expected;
}

static void _test_constants(void)
{
    _check(NRF_SF_RADIO_US_TO_TIMER_TICKS(123U) == 1968U,
           "microseconds to timer ticks");
    _check(NRF_SF_RADIO_TIMER_TICKS_TO_US(1968U) == 123U,
           "timer ticks to microseconds");
}

static void _test_initialization(void)
{
    uint32_t rampup_mode =
        (NRF_RADIO->MODECNF0 & RADIO_MODECNF0_RU_Msk) >>
        RADIO_MODECNF0_RU_Pos;

    _check(NRF_RADIO->POWER == 1U, "radio power");
    _check(NRF_RADIO->MODE == RADIO_MODE_MODE_Ble_1Mbit,
           "default radio mode");
    _check(NRF_RADIO->FREQUENCY == 2U, "default channel frequency");
    _check(_whitening_iv_matches(37U), "default whitening IV");
    _check(NRF_RADIO->TXPOWER == RADIO_TXPOWER_TXPOWER_0dBm,
           "default TX power");
#if NRF_SF_RADIO_FAST_RAMPUP
    _check(rampup_mode == RADIO_MODECNF0_RU_Fast, "fast ramp-up mode");
#else
    _check(rampup_mode == RADIO_MODECNF0_RU_Default,
           "default ramp-up mode");
#endif
}

static void _test_mode(void)
{
    nrf_sf_radio_set_mode(RADIO_MODE_MODE_Ble_2Mbit);
    _check(NRF_RADIO->MODE == RADIO_MODE_MODE_Ble_2Mbit,
           "set radio mode");
    nrf_sf_radio_set_mode(RADIO_MODE_MODE_Ble_1Mbit);
}

static void _test_channels(void)
{
    static const channel_test_t tests[] = {
        { 0U, 4U },
        { 20U, 46U },
        { 37U, 2U },
        { 39U, 80U },
    };
    bool passed = true;

    for (size_t i = 0; i < (sizeof(tests) / sizeof(tests[0])); i++) {
        nrf_sf_radio_set_ble_channel(tests[i].channel);
        if ((NRF_RADIO->FREQUENCY != tests[i].frequency) ||
            !_whitening_iv_matches(tests[i].channel)) {
            passed = false;
        }
    }

    _check(passed, "set BLE channel");
    nrf_sf_radio_set_ble_channel(37U);
}

static void _test_power(void)
{
    static const power_test_t tests[] = {
        { 3, RADIO_TXPOWER_TXPOWER_Pos4dBm },
        { 2, RADIO_TXPOWER_TXPOWER_0dBm },
        { -2, RADIO_TXPOWER_TXPOWER_Neg4dBm },
        { -6, RADIO_TXPOWER_TXPOWER_Neg8dBm },
        { -10, RADIO_TXPOWER_TXPOWER_Neg12dBm },
        { -14, RADIO_TXPOWER_TXPOWER_Neg16dBm },
        { -18, RADIO_TXPOWER_TXPOWER_Neg20dBm },
        { -25, RADIO_TXPOWER_TXPOWER_Neg30dBm },
    };
    bool passed = true;

    for (size_t i = 0; i < (sizeof(tests) / sizeof(tests[0])); i++) {
        nrf_sf_radio_set_power(tests[i].requested);
        if (NRF_RADIO->TXPOWER != tests[i].expected) {
            passed = false;
        }
    }

    _check(passed, "set and quantize TX power");
    nrf_sf_radio_set_power(0);
}

static void _test_timer(void)
{
    uint32_t timeout_ticks =
        NRF_SF_RADIO_US_TO_TIMER_TICKS(TEST_WAIT_US);
    uint32_t start_ticks = nrf_sf_radio_now_ticks();

    nrf_sf_radio_wait_until(NULL, timeout_ticks);

    uint32_t elapsed_ticks = nrf_sf_radio_now_ticks() - start_ticks;

    _check(elapsed_ticks >= timeout_ticks, "relative timer wait");
}

static void _test_transmission(void)
{
    uint8_t payload[] = "test";
    uint32_t now_ticks = nrf_sf_radio_now_ticks();
    uint32_t txen_ticks =
        now_ticks + NRF_SF_RADIO_US_TO_TIMER_TICKS(TEST_TX_DELAY_US);
    uint32_t end_deadline_ticks =
        now_ticks + NRF_SF_RADIO_US_TO_TIMER_TICKS(TEST_TX_DEADLINE_US);
    bool sent = nrf_sf_radio_tx_start(payload, txen_ticks,
                                      end_deadline_ticks,
                                      sizeof(payload));

    _check(sent, "scheduled transmission");
    if (sent) {
        uint32_t ready_ticks = nrf_sf_radio_get_last_ready_time_ticks();
        uint32_t address_ticks = nrf_sf_radio_get_last_address_time_ticks();
        uint32_t end_ticks = nrf_sf_radio_get_last_end_time_ticks();

        _check((ready_ticks != 0U) &&
               _time_not_before(address_ticks, ready_ticks) &&
               _time_not_before(end_ticks, address_ticks),
               "READY, ADDRESS and END timestamps");
    }
}

static void _test_scheduled_rx_timeout(void)
{
    uint8_t packet[TEST_RX_BUFFER_SIZE] = { 0 };
    uint8_t *payload = packet;
    uint32_t now_ticks = nrf_sf_radio_now_ticks();
    uint32_t rxen_ticks =
        now_ticks + NRF_SF_RADIO_US_TO_TIMER_TICKS(TEST_RX_DELAY_US);
    uint32_t address_deadline_ticks =
        now_ticks + NRF_SF_RADIO_US_TO_TIMER_TICKS(TEST_RX_WINDOW_US);
    uint32_t end_deadline_ticks =
        now_ticks + NRF_SF_RADIO_US_TO_TIMER_TICKS(TEST_RX_DEADLINE_US);
    uint8_t status = nrf_sf_radio_rx_start(&payload, rxen_ticks,
                                           address_deadline_ticks,
                                           end_deadline_ticks);

    _check(status == 1U, "scheduled RX ADDRESS timeout");
    _check(payload == packet, "RX timeout preserves buffer pointer");
}

int main(void)
{
    puts("nrf_sf_radio single-board test");

    _test_constants();

    nrf_sf_radio_start();

    _test_initialization();
    _test_mode();
    _test_channels();
    _test_power();
    _test_timer();
    _test_transmission();
    _test_scheduled_rx_timeout();

    if (failures != 0U) {
        printf("[FAILED] %u checks failed\n", failures);
        return 1;
    }

    puts("[SUCCESS]");
    return 0;
}
