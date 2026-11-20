/*
 * SPDX-FileCopyrightText: 2026 Xin He
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>

#include "nrf_sf_radio/link_radio.h"
#include "nrf_sf_radio/radio_driver.h"

#define TEST_BLE_CHANNEL       (20U)
#define TEST_TX_START_DELAY_US (500U)
#define TEST_TX_END_TIMEOUT_US (1000U)
#define TEST_REPORT_COUNT      (1000U)

int main(void)
{
    uint8_t tx_payload[] = "hello, world";
    uint32_t tx_count = 0U;

    nrf_sf_radio_start();
    nrf_sf_radio_set_ble_channel(TEST_BLE_CHANNEL);

    while (1) {
        uint32_t now = nrf_sf_radio_now_ticks();
        bool res;

        res = nrf_sf_radio_tx_start(
            tx_payload,
            now + NRF_SF_RADIO_US_TO_TIMER_TICKS(TEST_TX_START_DELAY_US),
            now + NRF_SF_RADIO_US_TO_TIMER_TICKS(TEST_TX_END_TIMEOUT_US),
            sizeof(tx_payload));
        if (!res) {
            puts("[FAILED] transmission failed");
            return 1;
        }

        tx_count++;
        if ((tx_count % TEST_REPORT_COUNT) == 0U) {
            puts("[OK] transmitter sending");
        }
    }

    return 0;
}
