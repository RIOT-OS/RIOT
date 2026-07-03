/*
 * Copyright (C) 2026 Xin He
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_nrf_sf_radio
 * @{
 *
 * @file        link_radio.h
 * @brief       Link layer radio TX/RX startup interface
 *
 * @author      Xin He <xin.he@mailbox.tu-dresden.de>
 *
 * @}
 */

#ifndef NRF_SF_RADIO_LINK_RADIO_H
#define NRF_SF_RADIO_LINK_RADIO_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name BLE-like packet header field lengths
 * @{
 */

/**
 * @brief   Length of the S0 field in bytes.
 */
#define NRF_SF_RADIO_S0_LEN          (1U)

/**
 * @brief   Length of the LENGTH field in bytes.
 */
#define NRF_SF_RADIO_LENGTH_LEN      (1U)

/**
 * @brief   Length of the advertising address field in bytes.
 */
#define NRF_SF_RADIO_ADV_ADDR_LEN    (6U)

/**
 * @brief   Total length of the link-layer packet header in bytes.
 */
#define NRF_SF_RADIO_HDR_LEN         (NRF_SF_RADIO_S0_LEN + \
                                      NRF_SF_RADIO_LENGTH_LEN + \
                                      NRF_SF_RADIO_ADV_ADDR_LEN)

/** @} */

/**
 * @brief   Remove the link-layer header from the received packet
 *
 * @param[in] packet   Received radio packet
 *
 * @return  Pointer to the payload after the link-layer header
 */
const uint8_t *nrf_sf_radio_payload(const uint8_t *packet);

/**
 * @brief   Send one payload at a scheduled radio start time
 *
 * @param[in] payload          Payload to be transmitted
 * @param[in] txen_ticks       Absolute timer tick for starting radio ramp-up
 * @param[in] event_end_ticks  Absolute timer tick by which END is expected
 * @param[in] payload_length   Payload length in bytes
 *
 * @return  true if the transmission completed before the end time
 * @return  false if the transmission failed or timed out
 */
bool nrf_sf_radio_tx_start(uint8_t *payload, uint32_t txen_ticks,
                           uint32_t event_end_ticks,
                           uint8_t payload_length);

/**
 * @brief   Start radio reception at a scheduled radio start time
 *
 * @param[out] rx_buffer            Buffer for the received radio packet
 * @param[in]  rxen_ticks           Absolute timer tick for starting radio ramp-up
 * @param[in]  rx_window_end_ticks  Absolute timer tick for ADDRESS timeout
 * @param[in]  rx_end_ticks         Absolute timer tick by which END is expected
 *
 * @return  true if a valid packet was received before the end time
 * @return  false if no valid packet was received or reception timed out
 */
uint8_t nrf_sf_radio_rx_start(uint8_t *rx_buffer, uint32_t rxen_ticks,
                           uint32_t rx_window_end_ticks,
                           uint32_t rx_end_ticks);

/**
 * @brief   Start radio reception immediately and wait for one packet
 *
 * @param[out] rx_buffer      Buffer for the received radio packet
 * @param[in]  rx_end_ticks   Relative timeout for END after ADDRESS
 * @param[in]  runtimeout_us  Relative timeout for ADDRESS in microseconds
 *
 * @return  Corrected ADDRESS timestamp in timer ticks on success
 * @return  0 on timeout or CRC failure
 */
uint32_t nrf_sf_radio_rx_listen_until_packet(uint8_t *rx_buffer,
                                             uint32_t rx_end_ticks,
                                             uint32_t runtimeout_us);

#ifdef __cplusplus
}
#endif

#endif /* NRF_SF_RADIO_LINK_RADIO_H */
