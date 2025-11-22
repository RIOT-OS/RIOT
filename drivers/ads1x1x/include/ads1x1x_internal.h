/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2018 Acutam Automation, LLC
 * SPDX-FileCopyrightText: 2025 Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ads1x1x
 * @{
 *
 * @file
 * @brief       Internal definitions for ADS101x/111x devices
 *
 * @author      Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADS101x/111x register addresses
 * @{
 */
#define ADS1X1X_CONV_RES_ADDR      (0)  /**< Conversion register */
#define ADS1X1X_CONF_ADDR          (1)  /**< Configuration register */
#define ADS1X1X_LOW_LIMIT_ADDR     (2)  /**< Low limit register */
#define ADS1X1X_HIGH_LIMIT_ADDR    (3)  /**< High limit register */
/** @} */

/**
 * @brief    ADS101x/111x Operational Status
 */
#define ADS1X1X_CONF_OS_CONV_MASK         (1 << 7) /**< Operational status bit */

/**
 * @name    ADS101x/111x mux settings
 * Supports both single mode and differential.
 * This has no effect on ADS1013-4 and ADS1113-4.
 * @{
 */
#define ADS1X1X_MUX_MASK           ((1 << 6) | (1 << 5) | (1 << 4)) /**< Mask for MUX bits */
#define ADS1X1X_AIN0_DIFFM_AIN1    ((0 << 6) | (0 << 5) | (0 << 4)) /**< Differential AIN0 - AIN1
                                                                        (default) */
#define ADS1X1X_AIN0_DIFFM_AIN3    ((0 << 6) | (0 << 5) | (1 << 4)) /**< Differential AIN0 - AIN3 */
#define ADS1X1X_AIN1_DIFFM_AIN3    ((0 << 6) | (1 << 5) | (0 << 4)) /**< Differential AIN1 - AIN3 */
#define ADS1X1X_AIN2_DIFFM_AIN3    ((0 << 6) | (1 << 5) | (1 << 4)) /**< Differential AIN2 - AIN3 */
#define ADS1X1X_AIN0_SINGM         ((1 << 6) | (0 << 5) | (0 << 4)) /**< Single-ended AIN0 */
#define ADS1X1X_AIN1_SINGM         ((1 << 6) | (0 << 5) | (1 << 4)) /**< Single-ended AIN1 */
#define ADS1X1X_AIN2_SINGM         ((1 << 6) | (1 << 5) | (0 << 4)) /**< Single-ended AIN2 */
#define ADS1X1X_AIN3_SINGM         ((1 << 6) | (1 << 5) | (1 << 4)) /**< Single-ended AIN3 */
/** @} */

/**
 * @name    ADS101x/111x programmable gain
 * Sets the full-scale range (max voltage value).
 * This has no effect on ADS1013 and ADS1113 (both use 2.048V FSR).
 * @{
 */
#define ADS1X1X_PGA_MASK         ((1 << 3) | (1 << 2) | (1 << 1))   /**< Mask for PGA bits */
#define ADS1X1X_PGA_FSR_6V144    ((0 << 3) | (0 << 2) | (0 << 1))   /**< +/-6.144V */
#define ADS1X1X_PGA_FSR_4V096    ((0 << 3) | (0 << 2) | (1 << 1))   /**< +/-4.096V */
#define ADS1X1X_PGA_FSR_2V048    ((0 << 3) | (1 << 2) | (0 << 1))   /**< +/-2.048V (default) */
#define ADS1X1X_PGA_FSR_1V024    ((0 << 3) | (1 << 2) | (1 << 1))   /**< +/-1.024V */
#define ADS1X1X_PGA_FSR_0V512    ((1 << 3) | (0 << 2) | (0 << 1))   /**< +/-0.512V */
#define ADS1X1X_PGA_FSR_0V256    ((1 << 3) | (0 << 2) | (1 << 1))   /**< +/-0.256V */
/** @} */

/**
 * @name ADS101x/111x operating modes
 * MODE bit in the config register
 * @{
 */
#define ADS1X1X_MODE_MASK         (1 << 0)  /**< Mask for MODE bit */
#define ADS1X1X_MODE_SINGLE       (1 << 0)  /**< Single-shot / power-down */
#define ADS1X1X_MODE_CONTINUOUS   (0 << 0)  /**< Continuous conversion */
/** @} */

/**
 * @name ADS101x/ADS111x Data Rate Settings
 * @brief Register bit definitions for configuring data rate (samples per second).
 * @{
 */

#define ADS1X1X_DATAR_UNDEF   (0xFF)   /**< Undefined / invalid data rate */
#define ADS1X1X_DATAR_MASK    ((1 << 7) | (1 << 6) | (1 << 5)) /**< Bit mask for data rate field */

/**
 * @name ADS111x Data Rates
 * @brief Valid data rates for ADS111x (in SPS).
 * @{
 */
#if MODULE_ADS111X
#  define ADS111X_DATAR_8       ((0 << 7) | (0 << 6) | (0 << 5)) /**< 8 SPS */
#  define ADS111X_DATAR_16      ((0 << 7) | (0 << 6) | (1 << 5)) /**< 16 SPS */
#  define ADS111X_DATAR_64      ((0 << 7) | (1 << 6) | (0 << 5)) /**< 64 SPS */
#  define ADS111X_DATAR_128     ((0 << 7) | (1 << 6) | (1 << 5)) /**< 128 SPS (default) */
#  define ADS111X_DATAR_250     ((1 << 7) | (0 << 6) | (0 << 5)) /**< 250 SPS */
#  define ADS111X_DATAR_475     ((1 << 7) | (0 << 6) | (1 << 5)) /**< 475 SPS */
#  define ADS111X_DATAR_860     ((1 << 7) | (1 << 6) | (0 << 5)) /**< 860 SPS (max) */
#endif
/** @} */

/**
 * @name ADS101x Data Rates
 * @brief Valid data rates for ADS101x (in SPS).
 * @{
 */
#if MODULE_ADS101X
#  define ADS101X_DATAR_128     ((0 << 7) | (0 << 6) | (0 << 5)) /**< 128 SPS */
#  define ADS101X_DATAR_250     ((0 << 7) | (0 << 6) | (1 << 5)) /**< 250 SPS */
#  define ADS101X_DATAR_490     ((0 << 7) | (1 << 6) | (0 << 5)) /**< 490 SPS */
#  define ADS101X_DATAR_920     ((0 << 7) | (1 << 6) | (1 << 5)) /**< 920 SPS */
#  define ADS101X_DATAR_1600    ((1 << 7) | (0 << 6) | (0 << 5)) /**< 1600 SPS (default) */
#  define ADS101X_DATAR_2400    ((1 << 7) | (0 << 6) | (1 << 5)) /**< 2400 SPS */
#  define ADS101X_DATAR_3300    ((1 << 7) | (1 << 6) | (0 << 5)) /**< 3300 SPS (max) */
#endif
/** @} */
/** @} */

/**
 * @name Comparator mode
 * Selects comparator operation
 * @{
 */
#define ADS1X1X_COMP_MODE_MASK        (1 << 4)  /**< Mask for COMP_MODE bit */
#define ADS1X1X_COMP_MODE_TRADITIONAL (0 << 4)  /**< Traditional comparator (default) */
#define ADS1X1X_COMP_MODE_WINDOW      (1 << 4)  /**< Window comparator */
/** @} */

/**
 * @name Comparator polarity
 * Controls ALERT/RDY pin polarity
 * @{
 */
#define ADS1X1X_COMP_POLARITY_MASK (1 << 3) /**< Mask for COMP_POLARITY bit */
#define ADS1X1X_COMP_POLARITY_LOW  (0 << 3) /**< Active low (default) */
#define ADS1X1X_COMP_POLARITY_HIGH (1 << 3) /**< Active high */
/** @} */

/**
 * @name Comparator latch
 * Latching comparator output
 * @{
 */
#define ADS1X1X_COMP_LATCH_MASK    (1 << 2) /**< Mask for COMP_LATCH bit */
#define ADS1X1X_COMP_LATCH_DISABLE (0 << 2) /**< Non-latching (default) */
#define ADS1X1X_COMP_LATCH_ENABLE  (1 << 2) /**< Latching until read */
/** @} */

/**
 * @name Comparator queue
 * Configures comparator trigger queue
 * @{
 */
#define ADS1X1X_COMP_QUEUE_MASK       ((1 << 1) | (1 << 0)) /**< Mask for COMP_QUEUE bits */
#define ADS1X1X_COMP_QUEUE_1          ((0 << 1) | (0 << 0)) /**< Assert after 1 conversion */
#define ADS1X1X_COMP_QUEUE_2          ((0 << 1) | (1 << 0)) /**< Assert after 2 conversions */
#define ADS1X1X_COMP_QUEUE_4          ((1 << 1) | (0 << 0)) /**< Assert after 4 conversions */
#define ADS1X1X_COMP_QUEUE_DISABLE    ((1 << 1) | (1 << 0)) /**< Disable comparator (default) */
/** @} */

/**
 * @name ADS101x/111x bit resolution
 * @{
 */
#define ADS1X1X_BITS_RES_UNDEF (0)  /**< Undefined resolution */

/**
 * @brief ADS101x 12-bit resolution
 */
#if MODULE_ADS101X
#  define ADS101X_BITS_RES    (12) /**< 12-bit resolution */
#endif

/**
 * @brief ADS111x 16-bit resolution
 */
#if MODULE_ADS111X
#  define ADS111X_BITS_RES    (16) /**< 16-bit resolution */
#endif
/** @} */

/**
 * @brief Mask for all alert-related configuration bits
 * (comparator mode, polarity, latch, queue).
 */
#define ADS1X1X_ALERT_MASK (ADS1X1X_COMP_QUEUE_MASK | ADS1X1X_COMP_LATCH_MASK | \
                            ADS1X1X_COMP_POLARITY_MASK | ADS1X1X_COMP_MODE_MASK)

/**
 * @brief Get the voltage reference for a given PGA setting
 *
 * @param[in] pga  PGA setting
 *
 * @return Voltage reference in millivolts
 */
static inline uint16_t _ads1x1x_get_pga_voltage(uint8_t pga)
{
    switch (pga) {
    case ADS1X1X_PGA_FSR_6V144: return 6144;
    case ADS1X1X_PGA_FSR_4V096: return 4096;
    case ADS1X1X_PGA_FSR_2V048: return 2048;
    case ADS1X1X_PGA_FSR_1V024: return 1024;
    case ADS1X1X_PGA_FSR_0V512: return 512;
    case ADS1X1X_PGA_FSR_0V256: return 256;
    default: return 0;
    }
}

#ifdef __cplusplus
}
#endif

/** @} */
