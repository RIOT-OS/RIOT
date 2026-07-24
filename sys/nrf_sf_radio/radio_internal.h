/*
 * SPDX-FileCopyrightText: 2026 Xin He
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_nrf_sf_radio
 * @{
 *
 * @file
 * @brief       Internal radio helper interface
 *
 * @author      Xin He <xin.he@mailbox.tu-dresden.de>
 *
 * @}
 */

#pragma once

#include <stdint.h>

/**
 * @brief   Busy-wait to a time point or condition
 *
 * @param[in]  event           Required condition
 * @param[in]  deadline_ticks  Time point to wait
 */
void nrf_sf_radio_wait_until_abs(volatile uint32_t *event,
                                 uint32_t deadline_ticks);

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
