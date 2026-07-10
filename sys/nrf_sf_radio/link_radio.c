/*
 * SPDX-FileCopyrightText: 2026 Xin He
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_nrf_sf_radio
 * @{
 *
 * @file
 * @brief       Link layer radio TX/RX startup implementation
 *
 * @author      Xin He <xin.he@mailbox.tu-dresden.de>
 *
 * @}
 */

#include <string.h>
#include "nrf_sf_radio/link_radio.h"
#include "nrf_sf_radio/radio_driver.h"

#define NRF_SF_RADIO_TX_CHAIN_DELAY_US (10U)
#define NRF_SF_RADIO_ADV_ADDR_LO (0xABABABUL)
#define NRF_SF_RADIO_ADV_ADDR_HI (0xABABC0UL)

static void _encode_packet_header(uint8_t *dst, const uint8_t *payload,
                                  uint8_t length);

static void _encode_packet_header(uint8_t *dst, const uint8_t *payload,
                                  uint8_t length)
{
    dst[0] = 0x42;
    dst[1] = (uint8_t)(NRF_SF_RADIO_ADV_ADDR_LEN + length);
    dst[2] = (uint8_t)(NRF_SF_RADIO_ADV_ADDR_LO & 0xFFu);
    dst[3] = (uint8_t)((NRF_SF_RADIO_ADV_ADDR_LO >> 8) & 0xFFu);
    dst[4] = (uint8_t)((NRF_SF_RADIO_ADV_ADDR_LO >> 16) & 0xFFu);
    dst[5] = (uint8_t)(NRF_SF_RADIO_ADV_ADDR_HI & 0xFFu);
    dst[6] = (uint8_t)((NRF_SF_RADIO_ADV_ADDR_HI >> 8) & 0xFFu);
    dst[7] = (uint8_t)((NRF_SF_RADIO_ADV_ADDR_HI >> 16) & 0xFFu);
    memcpy(&dst[8], payload, length);
}

bool nrf_sf_radio_tx_start(uint8_t *payload, uint32_t txen_ticks,
                           uint32_t event_end_ticks, uint8_t payload_length)
{
    uint8_t dst[NRF_SF_RADIO_HDR_LEN + payload_length];

    _encode_packet_header(dst, payload, payload_length);
    nrf_sf_radio_tx_arm(dst, txen_ticks);

    nrf_sf_radio_wait_until_abs(&NRF_RADIO->EVENTS_ADDRESS,
                                txen_ticks +
                                NRF_SF_RADIO_RAMPUP_TIME_TICKS +
                                NRF_SF_RADIO_US_TO_TIMER_TICKS(40));
    if (NRF_RADIO->EVENTS_ADDRESS != 0) {
        nrf_sf_radio_wait_until_abs(&NRF_RADIO->EVENTS_END,
                                    event_end_ticks +
                                    NRF_SF_RADIO_US_TO_TIMER_TICKS(1));
        if (NRF_RADIO->EVENTS_END == 0) {
            return false;
        }
        return true;
    }

    return false;
}

uint8_t nrf_sf_radio_rx_start(uint8_t **rx_buffer, uint32_t rxen_ticks,
                           uint32_t rx_window_end_ticks,
                           uint32_t rx_end_ticks)
{
    uint8_t *rx_pointer = *rx_buffer;
    nrf_sf_radio_rx_arm(rx_pointer, rxen_ticks);

    nrf_sf_radio_wait_until_abs(&NRF_RADIO->EVENTS_ADDRESS,
                                rx_window_end_ticks);

    if (NRF_RADIO->EVENTS_ADDRESS != 0) {
        nrf_sf_radio_wait_until_abs(&NRF_RADIO->EVENTS_END,
                                    rx_end_ticks +
                                    NRF_SF_RADIO_US_TO_TIMER_TICKS(
                                        NRF_SF_RADIO_TX_CHAIN_DELAY_US));
        if ((NRF_RADIO->EVENTS_END != 0) && (NRF_RADIO->CRCSTATUS != 0)) {
            *rx_buffer = &rx_pointer[NRF_SF_RADIO_HDR_LEN];
            return 0;
        }
        else if (NRF_RADIO->EVENTS_END == 0){
            return 2;
        }
        else {
            return 3;
        }
    }
    else {
        NRF_RADIO->TASKS_DISABLE = 1;
        return 1;
    }

}

uint32_t nrf_sf_radio_rx_listen_until_packet(uint8_t *rx_buffer,
                                             uint32_t rx_end_ticks,
                                             uint32_t runtimeout_us)
{
    uint32_t rx_ticks;

    nrf_sf_radio_try_rx_enable(rx_buffer);

    nrf_sf_radio_wait_until(&NRF_RADIO->EVENTS_ADDRESS,
                            NRF_SF_RADIO_US_TO_TIMER_TICKS(runtimeout_us));
    if (NRF_RADIO->EVENTS_ADDRESS != 0) {
        rx_ticks = nrf_sf_radio_get_last_address_time_ticks();
        nrf_sf_radio_wait_until(&NRF_RADIO->EVENTS_END, rx_end_ticks);
        if ((NRF_RADIO->EVENTS_END != 0) && (NRF_RADIO->CRCSTATUS == 1)) {
            return rx_ticks -
                   NRF_SF_RADIO_US_TO_TIMER_TICKS(
                       NRF_SF_RADIO_TX_CHAIN_DELAY_US);
        }
    }

    return 0;
}
