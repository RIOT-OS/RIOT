/*
 * SPDX-FileCopyrightText: 2026 Xin He
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>
#include <string.h>

#include "nrf_sf_radio/link_radio.h"
#include "nrf_sf_radio/radio_driver.h"

#define TEST_BLE_CHANNEL        (20U)
#define TEST_RX_BUFFER_SIZE     (253U)
#define TEST_RX_END_TIMEOUT_US  (500U)
#define TEST_RX_SCAN_TIMEOUT_US (1000U)
#define TEST_REPORT_COUNT       (1000U)

int main(void)
{
    static const uint8_t expected_payload[] = "hello, world";
    uint8_t rx_buffer[TEST_RX_BUFFER_SIZE] = { 0 };
    uint32_t rx_count = 0U;

    nrf_sf_radio_start();
    nrf_sf_radio_set_ble_channel(TEST_BLE_CHANNEL);

    while (1) {
        uint8_t *rx_payload = rx_buffer;
        uint32_t rx_ticks;

        rx_ticks = nrf_sf_radio_rx_listen_until_packet(
            &rx_payload,
            NRF_SF_RADIO_US_TO_TIMER_TICKS(TEST_RX_END_TIMEOUT_US),
            TEST_RX_SCAN_TIMEOUT_US);
        if (rx_ticks == 0) {
            continue;
        }
        if (memcmp(rx_payload, expected_payload,
                   sizeof(expected_payload)) != 0) {
            puts("[FAILED] unexpected payload");
            return 1;
        }

        rx_count++;
        if ((rx_count % TEST_REPORT_COUNT) == 0U) {
            puts("[OK] received expected payload");
            puts("[SUCCESS]");
        }
    }

    return 0;
}
