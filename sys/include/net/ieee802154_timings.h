/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     net_ieee802154
 * @{
 *
 * @file
 * @brief       IEEE 802.15.4 MAC/PHY timing calculations
 *
 * @author       Stepan Konoplev <stepan.konoplev@haw-hamburg.de>
 */

#include <stdint.h>

#include "net/ieee802154/radio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get the symbol duration for the current PHY configuration.
 *
 * @param[in] conf pointer to the config descriptor
 *
 * @return symbol duration in microseconds.
 */
uint16_t ieee802154_get_symbol_duration(const ieee802154_phy_conf_t *conf);

/**
 * @brief   Get the _phyCcaDuration_ value in microseconds.
 *
 * @param[in] conf pointer to the config descriptor
 *
 * @return CCA duration in microseconds.
 */
uint32_t ieee802154_get_cca_time(const ieee802154_phy_conf_t *conf);

/**
 * @brief   Get the _aTurnaroundTime_ PHY constant value in microseconds.
 *
 * @param[in] conf pointer to the config descriptor
 *
 * @return constant value in microseconds.
 */
uint32_t ieee802154_get_turnaround_time(const ieee802154_phy_conf_t *conf);

/**
 * @brief   Calculate the _aUnitBackoffPeriod_ value in microseconds.
 *
 * @param[in] conf pointer to the config descriptor
 *
 * @return unit backoff period in microseconds.
 */
uint32_t ieee802154_calculate_unit_backoff_period(const ieee802154_phy_conf_t *conf);

/**
 * @brief   Calculate the _macAckWaitDuration_ value in microseconds.
 *
 * @param[in] conf pointer to the config descriptor
 *
 * @return ACK wait duration in microseconds.
 */
uint32_t ieee802154_calculate_ack_wait_duration(const ieee802154_phy_conf_t *conf);

#ifdef __cplusplus
}
#endif

/** @} */
