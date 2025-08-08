/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_adcxx1c
 * @{
 *
 * @file
 * @brief       Register definition for ADCXX1C devices
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADCxx1C register addresses
 * @{
 */
#define ADCXX1C_CONV_RES_ADDR      (0)
#define ADCXX1C_ALERT_STATUS_ADDR  (1)
#define ADCXX1C_CONF_ADDR          (2)
#define ADCXX1C_LOW_LIMIT_ADDR     (3)
#define ADCXX1C_HIGH_LIMIT_ADDR    (4)
#define ADCXX1C_HYSTERESIS_ADDR    (5)
#define ADCXX1C_LOWEST_CONV_ADDR   (6)
#define ADCXX1C_HIGHEST_CONV_ADDR  (7)
/** @} */

/**
 * @name    ADCxx1C Config flags
 * @{
 */
#define ADCXX1C_CONF_ALERT_PIN_EN   (1 << 2)
#define ADCXX1C_CONF_ALERT_FLAG_EN  (1 << 3)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
