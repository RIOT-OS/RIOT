/*
 * SPDX-FileCopyrightText: 2026 UGA
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_tmp117
 * @ingroup     drivers_sensors
 * @brief       Driver for TMP117 temperature sensor
 *
 * @{
 * @file
 * @brief		Registers definition for the TMP117 temperature sensor
 * @author      léo cordier <leo.cordier@univ-grenoble-alpes.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    TMP117 registers
 * @{
 */
#define TMP117_REG_TEMP_RESULT       (0x00) /**< temperature register address */
#define TMP117_REG_CONFIG            (0x01) /**< configuration register address */
#define TMP117_REG_TEMP_HIGH_LIMIT   (0x02) /**< high temperature limit register address */
#define TMP117_REG_TEMP_LOW_LIMIT    (0x03) /**< low temperature limit register address */
#define TMP117_REG_EEPROM_UNLOCK     (0x04) /**< EEPROM unlock register address */
#define TMP117_REG_EEPROM_1          (0x05) /**< EEPROM 1 register address */
#define TMP117_REG_EEPROM_2          (0x06) /**< EEPROM 2 register address */
#define TMP117_REG_TEMP_OFFSET       (0x07) /**< temperature offset register address */
#define TMP117_REG_EEPROM_3          (0x08) /**< EEPROM 3 register address */
#define TMP117_REG_DEVICE_ID         (0x0F) /**< device ID register address */
/** @} */

/**
 * @name    TMP117 shift mask
 * @{
 */
#define TMP117_DATA_READY_SHIFT     13  /**< data ready bit shift in configuration register */
#define TMP117_CONV_MODE_SHIFT      10  /**< conversion mode bits shift in configuration register */
#define TMP117_CONV_CYCLE_SHIFT     7   /**< conversion cycle bits shift in configuration register */
#define TMP117_AVG_SHIFT            5   /**< averaging bits shift in configuration register */
/** @} */

/**
 * @name    TMP117 field mask
 * @{
 */
#define TMP117_CONV_MODE_MASK       (0x3 << TMP117_CONV_MODE_SHIFT)     /**< conversion mode bits mask in configuration register */
#define TMP117_CONV_CYCLE_MASK      (0x7 << TMP117_CONV_CYCLE_SHIFT)    /**< conversion cycle bits mask in configuration register */
#define TMP117_AVG_MASK             (0x3 << TMP117_AVG_SHIFT)           /**< averaging bits mask in configuration register */
#define TMP117_DATA_READY_MASK      (0x1 << TMP117_DATA_READY_SHIFT)    /**< data ready bit mask in configuration register */

/** @} */

/**
 * @name    TMP117 registers - default values
 * @{
 */

#define TMP117_REG_DEVICE_ID_DEFAULT_VAL         (0x0117) /**< default value of tmp117 device id register */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
