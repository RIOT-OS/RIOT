/*
 * Copyright (C) 2026 Xin He
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_nrf_sf_radio nRF synchronous flooding radio
 * @ingroup     sys
 * @{
 *
 * @file        radio_driver.h
 * @brief       Physical layer radio, timer and PPI configuration interface
 *
 * @author      Xin He <xin.he@mailbox.tu-dresden.de>
 */

#ifndef NRF_SF_RADIO_RADIO_DRIVER_H
#define NRF_SF_RADIO_RADIO_DRIVER_H

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
 * @param[in]       event          Required condition
 * @param[in]       timeout_ticks  Time interval to wait
 */
void nrf_sf_radio_wait_until(volatile uint32_t *event,
                             uint32_t timeout_ticks);

/**
 * @brief   Busy-wait to a time point or condition
 *
 * @param[in]       event           Required condition
 * @param[in]       deadline_ticks  Time point to wait
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
 * @param[in]       buf             Buffer for the transmitted radio packet
 * @param[in]       deadline_ticks  Scheduled radio start time
 */
void nrf_sf_radio_tx_arm(uint8_t *buf, uint32_t deadline_ticks);

/**
 * @brief   Start radio reception at the scheduled time
 *
 * @param[out]      buf             Buffer for the received radio packet
 * @param[in]       deadline_ticks  Scheduled radio start time
 */
void nrf_sf_radio_rx_arm(uint8_t *buf, uint32_t deadline_ticks);

/**
 * @brief   Start radio reception immediately
 *
 * @param[out]      buf             Buffer for the received radio packet
 */
void nrf_sf_radio_try_rx_enable(uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif /* NRF_SF_RADIO_RADIO_DRIVER_H */

/** @} */
