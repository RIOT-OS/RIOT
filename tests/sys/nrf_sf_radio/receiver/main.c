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
    uint8_t rx_buffer[50] = {0};
    uint32_t res;
    while (1) {   
        uint8_t *rx_payload = rx_buffer;
        res = nrf_sf_radio_rx_listen_until_packet(&rx_payload, 
            NRF_SF_RADIO_US_TO_TIMER_TICKS(500), 1000);
        if (res) {
            printf("%s\n", rx_payload);
            return 0;
        }
    }

    return 0;
}