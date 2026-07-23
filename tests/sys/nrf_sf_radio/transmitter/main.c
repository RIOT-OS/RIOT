/*
 * SPDX-FileCopyrightText: 2026 Xin He
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#include <stdio.h>
#include "nrf_sf_radio/link_radio.h"
#include "nrf_sf_radio/radio_driver.h"

int main(void)
{
    nrf_sf_radio_start();
    uint8_t tx_payload[13] = "hello, world";
    bool res;
    while (1) {
        uint32_t now = nrf_sf_radio_now_ticks();

        res = nrf_sf_radio_tx_start(tx_payload, now+NRF_SF_RADIO_US_TO_TIMER_TICKS(100),
        now + NRF_SF_RADIO_US_TO_TIMER_TICKS(600), sizeof(tx_payload));
        if (res) {
            printf("TX SUCCESSFUL\n");
        }
        else {
            printf("TX FAILED\n");
        }
    }


    return 0;
}