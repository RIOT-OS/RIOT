/*
 * SPDX-FileCopyrightText: 2026 Xin He
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_nrf_sf_radio
 * @{
 *
 * @file
 * @brief       Link layer radio TX/RX startup interface
 *
 * @author      Xin He <xin.he@mailbox.tu-dresden.de>
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif



/**
 * @brief   Send one payload at a scheduled radio start time
 *
 * @param[in]  frame_buf        Frame to be transmitted
 * @param[in]  txen_ticks       Absolute timer tick for starting radio ramp-up
 * @param[in]  event_end_ticks  Absolute timer tick by which END is expected
 * @param[in]  frame_length     Frame length in bytes
 *
 * @retval  true if the transmission completed before the end time
 * @retval  false if the transmission failed or timed out
 */
bool nrf_sf_radio_tx_start(uint8_t *frame_buf, uint32_t txen_ticks,
                           uint32_t event_end_ticks,
                           uint8_t frame_length);

/**
 * @brief   Start radio reception at a scheduled radio start time
 *
 * @param[in,out] rx_buffer            Buffer for the received radio packet
 * @param[in]     rxen_ticks           Absolute timer tick for starting radio ramp-up
 * @param[in]     rx_window_end_ticks  Absolute timer tick for ADDRESS timeout
 * @param[in]     rx_end_ticks         Absolute timer tick by which END is expected
 *
 * @retval  0 if valid packet received
 * @retval  1 if ADDRESS event timeout
 * @retval  2 if END event timeout
 * @retval  3 if CRC check failed
 */
uint8_t nrf_sf_radio_rx_start(uint8_t **rx_buffer, uint32_t rxen_ticks,
                              uint32_t rx_window_end_ticks,
                              uint32_t rx_end_ticks);

/**
 * @brief   Start radio reception immediately and wait for one packet
 *
 * @param[in,out] rx_buffer      Buffer for the received radio packet
 * @param[in]     rx_end_ticks   Relative timeout for END after ADDRESS
 * @param[in]     runtimeout_us  Relative timeout for ADDRESS in microseconds
 *
 * @return  Corrected ADDRESS timestamp in timer ticks on success
 * @retval  0 on timeout or CRC failure
 */
uint32_t nrf_sf_radio_rx_listen_until_packet(uint8_t **rx_buffer,
                                             uint32_t rx_end_ticks,
                                             uint32_t runtimeout_us);

#ifdef __cplusplus
}
#endif

/** @} */
