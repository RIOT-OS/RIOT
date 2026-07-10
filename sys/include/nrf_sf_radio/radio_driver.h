/*
 * SPDX-FileCopyrightText: 2026 Xin He
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_nrf_sf_radio nRF synchronous flooding radio
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Physical layer radio, timer and PPI configuration interface
 *
 * @author      Xin He <xin.he@mailbox.tu-dresden.de>
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   16 MHz timer: 1 microsecond equals 16 ticks
 */
#define NRF_SF_RADIO_TIMER_TICKS_PER_US    (16U)

/**
 * @brief   Convert microseconds to ticks
 */
#define NRF_SF_RADIO_US_TO_TIMER_TICKS(us) ((uint32_t)(us) << 4U)

/**
 * @brief   Convert ticks to microseconds
 */
#define NRF_SF_RADIO_TIMER_TICKS_TO_US(tk) ((uint32_t)(tk) >> 4U)

/**
 * @brief   ramp-up time ticks
 */
#define NRF_SF_RADIO_RAMPUP_TIME_TICKS     NRF_SF_RADIO_US_TO_TIMER_TICKS(40U)

/**
 * @brief   Initialize the radio timer and PPI
 */
void nrf_sf_radio_start(void);

/**
 * @brief   Busy-wait for a time interval or condition
 *
 * @param[in]  event          Required condition
 * @param[in]  timeout_ticks  Time interval to wait
 */
void nrf_sf_radio_wait_until(volatile uint32_t *event,
                             uint32_t timeout_ticks);

/**
 * @brief   Busy-wait to a time point or condition
 *
 * @param[in]  event           Required condition
 * @param[in]  deadline_ticks  Time point to wait
 */
void nrf_sf_radio_wait_until_abs(volatile uint32_t *event,
                                 uint32_t deadline_ticks);

/**
 * @brief   Get the timestamp of the last radio READY event
 *
 * @return  Timestamp of the last radio READY event
 */
uint32_t nrf_sf_radio_get_last_ready_time_ticks(void);

/**
 * @brief   Get the timestamp of the last radio END event
 *
 * @return  Timestamp of the last radio END event
 */
uint32_t nrf_sf_radio_get_last_end_time_ticks(void);

/**
 * @brief   Get the timestamp of the last radio ADDRESS event
 *
 * @return  Timestamp of the last radio ADDRESS event
 */
uint32_t nrf_sf_radio_get_last_address_time_ticks(void);

/**
 * @brief   Get the current timestamp
 *
 * @return  Current timestamp
 */
uint32_t nrf_sf_radio_now_ticks(void);

/**
 * @brief   Start radio transmission at the scheduled time
 *
 * @param[in]  buf             Buffer for the transmitted radio packet
 * @param[in]  deadline_ticks  Scheduled radio start time
 */
void nrf_sf_radio_tx_arm(uint8_t *buf, uint32_t deadline_ticks);

/**
 * @brief   Start radio reception at the scheduled time
 *
 * @param[out] buf             Buffer for the received radio packet
 * @param[in]  deadline_ticks  Scheduled radio start time
 */
void nrf_sf_radio_rx_arm(uint8_t *buf, uint32_t deadline_ticks);

/**
 * @brief   Start radio reception immediately
 *
 * @param[out] buf  Buffer for the received radio packet
 */
void nrf_sf_radio_try_rx_enable(uint8_t *buf);

/**
 * @brief   Set radio mode
 *
 * @param[in]  mode  RADIO MODE register value, e.g.
 *                   `RADIO_MODE_MODE_Ble_1Mbit`
 */
void nrf_sf_radio_set_mode(uint32_t mode);

/**
 * @brief   Set ble channel
 *
 * @param[in]  channel  BLE channel index, valid range: 0..39
 */
void nrf_sf_radio_set_ble_channel(uint8_t channel);

/**
 * @brief   Set TX power
 *
 * @param[in]  power  TX power in dBm
 */
void nrf_sf_radio_set_power(uint16_t power);

#ifdef __cplusplus
}
#endif

/** @} */
